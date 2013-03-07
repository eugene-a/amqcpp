#include "error.hpp"
#include <iostream>
#include <sstream>
#include <boost/scope_exit.hpp>

namespace
{
    template<typename T> const char* entity_name();

    template<>
    const char* entity_name<amqp_connection_close_t>()
    {
        return "connection";
    }

    template<>
    const char* entity_name<amqp_channel_close_t>()
    {
        return "channel";
    }

    struct CloseReturn
    {
        CloseReturn(const char* name):
            entity_name(name)
        {}

        const char* entity_name;
        uint16_t reply_code;
        amqp_bytes_t reply_text;
    };

    template<typename T> CloseReturn close_return(const void* decoded)
    {
        CloseReturn close_return(entity_name<T>());
        const T* close_reply = static_cast<const T*>(decoded);
        close_return.reply_code = close_reply->reply_code;
        close_return.reply_text = close_reply->reply_text;
        return close_return;
    }

    inline CloseReturn close_connection_return(const void* decoded)
    {
        return close_return<amqp_connection_close_t>(decoded);
    }

    inline CloseReturn close_channel_return(const void* decoded)
    {
        return close_return<amqp_channel_close_t>(decoded);
    }

    std::string error_message(const CloseReturn& close_return)
    {
        std::ostringstream stream;
        stream << "server " << close_return.entity_name << " connection error";
        stream << close_return.reply_code << ", message: ";

        const amqp_bytes_t& text = close_return.reply_text;
        stream.write(static_cast<char*>(text.bytes), text.len);
        return stream.str();
    }

    inline std::string error_message(const amqp_rpc_reply_t& rpc_reply)
    {
        switch (rpc_reply.reply_type)
        {
        case AMQP_RESPONSE_NONE:
            return "no RPC reply type";

        case AMQP_RESPONSE_LIBRARY_EXCEPTION:
            return amqp_error_string(rpc_reply.library_error);

        case AMQP_RESPONSE_SERVER_EXCEPTION:
            {
                const void* decoded = rpc_reply.reply.decoded;

                switch (rpc_reply.reply.id)
                {
                case AMQP_CONNECTION_CLOSE_METHOD:
                        return error_message(close_connection_return(decoded));

                case AMQP_CHANNEL_CLOSE_METHOD:
                        return error_message(close_channel_return(decoded));
                }
            }
        default:
            {
                std::ostringstream stream;
                stream << "unknown server error, method id: " << rpc_reply.reply.id;
                return stream.str();
            }
        }
    }

    inline std::string error_message(const std::string& context,
                              const amqp_rpc_reply_t& reply)
    {
        return context + ": " + error_message(reply);
    }
}

void check(const char* context, int rc, bool nothrow)
{
    if (rc < 0)
    {
        char* errstr = amqp_error_string(-rc);
        BOOST_SCOPE_EXIT(&errstr) {
           free(errstr);
        } BOOST_SCOPE_EXIT_END

        std::ostringstream stream;
        std::ostream& error_stream = nothrow ? stream : std::cerr;
        error_stream << context << ": " << errstr;

        if(nothrow)
            error_stream << std::endl;
        else
            throw std::runtime_error(stream.str());
    }
}

void check_rpc(const char* context, const amqp_rpc_reply_t& reply,
               bool nothrow)
{
    if(reply.reply_type != AMQP_RESPONSE_NORMAL)
    {
        if(nothrow)
            std::cerr << error_message(context, reply) << std::endl;
        else
            throw AmqpRpcError(context, reply);
    }
}

AmqpRpcError::AmqpRpcError(const std::string& context,
                           const amqp_rpc_reply_t& reply):
    runtime_error(error_message(context, reply)),
    reply_(reply)
{}
