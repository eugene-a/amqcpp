#include <iostream>
#include <amqp_channel.hpp>
#include <amqp_process.hpp>
#include <amqp_visitor.hpp>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cerr << "usage: consume <server> < queue>" << std::endl;
        return 1;
    }

    try
    {
        AmqpConnection conn(argv[1]);
        AmqpChannel channel(conn);
        amqp_bytes_t queue_name = to_amqp_bytes(argv[2]);
        QueueData queue_data(queue_name);
        channel.queue_declare(queue_data);
        ConsumeData consume_data(queue_name);
        channel.consume(consume_data);

        AmqpProcessor amqp_processor;
        AmqpVisitor visitor;

        bool delivered = false;
        amqp_frame_t frame;

        while(!delivered)
        {
            if(conn.wait_frame(frame) < 0)
            {
                std::cerr << "lost connection" << std::endl;
                return 1;
            }

            AmqpProcessor::Result result = amqp_processor.process_frame(frame);
            delivered = boost::apply_visitor(visitor, result);
        }

        const amqp_basic_properties_t* properties = visitor.properties();
        uint64_t delivery_tag = visitor.delivery_tag();
        std::string body = visitor.body();

        int correlation_id =  from_amqp_bytes<int>(properties->correlation_id);
        std::cout << "correlation id: " << correlation_id << std::endl;
        std::cout << "delivery_tag: " << delivery_tag << std::endl;
        std::cout << "body: " << body << std::endl;
        visitor.reset();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
