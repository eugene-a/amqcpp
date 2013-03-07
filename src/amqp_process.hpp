#ifndef FRAME_DISPATCH_HPP
#define FRAME_DISPATCH_HPP

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/variant/variant.hpp>
#include <amqp.h>

struct AmqpListener;

typedef std::pair<amqp_bytes_t, bool> BodyFragment;

class AmqpProcessor: boost::noncopyable
{
public:
    typedef boost::variant<int, const amqp_basic_deliver_t*,
    const amqp_basic_properties_t*, BodyFragment> Result;

    AmqpProcessor();
    Result process_frame(const amqp_frame_t& frame);
    ~AmqpProcessor();

private:
    boost::scoped_ptr<AmqpListener> listener_;
};

#endif // FRAME_DISPATCH_HPP
