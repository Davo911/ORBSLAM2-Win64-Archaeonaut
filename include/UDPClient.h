#ifndef UDPClient_hpp
#define UDPClient_hpp

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

namespace ORB_SLAM2 {

    using boost::asio::ip::udp;

    class UDPClient {
        boost::asio::io_service& io_service_;
        udp::socket socket_;
        udp::endpoint endpoint_;
        
    public:
        UDPClient();
        UDPClient(boost::asio::io_service& io_service, const std::string& host, const std::string& port);
        ~UDPClient();
        
        void send(const std::ostringstream& stream);
        void send(const std::string& msg);
        void send(const int value);
        void send(const char* data, const int length);
        void send(float* data, const int length);
    };
    
}

#endif
