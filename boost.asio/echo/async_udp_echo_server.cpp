/**
 * desc: async udp echo server with boost.asio
 * file: async_udp_echo_server.cpp
 *
 * author:  myw31415926
 * date:    20190716
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <cstdlib>

using boost::asio::ip::udp;

// UdpServer 类，处理 receive_from 和 send_to
// UDP 是无连接的，所以没有 Session
class UdpServer
{
public:
    UdpServer(boost::asio::io_context& io_context, short port)
        : sock_(io_context, udp::endpoint(udp::v4(), port))
    {
        // 启动后注册 UDP 接收事件 HandleReceiveFrom
        sock_.async_receive_from(boost::asio::buffer(data_, max_length), cli_endpoint_,
            boost::bind(&UdpServer::HandleReceiveFrom, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

private:
    // 读取完 UDP 数据，调用 HandleReceiveFrom
    void HandleReceiveFrom(const boost::system::error_code& ec, size_t bytes_recv)
    {
        if (!ec && bytes_recv > 0) {
            std::cout << "Client data[" << cli_endpoint_.address()
                  << ":" << cli_endpoint_.port() << "] ";
            std::cout.write(data_, bytes_recv);
            std::cout << std::endl;

            // 注册 UDP 发送事件
            sock_.async_send_to(boost::asio::buffer(data_, bytes_recv), cli_endpoint_,
                boost::bind(&UdpServer::HandleSendTo, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        } else {
            std::cerr << "UdpServer::HandleReceiveFrom error: " << ec.message() << std::endl;

            // 重新注册接收数据的回调
            sock_.async_receive_from(boost::asio::buffer(data_, max_length), cli_endpoint_,
                boost::bind(&UdpServer::HandleReceiveFrom, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
    }

    // 发送完 UDP 数据，调用 HandleSendTo
    void HandleSendTo(const boost::system::error_code& ec, size_t bytes_sent)
    {
        // 注册接收数据的回调，保证异步调用不会退出，可以对错误进行判断
        sock_.async_receive_from(boost::asio::buffer(data_, max_length), cli_endpoint_,
            boost::bind(&UdpServer::HandleReceiveFrom, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

private:
    udp::socket sock_;
    udp::endpoint cli_endpoint_;
    enum { max_length = 1024 };
    char data_[max_length];
};

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: async_udp_echo_server <port>" << std::endl;
        return 1;
    }

    try {
        boost::asio::io_context io_context;
        UdpServer srv(io_context, std::atoi(argv[1]));

        io_context.run();
    } catch (std::exception e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
