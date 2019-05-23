/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#include "TcpServerBase.h"

namespace tcp
{

TcpServerBase::TcpServerBase()
{

}
/*****************************************************************************/
bool TcpServerBase::CreateServer(std::uint16_t port, bool localHostOnly, std::int32_t maxConnections)
{
    /*************************************************************/
    /* Create an AF_INET stream socket to receive incoming       */
    /* connections on                                            */
    /*************************************************************/
    if (!Create())
    {
        return false;
    }

    /*************************************************************/
    /* Set socket to be non-blocking.  All of the sockets for    */
    /* the incoming connections will also be non-blocking since  */
    /* they will inherit that state from the listening socket.   */
    /*************************************************************/
    SetNonBlocking(GetSocket());

    /*************************************************************/
    /* Bind the socket                                           */
    /*************************************************************/
    if (!Bind(port, localHostOnly))
    {
        Close();
        return false;
    }

    /*************************************************************/
    /* Set the listen back log                                   */
    /*************************************************************/
    if (!Listen(maxConnections))
    {
        Close();
        return false;
    }

    return true;
}

} // namespace tcp

