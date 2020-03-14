#ifndef HTTPPROTOCOL_H
#define HTTPPROTOCOL_H

#include <cstdint>
#include <string>
#include <regex>
#include <sstream>

struct HttpRequest
{
    std::string method;
    std::string query;
    std::string protocol;
    std::map<std::string, std::string> params;
    std::map<std::string, std::string> headers;
    std::string body;
};

class HttpProtocol
{
public:
    HttpProtocol();

    bool ParseHeader(const std::string &payload, HttpRequest &request);
    std::string GenerateRequest(const HttpRequest &request);
    std::string GenerateHttpJsonResponse(const std::string &data);

private:
    void ParseUrlParameters(HttpRequest &request);
};

#endif // HTTPPROTOCOL_H
