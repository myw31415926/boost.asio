/**
 * desc: tcp echo server with boost.asio
 * file: tcp_echo_server.cpp
 *
 * author:  myw31415926
 * date:    20190709
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <cstdlib>

using boost::asio::ip::tcp;
typedef boost::shared_ptr<tcp::socket> SocketPtr;
const int max_length = 1024;

void Session(SocketPtr sock)
{
    try {
        while (true) {
            char data[max_length];

            boost::system::error_code errcode;

            // 读取Clinet发送的数据
            size_t length = sock->read_some(boost::asio::buffer(data), errcode);
            if (errcode == boost::asio::error::eof) {
                std::cout << "boost::asio::error::eof" << std::endl;
                break;              // 对端连接断开
            } else if (errcode) {   // 其他错误
                throw boost::system::system_error(errcode);
            }

            std::cout << "Client data: ";
            std::cout.write(data, length);
            std::cout << std::endl;

            // 数据发送回给Clinet
            boost::asio::write(*sock, boost::asio::buffer(data, length));
        }
    } catch (std::exception& e) {
        std::cerr << "Exception in thread: " << e.what() << std::endl;
    }
}

void Server(boost::asio::io_context &io_context, unsigned short port)
{
    // 声明一个IPv4的accept
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port));

    while (true) {
        // 声明一个tcp::socket，启动监听，等待链接
        SocketPtr sock(new tcp::socket(io_context));
        a.accept(*sock);

        // Client已链接，显示对端地址
        std::cout << "Client addr: " << sock->remote_endpoint().address() << std::endl;
        // 新建线程处理链接请求
        boost::thread t(boost::bind(Session, sock));
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: tcp_echo_server <port>" << std::endl;
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
