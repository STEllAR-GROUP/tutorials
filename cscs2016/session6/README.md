
class: center, middle

# Resource Management and Performance Issues
## Keeping it under control

[Overview](..)

Previous: [Worked 2D Stencil Example](../session5)

???

---
##When are tasks created
Continuations and .then constructs inside continuations

---
##Launch policies : sync
* A continuation attached to a future using sync policy does not need to
create a task.

* The task reduces to a function invocation on the same thread as
the future that becomes ready

* Why not use it all the time?
    * granularity of tasks
    * stack size

* It's ideal for small and quick tasks

---
## Launch policies : fork

* The task that spawns the child task will suspend and the child task
runs in its place

* The parent task goes back to the pending work queue

* This is a reversal
* For very resource hungry applications

launch::fork


---
##Move semantics

---
##callbacks on async_cb

---
##sliding semaphore

---
##direct actions and plain actions

---

class: center, middle
## Next

 [Debugging and Profiling](../session6)
