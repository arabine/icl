#ifndef TINYMAC_H
#define TINYMAC_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/aes.h"
#include "mbedtls/cmac.h"
#include "mbedtls/cipher.h"

typedef enum {
    TM_JOIN_REQ,
    TM_JOIN_ACCEPT,
    TM_JOIN_DENIED,
    TM_DATA_REQ,
    TM_DATA_ACCEPT,
    TM_DATA_DENIED,
} TmFrameType_t;


class TinyMac
{
public:
    TinyMac();

    void MacComputeMic(const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t *mic);
    void DecryptBuffer(const uint8_t *buffer, uint16_t size, const uint8_t *key, uint8_t *decBuffer);
    void EncryptBuffer(const uint8_t *buffer, uint16_t size, const uint8_t *key, uint8_t *encBuffer);
    uint8_t BuildJoinNetwork(uint8_t *packet, const uint8_t *devEui, const uint8_t *appEui, const uint8_t *key);
    bool ParseJoinNetwork(const uint8_t *packet, uint8_t size, uint8_t *devEui, uint8_t *appEui, const uint8_t *key);
private:
    mbedtls_aes_context AesContext;
    mbedtls_cipher_context_t AesCmacCtx;
};

#endif // TINYMAC_H
