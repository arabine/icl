/*=============================================================================
 * TarotClub - TcpSocket.cpp
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

#include "TcpSocket.h"
#include "Log.h"

#include <errno.h>  // errno, just like it says.
#include <fcntl.h>  // symbolic names for socket flags.

#include <cstring>
#include <iostream>
#include <sstream>

// Larger values will read larger chunks of data.
static const std::int32_t MAXRECV = 2048;


bool TcpSocket::mOneTimeInit = false;

#ifdef USE_WINDOWS_OS

const char* GetWinsockErrorString( int err )
{
    switch( err)
    {
    case 0:					return "No error";
    case WSAEINTR:			return "Interrupted system call";
    case WSAEBADF:			return "Bad file number";
    case WSAEACCES:			return "Permission denied";
    case WSAEFAULT:			return "Bad address";
    case WSAEINVAL:			return "Invalid argument";
    case WSAEMFILE:			return "Too many open sockets";
    case WSAEWOULDBLOCK:	return "Operation would block";
    case WSAEINPROGRESS:	return "Operation now in progress";
    case WSAEALREADY:		return "Operation already in progress";
    case WSAENOTSOCK:		return "Socket operation on non-socket";
    case WSAEDESTADDRREQ:	return "Destination address required";
    case WSAEMSGSIZE:		return "Message too long";
    case WSAEPROTOTYPE:		return "Protocol wrong type for socket";
    case WSAENOPROTOOPT:	return "Bad protocol option";
    case WSAEPROTONOSUPPORT:	return "Protocol not supported";
    case WSAESOCKTNOSUPPORT:	return "Socket type not supported";
    case WSAEOPNOTSUPP:		return "Operation not supported on socket";
    case WSAEPFNOSUPPORT:	return "Protocol family not supported";
    case WSAEAFNOSUPPORT:	return "Address family not supported";
    case WSAEADDRINUSE:		return "Address already in use";
    case WSAEADDRNOTAVAIL:	return "Can't assign requested address";
    case WSAENETDOWN:		return "Network is down";
    case WSAENETUNREACH:	return "Network is unreachable";
    case WSAENETRESET:		return "Net connection reset";
    case WSAECONNABORTED:	return "Software caused connection abort";
    case WSAECONNRESET:		return "Connection reset by peer";
    case WSAENOBUFS:		return "No buffer space available";
    case WSAEISCONN:		return "Socket is already connected";
    case WSAENOTCONN:		return "Socket is not connected";
    case WSAESHUTDOWN:		return "Can't send after socket shutdown";
    case WSAETOOMANYREFS:	return "Too many references, can't splice";
    case WSAETIMEDOUT:		return "Connection timed out";
    case WSAECONNREFUSED:	return "Connection refused";
    case WSAELOOP:			return "Too many levels of symbolic links";
    case WSAENAMETOOLONG:	return "File name too long";
    case WSAEHOSTDOWN:		return "Host is down";
    case WSAEHOSTUNREACH:	return "No route to host";
    case WSAENOTEMPTY:		return "Directory not empty";
    case WSAEPROCLIM:		return "Too many processes";
    case WSAEUSERS:			return "Too many users";
    case WSAEDQUOT:			return "Disc quota exceeded";
    case WSAESTALE:			return "Stale NFS file handle";
    case WSAEREMOTE:		return "Too many levels of remote in path";
    case WSASYSNOTREADY:	return "Network system is unavailable";
    case WSAVERNOTSUPPORTED:	return "Winsock version out of range";
    case WSANOTINITIALISED:	return "WSAStartup not yet called";
    case WSAEDISCON:		return "Graceful shutdown in progress";
    case WSAHOST_NOT_FOUND:	return "Host not found";
    case WSANO_DATA:		return "No host data of that type was found";
    }

    return "unknown";
}

#endif // USE_WINDOWS_OS

/**
 * @brief AnalyzeSocketError
 * @param context
 * @return -2 if not an error (retry later), -1 if it is a real error
 */
int TcpSocket::AnalyzeSocketError(const Peer &peer, const char* context)
{
    bool real_error = true;
    int e;
    int ret = -1;
#ifdef USE_WINDOWS_OS

    e = WSAGetLastError();
    const char* msg = GetWinsockErrorString( e );

    if (e == WSAEWOULDBLOCK)
    {
        real_error = false;
    }
#else

    e = errno;
    const char* msg = strerror( e );
    if (e == EAGAIN) // equals EWOULDBLOCK
    {
        real_error = false;
    }
#endif

    if (real_error)
    {
        std::stringstream ss;
        ss << "Socket error (" << e << ") in " << context << ": " << msg;
        TcpSocket(peer).Close();
        TLogNetwork(ss.str());
    }
    else
    {
        ret = -2;
    }

    return ret;
}
/*****************************************************************************/
int TcpSocket::AnalyzeSocketError(const char* context)
{
    return AnalyzeSocketError(mPeer, context);
}
/*****************************************************************************/
bool TcpSocket::Send(const std::string &input, const Peer &peer)
{
    bool ret = false;

    if (peer.isWebSocket)
    {
        ret = SendToSocket(BuildWsFrame(WEBSOCKET_OPCODE_BINARY, input), peer.socket);
    }
    else
    {
        ret = SendToSocket(input, peer.socket);
    }

    return ret;
}
/*****************************************************************************/
std::string TcpSocket::BuildWsFrame(std::uint8_t opcode, const std::string &data)
{
    std::stringstream  writer;
    std::uint32_t data_len = data.size();

    // We do not use fragmentation when sending data, so raise the FIN flag
    writer << static_cast<std::uint8_t>(0x80U + (opcode & 0x0FU));

    // Frame format: http://tools.ietf.org/html/rfc6455#section-5.2
    if (data_len < 126)
    {
        // Inline 7-bit length field
        writer << static_cast<std::uint8_t>(data_len);
    }
    else if (data_len <= 0xFFFF)
    {
        // 16-bit length field
        writer << static_cast<std::uint8_t>(126);
        writer << static_cast<std::uint16_t>(data_len);
    }
    else
    {
        // 64-bit length field
        writer << static_cast<std::uint8_t>(127);
        writer << static_cast<std::uint32_t>(0U); // hi part always zero (std::string is 32bits max!
        writer << static_cast<std::uint32_t>(data_len);
    }

    // Finally, append our data
    return writer.str() + data;
}
/*****************************************************************************/
bool TcpSocket::SendToSocket(const std::string &input, std::int32_t socket)
{
    bool ret = true;
    size_t size = input.size();
    const char *buf = input.c_str(); // bytes are linear in the string memory, so no problem to get the pointer

    while( size > 0 )
    {
        int n = ::send(socket, buf, size, 0);
        if (n < 0)
        {
            if (TcpSocket::AnalyzeSocketError(Peer(socket, false), "send()") == -1)
            {
                ret = false;
                break;
            }
        }
        else
        {
            size -= n;
            buf += n;
        }
    }

    return ret;
}
/*****************************************************************************/
void TcpSocket::Close(Peer &peer)
{
#ifdef USE_UNIX_OS
        ::shutdown(peer.socket, SHUT_RDWR);
        ::close(peer.socket);
#else
        ::shutdown(peer.socket, SD_BOTH);
        ::closesocket(peer.socket);
#endif
    peer.socket = -1;
}
/*****************************************************************************/
TcpSocket::TcpSocket()
    : mHost("127.0.0.1")
    , mPort(0U)
{
    std::memset(&mAddr, 0, sizeof(mAddr));
}
/*****************************************************************************/
TcpSocket::TcpSocket(const Peer &peer)
    : mHost("127.0.0.1")
    , mPort(0U)
    , mPeer(peer)
{
    memset(&mAddr, 0, sizeof(mAddr));
}
/*****************************************************************************/
TcpSocket::~TcpSocket()
{

}
/*****************************************************************************/
/**
 * @brief TcpSocket::Create
 * Server initialization
 * @return
 */
bool TcpSocket::Create()
{
    bool ret = false;
    mPeer.socket = socket(AF_INET, SOCK_STREAM, 0);

    if (IsValid())
    {
        std::int32_t on = 1;

        // Allows the socket to be bound to an address that is already in use.
        // For more information, see bind. Not applicable on ATM sockets.
        if ((setsockopt(mPeer.socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&on), sizeof(on)) == 0) &&
            (setsockopt(mPeer.socket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char *>(&on), sizeof(on)) == 0) &&
            (setsockopt(mPeer.socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char *>(&on), sizeof(on)) == 0))
        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
bool TcpSocket::SetBlocking(bool block)
{
    unsigned long on = 1;
    bool ret = false;

    if (block == true)
    {
        on = 0;
    }

#ifdef USE_UNIX_OS
    std::int32_t rc = ioctl(mPeer.socket, FIONBIO, (char *)&on);
#else
    std::int32_t rc = ioctlsocket(mPeer.socket, FIONBIO, &on);
#endif
    if (rc == 0)
    {
        ret = true;
    }
    return ret;
}
/*****************************************************************************/
bool TcpSocket::Bind(std::uint16_t port, bool localHostOnly)
{
    bool ret = false;

    memset(&mAddr, 0, sizeof(mAddr));

    if (IsValid())
    {
        mAddr.sin_family      = AF_INET;
        if (localHostOnly)
        {
            mAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        }
        else
        {
            mAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        }
        mAddr.sin_port        = htons(port);

        if (::bind(mPeer.socket,
                   reinterpret_cast<struct sockaddr *>(&mAddr),
                   sizeof(mAddr)) == 0)
        {
            ret = true;
            mPort = port;
        }
    }
    return ret;
}
/*****************************************************************************/
void TcpSocket::Close()
{
    if (IsValid())
    {
        Close(mPeer);
    }
}
/*****************************************************************************/
bool TcpSocket::Listen(std::int32_t maxConnections) const
{
    bool ret = false;

    if (IsValid())
    {
        if (::listen(mPeer.socket, maxConnections) == 0)
        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
bool TcpSocket::DataWaiting(std::uint32_t timeout)
{
    fd_set fds;
    FD_ZERO( &fds );
    FD_SET( mPeer.socket, &fds );

    struct timeval tv;
    tv.tv_sec = (long)timeout;
    tv.tv_usec = 0;

    bool ok = true;
    while(ok)
    {
        int r = select( mPeer.socket + 1, &fds, NULL, NULL, &tv);
        if (r < 0)
        {
            if (AnalyzeSocketError("select()") == -1)
            {
                ok = false;
            }
        }
        else
        {
            ok = false;
        }
    }

    if( FD_ISSET( mPeer.socket, &fds ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*****************************************************************************/
/**
 * @brief Accept
 * @return The new socket descriptor, valid if >=0
 */
int TcpSocket::Accept() const
{
    int new_sd = ::accept(mPeer.socket, NULL, NULL);

#ifdef _WIN32
    (void) SetHandleInformation((HANDLE) new_sd, HANDLE_FLAG_INHERIT, 0);
#else
    ::fcntl(new_sd, F_SETFD, FD_CLOEXEC);
#endif


#ifdef _WIN32
    unsigned long on = 1;
    ::ioctlsocket(new_sd, FIONBIO, &on);
#else
    int flags = ::fcntl(new_sd, F_GETFL, 0);
    ::fcntl(new_sd, F_SETFL, flags | O_NONBLOCK);
#endif

    return new_sd;
}
/*****************************************************************************/
bool TcpSocket::Connect(const std::string &host, const int port)
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

    }

    return ret;
}
/*****************************************************************************/
bool TcpSocket::Send(const std::string &input) const
{
    return Send(input, mPeer);
}
/*****************************************************************************/
/**
 * @brief TcpSocket::Initialize
 * One time initialization for the sub-system
 * @return
 */
bool TcpSocket::Initialize()
{
    if (!mOneTimeInit)
    {
        mOneTimeInit = true;
#ifdef USE_WINDOWS_OS
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
        {
            return false;
        }
#endif
    }

    return true;
}

/**
 * @brief TcpSocket::DnsToIpAddress
 *
 * Try to work out address from string. The string can be an IP or an domain address (eg: example.com)
 * Returns an empty string if bad.
 *
 * @param address
 * @return
 */
bool TcpSocket::HostNameToIpAddress(const std::string &address, sockaddr_in &ipv4)
{
    struct addrinfo hints, *res, *p;
    bool status = false;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(address.c_str(), NULL, &hints, &res) == 0)
    {
        for (p = res; p != NULL; p = p->ai_next)
        {
            // get the pointer to the address itself,
            // different fields in IPv4 and IPv6:
            // IPv4
            if (p->ai_family == AF_INET)
            {
                ipv4 = *(struct sockaddr_in *)(p->ai_addr);
                status = true;
            }
        }
        freeaddrinfo(res); // free the linked list
    }

    return status;
}
/*****************************************************************************/
std::int32_t TcpSocket::Recv(std::string &output)
{
    int result = 0; // changed from int to ssize_t
    char buffer[MAXRECV]; // book maximum space
    // Most likely, we will read a packet, or if the message
    // is very short, we will receive the entire message in
    // a short packet. But it might be a long one.
    result = ::recv(mPeer.socket, &buffer[0], MAXRECV, 0);

    if (result > 0)
    {
        output.assign(buffer, result);
    }
    else if (result < 0)
    {
        output.resize(0U); // resize to no size because of the error
        result = AnalyzeSocketError("recv()");
    }

    // return the status code, or the number of bytes read.
    return result;
}

//=============================================================================
// End of file TcpSocket.cpp
//=============================================================================
