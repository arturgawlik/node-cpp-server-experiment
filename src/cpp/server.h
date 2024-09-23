#include <napi.h>

class Server : public Napi::ObjectWrap<Server>
{
public:
    Server(const Napi::CallbackInfo &);
    void Listen(const Napi::CallbackInfo &);

    static Napi::Function GetClass(Napi::Env);

private:
    Napi::FunctionReference _cb;
};

struct uv_cb_data
{
    Napi::FunctionReference *cb;
    uv_loop_s *event_loop;
};
