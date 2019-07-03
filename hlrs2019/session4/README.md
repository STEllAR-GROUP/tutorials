
class: center, middle

# Exercises

[Overview](..)

Previous: [Building and Running HPX](../session3)

???
[Click here to view the Presentation](https://stellar-group.github.io/tutorials/hlrs2019/session4/)

---
## Goals

* (Optional: build HPX)
* Configure tutorial examples with HPX
* (Optional: configure your own project with HPX)
* Very basic to intermediate exercises
* Ask all the questions you didn't get to ask yet

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
    * Queue for today: R_course95
    * Queue for tomorrow: R_course96

---
## Build tutorial examples (on Hazelhen)
```sh
# get tutorial material, https://github.com/STEllAR-GROUP/tutorials
cp -r ~/tutorials ~/personal/space/tutorials

# create a build dir
mkdir build
cd build

# Environment already set up in ~/.bashrc

# for debug: ~/hpx/build/debug/environment.sh
# for profiling with APEX: ~/hpx/build/profiling-apex/environment.sh
source ~/hpx/build/release/environment.sh

#  CMake with examples path (debug: -DCMAKE_BUILD_TYPE=Debug/RelWithDebInfo)
cmake -DCMAKE_BUILD_TYPE=Release ../tutorials/examples

# make the demos
make -j4
```

---
## Build tutorial on laptop etc

* Clone tutorial repo as before

* Create build dir

* invoke CMake with PATH to HPX (build tree or install tree)

```sh
cmake -DHPX_DIR=${path_to_hpx_install} ${path_to_tutorials}/examples
cmake -DHPX_DIR=${path_to_hpx_build}   ${path_to_tutorials}/examples
```
* note that for a build tree you might want

```sh
cmake -DHPX_DIR=${path_to_hpx_build}/lib/cmake/HPX
  ${path_to_tutorials}/examples
```

* ...and set a build going

```sh
make -j4
```

---
## Exercises

* Exercises are in the tutorials repo https://github.com/STEllAR-GROUP/tutorials/tree/master/examples
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
