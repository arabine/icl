#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <cstdint>


#include "HttpProtocol.h"
#include "TcpClient.h"

class HttpClient
{
public:
    HttpClient();
    bool Get(const std::string &host, const std::string &path, uint16_t port, std::string &response);

private:
    HttpRequest request;
    HttpProtocol proto;
    tcp::TcpClient client;
    HttpReply mReply;
    bool mFirstPacket = false;
    bool mGetFinished = false;
    bool mRequestSuccess = false;

    std::string mCurrentChunk;
    int32_t mCurrentChunkSize = -1;

    bool ParseChunkSize(const std::string &data);
};

#endif // HTTP_CLIENT_H
