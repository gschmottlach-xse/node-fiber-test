#include "stack.hpp"

/*======================= D A T A  ================================*/

Nan::Persistent<v8::Function> Stack::ms_constructor;

/*======================= C O D E =================================*/

Stack::Stack()
    :
    Nan::ObjectWrap()
{
}

Stack::~Stack() {
    m_pal.Reset();
}

NAN_METHOD(Stack::New)
{
    if ( info.IsConstructCall() )
    {
        if ( info.Length() < 1 )
        {
            return Nan::ThrowSyntaxError("Expected platform abstraction layer (PAL)");
        }

        if ( !info[0]->IsObject() ) {
            return Nan::ThrowTypeError("Expected platform abstraction layer (PAL)");
        }

        // Invoked as a constructor: 'new Context(...)'
        Stack* stack = new Stack();
        if ( NULL == stack )
        {
            return Nan::ThrowSyntaxError("Failed to allocate stack");
        }
        stack->Wrap(info.This());

        stack->m_pal.Reset(info[0]->ToObject());
        info.GetReturnValue().Set(info.This());
    }
    // Invoked as a plain 'function Stack(...)' so turn it into a constructor
    // call
    else
    {
        v8::Local<v8::Value> argv[1] = {info[0]};
        Nan::MaybeLocal<v8::Function> cons =
                                        Nan::New<v8::Function>(ms_constructor);
        info.GetReturnValue().Set(Nan::NewInstance(cons.ToLocalChecked(),
                                        info.Length(), argv).ToLocalChecked());
    }
}

NAN_METHOD(Stack::ReadData)
{
    Stack* self;
    v8::Local<v8::Function> readDataFunc;
    v8::Local<v8::Value>    argValue;

    if ( info.Length() < 2 )
    {
        return Nan::ThrowSyntaxError("Expected two arguments");
    }

    if ( !info[0]->IsNumber() )
    {
        return Nan::ThrowTypeError("Expected a msec delay");
    }

    if ( !info[1]->IsNumber() )
    {
        return Nan::ThrowTypeError("Expected number of bytes to allocate");
    }

    self = Nan::ObjectWrap::Unwrap<Stack>(info.This());

    v8::Local<v8::Object> pal = Nan::New(self->m_pal);
    argValue = pal->Get(v8::String::NewFromUtf8(info.GetIsolate(), "readData"));
    readDataFunc = v8::Local<v8::Function>::Cast(argValue);
    if ( !readDataFunc->IsCallable() )
    {
        return Nan::ThrowError("PAL function 'readData' is not callable");
    }

    const int argc = 2;
    v8::Local<v8::Value> argv[argc] = {info[0], info[1]};
    Nan::Callback callback(readDataFunc);
    v8::Local<v8::Value> result = callback.Call(readDataFunc, argc, argv);
    return info.GetReturnValue().Set(result);
}

void
Stack::Init
    (
    Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target
    )
{
    Nan::HandleScope scope;
    v8::Local<v8::FunctionTemplate> tpl =
                                Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("Stack").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "readData", ReadData);

    ms_constructor.Reset(tpl->GetFunction());
    target->Set(Nan::New("Stack").ToLocalChecked(), tpl->GetFunction());
}
