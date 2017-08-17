#ifndef STACK_HPP
#define STACK_HPP

#include "nan.h"

class Stack : public Nan::ObjectWrap
{
//-------------------------------------------
// Attributes
//-------------------------------------------
private:
    static Nan::Persistent<v8::Function>    ms_constructor;
    Nan::Persistent<v8::Object>             m_pal;

//-------------------------------------------
// Methods
//-------------------------------------------
public:
    static void Init(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target);
    static NAN_METHOD(New);

    /* Function wrappers */
    static NAN_METHOD(ReadData);

private:
    Stack();
    ~Stack();
};

#endif  /* STACK_HPP */
