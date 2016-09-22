
class: center, middle

# Introduction to HPX
## Part 1
## Overview of the HPX Framework/Runtime

---

# Topics to cover
### What are lightweight threads
* and how are they different from normal threads

### What do we mean by a "Runtime system/framework"
* the dividing line between OS and application

### How do you write/design "Task based" programs
* it requires rethinking of some codes
  
### Where does HPX stop and your code begin
* how deep into the internals should you go
 
#### Why is the HPX code repo so big and complicated 
* and how do you find our way around it

---
## What about lightweight threads?
* It's the job of the operating system to manage 'hardware level' or 'kernel level' OS threads.

    * OS threads are expensive to create/destroy 
    * OS threads take a timeslice of CPU time 
    * Creating too many of them can hamper performance

* On Startup HPX creates one 'worker' thread per core

    * On each hardware thread, HPX runs its own Task Scheduler.

* HPX (Threads)/Tasks are executed on the HPX worker thread

    * which is really an OS thread
    * (you must be careful not to block the underlying thread - if you do, 
    no tasks can run on it)
    * each HPX 'task' is referred to as a _lightweight thread_ 
    
---
##  Advantage of lightweight threads
* The philosophy behind lightweight threads is to switch from one task to another
as quickly as possible as soon as anything
 
    * finishes 
    * needs to wait (suspend) 
    
* Suspended tasks   

    * Many tasks can run on the same worker thread (one after another)
    * or intermixed as one task suspends and another resumes 

* HPX does not ever interrupt your task directly

    * the OS worker thread may be suspended as it loses its time slice
    * the HPX task running at the time is therefore suspended
    * other (dependent) tasks might in turn suspend 
    
* Many small tasks are the key to sucess
    
    * with minimal dependencies on other tasks (if possible)
    * not too small (overheads of management), not too big (starvation of worker threads)
    * enough tasks in the queue helps ensure that no CPU cycles are wasted   

---
## HPX runtime schematic
<div class="crop">
    <img src="images/threads-schedulers.jpg" alt="" width="200" height="600">
</div>

---
## HPX runtime
* Similar to OpenMP/TBB, but ...

    * OpenMP has parallel regions where a thread pool exeutes your loops/tasks
    * outside those regions, code runs as usual (on normal 'OS threads')
    
* With HPX, the runtime is always active

    * the runtime is started on program startup     
    * it stays active until program termination
    * there are no parallel regions
    * everything is running on an HPX thread
    * everything is part of a task
    
* Disclaimer

    * you can manually start/stop the runtime (if you really want to)

---
## HPX is a (only a) library
* It's implemented as a C++ library/framework

    * provides threads/futures/locks/schedulers/etc
    * allows the user to ignore the threads/schedulers/locks
    * gives the user control over them too
    
* The user doesn't manage threads directly

    * user creates tasks
    * synchronizes between/amongst them
    * shouldn't need to worry about low level synchronization primitives
    * (but sometimes needs to)
    
* The runtime does it best to schedule tasks

    * but it can be abused easily
    * either by accident (careless programming)
    * or deliberately 
    
* A good understanding of how it all works really helps          
---
## More stuff
---
class: center, middle
## Next 

[Introduction to HPX : Part 2](../session2)
