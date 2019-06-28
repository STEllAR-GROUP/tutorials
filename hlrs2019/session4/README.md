
class: center, middle

# Exercises

[Overview](..)

Previous: [Building and Running HPX](../session3)

???
[Click here to view the Presentation](https://stellar-group.github.io/tutorials/hlrs2019/session4/)

---
## Using Hazelhen for the course

```sh
ssh -X rzvmpi23@hazelhen.hww.de
```

* Directory structure:
    * $HOME/hpx/
    * $HOME/group[01-17]

* Hazelhen documentation:
    * https://wickie.hlrs.de/platforms/index.php/Cray_XC40
    * https://wickie.hlrs.de/platforms/index.php/CRAY_XC40_Using_the_Batch_System
* Submitting jobs:
    * qsub -I -X -lnodes=1:ppn=24,walltime=1:00:00 -q R_courseXX
    * Queue for today: R_course96
    * Queue for tomorrow: R_course97

---
## Goals

* (Optional: build HPX)
* Configure project with HPX
* Very basic to intermediate exercises (fill in the gaps)

---
## Exercises

* Exercises are in the tutorials repo https://stellar-group.github.io/tutorials/examples/exercises
* `cmake [-DHPX_ROOT=/path/to/hpx] /path/to/tutorials/examples`
* `make tutorials.exercises.exercises` builds exercises
    * most fail to build, fix them
    * some are not parallelized, do so using HPX
* `make tutorials.exercises.solutios` builds solutions
* When you're done with the exercises: try to implement your own program with HPX
    * something you like
    * real application
* Full 2D stencil example tomorrow and much more

---
class: center, middle
## Tomorrow

[Worked 2D Stencil Example - From Serial to Distributed](../session5)
