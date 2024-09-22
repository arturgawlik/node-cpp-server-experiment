#include <napi.h>

class Server : public Napi::ObjectWrap<Server>
{
public:
    Server(const Napi::CallbackInfo &);
    void Listen(const Napi::CallbackInfo &);

    static Napi::Function GetClass(Napi::Env);

private:
    std::int32_t _port;
    Napi::FunctionReference _cb;
};
