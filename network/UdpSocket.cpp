#include "UdpSocket.h"

#include <string.h>

UdpSocket::UdpSocket()
{

}

bool UdpSocket::SetTimeout(uint32_t timeout)
{
    bool success = false;
    if (timeout > 0)
    {
        // LINUX
        struct timeval tv;
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        int ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        success = (ret == 0);
    }
    return success;
}


void UdpSocket::CreateClient()
{
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
    }
}

void UdpSocket::CreateServer(uint16_t port)
{
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
    }
}

int UdpSocket::WaitForData(UdpPeer &peer)
{
    char buffer[1024]; // FIXME: make this size external
    socklen_t len = sizeof(peer.addr);  //len is value/resuslt

    int n = recvfrom(sockfd, buffer, sizeof(buffer), MSG_WAITALL, &peer.addr, &len);
    if (n > 0)
    {
        peer.data.assign(buffer, n);
    }
    return n;
}

int UdpSocket::SendTo(const UdpPeer &peer, const uint8_t *data, uint32_t size)
{
    return sendto(sockfd, reinterpret_cast<const char *>(data), size, 0, &peer.addr, sizeof(peer.addr));
}

void UdpSocket::SetBroadcast()
{
    char broadcast = '1';

    if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,& broadcast,sizeof(broadcast)) < 0)
    {
#ifdef USE_UNIX_OS
        ::shutdown(sockfd, SHUT_RDWR);
        ::close(sockfd);
#else
        ::shutdown(sockfd, SD_BOTH);
        ::closesocket(sockfd);
#endif
    }
}
