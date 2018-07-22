#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <thread>

using namespace std;
using namespace boost::asio;

void sync_client()
{
    typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
    io_service service;
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
    socket_ptr sock(new ip::tcp::socket(service));
    sock->connect(ep);
    if (true)
    {
        sock->send(buffer("hello"));
        char data[512] = {0};
        size_t len = sock->read_some(buffer(data));
        cout << data << endl;
    }
    sock->close();
}

char data[512];
void async_client()
{
    io_service service;
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
    ip::tcp::socket sock(service);
    sock.async_connect(ep, [&](const boost::system::error_code &ec) {
        cout << ec << endl;
    });

    std::function<void(const boost::system::error_code &ec,
                       std::size_t bytes_transferred)>
        sendHandler;
    std::function<void(const boost::system::error_code &ec,
                       std::size_t bytes_transferred)>
        readHandler;
    sendHandler = [&](const boost::system::error_code &ec,
                      std::size_t bytes_transferred) {
        sock.async_receive(buffer(data), readHandler);
        cout<<data<<endl;
    };
    readHandler = [&](const boost::system::error_code &ec,
                      std::size_t bytes_transferred) {
        sock.async_send(buffer("hello"), sendHandler);
    };
    sock.async_send(buffer("hello"), sendHandler);
    service.run();
}

int main()
{
    async_client();
}