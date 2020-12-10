/**
 * MIT License
 * Copyright (c) 2019 Anthony Rabine
 */

#ifndef SHA1_HPP
#define SHA1_HPP

#include <iostream>
#include <string>
#include <cstdint>

/*****************************************************************************/
/**
 * @brief Sha1
 * This implementation supports two output formats (ASCII or binary)
 */
class Sha1
{
public:
    Sha1();
    void Update(const std::string &s);
    void Update(std::istream &is);
    std::string Final(bool ascii);
    static std::string FromFile(const std::string &filename, bool ascii);

private:
    static const unsigned int DIGEST_INTS = 5;  /* number of 32bit integers per SHA1 digest */
    static const unsigned int BLOCK_INTS = 16;  /* number of 32bit integers per SHA1 block */
    static const unsigned int BLOCK_BYTES = BLOCK_INTS * 4;

    std::uint32_t digest[DIGEST_INTS];
    std::string buffer;
    std::uint64_t transforms;

    void Reset();
    void Transform(std::uint32_t block[BLOCK_BYTES]);

    static void BufferToBlock(const std::string &buffer, std::uint32_t block[BLOCK_BYTES]);
    static void Read(std::istream &is, std::string &s, int max);
};


#endif  // SHA1_HPP

//=============================================================================
// End of file Sha1.h
//=============================================================================
