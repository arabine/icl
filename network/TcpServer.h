/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include "TcpServerBase.h"
#include "Observer.h"
#include "ThreadQueue.h"
#include "HttpProtocol.h"
#include "Pool.h"

#ifdef USE_LINUX_OS
#include <sys/epoll.h>
#endif

namespace tcp
{

/*****************************************************************************/
/**
 * @brief The TcpServer class
 *
 * General purpose Tcp Server thread.
 * Events on the server socket are throwed through the IEvent interface that
 * must be implemented in the user side of this class. The event handler is
 * required in the constructor.
 */
class TcpServer
{
public:
    class IEvent
    {
    public:
        enum CloseType
        {
            WAIT_SOCK_FAILED,
            TIMEOUT,
            CLOSED
        };

        virtual ~IEvent() ;

        /**
         * @brief NewConnection
         * Called when a new TCP/IP connection has been created
         * @param socket
         */
        virtual void NewConnection(const tcp::Conn &conn) = 0;

        /**
         * @brief ReadData
         * Called when new data have been sent by the peer to the server
         * @param socket
         * @param data
         */
        virtual void ReadData(const tcp::Conn &conn) = 0;

        /**
         * @brief ClientClosed
         * Called when a client has closed its connection
         * @param socket
         */
        virtual void ClientClosed(const tcp::Conn &conn) = 0;

        /**
         * @brief ServerTerminated
         * Called when the server is about to shutdown (mainly because of an internal problem)
         */
        virtual void ServerTerminated(tcp::TcpServer::IEvent::CloseType type) = 0;
    };

    TcpServer(IEvent &handler);

    virtual ~TcpServer(void);

    /**
     * @brief Start the Tcp thread server, with an optional WebSocket port to listen at
     * @param tcpPort
     * @param maxConnections
     * @param localHostOnly
     * @param wsPort
     * @return
     */
    bool Start(std::int32_t maxConnections, bool localHostOnly, std::uint16_t tcpPort, std::uint16_t wsPort = 0U);
    void Stop();
    void Join();
    bool IsStarted() { return mInitialized; }
    bool SendToAllClients(const std::string &data, bool wsOnly);

private:
    TcpServerBase   mTcpServer;
    TcpServerBase   mWsServer;
    std::thread mThread;
    std::vector<Conn> mClients;
    std::mutex mMutex; // To protect mClients
    bool mInitialized;
    IEvent     &mEventHandler;

#if defined(USE_WINDOWS_OS) || defined(USE_APPLE_OS)
    SocketType  mMaxSd;
    fd_set mMasterSet;
    void UpdateMaxSocket();
#endif

    thread_pool mPool;
    int mEpollFd;

    // Pipes on Linux to properly close the socket and quit select()
    int mReceiveFd;
    int mSendFd;

    void Run();
    void IncommingConnection(bool isWebSocket);
    void IncommingData(Conn &conn);
    void DeliverWsData(Conn &conn, std::string &buf);
    std::string WsOpcodeToString(std::uint8_t opcode);
    void UpdateClients();
    void RemoveClient(int fd);
};



} // namespace tcp

#endif // TCP_SERVER_H

//=============================================================================
// End of file TcpServer.h
//=============================================================================
