#include "TinyMac.h"
#include "TinyMac.h"

#include "mbedtls/gcm.h"
#include "mbedtls/aes.h"
#include <string>
#include "libutil.h"


TinyMac::TinyMac()
{

}

bool TinyMac::Decrypt(const uint8_t *data, uint8_t size)
{
    mPayloadSize = size - TM_MAC_SIZE;
    uint8_t Mic[16];
    uint8_t iv[12];
    memset(iv, 0, 12);

    mbedtls_gcm_context ctx;
    mbedtls_gcm_init (&ctx);
    mbedtls_gcm_setkey (&ctx, MBEDTLS_CIPHER_ID_AES, mKey, 128);
    mbedtls_gcm_starts (&ctx, MBEDTLS_GCM_DECRYPT, iv, 12, data, TM_MAC_HEADER_SIZE);

    int bound_size = (mPayloadSize >> 4) * 16;
    int last_packet_size = mPayloadSize - bound_size;

    if (bound_size > 0)
    {
        mbedtls_gcm_update (&ctx, bound_size, data + TM_MAC_HEADER_SIZE, mScratch);
    }
    if (last_packet_size > 0)
    {
        mbedtls_gcm_update (&ctx, last_packet_size, data + TM_MAC_HEADER_SIZE + bound_size, mScratch + bound_size);
    }

    // After the loop
    mbedtls_gcm_finish (&ctx, Mic, 16);
    mbedtls_gcm_free (&ctx);

    return (memcmp(&Mic[0], &data[size - 4], 4) == 0);
}

// Replace = true : encrypted data is copied over the clear text in the packet
void TinyMac::MacFinish(bool replace)
{
//    const mbedtls_cipher_info_t *cipher_info = mbedtls_cipher_info_from_type( MBEDTLS_CIPHER_AES_128_ECB );
    uint8_t Mic[16];
    uint8_t iv[12];
    memset(iv, 0, 12);

    mbedtls_gcm_context ctx;
    mbedtls_gcm_init (&ctx);
    mbedtls_gcm_setkey (&ctx, MBEDTLS_CIPHER_ID_AES, mKey, 128);
    mbedtls_gcm_starts (&ctx, MBEDTLS_GCM_ENCRYPT, iv, 12, mPacket, TM_MAC_HEADER_SIZE);

    int bound_size = (mPayloadSize >> 4) * 16;
    int last_packet_size = mPayloadSize - bound_size;

    mbedtls_gcm_update (&ctx, bound_size, mPacket + TM_MAC_HEADER_SIZE, mScratch);
    if (last_packet_size > 0) {
        mbedtls_gcm_update (&ctx, last_packet_size, mPacket + TM_MAC_HEADER_SIZE + bound_size, mScratch + bound_size);
    }

    // After the loop
    mbedtls_gcm_finish (&ctx, Mic, 16);
    mbedtls_gcm_free (&ctx);

    if (replace)
    {
        // replace clear text data with the encrypted one
         memcpy(&mPacket[TM_MAC_HEADER_SIZE], &mScratch[0], mPayloadSize);
    }

    // Add MIC at the end of the frame
    memcpy(&mPacket[mWrIndex], &Mic[0], 4);
    mWrIndex += 4;
}


void TinyMac::AddHeader(uint8_t cmd)
{
    Reset();

    mPacket[0] = cmd;
    mWrIndex++;
    beu32_put(&mPacket[mWrIndex], mDevAddr);
    mWrIndex += 4;
    beu32_put(&mPacket[mWrIndex], mFrameCounter);
    mWrIndex += 4;
}

void TinyMac::SetAddress(uint32_t addr)
{
    mDevAddr = addr;
}

void TinyMac::Reset()
{
    mPayloadSize = 0u;
    mWrIndex = 0U;
}

void TinyMac::AddBuffer(const uint8_t *data, uint8_t size)
{
    memcpy(&mPacket[mWrIndex], &data[0], size);
    mWrIndex += size;
    mPayloadSize += size;
}

uint8_t TinyMac::AddPadding(uint8_t *packet, uint8_t size)
{
    uint8_t padding = (16 - (size & 15));

    for (uint8_t i = 0; i < padding; i++)
    {
        packet[size + i] = i;
    }
    return size + padding;
}


void TinyMac::BuildJoinNetwork(const uint8_t * devEui, const uint8_t * appEui)
{
    // Mac
    mFrameCounter = 0U;
    mDevAddr = 0U;

    AddHeader(TM_JOIN_REQ);
    AddBuffer(devEui, 8);
    AddBuffer(appEui, 8);

    MacFinish(false);
}

void TinyMac::BuildData(const std::string &data)
{
    // Mac
    mFrameCounter++;

    AddHeader(TM_DATA_REQ);
    AddBuffer(reinterpret_cast<const uint8_t *>(data.c_str()), data.size());

    MacFinish(true);
}

void TinyMac::SetKey(uint8_t *key)
{
    memcpy(&mKey[0], key, 16);
}

/*
uint8_t TinyMac::BuildDataPacket(uint8_t *packet, const uint8_t * devEui, const uint8_t * appEui)
{
    // Mac
    mFrameCounter = 0U;
    mDevAddr = 0U;
    uint8_t idx = AddHeader(TM_JOIN_REQ, packet);

    // Build payload
    uint8_t payload_idx = 0;

    memcpy(&mScratch[payload_idx], &devEui[0], 8);
    payload_idx += 8;

    memcpy(&mScratch[payload_idx], &appEui[0], 8);
    payload_idx += 8;

    payload_idx = AddPadding(mScratch, payload_idx);

    uint8_t iv[12];
    memset(iv, 0, 12);

    uint32_t mic = MacComputeMic(packet, idx, &packet[idx], iv);

    beu32_put(&packet[idx], mic);

    return idx + 4;
}
*/
