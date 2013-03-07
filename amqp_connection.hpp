#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <boost/noncopyable.hpp>
#include <amqp.h>
#include "error.hpp"

class ConnectionState: boost::noncopyable
{
public:
    ConnectionState():
        state_(amqp_new_connection())
    {}

    operator amqp_connection_state_t()
    {
        return state_;
    }

    ~ConnectionState()
    {
        const int rc = amqp_destroy_connection(state_);
        check("Ending connection", rc, true);
    }

private:
    amqp_connection_state_t state_;
};

class AmqpConnection: boost::noncopyable
{
public:
    explicit AmqpConnection(const std::string& host = "localhost", int port = 5672);

    operator amqp_connection_state_t()
    {
        return state_;
    }

    void check_rpc(const char* context, bool nothrow = false)
    {
        const amqp_rpc_reply_t reply = amqp_get_rpc_reply(state_);
        ::check_rpc(context, reply, nothrow);
    }

    int wait_frame(amqp_frame_t& frame)
    {
        return amqp_simple_wait_frame(state_, &frame);
    }

    void release_buffers()
    {
        amqp_maybe_release_buffers(state_);
    }

    ~AmqpConnection()
    {
        const amqp_rpc_reply_t reply =
                amqp_connection_close(state_, AMQP_REPLY_SUCCESS);
        ::check_rpc("Closing connection", reply, true);
    }

private:
    ConnectionState state_;
};

#endif // CONNECTION_HPP
