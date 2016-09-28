
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

![The Solution we look for](images/solution.png)

* Running a 5 point stencil on each element:
    * `new(x, y) = 0.25 * (old(x-1, y) + old(x+1, y) + old(x, y-1) + old(x, y+1)) - old(x, y)
