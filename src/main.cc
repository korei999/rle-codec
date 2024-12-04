#include "adt/logs.hh"
#include "adt/file.hh"
#include "adt/Arena.hh"
#include "adt/defer.hh"
#include "adt/Vec.hh"

#include <limits>

using namespace adt;

struct EncodedChar
{
    u8 nRepeat {};
    u8 charCode {};
};

struct EncodedBuff
{
    VecBase<EncodedChar> vec {};
    u64 realByteSize {};
};

static bool
saveToOpenFile(const char* sPath)
{
    FILE* pf = fopen(sPath, "rb");

    if (!pf) return true;
    else
    {
        fclose(pf);
        return false;
    }
}

static EncodedBuff
encode(IAllocator* pAlloc, u8* pBuff, u64 size)
{
    decltype(EncodedChar::nRepeat) nConsecutive = 0;

    VecBase<EncodedChar> vec(pAlloc, size);

    for (u64 i = 0; i < size;)
    {
        u8 curr = pBuff[i];
        nConsecutive = 1;

        ++i;
        while (curr == pBuff[i] && i < size && nConsecutive < std::numeric_limits<decltype(EncodedChar::nRepeat)>::max())
        {
            ++nConsecutive;
            ++i;
        }

        VecPush(&vec, pAlloc, {nConsecutive, curr});
    }

    return {
        .vec = vec,
        .realByteSize = size
    };
}

static void
EncodedBuffWriteToFile(EncodedBuff* s, FILE* pFile)
{
    fwrite(&s->realByteSize, sizeof(s->realByteSize), 1, pFile);
    fwrite(VecData(&s->vec), sizeof((s->vec)[0]), VecSize(&s->vec), pFile);
}

static String
EncodedBuffDecode(EncodedBuff* s, IAllocator* pAlloc)
{
    String str = StringAlloc(pAlloc, s->realByteSize);

    u32 pos = 0;
    for (auto e : s->vec)
    {
        for (u32 r = 0; r < e.nRepeat; ++r)
            str[pos++] = e.charCode;
    }

    return str;
}

static EncodedBuff
buffToEncoder(const file::Buff buff)
{
    EncodedBuff eb {.realByteSize = *(u64*)buff.pData};
    VecBase<EncodedChar> vec {};
    vec.size = buff.size - sizeof(eb.realByteSize);

    auto* pData = (EncodedChar*)(buff.pData + sizeof(eb.realByteSize));
    vec.pData = pData;

    eb.vec = vec;
    return eb;
}

static void
usage(char* argv0)
{
    LOG_EXIT(
        "usage:\n"
        "\t{} [-e(encode)|-d(decode)] <input file> <output file>\n", argv0
    );
}

static void
encode(IAllocator* pAlloc, const char* sPath, const char* sOutName)
{
    auto file = file::load(pAlloc, sPath);
    if (!file) LOG_EXIT("quit...\n");

    auto encoded = encode(pAlloc, (u8*)file.data.pData, file.data.size);

    if (!saveToOpenFile(sOutName)) LOG_EXIT("File: '{}' exists\n", sOutName);

    FILE* pFile = fopen(sOutName, "wb");
    EncodedBuffWriteToFile(&encoded, pFile);
    fclose(pFile);
}

static void
decode(IAllocator* pAlloc, const char* sPath, const char* sOutName)
{
    auto oBuff = file::loadToBuff(pAlloc, sPath);
    if (!oBuff) LOG_EXIT("quit...\n");

    auto eb = buffToEncoder(oBuff.data);
    auto sOrig = EncodedBuffDecode(&eb, pAlloc);

    if (!saveToOpenFile(sOutName)) LOG_EXIT("File: '{}' exists\n", sOutName);
    FILE* pFileOut = fopen(sOutName, "wb");
    fwrite(sOrig.pData, 1, sOrig.size, pFileOut);
}

int
main(int argc, char** argv)
{
    if (argc < 4) usage(argv[0]);

    Arena arena(SIZE_1M);
    defer( freeAll(&arena) );

    if (argv[1] == String("-e"))
    {
        encode(&arena.super, argv[2], argv[3]);
        return 0;
    }
    else if (argv[1] == String("-d"))
    {
        decode(&arena.super, argv[2], argv[3]);
        return 0;
    }
    else usage(argv[0]);
}
