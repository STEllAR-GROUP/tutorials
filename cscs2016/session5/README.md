
class: center, middle

# Worked 2D Stencil Example
## From Serial to Distributed

[Overview](..)

Previous: [Hello World! - Options and Running Applications](../session4)

???
[Click here to view the Presentation](https://stellar-group.github.io/tutorials/cscs2016/session5/)

---
## The Problem we want to solve
### "Solving" a PDE on a 2D Grid

* DISCLAIMER: This has no real physics or mathematical background. Left as an excercise for the reader ;)

.left-column[
![The Solution we look for](images/solution.png)
]

.right-column[
* Running a 5 point stencil on each element:
    * `new(x, y) = 0.25 * (old(x-1, y) + old(x+1, y) + old(x, y-1) + old(x, y+1)) - old(x, y)`
* Boundaries are one
* High-Level Modern C++
]

---
## Modeling the Stencil in C++
### Memory structure

* Goal: Store Grid as `std::vector<double>`

![Grid](images/grid.png)

---
## Modeling the Stencil in C++
### Memory structure

* Goal: Store Grid as `std::vector<double>`

![Grid Row Access](images/grid_row_access.png)

---
## Modeling the Stencil in C++
### Memory structure

* Goal: Store Grid as `std::vector<double>`

![Grid Element Access](images/grid_element_access.png)

---
## Modeling the Stencil in C++
### Iterating the Grid

---
## Modeling the Stencil in C++
### Putting it all together

---
## Modeling the Stencil in C++ with HPX
### Adding shared memory parallelism

---
## Modeling the Stencil in C++ with HPX
### Adding NUMA awareness

---
## Modeling the Stencil in C++ with HPX
### Going distributed

---
## Detour: Global Objects in HPX
### Writing Components

---
## The Channel LCO
### Synchronization between Threads of Control

---
## Symbolic names
### Resolving Global Objects by name

---
## Parallelization
### Communication between Partitions

---
## Modeling the Stencil in C++ with HPX
### First Distributed Version

---
## Modeling the Stencil in C++ with HPX
### The importance of Oversubscription

---
## Modeling the Stencil in C++ with HPX
### Having more than one Partition per Locality

---
## Modeling the Stencil in C++ with HPX
### Futurization - Waiting is losing


---
class: center, middle
## Next

[Resource managment - Keeping things under control](../session6)

