#ifndef HTTP_FILE_SERVER_H
#define HTTP_FILE_SERVER_H

#include <cstdint>
#include <string>
#include <regex>
#include <sstream>

#include "TcpSocket.h"
#include "TcpServer.h"

struct HttpRequest
{
    std::string method;
    std::string query;
    std::string protocol;
    std::map<std::string, std::string> params;
    std::map<std::string, std::string> headers;
    std::string body;
};


class HttpFileServer : public tcp::TcpServer::IEvent
{

public:
    HttpFileServer(const std::string &rootDir);

    virtual void NewConnection(const tcp::Conn &conn);
    virtual void ReadData(const tcp::Conn &conn);
    virtual void ClientClosed(const tcp::Conn &conn);
    virtual void ServerTerminated(tcp::TcpServer::IEvent::CloseType type);

    virtual void WsReadData(const tcp::Conn &conn);
    virtual bool ReadDataPath(const tcp::Conn &conn, const HttpRequest &header);

    std::string Match(const std::string &msg, const std::string &patternString);
    void Send404(const tcp::Conn &conn, const HttpRequest &header);

    void SendHttpJson(const tcp::Conn &conn, const std::string &data);
    std::string GenerateJWT(int32_t level);
    bool CheckJWT(std::string &header, std::string &payload);

private:
    std::string mRootDir;
    std::string mSessionSecret;

    bool ParseHeader(const tcp::Conn &conn, HttpRequest &request);
    bool GetFile(const tcp::Conn &conn, HttpRequest &request);
};

#endif
