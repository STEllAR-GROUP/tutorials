
class: center, middle

# Exercises

[Overview](..)

Previous: [Building and Running HPX](../session3)

???
[Click here to view the Presentation](https://stellar-group.github.io/tutorials/cscs2019/session4/)

---
## Goals

* (Optional: build HPX)
* Configure tutorial examples with HPX
* (Optional: configure your own project with HPX)
* Very basic to intermediate exercises
* Ask all the questions you didn't get to ask yet

---
## Using Piz Daint for the course

TODO

---
## Build tutorial examples on Piz Daint
```sh
# TODO: Check setup on Piz Daint
cp -r ~/tutorials ~/personal/space/tutorials

# create a build dir
mkdir build
cd build

source ~/hpx/build/release/environment.sh

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
```

* ...and set a build going

```sh
make -j4
```

---
## Exercises

* Exercises are in the tutorials repo
  https://github.com/STEllAR-GROUP/tutorials/tree/master/examples
* `cmake [-DHPX_ROOT=/path/to/hpx] /path/to/tutorials/examples`
* `make tutorials.exercises.exercises` builds exercises
    * some fail to build, fix them
    * some are not parallelized, do so using HPX
* `make tutorials.exercises.solutions` builds solutions
* When you're done with the exercises: try to implement your own program with HPX
    * something you like
    * real application
* Full 2D stencil example tomorrow and much more

---
## Summary for afternoon

* Exercises: https://github.com/STEllAR-GROUP/tutorials/tree/master/examples
* `module load HPX` (TODO: check that this works)
* Create build directory and run: `cmake -DCMAKE_BUILD_TYPE=Release path/to/examples`
* Build all exercises: `make tutorials.exercises.exercises`
* Build all solutions: `make tutorials.exercises.solutions`
* Build individual exercise/solution: `make solution3`, `make exercise7`

---
class: center, middle
## Tomorrow

[Worked 2D Stencil Example - From Serial to Distributed](../session5)
