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

static EncodedBuff
encode(IAllocator* pAlloc, u8* pBuff, u64 size)
{
    decltype(EncodedChar::nRepeat) nConsecutive = 0;

    u64 maxConsecutive = 0;

    VecBase<EncodedChar> vec(pAlloc);

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
        if (nConsecutive > maxConsecutive) maxConsecutive = nConsecutive;

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

int
main(int argc, char** argv)
{
    if (argc < 3)
        LOG_EXIT(
            "usage:\n"
            "\t{} <file to compress> <compressed filename>\n", argv[0]
        );

    Arena arena(SIZE_1M);
    defer( freeAll(&arena) );

    String sPath = argv[1];
    auto file = file::load(&arena.super, sPath);
    if (!file) LOG_EXIT("quit...\n");

    auto encoded = encode(&arena.super, (u8*)file.data.pData, file.data.size);

    FILE* pFile = fopen(argv[2], "wb");
    EncodedBuffWriteToFile(&encoded, pFile);

    String sComp = StringAlloc(&arena.super, argv[2]);
    String sCompEnding = StringCat(&arena.super, sComp, ".kle");

    String sOrig = EncodedBuffDecode(&encoded, &arena.super);
    FILE* pOgFile = fopen(sCompEnding.pData, "wb");
    fwrite(sOrig.pData, sizeof(sOrig[0]), sOrig.size, pOgFile);
}
