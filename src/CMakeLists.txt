project(amqcpp)
cmake_minimum_required(VERSION 2.8)

set(CPP ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(THIRD_PARTY ${CPP}/../../../Thirdparty)
set(BOOST ${THIRD_PARTY}/boost/boost64bit)
set(RABBITMQ ${THIRD_PARTY}/librabbitmq/64-bit)

include_directories(${BOOST}/include ${RABBITMQ}/include)

add_library(${PROJECT_NAME} amqp_connection.hpp amqp_connection.cpp
amqp_channel.hpp error.hpp error.cpp amqp_listener.hpp
amqp_process.hpp amqp_process.cpp util.hpp
amqp_types.hpp amqp_types.cpp amqp_private.hpp amqp_visitor.hpp)

set(CMAKE_CXX_FLAGS -Wall )
