#ifndef HTTP_FILE_SERVER_H
#define HTTP_FILE_SERVER_H

#include <cstdint>
#include <string>
#include <regex>
#include <sstream>

#include "TcpSocket.h"
#include "TcpServer.h"

class BasicFileHandler : public tcp::TcpServer::IEvent
{

public:
    BasicFileHandler(const std::string &rootDir);

    virtual void NewConnection(const tcp::Conn &conn);
    virtual void ReadData(const tcp::Conn &conn);
    virtual void ClientClosed(const tcp::Conn &conn);
    virtual void ServerTerminated(tcp::TcpServer::IEvent::CloseType type);

    virtual void WsReadData(const tcp::Conn &conn);
    virtual void ReadDataPath(const tcp::Conn &conn, const std::string &resource);

    std::string Match(const std::string &msg, const std::string &patternString);
    void Send404(const tcp::Conn &conn, const std::string &resource);

private:
    std::string mRootDir;

};

#endif
