/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "TcpSocket.h"

namespace tcp
{

/*****************************************************************************/
class TcpClient : public TcpSocket
{
public:

    TcpClient();
    ~TcpClient() { }

    bool Initialize(int timeout = 0);
    bool Connect(const std::string &host, const int port);
    bool WebSocketHandshake(const std::string &path);
};

} // namespace tcp

#endif // TCPCLIENT_H

//=============================================================================
// End of file TcpClient.h
//=============================================================================
