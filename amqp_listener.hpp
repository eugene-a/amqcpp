#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <boost/mpl/vector/vector10.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <amqp.h>

namespace msmf = boost::msm::front;
namespace msmb = boost::msm::back;
namespace mpl = boost::mpl;

using msmf::Row;
using msmf::none;

struct Deliver {};

struct Header
{
    explicit Header(size_t size):
        body_size(size)
    {}

    size_t body_size;
};

struct Body
{
    Body(size_t size): fragment_size(size)
    {}

    size_t fragment_size;
};

struct Delivered {};

struct AmqpListener_: msmf::state_machine_def<AmqpListener_>
{
    struct Waiting: msmf::state<> {};

    struct Delivery_: msmf::state_machine_def<Delivery_>
    {
        struct Init: msmf::state<> {};

        struct Receiving: msmf::state<> {};

        struct Received: msmf::exit_pseudo_state<none>
        {
           typedef mpl::vector1<Delivered> flag_list;
        };

        typedef Init initial_state;

        struct init
        {
            template <typename EVT, typename FSM,
                      typename SourceState, typename TargetState>
            void operator()(EVT const& evt, FSM& fsm,
                            SourceState const&, TargetState const&)
            {
                fsm.received_size = 0;
                fsm.body_size = evt.body_size;
            }
        };

        struct append
        {
            template <typename EVT, typename FSM,
                      typename SourceState, typename TargetState>
            void operator()(EVT const& evt, FSM& fsm,
                            SourceState const&, TargetState const&)
            {
                fsm.received_size += evt.fragment_size;
            }
        };

        struct received
        {
            template <typename EVT, typename FSM,
                      typename SourceState, typename TargetState>
            bool operator()(EVT const& evt, FSM& fsm,
                            SourceState const&, TargetState const&)
            {
                return fsm.delivered();
            }
        };

        struct transition_table: mpl::vector<
                    //    Start       Event       Next         Action   Guard
                    Row < Init,       Header,     Receiving,   init,          none >,
                    Row < Receiving,  Body,       none,        append,        none >,
                    Row < Receiving,  none,       Received,    none,          received >
                > {};

        bool delivered() const
        {
            return received_size == body_size;
        }

        size_t received_size;
        size_t body_size;
        amqp_basic_properties_t* properties;
    };

    typedef msmb::state_machine<Delivery_> Delivery;
    typedef Waiting initial_state;

    struct transition_table: mpl::vector<
         //    Start                  Event      Next       Action    Guard
         Row < Waiting,               Deliver,   Delivery,  none,     none >,
         Row < Delivery::exit_pt<
               Delivery_::Received>,  none,      Waiting,   none,     none >
     > {};
};

struct AmqpListener: msmb::state_machine<AmqpListener_> {};

#endif // LISTENER_HPP
