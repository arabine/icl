#ifndef TINYMAC_H
#define TINYMAC_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>

typedef enum {
    TM_JOIN_REQ,
    TM_JOIN_ACCEPT,
    TM_JOIN_DENIED,
    TM_DATA_REQ,
    TM_DATA_ACCEPT,
    TM_DATA_DENIED,
} TmFrameType_t;

#define TM_MAX_PAYLOAD_SIZE 240
#define TM_MAC_HEADER_SIZE  (1 + 4 + 4) // 1 cmd + 4 addr + 4 fc +
#define TM_MAC_SIZE         (TM_MAC_HEADER_SIZE + 4) //   4 MIC
#define TM_MAX_PACKET_SIZE (TM_MAX_PAYLOAD_SIZE + TM_MAC_SIZE)

class TinyMac
{
public:
    TinyMac();

    void MacFinish(bool replace);
    void BuildJoinNetwork(const uint8_t *devEui, const uint8_t *appEui);

    const uint8_t *GetPacket() { return mPacket; };
    uint8_t GetSize() { return TM_MAC_SIZE + mPayloadSize; }

    const uint8_t *GetData() { return mScratch; };
    uint8_t GetDataSize() { return mPayloadSize; }

    void SetKey(uint8_t *key);

    bool Decrypt(const uint8_t *data, uint8_t size);
    void SetAddress(uint32_t addr);
    void BuildData(const std::string &data);
private:
    uint8_t AddPadding(uint8_t *packet, uint8_t size);
    void AddHeader(uint8_t cmd);
    void Reset();

    uint8_t mScratch[TM_MAX_PACKET_SIZE];
    uint8_t mPacket[TM_MAX_PACKET_SIZE];

    uint8_t mKey[16];

    uint8_t mPayloadSize;
    uint8_t mWrIndex;
    uint32_t mFrameCounter;
    uint32_t mDevAddr;
    void AddBuffer(const uint8_t *data, uint8_t size);
};

#endif // TINYMAC_H
