#ifndef HTTPPROTOCOL_H
#define HTTPPROTOCOL_H

#include <cstdint>
#include <string>
#include <regex>
#include <sstream>
#include <map>

struct HttpRequest
{
    std::string method;  // "GET" "PUT"
    std::string query;  //  /path/to/go
    std::string protocol;  // HTTP/1.1
    std::map<std::string, std::string> params;
    std::map<std::string, std::string> headers;
    std::string body;
};


struct HttpReply
{
    std::string protocol;
    std::string code;
    std::string result;
    std::map<std::string, std::string> params;
    std::map<std::string, std::string> headers;
    std::string body;
};

struct WebSocketRequest
{
    HttpRequest request;
    std::string key;
    std::string protocol;

    std::string Accept();
    std::string Upgrade();
};

class HttpProtocol
{
public:
    HttpProtocol();

    static bool ParseRequestHeader(const std::string &payload, HttpRequest &request);
    static bool ParseReplyHeader(const std::string &payload, HttpReply &reply);
    static std::string GenerateRequest(const HttpRequest &request);
    static std::string GenerateHttpJsonResponse(const std::string &data);
    static bool ParseWebSocketRequest(const std::string &payload, WebSocketRequest &ws);
    static bool GetRequestHeaderValue(const HttpRequest &request, const std::string &option, std::string &value);
    static bool GetReplyHeaderValue(const HttpReply &reply, const std::string &option, std::string &value);
    static std::string GenerateWebSocketRequest(WebSocketRequest &ws);
private:
    static void ParseUrlParameters(HttpRequest &request);
};

#endif // HTTPPROTOCOL_H
