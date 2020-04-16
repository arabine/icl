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
    int SendTo(const UdpPeer &peer, const std::string &data);

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
