#include "amqp_connection.hpp"
#include <cstdlib>

AmqpConnection::AmqpConnection(const std::string& host, int port)
{
    const int sockfd = amqp_open_socket(host.c_str(), port);
    amqp_set_sockfd(state_, sockfd);
    const amqp_rpc_reply_t reply = amqp_login(state_, "/", 0, 131072,
                     0, AMQP_SASL_METHOD_PLAIN, "guest", "guest");
    ::check_rpc("Logging in", reply);
}
