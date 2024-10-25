#include <napi.h>

class TcpServer : public Napi::ObjectWrap<TcpServer>
{
public:
    TcpServer(const Napi::CallbackInfo &);
    void Listen(const Napi::CallbackInfo &);

    static Napi::Function GetClass(Napi::Env);

private:
    Napi::FunctionReference _cb;
    Napi::FunctionReference _errorCb;
    uv_loop_t *_loop;
    uv_tcp_t *_server;
};

struct uv_cb_data
{
    Napi::FunctionReference *cb;
    Napi::FunctionReference *errorCb;
    uv_loop_s *event_loop;
};
