#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstring>
#include <string>
#include <amqp.h>

template<typename T>
inline amqp_bytes_t to_amqp_bytes(const T& data, bool copy = false)
{
    amqp_bytes_t result;
    result.bytes = const_cast<T*>(&data);
    result.len = sizeof data;
    return to_amqp_bytes(result, copy);
}

template<> inline
amqp_bytes_t to_amqp_bytes<amqp_bytes_t>(const amqp_bytes_t& bytes, bool copy)
{
    return copy ? amqp_bytes_malloc_dup(bytes) : bytes;
}

template<> inline
amqp_bytes_t to_amqp_bytes<const char*>(const char* const& str, bool copy)
{
    amqp_bytes_t result;
    result.bytes = const_cast<char*>(str);
    result.len = strlen(str);
    return to_amqp_bytes(result, copy);
}

template<> inline
amqp_bytes_t to_amqp_bytes<std::string>(const std::string& str, bool copy)
{
    amqp_bytes_t result;
    result.bytes = const_cast<char*>(str.data());
    result.len = str.size();
    return to_amqp_bytes(result, copy);
}

template<typename T>
inline T from_amqp_bytes(amqp_bytes_t src)
{
    assert(src.len == sizeof(T));
    T result;
    memcpy(&result, src.bytes, src.len);
    return result;
}

template<>
inline std::string from_amqp_bytes<std::string>(amqp_bytes_t src)
{
    return std::string(static_cast<char*>(src.bytes), src.len);
}

template<typename T> amqp_field_value_t to_field_value(T);

template<> inline
amqp_field_value_t to_field_value<amqp_field_value_t>(amqp_field_value_t val)
{
    return val;
}


template<>
inline amqp_field_value_t to_field_value<int8_t>(int8_t val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_I8;
    result.value.i8 = val;
    return result;
}

template<>
inline amqp_field_value_t to_field_value<uint8_t>(uint8_t val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_U8;
    result.value.u8 = val;
    return result;
}

template<>
inline amqp_field_value_t to_field_value<int16_t>(int16_t val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_I16;
    result.value.i16 = val;
    return result;
}

template<>
inline amqp_field_value_t to_field_value<uint16_t>(uint16_t val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_U16;
    result.value.u16 = val;
    return result;
}

template<>
inline amqp_field_value_t to_field_value<int32_t>(int32_t val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_I32;
    result.value.i32 = val;
    return result;
}

template<>
inline amqp_field_value_t to_field_value<uint32_t>(uint32_t val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_U32;
    result.value.u32 = val;
    return result;
}

template<>
inline amqp_field_value_t to_field_value<int64_t>(int64_t val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_I64;
    result.value.i64 = val;
    return result;
}

template<>
inline amqp_field_value_t to_field_value<uint64_t>(uint64_t val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_U64;
    result.value.u64 = val;
    return result;
}

template<>
inline amqp_field_value_t to_field_value<float>(float val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_F32;
    result.value.f32 = val;
    return result;
}

template<>
inline amqp_field_value_t to_field_value<double>(double val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_F64;
    result.value.f64 = val;
    return result;
}

template<>
inline amqp_field_value_t to_field_value<amqp_decimal_t>(amqp_decimal_t val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_F32;
    result.value.decimal = val;
    return result;
}

template<>
inline amqp_field_value_t to_field_value<amqp_bytes_t>(amqp_bytes_t val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_BYTES;
    result.value.bytes = val;
    return result;
}

template<>
inline amqp_field_value_t to_field_value<amqp_array_t>(amqp_array_t val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_ARRAY;
    result.value.array = val;
    return result;
}

template<>
inline amqp_field_value_t to_field_value<amqp_table_t>(amqp_table_t val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_TABLE;
    result.value.table = val;
    return result;
}

inline amqp_field_value_t to_boolean_field_value(amqp_boolean_t val)
{
    amqp_field_value_t result;
    result.kind = AMQP_FIELD_KIND_BOOLEAN;
    result.value.boolean = val;
    return result;
}

#endif // UTIL_HPP
