# Node Fiber Bug Demonstration

The module illustrates an (apparent) bug with the Node.js implementation of [fibers](https://github.com/laverdet/node-fibers). Specifically, this issue appears when calling a Node.js C++ addon (native) method which in turn calls back into Javascript from the context of a fiber. In this situation, as illustrated in the sample program (*fibertest.js*), both the native Node.js **Promise** implementation and **process.nextTick()** become comprised and no longer work correctly. The only work-around available is to replace the native Promise implementation with the compatible [Bluebird](http://bluebirdjs.com/docs/getting-started.html) implementation. Also, calls to *process.nextTick()* need to be replaced with *setImmediate()*.

## Environment

The apparent bugs was tested using Node.js v6.9.2 running on a Ubuntu 16.04 platform.

## Installation

```
git clone git@github.com:gschmottlach-xse/node-fiber-test.git
cd node-fiber-test
npm install
```

## Execution

The test program can be run with two options:

* **workaround** - This runs the test program with the following two work-arounds that allows the program to run correctly:
    1) Using *BlueBird's* Promise implementation.
    2) Substituting calls to *process.nextTick()* with *setImmediate()*.

* **noaddon** - This runs the test by calling the *generateData()* function directly rather than having the Node.js C++ addon call it indirectly. In both instances the *generateData()* function is called within the context of a fiber. If the function is called directly then it will work correctly with/without the work-around being applied.

With no arguments, the test program will demonstrate the bug and run **incorrectly**:
```
# node fibertest.js showbug
The program will NOT run correctly
```

Running the test program with the following arguments will result in a **successful** execution.

```
# node fibertest.js workaround
The program will run correctly.
Resolving buffer from Promise
Received resolved buffer
We got the buffer
Data read: <Buffer 68 68 68 68 68>

```

or

```
# node fibertest.js noaddon
The program will run correctly.
Resolving buffer from Promise
Received resolved buffer
We got the buffer
Data read: <Buffer 68 68 68 68 68>
```

or

```
# node fibertest.js workaround noaddon
The program will run correctly.
Resolving buffer from Promise
Received resolved buffer
We got the buffer
Data read: <Buffer 68 68 68 68 68>
```
