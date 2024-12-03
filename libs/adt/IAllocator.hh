#pragma once

#include "types.hh"

namespace adt
{

constexpr u64 align(u64 x, u64 to) { return ((x) + to - 1) & (~(to - 1)); }
constexpr u64 align8(u64 x) { return align(x, 8); }
constexpr bool isPowerOf2(u64 x) { return (x & (x - 1)) == 0; }

constexpr u64
nextPowerOf2(u64 x)
{
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    ++x;

    return x;
}

constexpr u64 SIZE_MIN = 2UL;
constexpr u64 SIZE_1K = 1024UL;
constexpr u64 SIZE_8K = 8UL * SIZE_1K;
constexpr u64 SIZE_1M = SIZE_1K * SIZE_1K; 
constexpr u64 SIZE_8M = 8UL * SIZE_1M; 
constexpr u64 SIZE_1G = SIZE_1M * SIZE_1K; 
constexpr u64 SIZE_8G = SIZE_1G * SIZE_1K;

struct IAllocator;

/* at least alloc() and free() or freeAll() must be supported */
struct AllocatorVTable
{
    void* (*alloc)(IAllocator* s, u64 mCount, u64 mSize);
    void* (*zalloc)(IAllocator* s, u64 mCount, u64 mSize);
    void* (*realloc)(IAllocator* s, void* p, u64 mCount, u64 mSize);
    void (*free)(IAllocator* s, void* p);
    void (*freeAll)(IAllocator* s);
};

struct IAllocator
{
    const AllocatorVTable* pVTable {};
};

[[nodiscard]] ADT_NO_UB constexpr void* alloc(IAllocator* s, u64 mCount, u64 mSize) { return s->pVTable->alloc(s, mCount, mSize); }
[[nodiscard]] ADT_NO_UB constexpr void* zalloc(IAllocator* s, u64 mCount, u64 mSize) { return s->pVTable->zalloc(s, mCount, mSize); }
[[nodiscard]] ADT_NO_UB constexpr void* realloc(IAllocator* s, void* p, u64 mCount, u64 mSize) { return s->pVTable->realloc(s, p, mCount, mSize); }
ADT_NO_UB constexpr void free(IAllocator* s, void* p) { s->pVTable->free(s, p); }
ADT_NO_UB constexpr void freeAll(IAllocator* s) { s->pVTable->freeAll(s); }

} /* namespace adt */
