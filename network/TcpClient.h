/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "TcpSocket.h"
#include "TlsClient.h"

namespace tcp
{

/*****************************************************************************/
class TcpClient
{
public:

    TcpClient(bool isWebSocket = false);
    ~TcpClient() { }

    void SetWebSocket(bool enable);
    void SetSecured(bool enable);
    bool Initialize(int timeout = 0);
    bool Connect(const std::string &host, const int port);
    bool IsConnected()
    {
        return mSocket.IsConnected();
    }

    std::string BuildWebSocketHandshake(const std::string &path);
    void SetWebSocketUri(const std::string &uri);

    bool Send(const std::string &input);
    bool RecvWithTimeout(std::string &output, size_t max_size, uint32_t timeout_ms);
    void Close();

private:
    std::string mHost;
    std::uint16_t mPort;
    bool mIsWebSocket = false;
    bool mIsSecured = false;
    std::string mWsUri;
    TlsClient mTls;
    TcpSocket mSocket;
};

} // namespace tcp

#endif // TCPCLIENT_H

//=============================================================================
// End of file TcpClient.h
//=============================================================================
