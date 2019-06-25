/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#include "TcpServer.h"
#include "Log.h"

#include <sys/epoll.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>


#define MAXEVENTS 64

namespace tcp
{

/*****************************************************************************/
TcpServer::TcpServer(IEvent &handler)
    : mInitialized(false)
    , mEventHandler(handler)
    , mReceiveFd(-1)
    , mSendFd(-1)
{

}
/*****************************************************************************/
TcpServer::~TcpServer()
{
    Stop();
}
/*****************************************************************************/
bool TcpServer::Start(std::int32_t maxConnections, bool localHostOnly, std::uint16_t tcpPort, std::uint16_t wsPort)
{
    Stop();

    bool valid = mTcpServer.CreateServer(tcpPort, localHostOnly, maxConnections);

    if (valid)
    {
        if (wsPort > 0U)
        {
            valid = mWsServer.CreateServer(wsPort, localHostOnly, maxConnections);
        }

        if (valid)
        {
            // Create the thread the first time only
            if (!mInitialized)
            {
                mThread = std::thread(&TcpServer::Run, this);
                mInitialized = true;
            }
        }
        else
        {
             std::cerr << "Websocket Server creation failure (port maybe not free)" << std::endl;
        }
    }
    else
    {
        std::cerr << "TCP Server creation failure (port maybe not free)" << std::endl;
    }
    return mInitialized;
}
/*****************************************************************************/
void TcpServer::Stop()
{
    if (mSendFd > 0)
    {
#ifdef USE_WINDOWS_OS
        _write(mSendFd, "1", 1);
#else
        (void) write(mSendFd, "1", 1);
#endif
    }
    mTcpServer.Close();
    mWsServer.Close();
    Join();
}
/*****************************************************************************/
void TcpServer::Join()
{
    if (mInitialized)
    {
        mThread.join();
        mInitialized = false;
    }
}
/*****************************************************************************/
// s must be a valid socket, otherwise the returning string is null
std::string TcpServer::GetPeerName(int s)
{
    std::stringstream ss;
    socklen_t len;
    struct sockaddr_storage addr;
    std::string ipstr;
    int port = 0;

    len = sizeof(addr);
    int ret = getpeername(s, reinterpret_cast<struct sockaddr*>(&addr), &len);
    if (ret == 0)
    {
        ipstr = TcpSocket::ToString(reinterpret_cast<struct sockaddr*>(&addr)); // point to an internal static buffer
        ss << ipstr << ":" << port;
    }
    else
    {
        ss << "Cannot get peer name!";
    }

    return ss.str();
}
/*****************************************************************************/
void TcpServer::Run()
{
    bool end_server = false;
    struct epoll_event events[MAXEVENTS];
    struct epoll_event ev;

    //************************************************************
    // Initialize the epoll stuff
    //************************************************************
    mEpollFd = epoll_create1(0);
    if (mEpollFd == -1) {
        perror("epoll_create1");
        exit(1);
    }

    // Add TCP server
    ev.data.fd = mTcpServer.GetSocket();
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mTcpServer.GetSocket(), &ev) == -1)
    {
        perror("epoll_ctl");
        exit(1);
    }

    // Add Websocket server
    ev.data.fd = mWsServer.GetSocket();
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mWsServer.GetSocket(), &ev) == -1)
    {
        perror("epoll_ctl");
        exit(1);
    }


#ifdef USE_UNIX_OS
    /*************************************************************/
    /* Use a pipe on Unix to gracefully quit the select(),       */
    /* close the socket and finally exit the thread              */
    /*************************************************************/
    int pipefd[2];
    if (pipe(pipefd) != 0)
    {
        std::cout << "Pipe creation error, exiting ..." << std::endl;
        return;
    }

    mReceiveFd = pipefd[0];
    mSendFd = pipefd[1];
    fcntl(mSendFd, F_SETFL, O_NONBLOCK);

    ev.data.fd = mReceiveFd;
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mReceiveFd, &ev) == -1)
    {
        perror("epoll_ctl");
        exit(1);
    }

#endif

    /*************************************************************/
    /* Initialize the timeval struct to N minutes.  If no        */
    /* activity after N minutes this program will end.           */
    /* 0 means unlimited (== no timeout)                         */
    /*************************************************************/
    //   timeout.tv_sec  = 0; //10 * 60;
    //   timeout.tv_usec = 0;

    /*************************************************************/
    /* Loop waiting for incoming connects or for incoming data   */
    /* on any of the connected sockets.                          */
    /*************************************************************/
    do
    {
        /**********************************************************/
        /* Call select() and wait N minutes for it to complete.   */
        /**********************************************************/
        //    printf("Waiting on select()...\n");
        int n = epoll_wait(mEpollFd, events, MAXEVENTS, -1);

        if (n < 0)
        {
            if (!TcpSocket::AnalyzeSocketError("epoll_wait"))
            {
                end_server = true;
                mEventHandler.ServerTerminated(IEvent::WAIT_SOCK_FAILED);
            }
        }
        else
        {
            for (int i = 0; i < n; i++)
            {
                if (events[i].events & EPOLLERR)
                {
                    /* An error has occured on this fd, or the socket is not
                    * ready for reading (why were we notified then?) */
                    fprintf (stderr, "epoll error. events=%u\n", events[i].events);
                    int       error = 0;
                    socklen_t errlen = sizeof(error);
                    if (getsockopt(events[i].data.fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen) == 0)
                    {
                        printf("error = %s\n", strerror(error));
                    }

                    close (events[i].data.fd);
                    continue;
                }
                else if (events[i].events & EPOLLRDHUP)
                {
                    std::cout << "EPOLLRDHUP on " << events[i].data.fd << std::endl;
                    continue;
                }
                else if (mReceiveFd == events[i].data.fd)
                {
                    end_server = true;
                    mEventHandler.ServerTerminated(IEvent::CLOSED);

                }
                else if (mTcpServer.GetSocket() == events[i].data.fd)
                {
                    if (mTcpServer.IsValid())
                    {
                        /****************************************************/
                        /* This is the listening socket                     */
                        /****************************************************/
                        mMutex.lock();
                        IncommingConnection(false);
                        mMutex.unlock();
                    }

                }
                else if (mWsServer.GetSocket() == events[i].data.fd)
                {
                    if (mWsServer.IsValid())
                    {
                        /****************************************************/
                        /* This is the listening socket                     */
                        /****************************************************/
                        mMutex.lock();
                        IncommingConnection(true);
                        mMutex.unlock();
                    }

                }
                else
                {
                    mMutex.lock();

                    // Scan for already connected clients
                    for (size_t j = 0; j < mClients.size(); j++)
                    {
                        if (mClients[j].peer.socket == events[i].data.fd)
                        {
                            /****************************************************/
                            /* This is not the listening socket, therefore an   */
                            /* existing connection must be readable             */
                            /****************************************************/
                            IncommingData(mClients[j]);
                        }
                    }

                    UpdateClients(); // refresh status, manage proper closing if necessary
                    mMutex.unlock();
                }
            }
        }

    }
    while (end_server == false);

    /*************************************************************/
    /* Cleanup all of the sockets that are open                  */
    /*************************************************************/
    mMutex.lock();
    for (size_t i = 0; i < mClients.size(); i++)
    {
        if (mClients[i].IsConnected())
        {
            TcpSocket::Close(mClients[i].peer);
        }
    }

    mClients.clear();
    mMutex.unlock();
}
/*****************************************************************************/
void TcpServer::IncommingConnection(bool isWebSocket)
{
    int new_sd;
    bool webSocket = false;

    //    printf("  Listening socket is readable\n");
    /*************************************************/
    /* Accept all incoming connections that are      */
    /* queued up on the listening socket before we   */
    /* loop back and call select again.              */
    /*************************************************/
    do
    {
        /**********************************************/
        /* Accept each incoming connection.  If       */
        /* accept fails with EWOULDBLOCK, then we     */
        /* have accepted all of them.  Any other      */
        /* failure on accept will cause us to end the */
        /* server.                                    */
        /**********************************************/
        if (isWebSocket)
        {
            new_sd = mWsServer.Accept();
            webSocket = true;
        }
        else
        {
            new_sd = mTcpServer.Accept();
        }

        if (new_sd >= 0)
        {
            TcpSocket::SetNonBlocking(new_sd);
            Conn newPeer(new_sd, webSocket);
            // Save peers descriptor
            mClients.push_back(newPeer);

            /**********************************************/
            /* Add the new incoming connection to the     */
            /* master read set                            */
            /**********************************************/
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = new_sd;
            if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, new_sd, &ev) == -1)
            {
                perror("Cannot add socket to epoll");
            }

            if (!webSocket)
            {
                // Signal a new client only if not a web socket (need a handshake before considering it is connected)
                mEventHandler.NewConnection(newPeer);
            }
        }

        /**********************************************/
        /* Loop back up and accept another incoming   */
        /* connection                                 */
        /**********************************************/
    }
    while (new_sd >= 0);
}
/*****************************************************************************/
void TcpServer::IncommingData(Conn &conn)
{
    TcpSocket socket(conn.peer);

    // printf("  Descriptor %d is readable\n", in_sock);
    /*************************************************/
    /* Receive all incoming data on this socket      */
    /* before we loop back and call select again.    */
    /*************************************************/

    /**********************************************/
    /* Receive data on this connection until the  */
    /* recv fails with EWOULDBLOCK.  If any other */
    /* failure occurs, we will close the          */
    /* connection.                                */
    /**********************************************/

    bool hasData = socket.Recv();
    if (hasData)
    {
        if (conn.peer.isWebSocket)
        {
            if (conn.IsClosed())
            {
                if (socket.ProceedWsHandshake())
                {
                    // Websocket handshake success, warn the application
                    conn.state = Conn::cStateConnected;
                    mEventHandler.NewConnection(conn);
                    hasData = false; // Handshake is not application data
                }
                else
                {
                    TLogError("Websocket handshake failure.");
                }
            }
            else
            {
                // Assume we are in the connected state, manage the frame to extract the data
                hasData = socket.DecodeWsData(conn);
            }
        }
        else
        {
            // Transfer the received data to to connection paypload
            socket.DeliverData(conn);
        }

        if (hasData)
        {
            mEventHandler.ReadData(conn);
        }
    }
    else
    {
        conn.state = Conn::cStateDeleteLater;
    }
}
/*****************************************************************************/
void TcpServer::UpdateClients()
{
    bool cleanUp = true;
    /*************************************************/
    /* Check to see if the connection has been       */
    /* closed by the client.                         */
    /* This clean up process includes removing the   */
    /* descriptor from the master set and            */
    /* determining the new maximum descriptor value  */
    /* based on the bits that are still turned on in */
    /* the master set.                               */
    /*************************************************/

    while (cleanUp)
    {
        bool haveDeleted = false;
        for (size_t i = 0; i < mClients.size(); i++)
        {
            Conn conn = mClients[i];
            if (conn.state == Conn::cStateDeleteLater)
            {
                mClients.erase(mClients.begin() + i);

                // Signal the disconnection
                // In case of the websocket, only warn upper layers if the handshake process has been done
                if (conn.IsConnected())
                {
                    mEventHandler.ClientClosed(conn);
                }

                // Remove the socket from epoll
                if (epoll_ctl(mEpollFd, EPOLL_CTL_DEL, conn.peer.socket, nullptr) == -1)
                {
                    perror("Cannot remove socket from epoll");
                }

                // Finally close it
                TcpSocket::Close(conn.peer);

                haveDeleted = true;
                break; // we have modified the array so we can't continue, leave the loop!!
            }
        }
        cleanUp = haveDeleted; // continue cleanup
    }

}


} // namespace tcp

//=============================================================================
// End of file TcpServer.cpp
//=============================================================================
