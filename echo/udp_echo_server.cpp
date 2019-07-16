/**
 * desc: udp echo server with boost.asio
 * file: udp_echo_server.cpp
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

void Server(boost::asio::io_context &io_context, unsigned short port)
{
    // 声明一个UDP的socket
    udp::socket sock(io_context, udp::endpoint(udp::v4(), port));

    while (true) {
        char data[max_length];
        udp::endpoint cli_endpoint;

        // 接收Client的数据
        size_t length = sock.receive_from(boost::asio::buffer(data, max_length), cli_endpoint);
        std::cout << "Client data[" << cli_endpoint.address()
                  << ":" << cli_endpoint.port() << "] ";
        std::cout.write(data, length);
        std::cout << std::endl;

        // 将数据发回Client
        sock.send_to(boost::asio::buffer(data, length), cli_endpoint);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: udp_echo_server <port>" << std::endl;
        return 1;
    }

    try {
        boost::asio::io_context io_context;
        Server(io_context, std::atoi(argv[1]));
    } catch (std::exception e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
