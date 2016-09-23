
class: center, middle

# Introduction to HPX
## Part 2

[Overview](..)

Previous: [Introduction to HPX - Part 1](../session1)

???
[Click here to view the Presentation](https://stellar-group.github.io/tutorials/cscs2016/session2/)

---
## Recap: What's HPX

* Solidly based on a theoretical foundation – a well defined, new execution
  model (ParalleX)
* Exposes a coherent and uniform, standards-oriented API for ease of
  programming parallel and distributed applications.
    * Enables to write fully asynchronous code using hundreds of millions of threads.
    * Provides unified syntax and semantics for local and remote operations.
* Developed to run at any scale
* Compliant C++ Standard implementation (and more)
* Open Source: Published under the Boost Software License

---
## Recap: What's HPX

HPX represents an innovative mixture of

* A global system-wide address space (AGAS - Active Global Address Space)
* Fine grain parallelism and lightweight synchronization
* Combined with implicit, work queue based, message driven computation
* Full semantic equivalence of local and remote execution, and
* Explicit support for hardware accelerators (through percolation)

---
## The HPX Programming Model

![The HPX Programming Model](images/model-0.png)

---
## The HPX Programming Model

![The HPX Programming Model](images/model-1.png)

---
## The HPX Programming Model

![The HPX Programming Model](images/model-2.png)

---
## The HPX Programming Model

![The HPX Programming Model](images/model-3.png)

---
## The HPX Programming Model

![The HPX Programming Model](images/model-4.png)

---
## The HPX API
### Borrowed from the C++ Standard

Class                           | Description
--------------------------------|-----------------------------------
`hpx::thread`                   | Low level thread of control
`hpx::mutex`                    | Low level synchronization facility
`hpx::lcos::local::condition_variable`      | Signal a condition
`hpx::future`                   | Asynchronous result transport (receiving end)
`hpx::promise`, `hpx::lcos::local::promise` | Asynchronous result transport (producing end)
`hpx::lcos::packaged_task`      | Asynchronous result transport (producing end)
`hpx::async`                    | Spawning tasks (returns a future)
`hpx::bind`                     | Binding Parameters to callables
`hpx::function`                 | Type erased function object
`hpx::tuple`                    | Tuple
`hpx::any`                      | Type erased object (similar to `void*`)
`hpx::parallel::for_each`, etc. | Parallel Algorithms
`hpx::compute::vector`          | Continous storage for N elements

Extensions to the standards APIs where necessary, maintaining full compatibility.

---
## The HPX API
### Lightweight Control Objects (LCOs)

 * Objects to synchronize between different threads of execution.
 * Ability to suspend and reactivate Tasks
 * Examples:
    * `mutex`
    * `condition_variable`
    * `channel`
    * `promise`
    * `future`
    * `when_all`, `when_any`
    * ...
 * More on those later

---
## What is a (the) future?

A future is an object representing a result which has not been calculated yet

.left-column[
![...](images/future.png)
]

.right-column[
* Enables transparent synchronization with producer
* Hides notion of dealing with threads
* Makes asynchrony manageable
* Allows for composition of several asynchronous operations
* Turns concurrency into parallelism
]

---
## Diving into the Future - The (basic) API

```
template <typename R>
class future
{
    // future constructors
    // Query the state
    // Waiting on the result
};

template <typename R>
class shared_future
{
    // Future constructors
    // Query the state
    // Waiting on the result
};
```

---
## Diving into the Future - The (basic) API

Constructing a `hpx::future<R>`

```
template <typename R>
class future
{
    // Future constructors
    // Construct an empty future.
    future();

    // Move a future to a new one
    future(future&& f);

    // Unwrap a future. The new future becomes ready when
    // the inner, and outer future are ready.
    explicit future(future<future>&& f);
    explicit future(shared_future<future>&& f);

    // Turn this future into a shared_future. Invalidates
    // the future!
    shared_future<T> share();

    // Query the state
    // Waiting on the result
};
```

---
## Diving into the Future - The (basic) API

Querying the state of the future

```
template <typename R>
class future
{
    // Future constructors

    // Query the state

    // Check if the future is ready yet.
    bool is_ready();

    // Check if the future has a value
    bool has_value();

    // Check if the future has an exception
    bool has_exception();

    // Waiting on the result
};
```

---
## Diving into the Future - The (basic) API

Waiting for the future to become ready
```
template <typename R>
class future
{
    // Future constructors
    // Query the state

    // Waiting on the result
    void wait();

    // Get the result. This function might block if the result has
    // not been computed yet. Invalidates the future!
    R get();

    // Attach a continuation. The function f gets called with
    // the (ready) future. Returns a new future with the result of
    // the invocation of f. Invalidates the future!
    template <typename F>
    auto then(F&& f);
};
```

---
## Diving into the Future - The (basic) API

Constructing a `hpx::shared_future<R>`

```
template <typename R>
class shared_future
{
    // Future constructors
    // Construct an empty future.
    shared_future();

    // Move a future to a new one
    shared_future(shared_future&& f);

    // Share ownership between two futures
    shared_future(shared_future&& f);

    // Unwrap a future. The new future becomes ready when
    // the inner, and outer future are ready.
    explicit future(shared_future<future>&& f);

    // Query the state
    // Waiting on the result
};
```

---
## Diving into the Future - The (basic) API

Waiting for the future to become ready
```
template <typename R>
class future
{
    // Future constructors
    // Query the state

    // Waiting on the result
    void wait();

    // Get the result. This function might block if the result has
    // not been computed yet.
    R const& get();

    // Attach a continuation. The function f gets called with
    // the (ready) future. Returns a new future with the result of
    // the invocation of f.
    template <typename F>
    auto then(F&& f) const;
};
```

---
## Producing Futures
### `hpx::async`

```
template <typename F, typename...Ts>
auto async(F&& f, Ts...&&ts)
 -> future<decltpype(f(std::forward<Ts>(ts)...)>;
```

* `F` is anything callable with the passed arguments (actions are callable)


```
template <typename F, typename...Ts>
auto async(launch_policy, F&& f, Ts...&&ts)
 -> future<decltpype(f(std::forward<Ts>(ts)...)>;
```

* `launch_policy` can be `async`, `sync`, `fork`, `deferred`


```
template <typename Executor typename F, typename...Ts>
auto async(Executor&&, F&& f, Ts...&&ts)
 -> future<decltpype(f(std::forward<Ts>(ts)...)>;
```

* `Executor` is a concept to be introduced [later](#executors)

---
## Producing Futures
### `hpx::lcos::local::promise`

```
hpx::lcos::local::promise<int> p;
hpx::future<int> f = p.get_future();
// f.is_ready() == false, f.get(); would lead to a deadlock
p.set_value(42);

// Print 42
std::cout << f.get() << std::endl;
```

---
## Producing Futures
### `hpx::lcos::promise`

```
hpx::lcos::promise<int> p;
hpx::future<int> f = p.get_future();
// f.is_ready() == false, f.get(); would lead to a deadlock
hpx::async(
    [](hpx::id_type promise_id)
    {
        hpx::set_lco_value(promise_id, 42);
    }
  , p.get_id());

// Print 42
std::cout << f.get() << std::endl;

```

---
## Producing Futures
### `hpx::make_ready_future`

* Producing futures that are ready at construction

```
template <typename T>
future<typename decay<T>::type> make_ready_future(T&& t);

future<void> make_ready_future();
```

---
## Producing Futures
### And beyond ...

* Futures are the main interface to express asynchrony
* Most API functions in HPX return futures
* This was just an excerpt ... let's see more!


---
## Composing Futures
### Sequential Composition

```
future<int> f1 = hpx::async(...);

// Call continuation once f1 is ready. f2 will become ready once
// the continuation has been run.
future<double> f2 = f1.then(
    [](future<int>&& f) { return f.get() + 0.0; });
```

--

* The continuation needs to take the future as parameter to allow
  for exception handling. Exceptions happening in asynchronous calls
  will get rethrown on `.get()`
* `then` accepts launch policies as well as [executors](#executors)
* `f1` will get invalidated.
--
 No invalidation:

```
shared_future<int> f1 = hpx::async(...);

// Call continuation once f1 is ready. f2 will become ready once
// the continuation has been run.
future<double> f2 = f1.then(
    [](future<int>&& f) { return f.get() + 0.0; });
```

---
## Composing Futures
### And Composition

```
future<int> f1 = hpx::async(...);
future<std::string> f2 = hpx::async(...);

auto all_f = hpx::when_all(f1, f2);

future<std::vector<float>> result = all_f.then(
    [](auto f) -> std::vector<float>
    {
        // ...
    });
```

--
* Allows for attaching continuations to more than one future
* `f1` and `f2` will get invalidated. (Use `shared_future` if you need them afterwards)
* Also accepts a `std::vector<future<T>>` or a pair of iterators

---
## Composing Futures
### Or Composition

```
std::vector<future<int>> fs = ...;

future<std::vector<future<int>>> ffs = hpx::when_any(fs).then(
    [](auto f)
    {
        auto res = f.get();
        return res.sequence;
    });
```

--
* Allows for waiting on *any* of the input futures
* Returns a `future<when_any_result<Sequence>>`:

```
template <typename Sequence>
struct when_any_result
{
   std::size_t index; // Index to a future that became ready
   Sequence futures;  // Sequence of futures
};
```


---
## Composing Futures
### Dataflow

* Shortcut to `when_all(...).then(...)`

```
future<int> f1 = hpx::async(...);
future<std::string> f2 = hpx::async(...);

future<double> f3 = hpx::dataflow(
    [](future<int>, future<std::string>) -> double
    {
        // ...
    });

```

--
* Calls the passed function whenever all arguments that were futures are ready
* Returns a future that becomes ready once the function has finished execution
* Accepts launch policies as well as [executors](#executors) as the first parameter

---
## Concepts of Parallelism
### Types of Parallelism

* Data parallelism
    * Parallel Algorithms
    * GPUs
* Task based asynchronous and continuation style parallelism
    * `future<R>`, `async`, etc...
* Instruction Level Parallelism
    * SIMD instructions

---
## Concepts of Parallelism
### Parallel Execution Properties

* The ***execution restrictions*** applicable for work items
* In what ***sequence*** work items have to be executed
* ***Where*** work items should be executed
* The ***parameters*** of the execution environment

---
## Concepts of Parallelism
### Bringin it all together
![Bringing it all together](images/parallelism-0.png)

---
## Concepts of Parallelism
### Bringin it all together
![Bringing it all together](images/parallelism-1.png)

---
## Concepts of Parallelism
### Bringin it all together
![Bringing it all together](images/parallelism-2.png)

---
## Concepts of Parallelism
### Bringin it all together
![Bringing it all together](images/parallelism-3.png)

---
## Concepts of Parallelism
### Bringin it all together
![Bringing it all together](images/parallelism-4.png)

---
## Concepts of Parallelism
### Bringin it all together
![Bringing it all together](images/parallelism-5.png)

---
## Execution Policies

---

name: executors

## Executors

---
## Data Placement

---
## Targets

---
## Example: SAXPY - The HPX Way

**Goal**: SAXPY routine with data locality

* `a[i] = b[i] * x + c[i]` for `i` from `0` to `N-1`
* Using parallel algorithms
* Explicit control over data locality
* No raw loops

---
## Example: SAXPY - The HPX Way

**Step 1**: Writing the serial version

.left-column[
```
std::vector<double> a = ...;
std::vector<double> b = ...;
std::vector<double> c = ...;
double x = ...;

std::transform(b.begin(),
  b.end(), c.begin(), a.begin(),
  [x](double bb, double cc)
  {
*   return bb * x + cc;
  }
);
```
]

.right-column[
* `bb` is `b[i]`
* `cc` is `c[i]`
* the calculated value gets written to `a[i]`
* [Complete code](https://github.com/STEllAR-GROUP/tutorials/tree/master/cscs2016/session2/saxpy/serial.cpp)
]

---
## Example: SAXPY - The HPX Way

**Step 2**: Parallelize it

.left-column[
```
std::vector<double> a = ...;
std::vector<double> b = ...;
std::vector<double> c = ...;
double x = ...;

*using hpx::parallel::par;
*hpx::parallel::transform(par
  b.begin(), b.end(), c.begin(),
  a.begin(),
  [x](double bb, double cc)
  {
    return bb * x + cc;
  }
);
```
]

.right-column[
* Replace the standard algorithm with a parallel one
* Set parallel exeuction policy
* [Complete code](https://github.com/STEllAR-GROUP/tutorials/tree/master/cscs2016/session2/saxpy/parallel.cpp)
]

---
## Example: SAXPY - The HPX Way

**Step 3**: Adding data locality

.left-column[
```
using hpx::compute::host;

typedef block_executor<>
    executor;
typedef block_allocator<double>
    allocator;

auto numa_domains = numa_nodes();
executor exec(numa_domains);
allocator alloc(numa_domains);

using hpx::compute::vector;

vector<double, executor> a = ...;
vector<double, executor> b = ...;
vector<double, executor> c = ...;
double x = ...;
```
]

.right-column[
```
*using hpx::parallel::par;
*auto policy = par.on(exec);
hpx::parallel::transform(
  policy, b.begin(),
  b.end(), c.begin(), a.begin(),
  [x](double bb, double cc)
  {
    return bb * x + cc;
  }
);
```
* Get targets for locality of data and execution
* Setup Executor and Allocator
* Run on the allocator
* [Complete code](https://github.com/STEllAR-GROUP/tutorials/tree/master/cscs2016/session2/saxpy/parallel_numa.cpp)
]

---
## Example: SAXPY - The HPX Way

**Optional Step**: Running it on the GPU

.left-column[
```
using hpx::compute::cuda;

typedef default_executor<>
    executor;
typedef allocator<double>
    allocator;

target device;
executor exec(device);
allocator alloc(device);

using hpx::compute::vector;

vector<double, executor> a = ...;
vector<double, executor> b = ...;
vector<double, executor> c = ...;
double x = ...;
```
]
.right-column[
```
*using hpx::parallel::par;
*auto policy = par.on(exec);
hpx::parallel::transform(
  policy, b.begin(),
  b.end(), c.begin(), a.begin(),
  [x] HPX_DEVICE
  (double bb, double cc)
  {
    return bb * x + cc;
  }
);
```
* Get targets for locality of data and execution
* Setup Executor and Allocator
* Run on the allocator
* [Complete code](https://github.com/STEllAR-GROUP/tutorials/tree/master/cscs2016/session2/saxpy/parallel_cuda.cpp)
* Works only for CUDA version 8 :(
]

---
class: center, middle
## Next Steps

[Building HPX and Applications](../session3)

[Running HPX Applications](../session4)
