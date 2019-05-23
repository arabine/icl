/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef WEB_SOCKET_H
#define WEB_SOCKET_H

#include <regex>

#include "Sha1.h"
#include "Base64.h"

/*****************************************************************************/
/**
 * @brief WebSocketRequest
 */
class WebSocketRequest
{
public:
    void Parse(const std::string &msg);
    bool IsProtocol(const std::string &proto);
    std::string Accept();
    std::string Upgrade();
    bool IsValid();

private:
    std::string Match(const std::string &msg, const std::string &patternString);

    std::string resource;
    std::string host;
    std::string origin;
    std::string key;
    std::string protocol;
};

#endif // WEB_SOCKET_H

//=============================================================================
// End of file WebSocket.h
//=============================================================================
