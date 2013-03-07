#ifndef ERROR_HPP
#define ERROR_HPP

#include <stdexcept>
#include <amqp.h>

class AmqpRpcError: public std::runtime_error
{
public:
    AmqpRpcError(const std::string& context,  const amqp_rpc_reply_t& reply);

    const amqp_rpc_reply_t& reply() const
    {
        return reply_;
    }

private:
    amqp_rpc_reply_t reply_;
};

void check(const char* context, int rc, bool nothrow = false);
void check_rpc(const char* context, const amqp_rpc_reply_t& reply,
               bool nothrow = false);


#endif // ERROR_HPP
