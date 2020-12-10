#include "ShaTwo.h"
#include <cstdlib>
#include <cstring>

bool Sha256::Update(const std::string &s)
{
    int ret = mbedtls_sha256_update_ret( &mCtx, reinterpret_cast<const unsigned char*>(s.data()), s.size());
    return (ret == 0);
}

bool Sha256::Final(std::string &hmac)
{
    unsigned char sha256sum[32];
    int ret = mbedtls_sha256_finish_ret( &mCtx, sha256sum );

    hmac = std::string(reinterpret_cast<const char*>(sha256sum), 32);
    return (ret == 0);
}

Sha256::Sha256()
{
     mbedtls_sha256_init( &mCtx );
     mbedtls_sha256_starts_ret(&mCtx, 0);
}

Sha256::~Sha256()
{
    mbedtls_sha256_free( &mCtx );
}

std::string hmac_compute(const std::string &key, const std::string &message)
{
    static const uint32_t block_size = 64;
    std::string copy_key = key;
    unsigned char ipad[block_size];
    unsigned char opad[block_size];

    memset( ipad, 0x36, block_size );
    memset( opad, 0x5C, block_size );

    if (key.size() > block_size)
    {
        Sha256 sha;
        sha.Update(key);
        sha.Final(copy_key);
    }

    for(uint32_t i = 0; i < copy_key.size(); i++ )
    {
        ipad[i] = static_cast<unsigned char>( ipad[i] ^ copy_key[i] );
        opad[i] = static_cast<unsigned char>( opad[i] ^ copy_key[i] );
    }

    std::string hmac;
    std::string first_digest;

    {
        Sha256 sha;
        sha.Update(std::string(reinterpret_cast<char*>(ipad), block_size));
        sha.Update(message);
        sha.Final(first_digest);
    }

    Sha256 sha;
    sha.Update(std::string(reinterpret_cast<char*>(opad), block_size));
    sha.Update(first_digest);
    sha.Final(hmac);

    // return hash(opad || hash(ipad || message)) // Where || is concatenation
    return hmac;
}
