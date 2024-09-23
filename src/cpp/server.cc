#include <iostream>
#include <uv.h>

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

    uv_loop_t *eventLoop;
    napi_status status = napi_get_uv_event_loop(env, &eventLoop);
    NAPI_THROW_IF_FAILED(env, status);

    this->_cb = Napi::Persistent(info[0].As<Napi::Function>());
}

void on_new_connection(uv_stream_t *server, int status)
{
    uv_cb_data *data = (uv_cb_data *)server->data;
    Napi::HandleScope scope(data->cb->Env());
    data->cb->MakeCallback(data->cb->Env().Global(), {Napi::String::New(data->cb->Env(), "new connection arrived")});
    // uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    // uv_tcp_init(eventLoop, client);
    // if (uv_accept(server, (uv_stream_t *)client) == 0)
    // {
    //     uv_read_start((uv_stream_t *)client, alloc_buffer, echo_read);
    // }
}

void Server::Listen(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::ObjectReference global = Napi::Persistent(env.Global());

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

    uv_loop_t *eventLoop;
    napi_status status = napi_get_uv_event_loop(env, &eventLoop);
    NAPI_THROW_IF_FAILED(env, status);

    uv_tcp_t server;
    uv_tcp_init(eventLoop, &server);

    sockaddr_in address;
    uv_ip4_addr("0.0.0.0", port.Int32Value(), &address);

    uv_tcp_bind(&server, (const struct sockaddr *)&address, 0);

    uv_cb_data *data = (uv_cb_data *)malloc(sizeof(uv_cb_data));
    data->cb = &this->_cb;
    server.data = data;

    int backlog = 1; // maxiumum length of queded connections
    int r = uv_listen((uv_stream_t *)&server, backlog, on_new_connection);
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
