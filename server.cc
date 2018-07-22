#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <thread>

using namespace std;
using namespace boost::asio;

void sync_server()
{
    typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
    io_service service;
    ip::tcp::endpoint ep(ip::tcp::v4(), 2001); // listen on 2001
    ip::tcp::acceptor acc(service, ep);
    while (true)
    {
        socket_ptr sock(new ip::tcp::socket(service));
        acc.accept(*sock);
        thread t([&](socket_ptr sock) {
            while (true)
            {
                char data[512];
                try
                {
                    size_t len = sock->read_some(buffer(data));
                    cout << data << endl;
                    if (len > 0)
                        write(*sock, buffer("ok", 2));
                }
                catch (exception &e)
                {
                    cout << e.what() << endl;
                    break;
                }
            }
            sock->close();
        },
                 sock);

        t.join();
    }
}

void async_server()
{
    typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
    io_service service;
    ip::tcp::endpoint ep(ip::tcp::v4(), 2001);
    ip::tcp::acceptor acc(service, ep);
    socket_ptr sock(new ip::tcp::socket(service));
    acc.async_accept(*sock, [&](const boost::system::error_code &err) {
        if (err)
            return;
        thread t([&](socket_ptr sock) {
            while (true)
            {
                char data[512];
                try
                {
                    size_t len = sock->read_some(buffer(data));
                    cout << data << endl;
                    if (len > 0)
                        write(*sock, buffer("ok", 2));
                }
                catch (exception &e)
                {
                    cout << e.what() << endl;
                    break;
                }
            }
            sock->close();
        },
                 sock);

        t.join();
    });
    service.run();
}

int main()
{
    async_server();
}