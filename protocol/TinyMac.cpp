#include "TinyMac.h"


#include "libutil.h"


TinyMac::TinyMac()
{

}

void TinyMac::MacComputeMic( const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t *mic )
{
    const mbedtls_cipher_info_t *cipher_info = mbedtls_cipher_info_from_type( MBEDTLS_CIPHER_AES_128_ECB );
    uint8_t Mic[16];

    mbedtls_cipher_init(&AesCmacCtx);
    mbedtls_cipher_setup( &AesCmacCtx, cipher_info);
    mbedtls_cipher_cmac_starts( &AesCmacCtx, key, 128 );
    mbedtls_cipher_cmac_update( &AesCmacCtx, buffer, size & 0xFF );
    mbedtls_cipher_cmac_finish( &AesCmacCtx, Mic );

    *mic = ( uint32_t )( ( uint32_t )Mic[3] << 24 | ( uint32_t )Mic[2] << 16 | ( uint32_t )Mic[1] << 8 | ( uint32_t )Mic[0] );
}


void TinyMac::DecryptBuffer( const uint8_t *buffer, uint16_t size, const uint8_t *key, uint8_t *decBuffer )
{
    mbedtls_aes_init(&AesContext);
    mbedtls_aes_setkey_dec(&AesContext, key, 16);

    mbedtls_aes_decrypt( &AesContext, buffer, decBuffer );
}

void TinyMac::EncryptBuffer( const uint8_t *buffer, uint16_t size, const uint8_t *key, uint8_t *encBuffer )
{
    mbedtls_aes_init(&AesContext);
    mbedtls_aes_setkey_enc(&AesContext, key, 16);

    mbedtls_aes_encrypt( &AesContext, buffer, encBuffer );
}


bool TinyMac::ParseJoinNetwork(const uint8_t *packet, uint8_t size, uint8_t * devEui, uint8_t * appEui, const uint8_t * key)
{
    bool success = false;
    uint8_t idx = 0;
    const uint8_t dataSize = 1 + 2 * 8; // command + DevEui + AppEui
    const uint8_t padding = dataSize % 16;
    const uint8_t packetSize = dataSize + padding + 4; // data + padding + MIC

    if ((packet[0] == TM_JOIN_REQ) && (packetSize == size))
    {
        idx++;
        memcpy(&devEui[0], &packet[idx], 8);
        idx += 8;

        memcpy(&appEui[0], &packet[idx], 8);
        idx += 8;

        idx += padding;

        uint32_t micComputed = 0;
        MacComputeMic(packet, idx, key, &micComputed);

        uint32_t micRead = 0;
        micRead = beu32_get(&packet[idx]);

        if (micComputed == micRead)
        {
            success = true;
        }
    }
    return success;
}

uint8_t TinyMac::BuildJoinNetwork(uint8_t *packet, const uint8_t * devEui, const uint8_t * appEui, const uint8_t * key)
{
    uint8_t idx = 0;

    packet[0] = TM_JOIN_REQ;
    idx++;

    memcpy(&packet[idx], &devEui[0], 8);
    idx += 8;

    memcpy(&packet[idx], &appEui[0], 8);
    idx += 8;

    uint8_t padding = idx%16;

    for (uint8_t i = 0; i < padding; i++)
    {
        packet[idx + i] = i;
    }
    idx += padding;

    uint32_t mic;
    MacComputeMic(packet, idx, key, &mic);

    beu32_put(&packet[idx], mic);
    return idx + 4;
}
