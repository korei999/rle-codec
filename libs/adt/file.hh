#pragma once

#include "String.hh"
#include "logs.hh"
#include "Opt.hh"
#include "defer.hh"

namespace adt
{
namespace file
{

struct Buff
{
    u8* pData {};
    u64 size {};
};

[[nodiscard]]
inline Opt<String>
load(IAllocator* pAlloc, String sPath)
{
    FILE* pf = fopen(sPath.pData, "rb");
    if (!pf)
    {
        LOG_WARN("Error opening '{}' file\n", sPath);
        return {};
    }
    defer(fclose(pf));

    String ret {};

    fseek(pf, 0, SEEK_END);
    long size = ftell(pf) + 1;
    rewind(pf);

    ret.pData = (char*)alloc(pAlloc, size, sizeof(char));
    ret.size = size - 1;
    fread(ret.pData, 1, ret.size, pf);

    return {ret, true};
}

[[nodiscard]]
inline Opt<Buff>
loadToBuff(IAllocator* pAlloc, String sPath)
{
    auto ret = load(pAlloc, sPath);
    if (ret)
    {
        Buff b {.pData = (u8*)ret.data.pData, .size = ret.data.size};
        return b;
    }
    else return {};
}

[[nodiscard]]
constexpr String
getPathEnding(String sPath)
{
    u32 lastSlash = StringLastOf(sPath, '/');
    return String(&sPath[lastSlash + 1], &sPath[sPath.size - 1] - &sPath[lastSlash]);
}

[[nodiscard]]
inline String
replacePathEnding(IAllocator* pAlloc, String sPath, String sEnding)
{
    u32 lastSlash = StringLastOf(sPath, '/');
    String sNoEnding = {&sPath[0], lastSlash + 1};
    String r = StringCat(pAlloc, sNoEnding, sEnding);
    return r;
}

} /* namespace file */
} /* namespace adt */
