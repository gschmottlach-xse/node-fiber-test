'use strict';

//
// Set to "true" to employ a workaround using Bluebird.js Promise
// and setImmediate() instead of process.nextTick().
//
var workaround = false;

//
// Set to 'true' to call the generateData() function via a
// Node.js C++ addon.
//
var callAddon = true;

const synccore = require('bindings')('synccore');
const Fiber = require('fibers');
const util = require('util');
const bluebird = require('bluebird');

//
// Global data
//
var stack;
var pal;
var data;
var Promise;
var progArgs = process.argv.slice(2);

while ( progArgs.length > 0 ) {
    switch ( progArgs[0] ) {
        case 'workaround':
            workaround = true;
            break;
        case 'noaddon':
            callAddon = false;
            break;
        default:
            break;
    }
    progArgs.shift();
}

if ( !workaround ) {
    // The Node.js built-in Promise does not work with this example
    Promise = global.Promise;
}
else {
    // Only the Bluebird Promise implementation will work here
    Promise = bluebird.Promise;
}


//
// This code is borrowed directly from synchronize.js (https://github.com/alexeypetrushin/synchronize)
//
function defer() {
    var fiber = Fiber.current;
    // if(fiber._defered) throw new Error("invalid usage, should be clear previous defer!")
    // fiber._defered = true
    // Prevent recursive call
    var called = false;

    if (!fiber) {
        throw new Error("no current Fiber, defer can't be used without Fiber!");
    }

    // Returning asynchronous callback.
    return function(err, result) {
        if (called) {
            throw new Error("defer can't be used twice!");
        }
        called = true;

        // Wrapping in setImmediate as a safe measure against not asynchronous usage.
        setImmediate(function() {
            // fiber._defered = false
            if (fiber._syncIsTerminated) {
                return;
            }
            if (err) {
                // Resuming fiber and throwing error.
                fiber.throwInto(err);
            }
            else {
                // Resuming fiber and returning result.
                fiber.run(result);
            }
        });
    };
}


//
// This function is called by the C++ Addon
//
function generateData(delay, nBytes, callback) {
    function doIt() {
        return new Promise(function(resolve /*, reject*/) {
            function allocBuf() {
                var buf = Buffer.alloc(nBytes, 'h');
                console.error('Resolving buffer from Promise');
                resolve(buf);
            }
            setTimeout(function() {
                if ( !workaround ) {
                    process.nextTick(allocBuf);
                }
                else {
                    setImmediate(allocBuf);
                }
            }, delay);
        });
    }

    return doIt()
            .then(function(buf) {
                console.error('Received resolved buffer');
                callback(null, buf);
            })
            .catch(function(reason) {
                console.error('Caught an error: ' + reason);
            });
}


//
// This object is passed to the Node.js C++ addon and a reference to it is
// persistenly stored by the addon.
//
pal = {
    readData: function(delay, nBytes) {
        var rc;
        try {
            rc = Fiber.yield(generateData(delay, nBytes, defer()));
            console.error('We got the buffer');
        }
        catch (err) {
            console.error('ReadData caught: ' + err);
        }
        return rc;
    }
};


//
// Start of the program
//

function main() {
    if ( workaround ) {
        console.error('The program will run correctly');
    }
    else if ( callAddon ) {
        console.error('The program will NOT run correctly.');
    }

    stack = new synccore.Stack(pal);
    Fiber(function() {
        if ( callAddon ) {
            data = stack.readData(1000, 5);
        }
        else {
            data = pal.readData(1000, 5);
        }
        console.log('Data read: ' + util.inspect(data));
    }).run();
}

try {
    main();
}
catch ( err ) {
    console.error(err);
}
