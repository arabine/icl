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
TcpClient::TcpClient()
{

}
/*****************************************************************************/
bool TcpClient::Initialize(int timeout)
{
    bool ok = false;
    /*************************************************************/
    /* Create an AF_INET stream socket                           */
    /*************************************************************/
    if (Create())
    {
        ok = true;
        SetNonBlocking(GetSocket());

        if (timeout > 0)
        {
            // LINUX
            struct timeval tv;
            tv.tv_sec = timeout / 1000;
            tv.tv_usec = (timeout % 1000) * 1000;
            int success = setsockopt(mPeer.socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

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

    if (HostNameToIpAddress(host, mAddr))
    {
   //     mAddr.sin_family = AF_INET;
   //     mAddr.sin_port = htons(port);
   //     mAddr.sin_addr.s_addr = inet_addr(mHost.c_str()); // Convert a string IPv4 into a structure

        if (mAddr.sin_addr.s_addr == INADDR_NONE)
        {
            //printf("inet_addr failed and returned INADDR_NONE\n");
            //     WSACleanup();
            return false;
        }
        if (mAddr.sin_addr.s_addr == INADDR_ANY)
        {
            //    printf("inet_addr failed and returned INADDR_ANY\n");
            return false;
        }
        mAddr.sin_port = htons(mPort);
        int retcode = ::connect(mPeer.socket, reinterpret_cast<sockaddr *>(&mAddr), sizeof(mAddr));
        if (retcode == 0)
        {
            ret = true;
        }
        else if (retcode < 0)
        {
            /* This is what we expect for non-blocking connect. */
            if (TcpSocket::AnalyzeSocketError("connect()"))
            {
                fd_set wrfds;
                struct timeval tout;

                tout.tv_sec = 5;
                tout.tv_usec = 0;

                FD_ZERO(&wrfds);

                FD_SET(mPeer.socket, &wrfds);

                // Wait for connection ...
                int result = select(mPeer.socket + 1, NULL, &wrfds, NULL, &tout); // FD_SETSIZE ?

                // Do we have one connection?
                if (result == 1)
                {
                    ret = IsConnected();
                }
            }
            else
            {
                // error, close the socket
                Close();
            }
        }
    }

    return ret;
}
/*****************************************************************************/
bool TcpClient::WebSocketHandshake(const std::string &path)
{
    // Si c'est un websocket, on envoie la demande WebSocket
    WebSocketRequest ws;

    ws.request.method = "POST";
    ws.request.protocol = "HTTP/1.1";
    ws.request.query = path; // eg: "/api/v1/machines/upstream/data"
    ws.request.body = "";
    ws.request.headers["Host"] = "www." + mHost;
    ws.request.headers["Content-length"] = std::to_string(0);

    if (Send(HttpProtocol::GenerateWebSocketRequest(ws)))
    {
        std::string output;
        if (RecvWithTimeout(output, 512, 5000))
        {
            std::cout << output << std::endl;
        }
    }
    return true;
}


} // namespace tcp

//=============================================================================
// End of file TcpClient.cpp
//=============================================================================
