/**
 * desc: async tcp echo server with boost.asio
 * file: async_tcp_echo_server.cpp
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

using boost::asio::ip::tcp;

// Session 类，读写数据
class Session
{
public:
    Session(boost::asio::io_context& io_context) : sock_(io_context) {}

    tcp::socket& Socket()
    {
        return sock_;
    }

    void Start()
    {
        // 显示对端地址
        std::cout << "Client addr: " << sock_.remote_endpoint().address() << std::endl;

        // 注册读数据的回调函数 HandleRead
        sock_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&Session::HandleRead, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

private:
    // 完成读数据后，调用 HandleRead
    void HandleRead(const boost::system::error_code& ec, size_t bytes_read)
    {
        if (!ec) {
            std::cout << "Client data: ";
            std::cout.write(data_, bytes_read);
            std::cout << std::endl;

            // 注册写数据的回调函数，buffer 中数据写完成后，调用 HandleWrite
            boost::asio::async_write(sock_, boost::asio::buffer(data_, bytes_read),
                boost::bind(&Session::HandleWrite, this,
                    boost::asio::placeholders::error));
        } else {
            // Client 退出或错误
            std::cerr << "Session::HandleRead error: " << ec.message() << std::endl;
            delete this;
        }
    }

    // // 完成写数据后，调用 HandleWrite
    void HandleWrite(const boost::system::error_code& ec)
    {
        if (!ec) {
            //  重新注册读数据的回调函数 HandleRead，保证异步调用不会退出
            sock_.async_read_some(boost::asio::buffer(data_, max_length),
                boost::bind(&Session::HandleRead, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        } else {
            std::cerr << "Session::HandleWrite error: " << ec.message() << std::endl;
            delete this;
        }
    }

private:
    tcp::socket sock_;
    enum { max_length = 1024 };
    char data_[max_length];
};

// TcpServer 类，处理 accept 请求
class TcpServer
{
public:
    TcpServer(boost::asio::io_context& io_context, short port)
        : io_context_(io_context),
          acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        StartAccept();
    }

private:
    // 绑定 Session 对象，注册 accept 的回调
    void StartAccept()
    {
        Session* new_session = new Session(io_context_);
        acceptor_.async_accept(new_session->Socket(), 
            boost::bind(&TcpServer::HandleAccept, this, new_session,
                boost::asio::placeholders::error));
    }

    // 每次 accept 连接后，调用 HandleAccept
    void HandleAccept(Session* new_session, const boost::system::error_code& ec)
    {
        if (!ec) {
            new_session->Start();
        } else {
            std::cerr << "TcpServer::HandleAccept error: " << ec.message() << std::endl;
            delete new_session;
        }

        StartAccept();  // 重新注册 accept 的回调
    }

private:
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
};

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: async_tcp_echo_server <port>" << std::endl;
        return 1;
    }

    try {
        boost::asio::io_context io_context;
        TcpServer srv(io_context, std::atoi(argv[1]));

        // 启动异步模式，还有注册的服务没有处理，就不会退出
        io_context.run();
    } catch (std::exception e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
