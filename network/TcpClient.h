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

    bool Initialize();
    bool Connect(const std::string &host, const int port);

    /**
     * @brief DataWaiting
     * @param timeout in milliseconds
     * @return
     */
    bool DataWaiting(std::uint32_t timeout);
};

} // namespace tcp

#endif // TCPCLIENT_H

//=============================================================================
// End of file TcpClient.h
//=============================================================================
