# Task based Programming with HPX Workshop

[Event Details](http://www.cscs.ch/events/event_detail/index.html?tx_seminars_pi1%5BshowUid%5D=167)
### Location/ Date
CSCS in Lugano, Switzerland

Thursday, October 5 and Friday, October 6, 2017

### Presenters
* **John Biddiscombe**, Swiss National Supercomputing Centre (CSCS)
* **Thomas Heller**, Computer Architecture, Friedrich-Alexander-University Erlangen-Nuremberg

[https://stellar-group.github.io/tutorials/cscs2017](Presentation)<br/>
[https://github.com/STEllAR-GROUP/tutorials](https://github.com/STEllAR-GROUP/tutorials)

---

# Thursday, October 5, 2017

* [9:00 to 10:30: Introduction to HPX - Part 1 (overview)](session1)
* Tea Break, 15 Minutes
* [10:45 to 12:15: Introduction to HPX - Part 2 (API)](session2)
* Lunch, 1 hour
* [13:15 to xx:xx: Building HPX - CMake Options and Dependencies](session3)
* [xx:xx to 14:45: Hello World! - Options and Running Applications](session4)
* Tea Break, 15 Minutes
* [15:00 to 16:40: Worked 2D Stencil Example - From Serial to Distributed](session5)

# Friday, October 6, 2017

* Tea Break, 15 Minutes
* [9:00 to 10:30: Resource Management and Performance Issues](session6)
* [10:45 to 12:15: Thomas's special session](sessionx)
* Lunch, 1 hour
* [13:15 to 14:45: Debugging HPX Applications](session7)
* Tea Break, 15 Minutes
* [15:00 to 16:30: Open for worked examples](session8)

---
## Acknowledgements

* Hartmut Kaiser (LSU)
* Bryce Lelbach (LBNL)
* Agustin Berge
* Patrick Diehl (Bonn)
* Matrin Stumpf (FAU)
* Arne Hendricks (FAU)
* Parsa Amini (LSU)
* And many others...

---
## Course setup

**Wifi** : Please see handouts, use eduroam if possible

**Login** : Logins for daint on handout sheet
```sh
ssh ela.cscs.ch
ssh daint```

**Reservation** : Course longs can use
```sh
scontrol show res hpx1
scontrol show res hpx2
```

**Tutorial repo** : Please login to daint and clone the tutorial repo there
```sh
git clone https://github.com/STEllAR-GROUP/tutorials```

**Shell setup** : For building examples we will be using clang compiler on daint and
you will need a module for HPX
```sh
source /apps/daint/UES/6.0.UP04/HPX/clang-setup.sh
module load /apps/daint/UES/6.0.UP04/HPX/hpx-clang```

