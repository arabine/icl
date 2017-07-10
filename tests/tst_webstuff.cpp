#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include <string>
#include <regex>


#include "TcpSocket.h"
#include "TcpServer.h"
#include "WebSocket.h"
#include "tst_webstuff.h"
#include "CouchDb.h"
#include "Http.h"

/*

GET /docs/index.html HTTP/1.1
Host: www.nowhere123.com
Accept-Language: en-us
Accept-Encoding: gzip, deflate
User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)
(blank line)


HTTP/1.1 200 OK
Date: Sun, 18 Oct 2009 08:56:53 GMT
Server: Apache/2.2.14 (Win32)
Last-Modified: Sat, 20 Nov 2004 07:16:26 GMT
ETag: "10000000565a5-2c-3e94b66c2e680"
Accept-Ranges: bytes
Content-Length: 44
Connection: close
Content-Type: text/html
X-Pad: avoid browser bug

<html><body><h1>It works!</h1></body></html>
*/

extern "C" {
const char *find_embedded_file(const char *name, size_t *size, const char **mime);
}

TstWebStuff::TstWebStuff()
{
}

bool continueServer = true;

class BasicFileHandler : public tcp::TcpServer::IEvent
{
public:
    BasicFileHandler()
        : mCounter(0)
    {

    }

    void NewConnection(const tcp::Conn &conn) {
        (void) conn;
    }

    void ReadData(const tcp::Conn &conn)
    {
        std::string resource = Match(conn.payload, "GET (.*) HTTP");

        std::cout << "Get file: " << resource << std::endl;

        if (resource == "/")
        {
            resource = "/index.html";
        }

        const char *data;
        const char *mime;
        size_t size;

        data = find_embedded_file(resource.c_str(), &size, &mime);

        std::string output;
        if (data != NULL)
        {
            std::cout << "File found, size: " << size << " MIME: " << mime << std::endl;

            std::stringstream ss;

            ss << "HTTP/1.1 200 OK\r\n";
            ss << "Content-type: " << std::string(mime) << "\r\n";
            ss << "Content-length: " << (int)size << "\r\n\r\n";
            ss << std::string(data, size) << std::flush;

            output = ss.str();
            mCounter++;
        }
        else
        {
            std::stringstream ss;
            std::string html = "<html><head><title>Not Found</title></head><body>404</body><html>";

            ss << "HTTP/1.1 404 Not Found\r\n";
            ss << "Content-type: text/html\r\n";
            ss << "Content-length: " << html.size() << "\r\n\r\n";
            ss << html << std::flush;

            output = ss.str();
        }
        tcp::TcpSocket::SendToSocket(output, conn.peer);

        if (mCounter >= 4)
        {
            continueServer = false;
        }
    }

    void ClientClosed(const tcp::Conn &conn)
    {
        (void) conn;
    }

    void ServerTerminated(tcp::TcpServer::IEvent::CloseType type)
    {
        (void) type;
    }


    std::string Match(const std::string &msg, const std::string &patternString)
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

private:
    int mCounter;
};



void TstWebStuff::HttpServer()
{
    BasicFileHandler fileHandler;
    tcp::TcpServer server(fileHandler);

    tcp::TcpSocket::Initialize();

    std::cout << "=============== OPEN BROWSER TO 127.0.0.1:8000 ===================" << std::endl;
    server.Start(10, true, 8000);

    while(continueServer);

    server.Stop();
}

void TstWebStuff::Http()
{
    tcp::TcpSocket::Initialize();
    std::string value;
    std::string expected;

    bool boolValue;
    bool boolExpected;
    CouchDb db;

    if (db.Connect())
    {
        db.Get("/");

        if (db.IsValid())
        {
            expected = "1.6.1";
            // key is at the root of the document
            if (!db.GetValue("version", value))
            {
                QFAIL("JSON Get value error");
            }
            QCOMPARE(value, expected);
        }
        else
        {
            QFAIL("Invalid reply");
        }

        // Delete any previously created test database
        db.Delete("/test");
        if (db.IsValid())
        {
            // We don't parse the reply because it depends if
            // the database was existing or not. The following tests will
            // fail later if anything goes wrong here, so don't bother.
        }
        else
        {
           QFAIL("Invalid reply");
        }

        // Get a document that does not exists
        db.Get("/test/foobar");
        if (db.IsValid())
        {
            expected = "not_found";
            if (!db.GetValue("error", value))
            {
                QFAIL("JSON Get value error");
            }
            QCOMPARE(value, expected);
        }
        else
        {
           QFAIL("Invalid reply");
        }

        // Create a database
        db.Put("/test");
        if (db.IsValid())
        {
            boolExpected = true;
            if (!db.GetValue("ok", boolValue))
            {
                QFAIL("JSON Get value error");
            }
            QCOMPARE(boolValue, boolExpected);
        }
        else
        {
           QFAIL("Invalid reply");
        }

        // Create a document
        JsonObject doc;
        doc.AddValue("coucou", 7);
        std::string docId = "mydoc";
        db.Put("/test/" + docId, doc.ToString(0U));
        if (db.IsValid())
        {
            boolExpected = true;
            if (!db.GetValue("ok", boolValue))
            {
                QFAIL("JSON Get value error");
            }
            QCOMPARE(boolValue, boolExpected);
            if (!db.GetValue("id", value))
            {
                QFAIL("JSON Get value error");
            }
            QCOMPARE(value, docId);
        }
        else
        {
           QFAIL("Invalid reply");
        }

        // Create a document without providing our own ID
        doc.Clear();
        doc.AddValue("hiphop", false);
        db.Post("/test", doc.ToString(0U));
        if (db.IsValid())
        {
            boolExpected = true;
            if (!db.GetValue("ok", boolValue))
            {
                std::cout << db.GetRawData() << std::endl;
                std::cout << doc.ToString(0U) << std::endl;
                QFAIL("JSON Get value error");
            }
        }
        else
        {
           QFAIL("Invalid reply");
        }
    }

}

void TstWebStuff::WebSocketUpgrade()
{

    // Test from RFC 6455...
    //$key=         "dGhlIHNhbXBsZSBub25jZQ=="
    //$cat_string=  "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    //$resultBase64="s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";

    std::string data = "GET / HTTP / 1.1\r\n";
    data += "Host: server.example.com\r\n";
    data += "Upgrade: websocket\r\n";
    data += "Connection: Upgrade\r\n";
    data += "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n";
    data += "Origin: http://example.com\r\n";
    data += "Sec-WebSocket-Protocol: superchat\r\n";

    WebSocketRequest req;
    req.Parse(data);

    if (req.IsValid())
    {
        std::string accept = req.Accept();
        if (accept == "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=")
        {
            std::cout << "Upgrade header: " << std::endl << req.Upgrade() << std::endl;
        }
        else
        {
            QFAIL("Invalid accept encoded string");
            std::cout << "Invalid accept encoded string: " << accept << std::endl;
        }
    }
    else
    {
        QFAIL("Invalid HTTP request");
        std::cout << "Invalid request" << std::endl;
    }
}

