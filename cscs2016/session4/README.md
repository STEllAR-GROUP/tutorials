
class: center, middle

# Running HPX Applications
## Command line options

[Overview](..)

Previous: [Building HPX](../session3)

???
[Click here to view the Presentation](https://stellar-group.github.io/tutorials/cscs2016/session4/)

---
## General

* HPX comes with a large set of options you can pass through the command line
* We will cover a few
* [Read the docs!](http://stellar-group.github.io/hpx/docs/html/hpx/manual/init.html)

---
## Overview
### Command line parameters

```INI
$ ./bin/hello_world --hpx:help

Usage: unknown HPX application [options]:

HPX options (allowed on command line only):
  --hpx:help [=arg(=minimal)]           print out program usage (default: this
                                        message), possible values: 'full'
                                        (additionally prints options from
                                        components)
  --hpx:version                         print out HPX version and copyright
                                        information
  --hpx:info                            print out HPX configuration information
  --hpx:options-file arg                specify a file containing command line
                                        options (alternatively: @filepath)

HPX options (additionally allowed in an options file):
  --hpx:run-agas-server                 run AGAS server as part of this runtime
                                        instance
  --hpx:run-hpx-main                    run the hpx_main function, regardless
                                        of locality mode

... And much more ...
```

---
## Overview
### The HPX INI Config

```INI
$ ./bin/hello_world --hpx:dump-config
Configuration after runtime start:
----------------------------------
============================
  [application]
  [hpx]
    'affinity' : 'pu'
    'bind' : 'balanced'
    'cmd_line' : './bin/hello_world --hpx:dump-config'
    'component_path' : '$[hpx.location]:$[system.executable_prefix]' -> '/apps/daint/hpx/0.9.99/gnu_530/debug:/users/heller/tutorials/examples/build/debug'
    'component_path_suffixes' : '/lib/hpx:/bin/hpx'
    'cores' : '1'
    'expect_connecting_localities' : '1'
    'finalize_wait_time' : '-1.0'
    'first_pu' : '0'

... And much more ...
```
* Can be set with `-I...` or `--hpx:option=...`
	* Example: `-Ihpx.bind=compact`

---
## Controlling CPU binding
### General

* Use `--hpx:print-bind` to show the selected bindings
* Use `--hpx:threads=N` to select the number of threads per locality
* Use `--hpx:cores=N` to select the number of cores

---
## Controlling CPU binding
### Binding the HPX worker threads to specific CPU Cores


---
## Distributed Runs
### General

---
## Distributed Runs
### Selecting parcelports

---
## Distributed Runs
### `hpxrun.py`

---
## Batch environments
### General

---
## Batch environments
### SLURM

---
## Batch environments
### MPI

---
## Debugging options

---
## Performance Counters

---
## Adding your own options
### Using Boost.ProgramOptions

---
## Adding your own options
### Using The HPX INI Config
