/**
 * desc: udp echo client with boost.asio
 * file: udp_echo_client.cpp
 *
 * author:  myw31415926
 * date:    20190709
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include <boost/asio.hpp>

#include <iostream>
#include <cstdlib>

using boost::asio::ip::udp;
const int max_length = 1024;

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: udp_echo_client <host> <port>" << std::endl;
        return 1;
    }

    try {
        boost::asio::io_context io_context;
        udp::resolver resolver(io_context);
        udp::resolver::results_type endpoints = resolver.resolve(udp::v4(), argv[1], argv[2]);

        udp::socket s(io_context, udp::endpoint(udp::v4(), 0));
        boost::asio::connect(s, endpoints);

        std::cout << "Enter message: ";
        char request[max_length];
        std::cin.getline(request, max_length);
        size_t request_length = std::strlen(request);
        s.send_to(boost::asio::buffer(request, request_length), *endpoints.begin());

        char reply[max_length];
        udp::endpoint srv_endpoint;
        size_t reply_length = s.receive_from(boost::asio::buffer(reply, max_length), srv_endpoint);
        std::cout << "Reply is:";
        std::cout.write(reply, reply_length);
        std::cout << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
