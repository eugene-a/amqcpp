#ifndef AMQP_TYPES_PRIVATE_HPP
#define AMQP_TYPES_PRIVATE_HPP

#include "amqp_types.hpp"

struct AmqpArrayData
{
    std::vector<AmqpFieldValue> entries;
};

class AmqpTableEntry
{
public:
    AmqpTableEntry(const AmqpBytes& key, const AmqpFieldValue value):
        key_(key),
        value_(value)
    {}

    operator amqp_table_entry_t();

private:
    AmqpBytes key_;
    AmqpFieldValue value_;
};

struct AmqpTableData
{
    std::vector<AmqpTableEntry> entries;
};

#endif // AMQP_TYPES_PRIVATE_HPP
