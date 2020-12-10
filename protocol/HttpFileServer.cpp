/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#include <fstream>
#include <locale>
#include <string>
#include "HttpFileServer.h"
#include "Util.h"
#include "Log.h"
#include "Zip.h"
#include "ShaTwo.h"
#include "Base64Util.h"
#include "JsonReader.h"

#ifdef USE_UNIX_OS
#include <sys/sendfile.h>
#endif

HttpFileServer::HttpFileServer(const std::string &rootDir)
    : mRootDir(rootDir)
{
    mSessionSecret = Util::GenerateRandomString(60);
    TLogInfo("[HTTP] Current session secret key: " + mSessionSecret);
}

HttpFileServer::~HttpFileServer()
{

}

tcp::TcpServer::IEvent::~IEvent(){

}

void HttpFileServer::NewConnection(const tcp::Conn &conn) {
    (void) conn;
}


std::string mime_type(const std::string &ext)
{
    if(ext == "htm")  return "text/html";
    if(ext == "html") return "text/html";
    if(ext == "php")  return "text/html";
    if(ext == "css")  return "text/css";
    if(ext == "txt")  return "text/plain";
    if(ext == "js")   return "application/javascript";
    if(ext == "json") return "application/json";
    if(ext == "xml")  return "application/xml";
    if(ext == "swf")  return "application/x-shockwave-flash";
    if(ext == "flv")  return "video/x-flv";
    if(ext == "png")  return "image/png";
    if(ext == "jpe")  return "image/jpeg";
    if(ext == "jpeg") return "image/jpeg";
    if(ext == "jpg")  return "image/jpeg";
    if(ext == "gif")  return "image/gif";
    if(ext == "bmp")  return "image/bmp";
    if(ext == "ico")  return "image/vnd.microsoft.icon";
    if(ext == "tiff") return "image/tiff";
    if(ext == "tif")  return "image/tiff";
    if(ext == "svg")  return "image/svg+xml";
    if(ext == "svgz") return "image/svg+xml";
    if(ext == "woff2") return "font/woff2";
    if(ext == "pdf")  return "application/pdf";

    return "application/text";
}

/*
Signature = HMAC-SHA256( encodeBase64Url(header) + '.' + encodeBase64Url(payload), secret )
const token = encodeBase64Url(header) + '.' + encodeBase64Url(payload) + '.' + encodeBase64Url(signature)


*/

// FIXME move level in application side
std::string HttpFileServer::GenerateJWT(const std::string &payload)
{
    std::string header = R"({"alg":"HS256","typ":"JWT"})";

  //  mSessionSecret = "coucou";
    // Compute signature
   // TLogInfo("[HTTP] Header: " + header);
   // TLogInfo("[HTTP] Payload: " + payload);
    std::string input_signature = Base64::Encode(header) + '.' + Base64::Encode(payload);

  //  TLogInfo("[HTTP] Input signature: " + input_signature);
    std::string hmac = Base64::Encode(hmac_compute(mSessionSecret, input_signature));
    //TLogInfo("[HTTP] HMAC-SHA256: " + );

    return input_signature + '.' + hmac;
}

bool HttpFileServer::CheckJWT(const std::string &header, const std::string &payload, const std::string &hash, JsonValue &json)
{
    bool success = false;

    // First, verify signature
    std::string input_signature = header + '.' + payload;

    std::string hmac = Base64::Encode(hmac_compute(mSessionSecret, input_signature));

    if (hmac == hash)
    {
        // Now, check expiration
        std::string decoded = Base64::Decode(payload);
        JsonReader reader;
        if (reader.ParseString(json, decoded))
        {
            if (json.IsObject())
            {
                uint32_t now = Util::CurrentTimeStamp();
                uint32_t iat = static_cast<uint32_t>(json.GetObj().GetValue("iat").GetInteger());
                uint32_t exp = static_cast<uint32_t>(json.GetObj().GetValue("exp").GetInteger());

                if (iat == exp)
                {
                    TLogInfo("[JWT] Authentication is valid forever");
                    success = true;
                }
                else if (now < exp)
                {
                    TLogInfo("[JWT] Authentication still valid for: " + std::to_string(exp - now) + " seconds");
                    success = true;
                }
            }
            else
            {
                TLogError("[JWT] Not an object");
            }
        }
        else
        {
            TLogError("[JWT] Json parse failure");
        }
    }
    else
    {
        TLogError("[JWT] Bad signature");
    }

    return success;
}

void HttpFileServer::DeletePartialConn(const tcp::Conn &conn)
{
    int index = -1;

    for (uint32_t i = 0; i < mPartials.size(); i++)
    {
        if (mPartials[i].conn == conn)
        {
            index = static_cast<int>(i);
        }
    }

    if (index >= 0)
    {
        TLogNetwork("[HTTP] Deleted chunked");
        mPartials.erase(mPartials.begin() + index);
    }
}

static const std::string cSupportedMethods[] = { "GET", "POST" };


void HttpFileServer::ParseUrlParameters(HttpRequest &request)
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

bool HttpFileServer::ParseHeader(const tcp::Conn &conn, HttpRequest &request)
{
 //   std::string request = "GET /index.asp?param1=hello&param2=128 HTTP/1.1";
    // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF

    std::string line;
    std::istringstream iss(conn.payload);

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
        if (body_start < conn.payload.length())
        {
            request.body = conn.payload.substr(body_start);
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

bool HttpFileServer::GetFile(const tcp::Conn &conn, HttpRequest &request)
{
    bool success = false;
    std::string resource = request.query;

  //  std::cout << "Get file: " << resource << std::endl;
    if (resource == "/")
    {
        resource = "/index.html";
    }

    std::string ext = Util::GetFileExtension(resource);

    if (ext.size() && (request.method == "GET"))
    {
        // it is a file
        std::string fullFilepath = mRootDir + resource;
   //     TLogInfo("Get resource: " + fullFilepath);
        if (Util::FileExists(fullFilepath))
        {
            std::string mime = mime_type(ext);
            std::uint64_t size = Util::FileSize(fullFilepath);
       //     std::cout << "File found, size: " << size << " MIME: " << mime << std::endl;

            std::stringstream ss;

            ss << "HTTP/1.1 200 OK\r\n";
            ss << "Content-type: " << std::string(mime) << "\r\n";

            if (ext == "pdf")
            {
                ss << "Content-Disposition: inline; filename=\"" <<  Util::GetFileName(fullFilepath) + "\"" + "\r\n";
            }
/*
            ss << "Access-Control-Allow-Origin: *\r\n";
            ss << "Content-length: " << size << "\r\n\r\n";

            tcp::TcpSocket::SendToSocket(ss.str(), conn.peer);

            int fd = open(fullFilepath.c_str(), O_RDONLY);
            if (fd >= 0)
            {
                ::sendfile(conn.peer.socket, fd, nullptr, size);
                close(fd);
            }
            */


            std::ifstream t(fullFilepath);
            std::string str;

            str.reserve(size);

            str.assign((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());

            char *output = new char[size*2];
            int compressed_size = Zip::CompressBuffer(str.c_str(), size, output);

         //   TLogInfo("Compressed size: " + std::to_string(compressed_size));
            if (compressed_size > 0)
            {
                 ss << "Content-length: " << compressed_size << "\r\n";
                 ss << "Content-Encoding: deflate\r\n\r\n";

                 tcp::TcpSocket::SendToSocket(ss.str(), conn.peer);
                 tcp::TcpSocket::SendToSocket(std::string(output, compressed_size), conn.peer);
            }
            delete[] output;


            success = true;

/*
            std::ifstream fin(fullFilepath, std::ifstream::binary);
            std::vector<char> buffer (1024,0); //reads only the first 1024 bytes

            bool first = true;
            while (!fin.eof()) {
                fin.read(buffer.data(), buffer.size());
                std::streamsize dataSize = fin.gcount();
                std::string chunk = {buffer.begin(), buffer.begin() + dataSize};
                if (first)
                {
                    first = false;
                    chunk = ss.str() + chunk;
                }
                tcp::TcpSocket::SendToSocket(chunk, conn.peer);
            }
*/
        }

    }
    return success;
}

void HttpFileServer::SendHttpJson(const tcp::Conn &conn, const std::string &data)
{
    std::stringstream ss;

    ss << "HTTP/1.1 200 OK\r\n";
    ss << "Content-type: application/json\r\n";
    ss << "Content-length: " << data.size() << "\r\n\r\n";
    ss << data;

    tcp::TcpSocket::SendToSocket(ss.str(), conn.peer);
}


void HttpFileServer::ReadData(const tcp::Conn &conn)
{
    if (conn.payload.size() == 0)
    {
        return;
    }

    if (conn.peer.isWebSocket)
    {
        WsReadData(conn);
        return;
    }

    HttpRequest request;

    bool process = true;
    bool valid = ParseHeader(conn, request);

    if (valid)
    {
        // Full data or multi-part HTML?
        if (request.method == "POST")
        {
            auto result = request.headers.find("content-length");
            if (result != request.headers.end())
            {
                std::uint32_t size = Util::FromString<std::uint32_t>(result->second);

                if (size > request.body.length())
                {
                    TLogNetwork("[HTTP] Chunked data detected");

                    ChunkedData chunked;

                    chunked.request = request;
                    chunked.conn = conn;
                    chunked.total_size = size;
                    chunked.data.append(request.body);
                    chunked.current_size += request.body.length();

                    mPartials.push_back(chunked);
                    process = false; // wait for full data
                }
            }
        }
    }
    else
    {
        process = false;
        bool found = false;
        for (auto & el: mPartials)
        {
            if (el.conn == conn)
            {
                found = true;
                el.data.append(conn.payload);
                el.current_size += conn.payload.length();
                el.counter++;

                TLogNetwork("[HTTP] Chunked part: " + std::to_string(el.counter) + " remaining: " + std::to_string(el.total_size - el.current_size));

                if (el.current_size >= el.total_size)
                {
                    TLogNetwork("[HTTP] Chunked finished");

                    request = el.request;
                    request.body = el.data; // finally, all data is here, replace buffer

                    process = true;
                    // Always remove pending chunked data
                    DeletePartialConn(conn);
                }
            }
        }

        if (!found)
        {
            TLogNetwork("[HTTP] Chunked not found!");
        }
    }


    if (process)
    {
        // First, serve local files
        if (!GetFile(conn, request))
        {
            // Then, try REST API
            if (ReadDataPath(conn, request))
            {
                // Found nothing, send error
                Send404(conn, request);
            }
        }
    }
}

void HttpFileServer::Send404(const tcp::Conn &conn, const HttpRequest &header)
{
    std::stringstream ss;
    std::string html = R"({ "result": false, "message": "404 Not Found" })";

    ss << "HTTP/1.1 404 Not Found\r\n";
    ss << "Content-type: application/json\r\n";
    ss << "Content-length: " << html.size() << "\r\n\r\n";
    ss << html << std::flush;

    tcp::TcpSocket::SendToSocket(ss.str(), conn.peer);

    TLogWarning("Resource not found: " + header.query);
}

void HttpFileServer::ClientClosed(const tcp::Conn &conn)
{
    DeletePartialConn(conn);
}

void HttpFileServer::ServerTerminated(tcp::TcpServer::IEvent::CloseType type)
{
    (void) type;
}

void HttpFileServer::WsReadData(const tcp::Conn &conn)
{
    (void) conn;
    // do nothing in this default implementation
}

bool HttpFileServer::ReadDataPath(const tcp::Conn &conn, const HttpRequest &request)
{
    (void) conn;
    (void) request;
    return true;
}

std::string HttpFileServer::Match(const std::string &msg, const std::string &patternString)
{
    std::regex pattern(patternString);
    std::smatch matcher;
    std::string subMatch;

    std::regex_search(msg, matcher, pattern);

    if (matcher.size() == 2)
    {
        // Extracted value is located at index 1
        subMatch = matcher[1].str();
    }
    return subMatch; // empty string if not found
}
