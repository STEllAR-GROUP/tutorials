
class: center, middle

# Debugging And Profiling

[Overview](..)

Previous: [Resource Management and Performance Issues](../session6)

---
## Topics covered

* Debugging Compile Time Errors
* Debugging Runtime Errors
* Performance Problems

---
## Revisiting the Stencil
### How good does it scale?

* Weak Scaling: Using 30000x30000 grid points per locality

![2D Stencil - Weak Scaling](images/weak_scaling0.png)

* Strong Scaling: Using 30000x960000 total grid points

![2D Stencil - Strong Scaling](images/strong_scaling0.png)

---
## Revisiting the Stencil
### Improvements

* [The importance of Oversubscription]()
* [Having more than one Partition per Locality]()
* [Futurization - Waiting is losing]()

---
## Revisiting the Stencil
### How good does it scale now?

* Weak Scaling: Using 30000x30000 grid points per locality

![2D Stencil - Weak Scaling](images/weak_scaling1.png)

* Strong Scaling: Using 30000x960000 total grid points

![2D Stencil - Strong Scaling](images/strong_scaling1.png)

---
class: center, middle
## Next

[Open for worked examples](../session8)

