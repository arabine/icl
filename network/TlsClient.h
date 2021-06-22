#ifndef TLS_CLIENT
#define TLS_CLIENT

#include <stdint.h>
#include <stdbool.h>
#include <string>

#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include "TcpSocket.h"

namespace tcp
{
/*****************************************************************************/
typedef struct
{
    uint8_t data[8 * 1024];
    int32_t size;
} read_buff_t;

typedef struct
{
    mbedtls_ssl_context *ssl;
    mbedtls_net_context *net;
    ISocket *client;

} io_ctx_t;

class TlsClient
{
public:
    TlsClient(tcp::ISocket *s);

    bool Connect(const char *server_name);
    bool Request(const uint8_t *data, uint32_t size, read_buff_t *read_buf);
    void Close();
    void WaitData(read_buff_t *read_buf);   
    bool Read(read_buff_t *read_buf);
    bool Write(const uint8_t *data, uint32_t size);

private:
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    io_ctx_t io_ctx;
};

} //namespace tcp

#endif // TLS_CLIENT
