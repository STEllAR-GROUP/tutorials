
class: center, middle

# Resource Management and Performance Issues
## Keeping it under control

[Overview](..)

Previous: [Worked 2D Stencil Example](../session5)

???

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
    );```
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
##direct actions and plain actions

---
##Start/Stop the runtime

---

class: center, middle
## Next

 [Debugging and Profiling](../session6)
