
class: center, middle

# Resource Management and Performance Issues
## Making the best use of the machine

[Overview](..)

Previous: [Worked 2D Stencil Example](../session5)

???

---
##Resource Partitioner (RP)
* Main objective : More than one thread pool

* Why would you do this?
    * Control one set of tasks independently from another
    * Separate out functionality that might conflict
        * system related activities that use blocking calls
        * (even std::cout can be blocking)

* How does it affect code
    * Implies >1 Scheduler
    * Requires >1 Executor
        * Remember Executors are the 'where' of tasks

* **Optional**- new feature introduced post 1.0 release

---
##Resource Partitioner Basics
* RP must exist before the runtime is 'up'
    * Core binding / threads command line params needed
    * User needs to tell the pools what to own
        * how to 'partition' up the node
    * Should be reasonably hardware agnostic
        * simple abstractions for cores etc

* RP (currently) understands
    * Numa Domains (or aliased to sockets if the OS lacks them)
    * Cores : one or more PUs
    * PUs : The processing units or hyperthreads on a core

---
##Resource Partitioner Basic Creation
* Create RP and pass in options object, plus command line
```
    hpx::resource::partitioner rp(desc_cmdline, argc, argv);
```
* You want an extra thread pool to do MPI tasks
```
    rp.create_thread_pool("mpi",
        hpx::resource::scheduling_policy::local_priority_fifo);
    // Other schedulers are available (but use this one!)
```
* Give the pool something to work with
```
    // assumes you have already fetched mpi_threads from somewhere
    int count = 0;
    for (const hpx::resource::numa_domain& d : rp.numa_domains()) {
        for (const hpx::resource::core& c : d.cores()) {
            for (const hpx::resource::pu& p : c.pus()) {
                if (count < mpi_threads)
                {
                    std::cout << "Added pu " << count++ << " to mpi pool\n";
                    rp.add_resource(p, "mpi");
                }
            }
        }
    }
```

---
##Resource Partitioner : Get an Executor
* We need to launch certain tasks on our MPI pool
    * Here we skip the mpi_pool if nranks==1

```
    // create a generic shceduling executor that can be pool or default
    hpx::threads::scheduled_executor mpi_executor;
    //
    int m_size = a.comm().size();
    if (use_pools && m_size>1) {
        // get pool executor
        hpx::threads::executors::pool_executor mpi_exec("mpi");
        mpi_executor = mpi_exec;
        std::cout << "\n[hpx_main] got mpi executor " << std::endl;
    }
    else {
        // if we do not need our special pool, use default executor
        mpi_executor = hpx::parallel::execution::default_executor();
    }
```

---
##Resource Partitioner : Launch tasks
* Same as any other task, but use the pool (mpi) executor

```
    // Broadcast matrix block
    comm2D_ft = hpx::dataflow(
        // pass our executor to async launch
        mpi_executor,
        [root, taskname](hpx::future<CommType> comm2D_ft,
                         hpx::shared_future<Matrix<T>> diag_block_sf)
        {
            // give this task a name in APEX
            hpx::util::annotate_function apex_profiler("Comm");
            //
            const auto& diag_block = diag_block_sf.get();
            auto comm2D = comm2D_ft.get();
            // call broadcast function on matrix tile
            comm2D->colBcast(Message(diag_block.ptr(), diag_block.ld()), root);
            return comm2D;
        },
        comm2D_ft, diag_block_sf
    );
```
* This example is using `dataflow == when_all(comm2D_ft, diag_block_sf).then(lambda)`

---
##Resource Partitioner : Caution
* Continuation Tasks are launched on the pool that the parent is running on
* If the communication task spawns child tasks

<img src="images/mpi-disaster.png" alt="" height="300px" >

* Here we see that for N>1 (of the green plot), the perfomance falls to single threaded
because there is only one thread on the MPI pool.

---
##Resource Partitioner : Caution #2

* Solution : Don't use default_excutor if you have N>1 pools

* Use a `pool_executor` on the default pool!

```
// setup executors for different task priorities on the default (matrix) pool
hpx::threads::scheduled_executor matrix_HP_executor =
    hpx::threads::executors::pool_executor("default",
    hpx::threads::thread_priority_high);

hpx::threads::scheduled_executor matrix_normal_executor =
    hpx::threads::executors::pool_executor("default",
    hpx::threads::thread_priority_default);
```
* Then use that executor everywhere you would normally use a default one
(or where nd executor is ommitted)

---
##Resource Partitioner : Custom Scheduler / Default pool
* The API for this will almost certainly change

```
    rp.create_thread_pool("default",
      [](hpx::threads::policies::callback_notifier& notifier,
              std::size_t num_threads, std::size_t thread_offset,
              std::size_t pool_index, std::string const& pool_name)
      ->  std::unique_ptr<hpx::threads::detail::thread_pool_base>
    {
        std::unique_ptr<high_priority_sched> scheduler(
            new high_priority_sched(num_threads, hp_queues,
                                    numa_sensitive, numa_pinned,
                                    "shared-priority-scheduler"));

        scheduler_mode mode = scheduler_mode(
            scheduler_mode::do_background_work);

        std::unique_ptr<hpx::threads::detail::thread_pool_base> pool(
            new hpx::threads::detail::scheduled_thread_pool<
                    high_priority_sched
                >(std::move(scheduler), notifier,
                    pool_index, pool_name, mode, thread_offset));
        return pool;
    });
```

---
##Rename Default Pool
* You want N pools, you can loop over
* 1 per numa domain (let's say 4)
* The first pool is always called "default"

```
    // create the resource partitioner
    hpx::resource::partitioner rp(argc, argv);

    // before adding pools - set the default pool name to "pool-0"
    rp.set_default_pool_name("pool-0");

    // create N pools
    int numa_count = 0;
    for (const hpx::resource::numa_domain& d : rp.numa_domains())
    {
        // create pool
        std::string pool_name = "pool-"+std::to_string(numa_count);
        rp.create_thread_pool(pool_name,
            hpx::resource::scheduling_policy::local_priority_fifo);
        // add domain to it
        rp.add_resource(d, pool_name);
        numa_count++;
    }
```

---
##Move semantics
* async functions don't happen _right now_
    * async calls go onto the thread queue and are executed later
    * you can't get away with passing arguments by reference
    * you usually don't want to copy by value

* __Move arguments whenever you can__
```
    std::vector<double> dummy;
    ...
    hpx::parallel::for_loop(par(task), begin(x), end(x)) {
        [&dummy](iterator it) {
            int index = offset(it);
            double v = compute_thing(dummy[index]);
            // wait for a segfault
        }
    }
```
    * This example is too easy, but it happens frequently.

    * Moving saves copies, saves resources, saves time. If you only need it once
    then move it and don't waste time on copies.

---
##Callbacks using async_cb
* You pass something into an async, and you want to reuse the memory held by that object
as soon as it has 'gone' (i.e. been copied internally by the network for sending,
or been RDMA'd to a remote node, etc)
```
    std::shared_ptr<general_buffer_type> temp_buffer =
        std::make_shared<general_buffer_type>(
            static_cast<char*>(buffer), options.transfer_size_B,
            general_buffer_type::reference);

    auto temp_future =
        hpx::async_cb(actWrite, locality,
                hpx::util::bind(&my_callback, buffer_index, _1, _2),
                *temp_buffer,
                memory_offset, options.transfer_size_B
        ).then(
            hpx::launch::sync,
            [send_rank](hpx::future<int> &&fut) -> int {
                int result = fut.get();
                --FuturesWaiting[send_rank];
                return result;
            }
        );
```
* `my_callback` will be triggered when it is safe to use the arguments that were
passed into the async function.

    * (the placeholders refer to an error code and a parcel reference -
    these are returned in the callback so you can take action in case of an error)

---
##Chunk sizes
* You might call an async parallel::algorithm as follows
```
    std::vector<double> dummy;
    static_chunk_size param(42); // the perfect number for my test!
    ...
    hpx::parallel::for_loop(par(task).with(param), begin(x), end(x)) {
        [dummy=std::move(dummy)](iterator it) {
            int index = offset(it);
            double v = compute_thing(dummy[index]);
        }
    }
```
* The runtime performs the first iteration and times it

* Then chooses a chunk size to break the loop into N tasks

* You might want to 'guide' the algorithm if you know better than the runtime

* The chunk_size is the number of iterations to use per task, not the number of tasks

---
##When are tasks created
* As previously mentioned
```
    auto my_future = hpx:async(something).then(another_thing).then(yet_more);
```
* `my_future` and `something` are instantiated when that line of code is hit,
but `another_thing` isn't created until `something` completes, and `yet_more`
isn't created until `another_thing` completes.

```
    std::vector<shared_future<T>> futures;
    ...
    // make_ready_futures in futures to initialize list
    ...
    for (int i=0; i<1024) {
        for (int j=0; j<1024) {
            int f_index1 = my_complex_indexing_scheme1(i,j);
            int f_index2 = my_complex_indexing_scheme2(i,j);
            future[f_index1] = future[f_index2].then(
                [](auto &&f){
                    return something_wonderful(f.get());
                }
            );
        }
    }
```
* It looks harmless enough, but each future creation is asynchronous, there are no waits
in the loop.

* A million futures have just been instantiated and tasks queued for the max value
returned by my_complex_indexing_scheme

---
##Launch policies : sync
* A continuation attached to a future using sync policy does not need to
create another new task.

* The task reduces to a function invocation on the same thread as
the future that becomes ready
```
    future_2 = future_1.then(hpx::launch::sync,
        [](auto &&f){
            return something_wonderful(f.get());
        }
    );
```

* Why not use it all the time?
    * granularity of tasks
    * stack size

* It's ideal for small and quick tasks

---
## Stack size?
* consider the following
```
    for (int i=0; i<1024) {
        for (int j=0; j<1024) {
            int f_index1 = my_complex_indexing_scheme1(i,j);
            int f_index2 = my_complex_indexing_scheme2(i,j);
            future[f_index1] = future[f_index2].then(hpx::launch::sync,
                [](auto &&f){
                    return something_wonderful(f.get());
                }
            );
        }
    }
```
* if the index returned by `f_index2` and `f_index1` ping pong between two
values repeatedly, you can get a situation where your calling <br \>
`f1.then(f2.then(f3.then(f4.then(...))));`

* since each task runs on the same frame as the last, the stack pointer
for each will keep incrementing until there's an overflow.

---
##Change the stack size
* `-Ihpx.stacks.small_size=0x4000` will set the stack size on the command line
    * nice, but this sets the stack size for _every_ task that runs

```
    hpx::threads::executors::default_executor large_stack_executor(
        hpx::threads::thread_stacksize_large);

    hpx::future<void> f =
        hpx::async(large_stack_executor, &run_with_large_stack);
```
* You can create a large stack executor and use it for specific tasks.

* Note that you can also create executors using other parameters
```
    hpx::threads::executors::default_executor fancy_executor(
        hpx::threads::thread_priority_critical,
        hpx::threads::thread_stacksize_large);

    hpx::future<void> f =
        hpx::async(fancy_executor, &run_with_large_stack);
```

---
## Launch policies : fork
```
    future_2 = future_1.then(hpx::launch::fork,
        [](auto &&f){
            return something_wonderful(f.get());
        }
    );
```
* The task that spawns the child will suspend and the child runs in its place
    * The parent task goes back to the pending work queue

* This is a reversal of the usual '*work stealing*' pardigm
    * It's known as '*Parent Stealing*'

* when a worker becomes free, the parent will be stolen/run
    The parent can never create 'too many' children

* For very resource hungry applications
    * you limit the number of child tasks to the number of worker threads

* There will be a penalty when a worker finishes:
    * switch to parent : spawn a child : switch to child

---
##Sliding Semaphore
* Can we unroll the lops a bit more slowly? - can we have N iterations
'in flight' at a time?
```
    hpx::lcos::local::sliding_semaphore sem(N);
    //
    for (int i=0; i<max_rows) {
        for (int j=0; j<max_cols) {
            int f_index1 = my_complex_indexing_scheme1(i,j);
            int f_index2 = my_complex_indexing_scheme2(i,j);
            future[f_index1] = future[f_index2].then(hpx::launch::sync,
                [](auto &&f){
                    auto temp = something_wonderful(f.get());
                    // at the end of each iteration, signal our semaphore
                    if (j==max_cols-1) sem.signal(i);
                    return temp;
                }
            );
        }
        sem.wait(i);
    }
```
* Now there will only be N outer loops executed at a time - and each time one completes a
new one can begin.

* Note that this is a contrived example and real-world use might require more thought

---
##Plain and Direct Actions
* An action is a remote function call, returning a future
* A plain action is the normal kind of action
    * When you call `hpx::async(action, locality, args ...);`
    * 1) The message is despatched immediately on the current task
    and a future is returned
    * 2) When the message arrives, it will be decoded
        * but only when the remote node finishes a task and polls the network
    * 3) once decoded the action becomes a task on the pending queue
    * 4) the action executes when a worker becomes free
        * fast response is not guaranteed!
* A direct action skips steps 3,4
    * after decoding, the task is executed directly on the decoding thread
    * this creates a faster turnaround for certain actions
    * it can be used for short remote functions (such as this)

```
hpx::serialization::serialize_buffer<char>
message(hpx::serialization::serialize_buffer<char> const& receive_buffer)
{
    return receive_buffer;
}
```
---
##Serialize_Buffer
* Sending an array can be optimized by using a `serialize_buffer`
* Any data block that is bitwise serializable can be cast to a pointer and passed into
a `serialize_buffer`
* The `serialize_buffer` has a specialization in the `hpx::serialization` layer to
pass the pointer thought without any overheads (it can be zero copied or RDMA'd)
```
    typedef hpx::serialization::serialize_buffer<char> buffer_type;
    buffer_type recv_buffer;
    ...
    recv_buffer = msg(dest, buffer_type(send_buffer, size, buffer_type::reference));
```
* The constructor of the `serialize_buffer` can copy, take ownership or a just take
a reference to the underlying data.

---
##Latency Example
* A common benchmark is to ping pong a message back and forth between 2 nodes
* Using MPI, a thread calls `send` then blocks on `receive`
    * On the remote node, one blocks on `receive` and then calls `send`
    * turnaround is fast

* Given what we know about actions : how fast is HPX compared to MPI?

* Caveat : HPX isn't designed to have a fast response to ping-pong type messages
    * we will instead measure the average time for 1, when N messages are in flight
    at once

---
##Latency V0
* Synchronous send and receive of a message

* An action is spawned to do nothing other than send a message back
    * We wait for the return after every send

* Do this in a loop and find the averaage time

* Using more threads helps slightly as polling the network is faster

* Changing the window size has no effect

[See the source code](https://github.com/STEllAR-GROUP/tutorials/blob/master/examples/01_latency/latency.cpp#L75)

* Note the use of DIRECT_ACTION, `serialize_buffer`

---
##Latency V1
* Vector of futures

* Spawn N actions and store the futures in a vector

* Wait on the vector of futures until all complete

* take the time and compute the average for 1

* Gives a more realistic answer than v0, but we are not really measuring N

[See the source code](https://github.com/STEllAR-GROUP/tutorials/blob/master/examples/01_latency/latency.cpp#L111)

* Note : We are actually measuring a sawtooth from 0 to N
<crop>
    <img src="images/sawtooth.jpg" alt="" height="300px" >
</crop>

---
##Latency V2
* Simple Atomic Counter and Condition Variable

* Spawn N messages, each time one returns, increment a counter

* When the counter reaches N, restart

* Simple, but still a sawtooth

[See the source code](https://github.com/STEllAR-GROUP/tutorials/blob/master/examples/01_latency/latency.cpp#L154)

---
##Latency V3
* Sliding Semaphore
* Loop over sends, and track how many are in flight with a sliding semaphore
* This will maintain N in flight using a sliding window, so that when <N are in flight
the loop continues, when N are in floght, the loop blocks.
    * Note, we actually set the window to N-1 so that we can measure 1 because
sliding semaphore uses > and not >= as the test internally
* we have got past the sawtooth, but there's a nasty bug
* The Nth message may return before the N-1 (or N-2 etc)th message because when multiple
threads are used, on the remote node, one might get suspended by the OS and return after
a later one
* Our semaphore is therefore 'noisy' and we don't have exactly N in flight
* Can segfault if one late message returns after the semaphore goes out of scope
* Add an extra condition variable at the end to make sure we keep semaphore alive
until the last message has returned
    * (this also means the timing is correct on the last iteration)

[See the source code](https://github.com/STEllAR-GROUP/tutorials/blob/master/examples/01_latency/latency.cpp#L211)

---
##Latency V4
* Sliding Semaphore with Atomic

* The bug in V3 is caused by the noisy/random return of messages

* We can easily fix this by using an atomic counter instead of the loop index
for triggering our semaphore.

* We no longer need the condition variable at the end to prevent segfaults on the
semaphore access.

[See the source code](https://github.com/STEllAR-GROUP/tutorials/blob/master/examples/01_latency/latency.cpp#L275)

* V5 : Suggestions welcome for an even better version

---
class: center, middle
## Next

 [Debugging and Profiling](../session6)
