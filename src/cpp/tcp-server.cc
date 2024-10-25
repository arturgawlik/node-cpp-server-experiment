#include <iostream>
#include <uv.h>

#include "tcp-server.h"

TcpServer::TcpServer(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Callbacks must be provided").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsFunction() || !info[1].IsFunction())
    {
        Napi::TypeError::New(env, "Callback must be a function").ThrowAsJavaScriptException();
        return;
    }

    this->_cb = Napi::Persistent(info[0].As<Napi::Function>());
    this->_errorCb = Napi::Persistent(info[1].As<Napi::Function>());

    uv_loop_t *eventLoop;
    napi_status status = napi_get_uv_event_loop(env, &eventLoop);
    NAPI_THROW_IF_FAILED(env, status);

    this->_loop = eventLoop;
}

void emit_uv_error(Napi::FunctionReference *callback, int result)
{
    Napi::Env env = callback->Env();
    Napi::Object err = Napi::Object::New(env);
    err.Set("code", Napi::String::New(env, uv_err_name(result)));
    callback->MakeCallback(env.Global(), {env.Undefined(), env.Undefined(), err});
}

void on_close(uv_handle_t *handle)
{
    std::cout << "on_close called";
}

void on_new_connection(uv_stream_t *server, int status)
{
    uv_cb_data *serverData = (uv_cb_data *)server->data;
    Napi::FunctionReference *jsCallback = serverData->cb;
    uv_loop_t *loop = serverData->event_loop;

    Napi::HandleScope scope(jsCallback->Env());

    jsCallback->MakeCallback(jsCallback->Env().Global(), {Napi::String::New(jsCallback->Env(), "new connection arrived")});

    uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    int tcpInitRes = uv_tcp_init(uv_default_loop(), client);

    int tcpAcceptRes = uv_accept(server, (uv_stream_t *)client);
}

void TcpServer::Listen(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    uv_loop_t *loop = this->_loop;

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Port must be provided").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Port must be a number").ThrowAsJavaScriptException();
        return;
    }

    Napi::Number port = info[0].As<Napi::Number>();

    int result = 0;
    this->_server = new uv_tcp_t();

    std::cout << "before uv_tcp_init\n";
    result = uv_tcp_init(loop, this->_server);
    if (result != 0)
    {
        emit_uv_error(&this->_errorCb, result);
        return;
    }
    std::cout << "after uv_tcp_init\n";

    sockaddr_in address;
    std::cout << "before uv_ip4_addr\n";
    result = uv_ip4_addr("0.0.0.0", port.Int32Value(), &address);
    if (result != 0)
    {
        emit_uv_error(&this->_errorCb, result);
        return;
    }
    std::cout << "after uv_ip4_addr\n";

    std::cout << "before uv_tcp_bind\n";
    result = uv_tcp_bind(this->_server, (const struct sockaddr *)&address, 0);
    if (result != 0)
    {
        emit_uv_error(&this->_errorCb, result);
        return;
    }
    std::cout << "after uv_tcp_bind\n";

    uv_cb_data *data = (uv_cb_data *)malloc(sizeof(uv_cb_data));
    data->cb = &this->_cb;
    data->errorCb = &this->_errorCb;
    data->event_loop = loop;
    this->_server->data = data;

    int backlog = 511; // maxiumum length of queued connections
    std::cout << "before uv_listen\n";
    result = uv_listen((uv_stream_t *)this->_server, backlog, on_new_connection);
    if (result != 0)
    {
        emit_uv_error(&this->_errorCb, result);
        return;
    }
    std::cout << "after uv_listen\n";
}

Napi::Function TcpServer::GetClass(Napi::Env env)
{
    return DefineClass(env, "TcpServer", {TcpServer::InstanceMethod("listen", &TcpServer::Listen)});
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::String name = Napi::String::New(env, "TcpServer");
    exports.Set(name, TcpServer::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
