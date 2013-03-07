#include "amqp_process.hpp"
#include "amqp_listener.hpp"
#include "util.hpp"
#include <iostream>

namespace
{
    inline bool is_deliver(const amqp_frame_t& frame)
    {
        return frame.frame_type == AMQP_FRAME_METHOD &&
                frame.payload.method.id == AMQP_BASIC_DELIVER_METHOD;
    }

    inline amqp_basic_deliver_t* delivery_decoded(const amqp_frame_t& frame)
    {
        void* decoded = frame.payload.method.decoded;
        return static_cast<amqp_basic_deliver_t*>(decoded);
    }

    inline bool is_header(const amqp_frame_t& frame)
    {
        return frame.frame_type == AMQP_FRAME_HEADER;
    }

    inline uint64_t body_size(const amqp_frame_t& frame)
    {
        return frame.payload.properties.body_size;
    }


    inline bool is_body(const amqp_frame_t& frame)
    {
        return frame.frame_type == AMQP_FRAME_BODY;
    }

    inline amqp_basic_properties_t* properties(const amqp_frame_t& header)
    {
        return static_cast<amqp_basic_properties_t*>
                (header.payload.properties.decoded);
    }

    inline const amqp_bytes_t& get_body_fragment(const amqp_frame_t& body)
    {
        return body.payload.body_fragment;
    }
}

AmqpProcessor::AmqpProcessor():
    listener_(new AmqpListener)
{
    listener_->start();
}

AmqpProcessor::~AmqpProcessor()
{}

AmqpProcessor::Result AmqpProcessor::process_frame(const amqp_frame_t& frame)
{
    Result result;

    if(is_deliver(frame))
    {
        listener_->process_event(Deliver());
        result = delivery_decoded(frame);
    }
    else if(is_header(frame))
    {
        listener_->process_event(Header(body_size(frame)));
        result = properties(frame);
    }
    else if(is_body(frame))
    {
        amqp_bytes_t fragment = get_body_fragment(frame);
        listener_->process_event(Body(fragment.len));

        AmqpListener::Delivery& delivery =
                listener_->get_state<AmqpListener::Delivery&>();

        bool last = delivery.is_flag_active<Delivered>();
        result = std::make_pair(fragment, last);

    }
    else
    {
        std::cout << "unprocessed frame: " <<  frame.frame_type << std::endl;
    }
    return result;
}

#pragma GCC diagnostic ignored "-Wunused-variable"
