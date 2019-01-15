
#include <fstream>
#include "HttpFileServer.h"
#include "Util.h"
#include "Log.h"
#include "Zip.h"
#include <sys/sendfile.h>

BasicFileHandler::BasicFileHandler(const std::string &rootDir)
    : mRootDir(rootDir)
{

}

void BasicFileHandler::NewConnection(const tcp::Conn &conn) {
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

bool BasicFileHandler::ParseHeader(const tcp::Conn &conn, HttpRequest &request)
{
 //   std::string request = "GET /index.asp?param1=hello&param2=128 HTTP/1.1";

    // separate the 3 main parts
    std::istringstream iss(conn.payload);

    if(!(iss >> request.method >> request.query >> request.protocol))
    {
        TLogError("HTTP parse error");
        return false;
    }

    // reset the std::istringstream with the query string
/*
    iss.clear();
    iss.str(query);

    std::string url;

    if(!std::getline(iss, url, '?')) // remove the URL part
    {
        std::cout << "ERROR: parsing request url\n";
        return 1;
    }

    // store query key/value pairs in a map
    std::string keyval, key, val;

    while(std::getline(iss, keyval, '&')) // split each term
    {
        std::istringstream iss(keyval);

        // split key/value pairs
        if(std::getline(std::getline(iss, key, '='), val))
            params[key] = val;
    }
    */

    std::string line;
    std::string::size_type index;
    iss.clear();
    iss.str(conn.payload);

    while (std::getline(iss, line) && line != "\r")
    {
        line.pop_back();
        index = line.find(':', 0);
        if(index != std::string::npos)
        {
            request.headers.insert(std::make_pair(line.substr(0, index), line.substr(index + 1)));
        }
    }

    uint32_t body_start = iss.tellg();
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
    return true;
}

bool BasicFileHandler::GetFile(const tcp::Conn &conn, HttpRequest &request)
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

         //   ss << "Access-Control-Allow-Origin: *\r\n";

//            tcp::TcpSocket::SendToSocket(ss.str(), conn.peer);

//            int fd = open(fullFilepath.c_str(), O_RDONLY);
//            if (fd >= 0)
//            {
//                ::sendfile(conn.peer.socket, fd, nullptr, size);
//                close(fd);
//            }


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

                tcp::TcpSocket::SendToSocket(ss.str() + std::string(output, compressed_size), conn.peer);
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

void BasicFileHandler::SendHttpJson(const tcp::Conn &conn, const std::string &data)
{
    std::stringstream ss;

    ss << "HTTP/1.1 200 OK\r\n";
    ss << "Content-type: application/json\r\n";
    ss << "Content-length: " << data.size() << "\r\n\r\n";
    ss << data;

    tcp::TcpSocket::SendToSocket(ss.str(), conn.peer);
}


void BasicFileHandler::ReadData(const tcp::Conn &conn)
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
    ParseHeader(conn, request);

    // First, try REST API
    if (ReadDataPath(conn, request))
    {
        // Else, serve files
        if (!GetFile(conn, request))
        {
            // Found nothing, send error
            Send404(conn, request);
        }
    }
}

void BasicFileHandler::Send404(const tcp::Conn &conn, const HttpRequest &header)
{
    std::stringstream ss;
    std::string html = "<html><head><title>Not Found: " + header.query + "</title></head><body>404</body><html>";

    ss << "HTTP/1.1 404 Not Found\r\n";
    ss << "Content-type: text/html\r\n";
    ss << "Content-length: " << html.size() << "\r\n\r\n";
    ss << html << std::flush;

    tcp::TcpSocket::SendToSocket(ss.str(), conn.peer);

    TLogWarning("Resource not found: " + header.query);
}

void BasicFileHandler::ClientClosed(const tcp::Conn &conn)
{
    (void) conn;
}

void BasicFileHandler::ServerTerminated(tcp::TcpServer::IEvent::CloseType type)
{
    (void) type;
}

void BasicFileHandler::WsReadData(const tcp::Conn &conn)
{
    (void) conn;
    // do nothing in this default implementation
}

bool BasicFileHandler::ReadDataPath(const tcp::Conn &conn, const HttpRequest &request)
{
    (void) conn;
    (void) request;
    return true;
}

std::string BasicFileHandler::Match(const std::string &msg, const std::string &patternString)
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
