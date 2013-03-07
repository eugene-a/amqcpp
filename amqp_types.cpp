#include "amqp_types.hpp"
#include "amqp_private.hpp"

AmqpTableEntry::operator amqp_table_entry_t()
{
    amqp_table_entry_t table_entry;
    table_entry.key = key_;
    table_entry.value = value_;
    return table_entry;
}

AmqpArray::AmqpArray():
    data_(new AmqpArrayData)
{}

amqp_array_t AmqpArray::data()
{
    const size_t size = entries_.size();
    amqp_array_t array;
    array.num_entries = size;
    array.entries = size > 0 ? &entries_[0] : 0;
    return array;
}

AmqpArray::~AmqpArray()
{}

void AmqpArray::add(const AmqpFieldValue& value)
{
    data_->entries.push_back(value);
    entries_.push_back(data_->entries.back());
}

AmqpTable::AmqpTable():
    data_(new AmqpTableData)
{}

AmqpTable::AmqpTable(const amqp_table_t& table):
    data_(new AmqpTableData)
{
    for(int i = 0; i < table.num_entries; ++i)
    {
        const amqp_table_entry_t& entry = table.entries[i];
        add(entry.key, entry.value);
    }
}

void AmqpTable::add(const AmqpBytes& key, const AmqpFieldValue& value)
{
    data_->entries.push_back(AmqpTableEntry(key, value));
    entries_.push_back(data_->entries.back());
}

amqp_table_t AmqpTable::data()
{
    const size_t size = entries_.size();
    amqp_table_t table;
    table.num_entries = size;
    table.entries = size > 0 ? &entries_[0] : 0;
    return table;
}

AmqpTable::~AmqpTable()
{}

void AmqpFieldValue::assign(const AmqpFieldValue& other)
{
    this->holder_ = other.holder_;
    switch(other.value_.kind)
    {
    case AMQP_FIELD_KIND_BYTES:
        value_ = to_field_value(boost::get<AmqpBytes>(holder_).data());
        break;

    case AMQP_FIELD_KIND_ARRAY:
        value_ = to_field_value(boost::get<AmqpArray>(holder_).data());
        break;

    case AMQP_FIELD_KIND_TABLE:
        value_ = to_field_value(boost::get<AmqpTable>(holder_).data());
        break;

    default:
        value_ = other.value_;
        break;
    }
}

void AmqpProperties::set(const amqp_basic_properties_t& props)
{
    flags_ = props._flags;

    if(flags_ & AMQP_BASIC_CONTENT_TYPE_FLAG)
        content_type_ = props.content_type;

    if(flags_ & AMQP_BASIC_CONTENT_ENCODING_FLAG)
        content_encoding_ = props.content_encoding;

    if(flags_ & AMQP_BASIC_HEADERS_FLAG)
        headers_ = props.headers;

    if(flags_ & AMQP_BASIC_DELIVERY_MODE_FLAG)
        delivery_mode_ = props.delivery_mode;

    if(flags_ & AMQP_BASIC_PRIORITY_FLAG)
        priority_ = props.priority;

    if(flags_ & AMQP_BASIC_CORRELATION_ID_FLAG)
        correlation_id_ = props.correlation_id;

    if(flags_ & AMQP_BASIC_REPLY_TO_FLAG)
        reply_to_ = props.reply_to;

    if(flags_ & AMQP_BASIC_EXPIRATION_FLAG)
        expiration_ = props.expiration;

    if(flags_ & AMQP_BASIC_MESSAGE_ID_FLAG)
        message_id_ = props.message_id;

    if(flags_ & AMQP_BASIC_TIMESTAMP_FLAG)
        timestamp_ = props.timestamp;

    if(flags_ & AMQP_BASIC_TYPE_FLAG)
        type_ = props.type;

    if(flags_ & AMQP_BASIC_USER_ID_FLAG)
        user_id_ = props.user_id;

    if(flags_ & AMQP_BASIC_APP_ID_FLAG)
        app_id_ = props.app_id;

    if(flags_ & AMQP_BASIC_CLUSTER_ID_FLAG)
        cluster_id_ = props.cluster_id;
}

AmqpProperties::operator amqp_basic_properties_t()
{
    amqp_basic_properties_t result;
    result._flags = flags_;
    result.content_type = content_type_;
    result.content_encoding = content_encoding_;
    result.headers = headers_;
    result.delivery_mode = delivery_mode_;
    result.priority = priority_;
    result.correlation_id = correlation_id_;
    result.reply_to = reply_to_;
    result.expiration = expiration_;
    result.message_id = message_id_;
    result.timestamp = timestamp_;
    result.type = type_;
    result.user_id = user_id_;
    result.app_id = app_id_;
    result.cluster_id = cluster_id_;
    return result;
}
