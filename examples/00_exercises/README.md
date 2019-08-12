# HPX exercises

These exercises are intended to go with the course slides (HLRS2019 and
CSCS2019), but can be done without the course as well with some additional
reading.

The exercises increase in complexity (but not necessarily difficulty).

- 1-3 cover basic task-based programming constructs in HPX
- 4-5 cover basic data-parallelism in HPX
- 6 is a more advanced recursive parallelism exercise
- 7-8 cover basic distributed computing in HPX
- 9 extends data parallelism to the GPU

Some exercises do not compile at all, some build but fail at runtime, and some
build correctly but need to parallelized or modified. See the comments in the
source to see which type of exercise you are dealing with.

All exercises and solutions can be built with the target `tutorial.exercises`
(e.g. `make tutorial.exercises`). All exercises can be built with the target
`tutorial.exercises.exercises`, and all solutions with
`tutorial.exercises.solutions`. Individual exercises and solutions can be built
with the targets `exercisesN` and `solutionN` with `N` replaced by a suitable
number.
