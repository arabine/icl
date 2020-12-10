#include "HttpProtocol.h"
#include "ShaOne.h"
#include "Base64Util.h"
#include "Util.h"

static const std::string cSupportedMethods[] = { "GET", "POST" };


HttpProtocol::HttpProtocol()
{

}


void HttpProtocol::ParseUrlParameters(HttpRequest &request)
{
    std::istringstream iss(request.query);

    std::string url;

    if(!std::getline(iss, url, '?')) // remove the URL part
    {
        return;
    }

    // store query key/value pairs in a map
    std::string keyval, key, val;

    while(std::getline(iss, keyval, '&')) // split each term
    {
        std::istringstream iss(keyval);

        // split key/value pairs
        if(std::getline(std::getline(iss, key, '='), val))
        {
            request.params[key] = val;
        }
    }
}

bool HttpProtocol::ParseRequestHeader(const std::string &payload, HttpRequest &request)
{
 //   std::string request = "GET /index.asp?param1=hello&param2=128 HTTP/1.1";
    // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF

    std::string line;
    std::istringstream iss(payload);

    bool valid = false;

    // separate the first 3 main parts
    if (std::getline(iss, line))
    {
        line.pop_back(); // remove \r
        std::vector<std::string> parts = Util::Split(line, " ");

        if (parts.size() == 3)
        {
            request.method = parts[0];
            request.query = parts[1];
            request.protocol = parts[2];


            for (const auto & s : cSupportedMethods)
            {
                if (request.method == s)
                {
                    valid = true;
                }
            }
        }
    }

    if (valid)
    {
        // Parse optional URI parameters
        ParseUrlParameters(request);

        // Continue parsing the header
        std::string::size_type index;

        while (std::getline(iss, line))
        {
            if (line != "\r")
            {
                line.pop_back();
                index = line.find(':', 0);
                if(index != std::string::npos)
                {
                    // Convert all header options to lower case (header params are case insensitive in the HTTP spec
                    std::string option = line.substr(0, index);
                    std::transform(option.begin(), option.end(), option.begin(), ::tolower);
                    request.headers.insert(std::make_pair(option, line.substr(index + 1)));
                }
            }
            else
            {
                break; // detected HTTP separator \r\n between header and body
            }
        }

        uint32_t body_start = static_cast<uint32_t>(iss.tellg());
        if (body_start < payload.length())
        {
            request.body = payload.substr(body_start);
        }
       // std::cout << request.body << std::endl;
    /*
        for(auto& kv: m) {
            std::cout << "KEY: `" << kv.first << "`, VALUE: `" << kv.second << '`' << std::endl;
        }

        std::cout << "protocol: " << header.protocol << '\n';
        std::cout << "method  : " << header.method << '\n';
        std::cout << "query   : " << header.query << '\n';
    */
    }

    return valid;
}

bool HttpProtocol::ParseReplyHeader(const std::string &payload, HttpReply &reply)
{
    std::string line;
    std::istringstream iss(payload);

    bool valid = false;

    // separate the first 3 main parts
    if (std::getline(iss, line))
    {
        line.pop_back(); // remove \r
        std::vector<std::string> parts = Util::Split(line, " ");

        if (parts.size() == 3)
        {
            reply.protocol = parts[0];
            reply.code = parts[1];
            reply.result = parts[2];
            valid = true;
        }
    }

    if (valid)
    {
        // Continue parsing the header
        std::string::size_type index;

        while (std::getline(iss, line))
        {
            if (line != "\r")
            {
                line.pop_back();
                index = line.find(':', 0);
                if(index != std::string::npos)
                {
                    // Convert all header options to lower case (header params are case insensitive in the HTTP spec
                    std::string option = line.substr(0, index);
                    std::transform(option.begin(), option.end(), option.begin(), ::tolower);
                    reply.headers.insert(std::make_pair(option, line.substr(index + 1)));
                }
            }
            else
            {
                break; // detected HTTP separator \r\n between header and body
            }
        }

        uint32_t body_start = static_cast<uint32_t>(iss.tellg());
        if (body_start < payload.length())
        {
            reply.body = payload.substr(body_start);
        }
    }

    return valid;
}


std::string HttpProtocol::GenerateRequest(const HttpRequest &request)
{
    std::stringstream ss;

    ss << request.method << " " << request.query << " " << request.protocol << "\r\n";

    for(auto const& x : request.headers )
    {
        ss << x.first << ": " << x.second << "\r\n";
    }

    ss << "\r\n";
    ss << request.body;

    return ss.str();
}

std::string HttpProtocol::GenerateHttpJsonResponse(const std::string &data)
{
    std::stringstream ss;

    ss << "HTTP/1.1 200 OK\r\n";
    ss << "Content-type: application/json\r\n";
    ss << "Content-length: " << data.size() << "\r\n\r\n";
    ss << data;

    return ss.str();
}

std::string HttpProtocol::GenerateWebSocketRequest(WebSocketRequest &ws)
{
    ws.request.headers["Connection"] = "keep-alive, Upgrade";
    ws.request.headers["Upgrade"] = "websocket";
    ws.request.headers["Sec-WebSocket-Key"] = Base64::Encode(Util::GenerateRandomString(23));
    ws.request.headers["Sec-WebSocket-Version"] = "13";
//    ws.request.headers["Origin"] = "http://127.0.0.1:8080";

//    ws.request.headers["Sec-WebSocket-Protocol"] = ws.protocol;

    return GenerateRequest(ws.request);
}

bool HttpProtocol::ParseWebSocketRequest(const std::string &payload, WebSocketRequest &ws)
{
    bool ok = false;
    if (ParseRequestHeader(payload, ws.request))
    {

        ok = GetRequestHeaderValue(ws.request, "Sec-WebSocket-Key", ws.key);
        ok = ok && GetRequestHeaderValue(ws.request, "Sec-WebSocket-Protocol", ws.protocol);
    }
    return ok;
}

bool HttpProtocol::GetRequestHeaderValue(const HttpRequest &request, const std::string &option, std::string &value)
{
    bool ok = false;

    std::string opt = option;
    std::transform(opt.begin(), opt.end(), opt.begin(), ::tolower);
    if (request.headers.count(opt))
    {
        ok = true;
        value = request.headers.at(opt);
    }

    return ok;
}

bool HttpProtocol::GetReplyHeaderValue(const HttpReply &reply, const std::string &option, std::string &value)
{
    bool ok = false;

    std::string opt = option;
    std::transform(opt.begin(), opt.end(), opt.begin(), ::tolower);
    if (reply.headers.count(opt))
    {
        ok = true;
        value = reply.headers.at(opt);
    }

    return ok;
}

std::string WebSocketRequest::Accept()
{
    /*
            The client sends a Sec-WebSocket-Key which is a random value that has been base64 encoded.
            To form a response, the magic string 258EAFA5-E914-47DA-95CA-C5AB0DC85B11 is appended to this (undecoded) key.
            The resulting string is then hashed with SHA-1, then base64 encoded. Finally, the resulting reply occurs in
            the header Sec-WebSocket-Accept.
        */
    std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string encoded = key + magic;

    Sha1 sha;
    sha.Update(encoded);
    encoded = sha.Final(false);
    encoded = Base64::Encode(encoded);
    return encoded;
}

std::string WebSocketRequest::Upgrade()
{
    std::string accept = Accept();

    std::string header = "HTTP/1.1 101 Switching Protocols\r\n";
    header += "Upgrade: websocket\r\n";
    header += "Connection: Upgrade\r\n";
    header += "Sec-WebSocket-Version: 13\r\n";
    header += "Sec-WebSocket-Accept: " + accept + "\r\n";

    if (protocol.size() != 0)
    {
        header += "Sec-WebSocket-Protocol: " + protocol + "\r\n";
    }
    header += "\r\n";

    return header;
}
