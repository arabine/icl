# ------------------------------------------------------------------------------
# Util files
# ------------------------------------------------------------------------------
HEADERS += Log.h \
    $$PWD/protocol/TinyMac.h \
    ErrorHelper.h \
    Observer.h \
    Util.h \
    ThreadQueue.h \
    GetOptions.h \
    Value.h \
    UniqueId.h \
    Semaphore.h \
    Console.h \
    EventLoop.h \
    DurationTimer.h \
    Pool.h \
    libutil.h

SOURCES += Log.cpp \
    $$PWD/protocol/TinyMac.cpp \
    Util.cpp \
    Value.cpp \
    UniqueId.cpp \
    Console.cpp \
    EventLoop.cpp

# ------------------------------------------------------------------------------
# I/O files
# ------------------------------------------------------------------------------
HEADERS += ByteStreamReader.h \
    ByteStreamWriter.h \
    ByteArray.h

SOURCES += ByteArray.cpp \
    ByteStreamReader.cpp \
    ByteStreamWriter.cpp


# ------------------------------------------------------------------------------
# LoRa MAC layer
# ------------------------------------------------------------------------------
icl_lora {
HEADERS += LoRaMac-api-v3.h \
    LoRaMac.h \
    LoRaMacCrypto.h

SOURCES += LoRaMac-api-v3.cpp \
    LoRaMac.cpp \
    LoRaMacCrypto.cpp
}

# ------------------------------------------------------------------------------
# Network files
# ------------------------------------------------------------------------------
HEADERS += TcpSocket.h \
    TcpServer.h \
    TcpClient.h \
    UdpSocket.h \
    WebSocket.h \
    TcpServerBase.h

SOURCES += TcpSocket.cpp \
    TcpServerBase.cpp \
    TcpClient.cpp \
    UdpSocket.cpp \
    WebSocket.cpp

DEFINES += ASIO_STANDALONE

linux {
    SOURCES += TcpServerEpoll.cpp
}

windows {
    SOURCES += TcpServer.cpp
 #   SOURCES += wepoll.cpp
 #   HEADERS += wepoll.h
}

# ------------------------------------------------------------------------------
# Protocol files
# ------------------------------------------------------------------------------
icl_http {
    HEADERS += Http.h HttpFileServer.h HttpProtocol.h
    SOURCES += Http.cpp HttpFileServer.cpp HttpProtocol.cpp
}

# ------------------------------------------------------------------------------
# Database files
# ------------------------------------------------------------------------------

icl_database {
    HEADERS += DataBase.h
    SOURCES += DataBase.cpp sqlite3.c
}

icl_couchdb {
    HEADERS += CouchDb.h
    SOURCES += CouchDb.cpp
}

# ------------------------------------------------------------------------------
# Security files
# ------------------------------------------------------------------------------
HEADERS += Base64.h Sha1.h Sha256.h

SOURCES += Base64.cpp Sha1.cpp Sha256.cpp

# ------------------------------------------------------------------------------
# ZIP files
# ------------------------------------------------------------------------------

icl_zip {
HEADERS += Zip.h
SOURCES += Zip.cpp miniz.c
}

# ------------------------------------------------------------------------------
# JSON files
# ------------------------------------------------------------------------------
HEADERS += JsonWriter.h \
   JsonReader.h \
   JsonValue.h

SOURCES += JsonWriter.cpp \
    JsonReader.cpp \
    JsonValue.cpp


# ------------------------------------------------------------------------------
# JSEngine and JSON files
# ------------------------------------------------------------------------------
HEADERS += duktape.h duk_config.h \
    JSEngine.h \
    IScriptEngine.h

SOURCES += duktape.c \
    JSEngine.cpp

#CONFIG(debug, debug|release) {
#    win32 {
#        SOURCES += duk_trans_socket_windows.c
#    }
#}

VPATH += $$ICL_DIR/network
VPATH += $$ICL_DIR/network/asio
VPATH += $$ICL_DIR/util
VPATH += $$ICL_DIR/zip
VPATH += $$ICL_DIR/security
VPATH += $$ICL_DIR/jsengine
VPATH += $$ICL_DIR/protocol
VPATH += $$ICL_DIR/json
VPATH += $$ICL_DIR/io
VPATH += $$ICL_DIR/db
VPATH += $$ICL_DIR/lora

INCLUDEPATH += $$ICL_DIR/network
INCLUDEPATH += $$ICL_DIR/network/asio
INCLUDEPATH += $$ICL_DIR/util
INCLUDEPATH += $$ICL_DIR/zip
INCLUDEPATH += $$ICL_DIR/security
INCLUDEPATH += $$ICL_DIR/jsengine
INCLUDEPATH += $$ICL_DIR/protocol
INCLUDEPATH += $$ICL_DIR/json
INCLUDEPATH += $$ICL_DIR/io
INCLUDEPATH += $$ICL_DIR/db
INCLUDEPATH += $$ICL_DIR/lora
INCLUDEPATH += $$ICL_DIR/date
INCLUDEPATH += $$ICL_DIR
