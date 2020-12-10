/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#include "TcpClient.h"
#include "HttpProtocol.h"

#include <iostream>

namespace tcp
{

/*****************************************************************************/
TcpClient::TcpClient(bool isWebSocket)
    : mIsWebSocket(isWebSocket)
    , mWsUri("/")
    , mTls(&mSocket)
{

}
/*****************************************************************************/
void TcpClient::Close()
{
    mSocket.Close();
    if (mIsSecured)
    {
        mTls.Close();
    }
}
/*****************************************************************************/
void TcpClient::SetWebSocket(bool enable)
{
    mIsWebSocket = enable;
}
/*****************************************************************************/
void TcpClient::SetSecured(bool enable)
{
    mIsSecured = enable;
}
/*****************************************************************************/
bool TcpClient::Initialize(int timeout)
{
    bool ok = false;
    /*************************************************************/
    /* Create an AF_INET stream socket                           */
    /*************************************************************/
    if (mSocket.Create())
    {
        ok = true;
        mSocket.SetNonBlocking(mSocket.GetSocket());

        if (timeout > 0)
        {
            // LINUX
            struct timeval tv;
            tv.tv_sec = timeout / 1000;
            tv.tv_usec = (timeout % 1000) * 1000;
            int success = setsockopt(mSocket.GetSocket(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

            if (success != 0)
            {
                ok = false;
            }
            // WINDOWS
    //        DWORD timeout = timeout_in_seconds * 1000;
    //        setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);

        }
    }

    return ok;
}
/*****************************************************************************/
bool TcpClient::Connect(const std::string &host, const int port)
{
    bool ret = false;

    mHost = host;
    mPort = port;

    if (mSocket.Connect(host, port))
    {
        ret = true;
        if (mIsSecured)
        {
            if (mTls.Connect(mHost.data()))
            {
                ret = true;
            }
        }

        if (ret && mIsWebSocket)
        {
            std::string req = BuildWebSocketHandshake(mWsUri);
            //    std::cout << "-----------------------------" << std::endl;
            //    std::cout << wreq << std::endl;
            //    std::cout << "-----------------------------" << std::endl;

            if (mIsSecured)
            {
                mTls.Write(reinterpret_cast<const uint8_t *>(req.data()), req.size());
            }
            else
            {
                mSocket.Send(req);
            }
        }
    }
    else
    {
        Close();
    }

    return ret;
}
/*****************************************************************************/
std::string TcpClient::BuildWebSocketHandshake(const std::string &path)
{
    WebSocketRequest ws;

    ws.request.method = "GET";
    ws.request.protocol = "HTTP/1.1";
    ws.request.query = path; // eg: "/api/v1/machines/upstream/data"
    ws.request.body = "";
    ws.protocol = "tarotclub";
    ws.request.headers["Host"] = mHost;
//    ws.request.headers["Content-length"] = std::to_string(0);

    return HttpProtocol::GenerateWebSocketRequest(ws);
}
/*****************************************************************************/
void TcpClient::SetWebSocketUri(const std::string &uri)
{
    mWsUri = uri;
}
/*****************************************************************************/
bool TcpClient::Send(const std::string &input)
{
    std::string data = mIsWebSocket ? TcpSocket::BuildWsFrame(TcpSocket::WEBSOCKET_OPCODE_TEXT, input) : input;

    bool success = false;
    if (mIsSecured)
    {
        success = mTls.Write(reinterpret_cast<const uint8_t *>(data.data()), data.size());
    }
    else
    {
        success = mSocket.Send(data);
    }

    return success;
}
/*****************************************************************************/
bool TcpClient::RecvWithTimeout(std::string &output, size_t max_size, uint32_t timeout_ms)
{
    bool hasData = false;
    std::string buffer;

    if (mIsSecured)
    {
        read_buff_t rb;
        mTls.WaitData(&rb);
        if (rb.size > 0)
        {
            buffer.assign(reinterpret_cast<char *>(rb.data), rb.size);
            hasData = true;
        }
    }
    else
    {
        hasData = mSocket.RecvWithTimeout(buffer, max_size, timeout_ms);
    }

    if (hasData)
    {
        if (mIsWebSocket)
        {
            TcpSocket::WS_RESULT res = TcpSocket::DecodeWsData(buffer, output);

            if (res != TcpSocket::WS_DATA)
            {
                hasData = false;
                if (res == TcpSocket::WS_SEND_PONG)
                {

                    std::string pongData = TcpSocket::BuildWsFrame(TcpSocket::WEBSOCKET_OPCODE_PONG, std::string());
                    if (mIsSecured)
                    {
                        mTls.Write(reinterpret_cast<const uint8_t *>(pongData.data()), pongData.size());
                    }
                    else
                    {
                        mSocket.Send(pongData);
                    }
                }
            }
        }
        else
        {
            output = buffer;
        }
    }

    return hasData;
}


} // namespace tcp

//=============================================================================
// End of file TcpClient.cpp
//=============================================================================
