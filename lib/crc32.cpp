#include "crc32.h"
#include <array>
#include <optional>

std::string HexToStr( unsigned char *digest)
{ 
    char buf[9];
    for (int i=0; i<4; i++)
        sprintf(buf+i*2, "%02x", digest[i]);
    buf[8]=0;

    return std::string(buf);
}

std::string crc32(const unsigned char buf[], unsigned int len)
{
    static std::optional<std::array<unsigned char,256>> crcTable;
    unsigned long crc;
    if( !crcTable ){
        for (int i = 0; i < 256; i++)
        {
            crc = i;
            for (int j = 0; j < 8; j++)
                crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
            (*crcTable)[i] = crc;
        };
    }
    crc = 0xFFFFFFFFUL;
    while (len--){
        crc = (*crcTable)[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);
    }
    crc ^= 0xFFFFFFFFUL;
    return HexToStr((unsigned char*)&crc);
}
