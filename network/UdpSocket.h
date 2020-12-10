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

    bool SetAddress(const std::string &host, uint16_t port);
};

class UdpSocket
{
public:
    UdpSocket();
    void CreateServer(uint16_t port);

    int WaitForData(UdpPeer &peer);
    int SendTo(const UdpPeer &peer, const uint8_t *data, uint32_t size);

    void SetBroadcast();

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
