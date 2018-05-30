#include "UDPClient.h"
#include "Log.h"

namespace ORB_SLAM2 {

    
    UDPClient::UDPClient(boost::asio::io_service& io_service, const std::string& host, const std::string& port)
    : io_service_(io_service), socket_(io_service, udp::endpoint(udp::v4(), 0)) {
        udp::resolver resolver(io_service_);
        udp::resolver::query query(udp::v4(), host, port);
        udp::resolver::iterator iter = resolver.resolve(query);
        endpoint_ = *iter;
    }

    UDPClient::~UDPClient() {
        socket_.close();
    }

    void UDPClient::send(const std::ostringstream &stream) {
        std::string str = stream.str();
        auto size = socket_.send_to(boost::asio::buffer(str, str.length()), endpoint_);
        Log::write(__FUNCTION__, "Client sent bytes:", size);
    }

    void UDPClient::send(const std::string& msg) {
        auto size = socket_.send_to(boost::asio::buffer(msg, msg.size()), endpoint_);
        Log::write(__FUNCTION__, "Client sent bytes:", size);
    }

    void UDPClient::send(const int value) {
        auto size = socket_.send_to(boost::asio::buffer(&value, sizeof(value)), endpoint_);
        Log::write(__FUNCTION__, "Client sent bytes:", size);
    }

    void UDPClient::send(const char* data, const int length) {
        auto size = socket_.send_to(boost::asio::buffer(data, length), endpoint_);
        Log::write(__FUNCTION__, "Client sent bytes:", size);
    }

    void UDPClient::send(float* data, const int length) {
        char const *p = reinterpret_cast<char const *>(data);
        auto size = socket_.send_to(boost::asio::buffer(p, length*sizeof(float)), endpoint_);
        Log::write(__FUNCTION__, "Client sent bytes:", size);
    }
    
}
