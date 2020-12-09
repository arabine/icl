#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <cstdint>
#include <string>
#include <vector>
#include "string.h"

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
#include <poll.h>


#endif

#ifdef USE_WINDOWS_OS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _MSC_VER
#include <io.h>

#include <WS2tcpip.h>
#include <Windows.h>
#include <WinSock2.h>

#endif


#ifdef __MINGW32__
#include <unistd.h>

#include <ws2tcpip.h>
#include <windows.h>
#endif

typedef SOCKET  SocketType;
static const SOCKET cSocketInvalid = INVALID_SOCKET;

#endif // USE_WINDOWS_OS

#ifndef bzero
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#endif



struct UdpPeer
{
    struct sockaddr addr;
    std::string data;

    UdpPeer() {
        memset(&addr, 0, sizeof(addr));
    }

    bool SetAddress(const std::string &host, uint16_t port)
    {
        struct sockaddr_in     servaddr;
        struct hostent *he;

        if ((he=gethostbyname(host.c_str())) == NULL) {  /* get the host info */
            return false;
        }

        memset(&servaddr, 0, sizeof(servaddr));

        // Filling server information
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);

        /* build the server's Internet address */
         // bzero((char *) &serveraddr, sizeof(serveraddr));
        memcpy((char *)&servaddr.sin_addr.s_addr, (char *)he->h_addr, he->h_length);
        bzero(&(servaddr.sin_zero), 8);     /* zero the rest of the struct */

        memcpy(&addr, &servaddr, sizeof(addr));
        return true;
    }
};

class UdpSocket
{
public:
    UdpSocket();
    void CreateServer(uint16_t port);

    int WaitForData(UdpPeer &peer);
    int SendTo(const UdpPeer &peer, const uint8_t *data, uint32_t size);

    void CreateClient();

    /**
     * @brief SetTimeout
     * @param timeout in ms
     * @return
     */
    bool SetTimeout(uint32_t timeout);
private:
    int sockfd;
};

#endif // UDPSOCKET_H
