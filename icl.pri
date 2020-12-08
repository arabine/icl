# ------------------------------------------------------------------------------
# Util files
# ------------------------------------------------------------------------------
HEADERS += Log.h \
    Jwt.h \
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
    TlsClient.h \
    TcpServerBase.h

SOURCES += TcpSocket.cpp \
    TcpServerBase.cpp \
    TcpClient.cpp \
    TlsClient.cpp \
    UdpSocket.cpp

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
    HEADERS += HttpProtocol.h Http.h
    SOURCES += HttpProtocol.cpp Http.cpp

}

icl_http_server {
    HEADERS += HttpFileServer.h
    SOURCES += HttpFileServer.cpp
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

SOURCES += duktape.c duk_module_duktape.c \
    JSEngine.cpp

#CONFIG(debug, debug|release) {
#    win32 {
#        SOURCES += duk_trans_socket_windows.c
#    }
#}

# ------------------------------------------------------------------------------
# mBed TLS
# ------------------------------------------------------------------------------
#DEFINES += -DMBEDTLS_CONFIG_FILE='<foo.h>'"

HEADERS +=  mbedtls/aes.h mbedtls/config.h

SOURCES +=  aes.c \
aesni.c \
arc4.c \
aria.c \
asn1parse.c \
asn1write.c \
base64.c \
bignum.c \
blowfish.c \
camellia.c \
ccm.c \
certs.c \
chacha20.c \
chachapoly.c \
cipher.c \
cipher_wrap.c \
cmac.c \
ctr_drbg.c \
debug.c \
des.c \
dhm.c \
ecdh.c \
ecdsa.c \
ecjpake.c \
ecp.c \
ecp_curves.c \
entropy.c \
entropy_poll.c \
error.c \
gcm.c \
havege.c \
hkdf.c \
hmac_drbg.c \
md2.c \
md4.c \
md5.c \
md.c \
md_wrap.c \
memory_buffer_alloc.c \
net_sockets.c \
nist_kw.c \
oid.c \
padlock.c \
pem.c \
pk.c \
pkcs11.c \
pkcs12.c \
pkcs5.c \
pkparse.c \
pk_wrap.c \
pkwrite.c \
platform.c \
platform_util.c \
poly1305.c \
ripemd160.c \
rsa.c \
rsa_internal.c \
sha1.c \
sha256.c \
sha512.c \
ssl_cache.c \
ssl_ciphersuites.c \
ssl_cli.c \
ssl_cookie.c \
ssl_srv.c \
ssl_ticket.c \
ssl_tls.c \
threading.c \
timing.c \
version.c \
version_features.c \
x509.c \
x509_create.c \
x509_crl.c \
x509_crt.c \
x509_csr.c \
x509write_crt.c \
x509write_csr.c \
xtea.c \

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
VPATH += $$ICL_DIR/mbedtls/library
VPATH += $$ICL_DIR/mbedtls/include/mbedtls
VPATH += $$ICL_DIR/mbedtls/include

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
INCLUDEPATH += $$ICL_DIR/mbedtls/include
INCLUDEPATH += $$ICL_DIR
