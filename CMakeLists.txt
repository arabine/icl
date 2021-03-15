cmake_minimum_required(VERSION 3.5)

project(icl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_VERBOSE_MAKEFILE on )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX AND NOT APPLE)
    add_definitions(-DUSE_UNIX_OS)
endif()

if(WIN32)
    add_definitions(-DUSE_WINDOWS_OS)
endif()

if(UNIX)
add_definitions(-fPIC)
endif()

add_definitions(-DUNICODE)

set(ICL_MBEDTLS
    aes.c
    aesni.c
    arc4.c
    aria.c
    asn1parse.c
    asn1write.c
    base64.c
    bignum.c
    blowfish.c
    camellia.c
    ccm.c
    certs.c
    chacha20.c
    chachapoly.c
    cipher.c
    cipher_wrap.c
    cmac.c
    ctr_drbg.c
    debug.c
    des.c
    dhm.c
    ecdh.c
    ecdsa.c
    ecjpake.c
    ecp.c
    ecp_curves.c
    entropy.c
    entropy_poll.c
    error.c
    gcm.c
    havege.c
    hkdf.c
    hmac_drbg.c
    md2.c
    md4.c
    md5.c
    md.c
    md_wrap.c
    memory_buffer_alloc.c
    net_sockets.c
    nist_kw.c
    oid.c
    padlock.c
    pem.c
    pk.c
    pkcs11.c
    pkcs12.c
    pkcs5.c
    pkparse.c
    pk_wrap.c
    pkwrite.c
    platform.c
    platform_util.c
    poly1305.c
    ripemd160.c
    rsa.c
    rsa_internal.c
    sha1.c
    sha256.c
    sha512.c
    ssl_cache.c
    ssl_ciphersuites.c
    ssl_cli.c
    ssl_cookie.c
    ssl_srv.c
    ssl_ticket.c
    ssl_tls.c
    threading.c
    timing.c
    version.c
    version_features.c
    x509.c
    x509_create.c
    x509_crl.c
    x509_crt.c
    x509_csr.c
    x509write_crt.c
    x509write_csr.c
    xtea.c
)

list(TRANSFORM ICL_MBEDTLS PREPEND mbedtls/library/)

add_library(
    icl
    STATIC 

    zip/Zip.cpp

    util/Util.cpp
    util/Log.cpp
    util/Value.cpp
    util/SharedLibrary.cpp
    util/EventLoop.cpp

    network/TcpServerBase.cpp
    network/TcpServerBase.h
    network/TcpSocket.cpp
    network/TcpClient.cpp
    network/TlsClient.cpp

    protocol/Http.cpp
    protocol/HttpFileServer.cpp
    protocol/HttpProtocol.cpp

    security/Base64Util.cpp
    security/ShaOne.cpp
    security/ShaTwo.cpp

    jsengine/JSEngine.cpp
    jsengine/duktape.c
    jsengine/duk_module_duktape.c
    json/JsonReader.cpp
    json/JsonWriter.cpp
    json/JsonValue.cpp

    ${ICL_MBEDTLS}
)


target_include_directories (
    icl
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/zip
    ${CMAKE_CURRENT_SOURCE_DIR}/util
    ${CMAKE_CURRENT_SOURCE_DIR}/jsengine
    ${CMAKE_CURRENT_SOURCE_DIR}/json
    ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls/include
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/security
    ${CMAKE_CURRENT_SOURCE_DIR}/io
    ${CMAKE_CURRENT_SOURCE_DIR}/db
    ${CMAKE_CURRENT_SOURCE_DIR}/date
)
