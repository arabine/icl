/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef HTTP_FILE_SERVER_H
#define HTTP_FILE_SERVER_H

#include <cstdint>
#include <string>
#include <regex>
#include <sstream>

#include "TcpSocket.h"
#include "TcpServer.h"
#include "JsonValue.h"
#include "HttpProtocol.h"

struct ChunkedData
{
    HttpRequest request;
    tcp::Conn conn;
    std::string data;
    uint32_t current_size;
    uint32_t total_size;
    uint32_t counter;

    ChunkedData()
        : current_size(0U)
        , total_size(0U)
        , counter(1U)
    {

    }

};

class HttpFileServer : public tcp::TcpServer::IEvent
{

public:
    HttpFileServer(const std::string &rootDir);
    ~HttpFileServer();

    virtual void NewConnection(const tcp::Conn &conn);
    virtual void ReadData(const tcp::Conn &conn);
    virtual void ClientClosed(const tcp::Conn &conn);
    virtual void ServerTerminated(tcp::TcpServer::IEvent::CloseType type);

    virtual void WsReadData(const tcp::Conn &conn);
    virtual void ReadDataPath(const tcp::Conn &conn, const HttpRequest &header);

    std::string Match(const std::string &msg, const std::string &patternString);
    void Send404(const tcp::Conn &conn, const HttpRequest &header);
    void Send403(const tcp::Conn &conn);
    void SetLocalhostOnly(bool enable);
    void SendHttpJson(const tcp::Conn &conn, const std::string &data);
    std::string GenerateJWT(const std::string &payload);
    bool CheckJWT(const std::string &header, const std::string &payload, const std::string &hash, JsonValue &json);

private:
    std::string mRootDir;
    std::string mSessionSecret;
    bool mLocalHostOnly = false;

    std::vector<ChunkedData> mPartials;

    void DeletePartialConn(const tcp::Conn &conn);
    bool GetFile(const tcp::Conn &conn, HttpRequest &request);
};


#endif
