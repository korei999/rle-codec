#pragma once

#include "String.hh"

namespace adt
{

struct BitReader
{
    u64 storedBits {};
    u8* pBuff {};
    u32 byteSize {};
    u32 bytePos {};
    u32 bitOff {}; /* bit offset after pBuff[bytePos] */
    u8 nStoredBits {};

    constexpr BitReader() = default;

    constexpr BitReader(u8* _pBuff, u32 _byteSize)
        : pBuff(_pBuff), byteSize(_byteSize) {}

    constexpr BitReader(String str)
        : pBuff((u8*)str.pData), byteSize(str.size) {}
};

constexpr u8
BitNext(BitReader* s)
{
    return {};
}

} /* namespace adt */
