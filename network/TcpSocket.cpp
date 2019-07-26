/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#include "TcpSocket.h"
#include "WebSocket.h"
#include "Log.h"

#include <errno.h>  // errno, just like it says.
#include <fcntl.h>  // symbolic names for socket flags.

#include <cstring>
#include <iostream>
#include <sstream>

namespace tcp
{

bool TcpSocket::mOneTimeInit = false;

#ifdef USE_WINDOWS_OS

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

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
 * @return true if not an error (retry later), false if it is a real error
 */
bool TcpSocket::AnalyzeSocketError(const char* context)
{
    bool ok = false;
    int e;

    // IN PROGRESS is for non-blocking connect

#ifdef USE_WINDOWS_OS

    e = WSAGetLastError();
    const char* msg = GetWinsockErrorString( e );

    if ((e == WSAEWOULDBLOCK) || (e == WSAEINPROGRESS))
    {
        ok = true;
    }
#else

    e = errno;
    const char* msg = strerror( e );
    if ((e == EAGAIN) // equals EWOULDBLOCK
        || (e == EINPROGRESS) || EINTR)
    {
        ok = true;
    }
#endif

    if (!ok)
    {
        std::stringstream ss;
        ss << "Socket error (" << e << ") in " << context << ": " << msg;
        TLogNetwork(ss.str());
    }

    return ok;
}
/*****************************************************************************/
bool TcpSocket::IsConnected()
{
    bool connected = false;
    int optval = -1;
    socklen_t optlen = sizeof (optval);

    if (getsockopt(mPeer.socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&optval), &optlen) == 0)
    {
        /**
         * getsockopt() puts the errno value
         * for connect into optval so 0 means
         * no-error.
         */
        if (optval == 0)
        {
            connected = true;
        }
    }

    return connected;
}
/*****************************************************************************/
bool TcpSocket::Send(const std::string &input, const Peer &peer)
{
    bool ret = false;

    if (peer.isWebSocket)
    {
        ret = SendToSocket(BuildWsFrame(WEBSOCKET_OPCODE_TEXT, input), peer);
    }
    else
    {
        ret = SendToSocket(input, peer);
    }

    return ret;
}
/*****************************************************************************/
std::string TcpSocket::BuildWsFrame(std::uint8_t opcode, const std::string &data)
{
    std::stringstream  writer;
    std::uint32_t data_len = static_cast<std::uint32_t>(data.size());
    std::uint8_t ws_header[10];
    std::uint32_t header_len = 0U;

    // We do not use fragmentation when sending data, so raise the FIN flag
    ws_header[0] = static_cast<std::uint8_t>(0x80U + (opcode & 0x0FU));

    // Frame format: http://tools.ietf.org/html/rfc6455#section-5.2
    if (data_len < 126)
    {
        // Inline 7-bit length field
        ws_header[1] = static_cast<std::uint8_t>(data_len);
        header_len = 2U;
    }
    else if (data_len <= 0xFFFF)
    {
        // 16-bit length field
        ws_header[1] = static_cast<std::uint8_t>(126);
        ws_header[2] = static_cast<std::uint8_t>((data_len >> 8) & 0xFFU);
        ws_header[3] = static_cast<std::uint8_t>(data_len & 0xFFU);
        header_len = 4U;
    }
    else
    {
        // 64-bit length field
        ws_header[1] = static_cast<std::uint8_t>(127);

        // hi part always zero (std::string is 32bits max!)
        ws_header[2] = 0U;
        ws_header[3] = 0U;
        ws_header[4] = 0U;
        ws_header[5] = 0U;
        // Low part
        ws_header[6] = static_cast<std::uint8_t>((data_len >> 24) & 0xFFU);
        ws_header[7] = static_cast<std::uint8_t>((data_len >> 16) & 0xFFU);
        ws_header[8] = static_cast<std::uint8_t>((data_len >> 8) & 0xFFU);
        ws_header[9] = static_cast<std::uint8_t>(data_len & 0xFFU);
        header_len = 10U;
    }

    // Finally, append our data
    return std::string(reinterpret_cast<char *>(&ws_header[0]), header_len) + data;
}
/*****************************************************************************/
bool TcpSocket::SendToSocket(const std::string &input, const Peer &peer)
{
    bool ret = true;
    size_t size = input.size();
    const char *buf = input.c_str(); // bytes are linear in the string memory, so no problem to get the pointer

    while( size > 0 )
    {
        //  MSG_NOSIGNAL = Do not generate SIGPIPE
        int n = ::send(peer.socket, buf, size, MSG_NOSIGNAL);
        if (n < 0)
        {
            ret = TcpSocket::AnalyzeSocketError("send()");
            break;
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
bool TcpSocket::ProceedWsHandshake()
{
    bool ret = false;
    // Process the handshake, upgrade into our own protocol
    WebSocketRequest ws;

    ws.Parse(mBuff);
    if (ws.IsValid())
    {
        // Trick here: send handshake using raw tcp, not with websocket framing
        TcpSocket::SendToSocket(ws.Upgrade(), mPeer);
        ret = true;
    }

    return ret;
}
/*****************************************************************************/
bool TcpSocket::Recv()
{
    return Recv(mBuff, mPeer);
}
/*****************************************************************************/
bool TcpSocket::Recv(std::string &output)
{
    return Recv(output, mPeer);
}
/*****************************************************************************/
/**
 * @brief TcpSocket::Recv
 * @return > 0 if data has been read
 */
bool TcpSocket::Recv(std::string &output, const Peer &peer)
{
    // Most likely, we will read a packet, or if the message
    // is very short, we will receive the entire message in
    // a short packet. But it might be a long one.

    long count = 0;
    ioctl(peer.socket, FIONREAD, &count);
    bool ret = false;

    if (count > 0)
    {
        char buff[MAXRECV];
        long n;
        do
        {
            n = ::recv(peer.socket, &buff[0], sizeof(buff), 0);

            if (n < 0)
            {
                // Other value than positive means end of recv, determine the reason
                ret = TcpSocket::AnalyzeSocketError("recv()");
            }
            else if (n > 0)
            {
                output.insert(output.size(), &buff[0], static_cast<size_t>(n)); // append data into our procol buffer
                count -= n;
                ret = true;
            }
        }
        while ((count > 0) && (n > 0));
      //  TLogNetwork("[SOCKET] Rcv size: " + std::to_string(output.size()));
    }
    else
    {
     //   TLogNetwork("[SOCKET] No data to read from socket.");
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
    peer.socket = cSocketInvalid;
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
        std::int32_t off = 0;
 //       int send_buffer = 16*1024;

        // Allows the socket to be bound to an address that is already in use.
        // For more information, see bind. Not applicable on ATM sockets.
        if ((setsockopt(mPeer.socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&on), sizeof(on)) == 0) &&
            (setsockopt(mPeer.socket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char *>(&on), sizeof(on)) == 0) &&
    //        (setsockopt(mPeer.socket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char *>(&send_buffer), sizeof(send_buffer)) == 0) &&
            (setsockopt(mPeer.socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char *>(&off), sizeof(off)) == 0))

        {
            ret = true;
        }
    }
    return ret;
}
/*****************************************************************************/
void TcpSocket::SetNonBlocking(SocketType socket)
{
#ifdef _WIN32
    unsigned long on = 1;
    ::ioctlsocket(socket, FIONBIO, &on);
#else
    int flags = ::fcntl(socket, F_GETFL, 0);

    if (flags < 0)
    {
        flags = 0;
    }

    ::fcntl(socket, F_SETFL, flags | O_NONBLOCK);
#endif
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
			mAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
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
/**
 * @brief Accept
 * @return The new socket descriptor, valid if >=0
 */
int TcpSocket::Accept() const
{
    int new_sd = ::accept(mPeer.socket, nullptr, nullptr);

//#ifdef _WIN32
//    (void) SetHandleInformation((HANDLE) new_sd, HANDLE_FLAG_INHERIT, 0);
//#else
//    ::fcntl(new_sd, F_SETFD, FD_CLOEXEC);
//#endif


    SetNonBlocking(new_sd);

    return new_sd;
}
/*****************************************************************************/
bool TcpSocket::Send(const std::string &input)
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

    if (getaddrinfo(address.c_str(), nullptr, &hints, &res) == 0)
    {
        for (p = res; p != nullptr; p = p->ai_next)
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
/*
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - +-------------------------------+
|                               |Masking-key, if MASK set to 1  |
+-------------------------------+-------------------------------+
| Masking-key (continued)       |          Payload Data         |
+-------------------------------- - - - - - - - - - - - - - - - +
:                     Payload Data continued ...                :
+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
|                     Payload Data continued ...                |
+---------------------------------------------------------------+
*/
bool TcpSocket::DecodeWsData(Conn &conn)
{
    // Return true if data reception is complete
    bool ret = false;

    // Having buf unsigned char * is important, as it is used below in arithmetic
    uint64_t i, len, mask_len = 0, header_len = 0, data_len = 0;

    std::uint32_t buf_len = static_cast<std::uint32_t>(mBuff.size());

    /* Extracted from the RFC 6455 Chapter 5-2
     *
    The length of the "Payload data", in bytes: if 0-125, that is the
    payload length.  If 126, the following 2 bytes interpreted as a
    16-bit unsigned integer are the payload length.  If 127, the
    following 8 bytes interpreted as a 64-bit unsigned integer (the
    most significant bit MUST be 0) are the payload length.  Multibyte
    length quantities are expressed in network byte order.  Note that
    in all cases, the minimal number of bytes MUST be used to encode
    the length, for example, the length of a 124-byte-long string
    can't be encoded as the sequence 126, 0, 124.  The payload length
    is the length of the "Extension data" + the length of the
    "Application data".  The length of the "Extension data" may be
    zero, in which case the payload length is the length of the
    "Application data". */
    if (buf_len >= 2)
    {
        len = mBuff[1] & 127;
        mask_len = mBuff[1] & 128 ? 4 : 0;
        if (len < 126 && buf_len >= mask_len)
        {
            data_len = len;
            header_len = 2 + mask_len;
        }
        else if (len == 126 && buf_len >= 4 + mask_len)
        {
            header_len = 4 + mask_len;
            uint8_t byte1 = static_cast<uint8_t>(mBuff[2]);
            uint8_t byte2 = static_cast<uint8_t>(mBuff[3]);
            data_len = (static_cast<uint64_t>(byte1) << 8) + byte2;
        }
        else if (buf_len >= 10 + mask_len)
        {
            header_len = 10 + mask_len;
            data_len = (uint64_t) (((uint64_t) htonl(* (uint32_t *) &mBuff[2])) << 32) + htonl(* (uint32_t *) &mBuff[6]);
        }
    }

    // frame_len = header_len + data_len;
    // Apply mask if necessary
    char *mask_ptr = (&mBuff[0] + header_len) - mask_len; // pointer to the mask located in the header
    if (mask_len > 0)
    {
        for (i = 0; i < data_len; i++)
        {
            mBuff[i + header_len] ^= mask_ptr[i % 4];
        }
    }

    std::uint8_t opcode = static_cast<uint8_t>(mBuff[0]) & 0xFU;
    bool FIN = (static_cast<uint8_t>(mBuff[0]) & 0x80U) == 0x80U;
    TLogNetwork("received opcode: " + WsOpcodeToString(opcode));

    /*
    Manage fragmentation here: extract from the RFC:

    EXAMPLE: For a text message sent as three fragments, the first
      fragment would have an opcode of 0x1 and a FIN bit clear, the
      second fragment would have an opcode of 0x0 and a FIN bit clear,
      and the third fragment would have an opcode of 0x0 and a FIN bit
      that is set.

      */
    if(opcode == TcpSocket::WEBSOCKET_OPCODE_PING)
    {
        Send(BuildWsFrame(TcpSocket::WEBSOCKET_OPCODE_PONG, std::string()));
    }
    else if (opcode == TcpSocket::WEBSOCKET_OPCODE_CONNECTION_CLOSE)
    {
        conn.payload.clear();
        conn.state = Conn::cStateDeleteLater;
    }
    else
    {
        if ((opcode == TcpSocket::WEBSOCKET_OPCODE_TEXT) ||
            (opcode == TcpSocket::WEBSOCKET_OPCODE_BINARY))
        {
            conn.payload.clear();
            conn.payload += mBuff.substr(header_len, data_len);
        }
        else if(opcode == TcpSocket::WEBSOCKET_OPCODE_CONTINUATION)
        {
            conn.payload += mBuff.substr(header_len, data_len);
        }

        if (FIN)
        {
            // We can deliver data to the consumer
            ret = true;
        }
    }

    return ret;
}
/*****************************************************************************/
void TcpSocket::DeliverData(Conn &conn)
{
    conn.payload = mBuff;
    mBuff.clear();
}
/*****************************************************************************/
std::string TcpSocket::WsOpcodeToString(std::uint8_t opcode)
{
    std::string ocString;
    if (opcode == TcpSocket::WEBSOCKET_OPCODE_CONTINUATION)
    {
        ocString = "WebSocket opcode: continuation";
    }
    else if (opcode == TcpSocket::WEBSOCKET_OPCODE_TEXT)
    {
        ocString = "WebSocket opcode: text data";
    }
    else if (opcode == TcpSocket::WEBSOCKET_OPCODE_BINARY)
    {
        ocString = "WebSocket opcode: binary data";
    }
    else if (opcode == TcpSocket::WEBSOCKET_OPCODE_CONNECTION_CLOSE)
    {
        ocString = "WebSocket opcode: connection close";
    }
    else if (opcode == TcpSocket::WEBSOCKET_OPCODE_PING)
    {
        ocString = "WebSocket opcode: ping";
    }
    else if (opcode == TcpSocket::WEBSOCKET_OPCODE_PONG)
    {
        ocString = "WebSocket opcode: pong";
    }
    else
    {
        ocString = "Unkown WebSocket opcode";
    }
    return ocString;
}

#ifdef __MINGW32__
// MinGW does not have inet_ntop() defined, emulate it
const char* inet_ntop(int af, const void* src, char* dst, int cnt){

    struct sockaddr_in srcaddr;

    memset(&srcaddr, 0, sizeof(struct sockaddr_in));
    memcpy(&(srcaddr.sin_addr), src, sizeof(srcaddr.sin_addr));

    srcaddr.sin_family = af;
    if (WSAAddressToStringA((struct sockaddr*) &srcaddr, sizeof(struct sockaddr_in), 0, dst, (LPDWORD) &cnt) != 0) {
        DWORD rv = WSAGetLastError();
        std::stringstream ss;
        ss << "WSAAddressToString(): " << rv;
        TLogError(ss.str());
        return nullptr;
    }
    return dst;
}
#endif // __MINGW32__

std::string TcpSocket::ToString(const struct sockaddr *sa)
{
	static const std::uint32_t cMaxSize = 45U; // enough for IPv6
	char str[cMaxSize];
	bool ok = true;

	switch (sa->sa_family) {
	case AF_INET:
		inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), str, cMaxSize);
		break;

	case AF_INET6:
		inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr), str, cMaxSize);
		break;

	default:
		ok = false;
		break;
	}

	std::string ip(str);
	if (!ok)
	{
		ip = "Unknown AF";
	}
	ip.resize(cMaxSize);

	return ip;
}


} // namespace tcp

//=============================================================================
// End of file TcpSocket.cpp
//=============================================================================
