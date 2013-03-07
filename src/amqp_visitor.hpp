#ifndef AMQP_VISITOR_HPP
#define AMQP_VISITOR_HPP

#include "amqp_types.hpp"

typedef std::pair<amqp_bytes_t, bool> BodyFragment;

class AmqpVisitor: public boost::static_visitor<bool>
{
public:
    uint64_t delivery_tag() const
    {
        return delivery_tag_;
    }

    const std::string& body() const
    {
        return body_;
    }

    const amqp_basic_properties_t* properties() const
    {
        return properties_;
    }

    void reset()
    {
        body_.clear();
    }

    bool operator()(const amqp_basic_deliver_t* deliver)
    {
        delivery_tag_ = deliver->delivery_tag;
        return false;
    }

    bool operator()(const amqp_basic_properties_t* props)
    {
        properties_ = props;
        return false;
    }

    bool operator()(const BodyFragment& body_fragment)
    {
        const amqp_bytes_t& fragment = body_fragment.first;
        body_.append(static_cast<char*>(fragment.bytes), fragment.len);
        return body_fragment.second;
    }

    bool operator()(int)
    {
        return false;
    }

private:
    uint64_t delivery_tag_;
    const amqp_basic_properties_t* properties_;
    std::string body_;
};

#endif // AMQP_VISITOR_HPP
