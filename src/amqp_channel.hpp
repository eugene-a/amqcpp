#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "amqp_connection.hpp"
#include "amqp_types.hpp"

struct QueueData
{
    explicit QueueData(amqp_bytes_t name, amqp_boolean_t excl = 0):
        queue(name),
        passive(0),
        durable(0),
        exclusive(excl),
        auto_delete(0),
        arguments(amqp_empty_table)
    {}

    amqp_bytes_t queue;
    amqp_boolean_t passive;
    amqp_boolean_t durable;
    amqp_boolean_t exclusive;
    amqp_boolean_t auto_delete;
    amqp_table_t arguments;
};

struct ConsumeData
{
    explicit ConsumeData(amqp_bytes_t q, amqp_boolean_t excl = 0):
        queue(q),
        consumer_tag(amqp_empty_bytes),
        no_local(1),
        no_ack(0),
        exclusive(excl),
        arguments(amqp_empty_table)
    {}

    amqp_bytes_t queue;
    amqp_bytes_t consumer_tag;
    amqp_boolean_t no_local;
    amqp_boolean_t no_ack;
    amqp_boolean_t exclusive;
    amqp_table_t arguments;
};

struct PublishData
{
    PublishData(amqp_bytes_t rk, const std::string& body,
                const amqp_basic_properties_t* props = 0
                ):
        exchange(amqp_empty_bytes),
        routing_key(rk),
        mandatory(0),
        immediate(0),
        message(body, props)
    {}

    amqp_bytes_t exchange;
    amqp_bytes_t routing_key;
    amqp_boolean_t mandatory;
    amqp_boolean_t immediate;
    AmqpMessage message;
};

class AmqpChannel: boost::noncopyable
{
public:
    explicit AmqpChannel(AmqpConnection& conn, int channel = 1):
        conn_(conn),
        channel_(channel)
    {
        amqp_channel_open(conn_, channel_);
        conn_.check_rpc("Openning channel");

    }

    ~AmqpChannel()
    {
        const amqp_rpc_reply_t reply =
                amqp_channel_close(conn_, channel_, AMQP_REPLY_SUCCESS);
        check_rpc("Closing channel", reply, true);
    }

    const amqp_queue_declare_ok_t& queue_declare(const QueueData& data)
    {
        const amqp_queue_declare_ok_t* ok =
                amqp_queue_declare(conn_, channel_, data.queue, data.passive,
                                   data.durable, data.exclusive, data.auto_delete,
                                   data.arguments);
        conn_.check_rpc("Declaring queue");
        return *ok;
    }

    const amqp_basic_consume_ok_t& consume(const ConsumeData& data)
    {
        const amqp_basic_consume_ok_t* ok =
                amqp_basic_consume(conn_, channel_, data.queue,
                                   data.consumer_tag, data.no_local,
                                   data.no_ack, data.exclusive,
                                   data.arguments);
        conn_.check_rpc("Consuming");
        return *ok;
    }

    void publish(PublishData& data)
    {
        amqp_basic_properties_t properties = data.message.properties;
        amqp_basic_properties_t* props =
                properties._flags == 0 ? 0 : &properties;

        const int rc =
                amqp_basic_publish(conn_, channel_, data.exchange,
                                   data.routing_key, data.mandatory,
                                   data.immediate, props, data.message.body);
        check("Publishing", rc);
    }

    void ack(uint64_t delivery_tag, bool multiple = false)
    {
        const int rc = amqp_basic_ack(conn_, channel_, delivery_tag, multiple);
        check("Ack", rc);
    }

private:
    AmqpConnection& conn_;
    const amqp_channel_t channel_;
};
#endif // CHANNEL_HPP
