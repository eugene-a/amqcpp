#ifndef AMQP_TYPES_HPP
#define AMQP_TYPES_HPP

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include <amqp.h>
#include "util.hpp"

class AmqpBytes
{
public:
    AmqpBytes(): data_()
    {}

    template<typename T> AmqpBytes(const T& value):
        data_(to_amqp_bytes(value, true))
    {}

    AmqpBytes(const AmqpBytes& other):
        data_(amqp_bytes_malloc_dup(other.data_))
    {}

    template<typename T> AmqpBytes& operator = (const T& value)
    {
        if(data_.len > 0)
            amqp_bytes_free(data_);
        data_ = to_amqp_bytes(value, true);
        return *this;
    }

    AmqpBytes& operator = (const AmqpBytes& other)
    {
        if(&other != this)
            *this = other.data_;
        return *this;
    }

    const amqp_bytes_t& data() const
    {
        return data_;
    }

    operator const amqp_bytes_t&() const
    {
        return data_;
    }

    ~AmqpBytes()
    {
        if(data_.len > 0)
            amqp_bytes_free(data_);
    }

private:
    amqp_bytes_t data_;
};

class AmqpFieldValue;
struct AmqpArrayData;
struct AmqpTableData;

class AmqpArray
{
public:
    AmqpArray();

    void add(const AmqpFieldValue& value);
    amqp_array_t data();

    operator amqp_array_t()
    {
        return data();
    }

    ~AmqpArray();

private:
    boost::shared_ptr<AmqpArrayData> data_;
    std::vector<amqp_field_value_t> entries_;
};

class AmqpTable
{
public:
    AmqpTable();
    AmqpTable(const amqp_table_t&);
    void add(const AmqpBytes& key, const AmqpFieldValue& value);
    amqp_table_t data();

    operator amqp_table_t()
    {
        return data();
    }

    ~AmqpTable();

private:
    boost::shared_ptr<AmqpTableData> data_;
    std::vector<amqp_table_entry_t> entries_;
};

class AmqpFieldValue
{
public:
    AmqpFieldValue()
    {
        value_.kind = AMQP_FIELD_KIND_VOID;
    }

    AmqpFieldValue(const AmqpFieldValue& other)
    {
        assign(other);
    }

    template<typename T> AmqpFieldValue(const T& value):
        value_(to_field_value(value))
    {}

    AmqpFieldValue(amqp_boolean_t value, bool boolean):
        value_(boolean ? to_boolean_field_value(value) : to_field_value(value))
    {}

    AmqpFieldValue(const AmqpBytes& value):
        holder_(value),
        value_(to_field_value(boost::get<AmqpBytes>(holder_).data()))
    {}

    AmqpFieldValue(const AmqpArray& value):
        holder_(value),
        value_(to_field_value(boost::get<AmqpArray>(holder_).data()))
    {}

    AmqpFieldValue(const AmqpTable& value):
       holder_(value),
       value_(to_field_value(boost::get<AmqpTable>(holder_).data()))
    {}

    AmqpFieldValue& operator = (const AmqpFieldValue& other)
    {
       if(&other != this)
           assign(other);
       return *this;
    }

    operator const amqp_field_value_t&()
    {
        return value_;
    }

private:
    void assign(const AmqpFieldValue&);

    boost::variant<int, AmqpBytes, AmqpArray, AmqpTable> holder_;
    amqp_field_value_t value_;
};

class AmqpProperties
{
public:
    AmqpProperties(const amqp_basic_properties_t* props = 0):
        flags_()
    {
        if(props != 0)
            set(*props);
    }

    void set(const amqp_basic_properties_t&);

    amqp_flags_t flags() const
    {
        return flags_;
    }

    const amqp_bytes_t& content_type() const
    {
        return content_type_;
    }

    void content_type(const amqp_bytes_t& value)
    {
        content_type_ = value;
        flags_ |= AMQP_BASIC_CONTENT_TYPE_FLAG;
    }

    const amqp_bytes_t& content_encoding() const
    {
        return content_encoding_;
    }

    void content_encoding(const amqp_bytes_t& value)
    {
        content_encoding_ = value;
        flags_ |= AMQP_BASIC_CONTENT_ENCODING_FLAG;
    }

    const AmqpTable& headers() const
    {
        return headers_;
    }

    void add_header(const AmqpBytes& key, const AmqpFieldValue& value)
    {
        headers_.add(key, value);
        flags_ |= AMQP_BASIC_HEADERS_FLAG;
    }

    uint8_t delivery_mode() const
    {
        return delivery_mode_;
    }

    void delivery_mode(uint8_t value)
    {
        delivery_mode_ = value;
        flags_ |= AMQP_BASIC_DELIVERY_MODE_FLAG;
    }

    uint8_t priority() const
    {
        return priority_;
    }

    void priority(uint8_t value)
    {
        priority_ = value;
        flags_ |= AMQP_BASIC_PRIORITY_FLAG;
    }

    const amqp_bytes_t& correlation_id() const
    {
        return correlation_id_;
    }

    void correlation_id(const amqp_bytes_t& value)
    {
        correlation_id_ = value;
        flags_ |= AMQP_BASIC_CORRELATION_ID_FLAG;
    }

    const amqp_bytes_t& reply_to() const
    {
        return reply_to_;
    }

    void reply_to(const amqp_bytes_t& value)
    {
        reply_to_ = value;
        flags_ |= AMQP_BASIC_REPLY_TO_FLAG;
    }

    const amqp_bytes_t& expiration() const
    {
        return expiration_;
    }

    void expiration(const amqp_bytes_t& value)
    {
        expiration_ = value;
        flags_ |= AMQP_BASIC_EXPIRATION_FLAG;
    }

    const amqp_bytes_t& message_id() const
    {
        return message_id_;
    }

    void message_id(const amqp_bytes_t& value)
    {
        message_id_ = value;
        flags_ |= AMQP_BASIC_MESSAGE_ID_FLAG;
    }

    uint64_t timestamp() const
    {
        return timestamp_;
    }

    void timestamp(uint64_t value)
    {
        timestamp_ = value;
        flags_ |= AMQP_BASIC_TIMESTAMP_FLAG;
    }

    const amqp_bytes_t& type() const
    {
        return message_id_;
    }

    void type(const amqp_bytes_t& value)
    {
        type_ = value;
        flags_ |= AMQP_BASIC_TYPE_FLAG;
    }

    const amqp_bytes_t& user_id() const
    {
        return user_id_;
    }

    void user_id(const amqp_bytes_t& value)
    {
        user_id_ = value;
        flags_ |= AMQP_BASIC_USER_ID_FLAG;
    }

    const amqp_bytes_t& app_id() const
    {
        return app_id_;
    }

    void app_id(const amqp_bytes_t& value)
    {
        app_id_ = value;
        flags_ |= AMQP_BASIC_APP_ID_FLAG;
    }

    const amqp_bytes_t& cluster_id() const
    {
        return cluster_id_;
    }

    void cluster_id(const amqp_bytes_t& value)
    {
        cluster_id_ = value;
        flags_ |= AMQP_BASIC_CLUSTER_ID_FLAG;
    }

    operator amqp_basic_properties_t();

private:
    amqp_flags_t flags_;

    AmqpBytes content_type_;
    AmqpBytes content_encoding_;
    AmqpTable headers_;
    uint8_t delivery_mode_;
    uint8_t priority_;
    AmqpBytes correlation_id_;
    AmqpBytes reply_to_;
    AmqpBytes expiration_;
    AmqpBytes message_id_;
    uint64_t timestamp_;
    AmqpBytes type_;
    AmqpBytes user_id_;
    AmqpBytes app_id_;
    AmqpBytes cluster_id_;
};

struct AmqpMessage
{
    AmqpMessage()
    {}

    template<typename T>
    AmqpMessage(const T& b, const amqp_basic_properties_t* props = 0):
        properties(props),
        body(b)
    {}

    AmqpProperties properties;
    AmqpBytes body;
};

#endif // AMQP_TYPES_HPP
