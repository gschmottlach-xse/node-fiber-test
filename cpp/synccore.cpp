#ifndef SYNCCORE_HPP
#define SYNCCORE_HPP

#include "stack.hpp"

NAN_MODULE_INIT(initAll)
{
    Nan::HandleScope scope;

    // Initialize the methods associated the fake Stack
    Stack::Init(target);
}

NODE_MODULE(synccore, initAll)

#endif  /* SYNCCORE_HPP */
