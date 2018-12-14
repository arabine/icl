
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


    return "application/text";
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

    std::string resource = Match(conn.payload, "GET (.*) HTTP");

  //  std::cout << "Get file: " << resource << std::endl;
    if (resource == "/")
    {
        resource = "/index.html";
    }

    std::string ext = Util::GetFileExtension(resource);

    if (ext.size())
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
        else
        {
            Send404(conn, resource);
        }
    }
    else
    {
        // It is a path, handle it (FIXME)
        ReadDataPath(conn, resource);
    }
}

void BasicFileHandler::Send404(const tcp::Conn &conn, const std::string &resource)
{
    std::stringstream ss;
    std::string html = "<html><head><title>Not Found: " + resource + "</title></head><body>404</body><html>";

    ss << "HTTP/1.1 404 Not Found\r\n";
    ss << "Content-type: text/html\r\n";
    ss << "Content-length: " << html.size() << "\r\n\r\n";
    ss << html << std::flush;

    tcp::TcpSocket::SendToSocket(ss.str(), conn.peer);

    TLogWarning("Resource not found: " + resource);
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

void BasicFileHandler::ReadDataPath(const tcp::Conn &conn, const std::string &resource)
{
    (void) conn;
    Send404(conn, resource);
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
