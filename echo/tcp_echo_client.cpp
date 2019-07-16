/**
 * desc: tcp echo client with boost.asio
 * file: tcp_echo_client.cpp
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

using boost::asio::ip::tcp;
const int max_length = 1024;

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: tcp_echo_client <host> <port>" << std::endl;
        return 1;
    }

    try {
        boost::asio::io_context io_context;

        // resolver将指定的URL和服务转化为相应的ip和port的endpoint对象
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(tcp::v4(), argv[1], argv[2]);

        // 链接Server
        tcp::socket s(io_context);
        boost::asio::connect(s, endpoints);

        std::cout << "Enter message: ";
        char request[max_length];
        std::cin.getline(request, max_length);
        size_t request_length = std::strlen(request);
        boost::asio::write(s, boost::asio::buffer(request, request_length));

        // 等待Server的应答
        char reply[max_length];
        size_t reply_length = boost::asio::read(s, boost::asio::buffer(reply, request_length));
        std::cout << "Reply is:";
        std::cout.write(reply, reply_length);
        std::cout << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
