# Node Fiber Bug Demonstration

The module illustrates an (apparent) bug with the Node.js implementation of [fibers](https://github.com/laverdet/node-fibers). Specifically, this issue surfaces when calling a Node.js C++ addon (native) method which in turn calls back into JavaScript from the context of a fiber. In this situation, as illustrated in the sample program (*fibertest.js*), both the native Node.js **Promise** implementation and **process.nextTick()** become compromised and no longer work correctly. The only work-around available is to replace the native Promise implementation with the API compatible [Bluebird](http://bluebirdjs.com/docs/getting-started.html) implementation. Also, calls to *process.nextTick()* need to be replaced with *setImmediate()*.

The issue seems to related to making a call indirectly through the C++ addon. The C++ addon merely bridges a JavaScript call to C++ and then back into JavaScript. This call-chain (Javascript -> C++ -> Javascript) executes in a fiber/coroutine.

Preliminary investigations *seem* to indicate the Node.js/V8 microtasks used to execute functions on the "next tick" and resolve the native Promise implementation fails to run in some circumstances. Specifically [*internal/process/next_tick.js:runMicrotasksCallback*](https://github.com/nodejs/node/blob/master/lib/internal/process/next_tick.js) stops being called and thus the native Promise's aren't resolved nor are process.nexTick() functions run.

## Update!

The issue demonstrated by this program has been resolved. Thanks to the efforts and insights provided by Node.js Fiber's Marcel Laverdet the problem illustrated here was in fact my fault. In my C++ addon code I was using Nan::Callback() to make the call back into JavaScript rather than the correct Nan::Call(). See [Issue #350](https://github.com/laverdet/node-fibers/issues/350#issuecomment-323204825) for more details.

## Environment

The apparent bug was tested using Node.js v6.9.2 running on a x86 64-bit Ubuntu 16.04 platform.

## Installation

```
git clone git@github.com:gschmottlach-xse/node-fiber-test.git
cd node-fiber-test
npm install
```

## Execution

The test program can be run with two options:

* **workaround** - This runs the test program with the following two work-arounds that allow the program to run correctly:
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
