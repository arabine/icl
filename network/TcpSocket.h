/*=============================================================================
 * TarotClub - TcpSocket.h
 *=============================================================================
 * Multi OS simple TCP socket wrapper class
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <cstdint>
#include <string>
#include <vector>

#ifdef USE_UNIX_OS

// FIXME: use __GNUC__ macro for GCC under Linux
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>

typedef std::int32_t  SocketType;
static const std::int32_t cSocketInvalid = -1;

#endif

#ifdef USE_WINDOWS_OS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _MSC_VER
#include <io.h>
#endif

#include <ws2tcpip.h>
#include <windows.h>

#ifdef __MINGW32__
#include <unistd.h>
#endif

typedef SOCKET  SocketType;
static const SOCKET cSocketInvalid = INVALID_SOCKET;

#endif // USE_WINDOWS_OS

namespace tcp
{

/*****************************************************************************/
/**
 * @brief Simple wrapper around the socket
 */
struct Peer
{      
    Peer()
        : socket(cSocketInvalid)
        , isWebSocket(false)
    {

    }

    Peer(std::int32_t s, bool ws)
        : socket(s)
        , isWebSocket(ws)
    {

    }

    bool IsValid() const
    {
        return socket != cSocketInvalid;
    }

    inline bool operator ==(const Peer &rhs)
    {
        return (socket == rhs.socket);
    }

    inline Peer & operator =(const Peer &rhs)
    {
       socket = rhs.socket;
       isWebSocket = rhs.isWebSocket;
       return *this;
    }

    SocketType socket;
    bool isWebSocket;
};

/*****************************************************************************/
/**
 * @brief Connection state with the server, holds incoming data
 */
struct Conn
{
    static const std::uint8_t cStateClosed      = 0U;
    static const std::uint8_t cStateConnected   = 1U;
    static const std::uint8_t cStateDeleteLater = 2U;

    Conn()
        : state(cStateClosed)
    {

    }

    Conn(std::int32_t s, bool ws)
        : peer(s, ws)
        , state(cStateClosed)
    {

    }

    Conn(const Peer &peer)
        : peer(peer)
        , state(cStateClosed)
    {

    }

    bool IsClosed() const { return state == cStateClosed; }
    bool IsConnected()
    {
        bool connected = false;
        if (peer.IsValid())
        {
            if (peer.isWebSocket)
            {
                if (state != cStateClosed)
                {
                    connected = true;
                }
            }
            else
            {
                connected = true;
            }
        }
        return connected;
    }

    inline bool operator ==(const SocketType &rhs)
    {
        return (peer.socket == rhs);
    }

    inline Conn & operator =(const SocketType &rhs)
    {
       peer.socket = rhs;
       return *this;
    }

    inline bool operator >(const SocketType &rhs)
    {
        return (peer.socket > rhs);
    }

    inline bool operator <(const Conn &rhs)
    {
        return (peer.socket < rhs.peer.socket);
    }

    Peer peer;
    std::uint8_t state;
    std::string payload;
};
/*****************************************************************************/
class TcpSocket
{
public:
    // Websocket opcodes, from http://tools.ietf.org/html/rfc6455
    static const std::uint8_t WEBSOCKET_OPCODE_CONTINUATION     = 0x00U;
    static const std::uint8_t WEBSOCKET_OPCODE_TEXT             = 0x01U;
    static const std::uint8_t WEBSOCKET_OPCODE_BINARY           = 0x02U;
    static const std::uint8_t WEBSOCKET_OPCODE_CONNECTION_CLOSE = 0x08U;
    static const std::uint8_t WEBSOCKET_OPCODE_PING             = 0x09U;
    static const std::uint8_t WEBSOCKET_OPCODE_PONG             = 0x0AU;

    // Larger values will read larger chunks of data at one time (without fragmentation)
    static const std::int32_t MAXRECV = 16*1024;

    TcpSocket();
    TcpSocket(const Peer &peer);
    virtual ~TcpSocket();

    // Getters
	SocketType  GetSocket() const
    {
        return mPeer.socket;
    }
    int  GetIPAddr() const
    {
        return mAddr.sin_addr.s_addr;
    }
    int  GetRealPort() const
    {
        return mPort;
    }

    // Helpers
    bool IsValid() const
    {
        return mPeer.IsValid();
    }

    bool Create();
    bool Bind(std::uint16_t port, bool localHostOnly);
    void Close();
    bool Listen(std::int32_t maxConnections) const;
    bool HostNameToIpAddress(const std::string &address, sockaddr_in &ipv4);
    int Accept() const;
    bool Recv();
    bool Recv(std::string &output);
    bool Send(const std::string &input);
    bool ProceedWsHandshake();
    bool DecodeWsData(Conn &conn);
    void DeliverData(Conn &conn);
    bool IsConnected();

    // Static
    static bool Initialize();
    // return true if socket has data waiting to be read
    static bool AnalyzeSocketError(const char* context);
    static void SetNonBlocking(SocketType socket);
    static bool Send(const std::string &input, const Peer &peer);
    static void Close(Peer &peer);
    static std::string BuildWsFrame(std::uint8_t opcode, const std::string &data);
    static bool Recv(std::string &output, const Peer &peer);
    static bool SendToSocket(const std::string &input, const Peer &peer);

    //Convert a struct sockaddr address to a string, IPv4 and IPv6
	static std::string ToString(const struct sockaddr *sa);

protected:
    std::string mHost;
    std::uint16_t mPort;
    Peer mPeer;
    std::string mBuff;
    sockaddr_in mAddr;
    static bool mOneTimeInit;

private:
    static std::string WsOpcodeToString(std::uint8_t opcode);
};

} // namespace tcp

#endif // TCPSOCKET_H

//=============================================================================
// End of file TcpSocket.h
//=============================================================================
