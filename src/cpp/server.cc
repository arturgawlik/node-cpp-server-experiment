#include <iostream>
#include "server.h"

Server::Server(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Callback must be provided").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsFunction())
    {
        Napi::TypeError::New(env, "Callback must be a function").ThrowAsJavaScriptException();
        return;
    }

    this->_cb = Napi::Persistent(info[0].As<Napi::Function>());
}

void Server::Listen(const Napi::CallbackInfo &info)
{
    std::cout << "before cb call\n";
    this->_cb.Call(info.Env().Global(), {Napi::String::New(info.Env(), "hello from cc cb")});
    std::cout << "after cb call\n";
}

Napi::Function Server::GetClass(Napi::Env env)
{
    return DefineClass(env, "Server", {Server::InstanceMethod("listen", &Server::Listen)});
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::String name = Napi::String::New(env, "Server");
    exports.Set(name, Server::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
