/* Borrowed from OpenBSD's red-black tree implementation. */

/*
 * Copyright 2002 Niels Provos <provos@citi.umich.edu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "IAllocator.hh"
#include "String.hh"
#include "utils.hh"
#include "Pair.hh"

#include <cstdio>
#include <cassert>

#ifdef _WIN32
    #undef IN
#endif

namespace adt
{

enum class RB_COLOR : u8 { BLACK, RED };
enum class RB_ORDER : u8 { PRE, IN, POST };

template<typename T>
struct RBNode
{
    RBNode* left {};
    RBNode* right {};
    RBNode* parent {};
    enum RB_COLOR color {};
    T data {};
};

template<typename T>
struct RBTreeBase
{
    RBNode<T>* pRoot = nullptr;
    u64 size = 0;
};

template<typename T>
inline RBNode<T>* RBRoot(RBTreeBase<T>* s);

template<typename T>
inline RBNode<T>* RBNodeAlloc(IAllocator* pA, const T& data);

template<typename T>
inline bool RBEmpty(RBTreeBase<T>* s);

template<typename T>
inline RBNode<T>* RBRemove(RBTreeBase<T>* s, RBNode<T>* elm);

template<typename T>
inline void RBRemoveAndFree(RBTreeBase<T>* s, IAllocator* p, RBNode<T>* elm);

template<typename T>
inline RBNode<T>* RBInsert(RBTreeBase<T>* s, RBNode<T>* elm, bool bAllowDuplicates);

template<typename T>
inline RBNode<T>* RBInsert(RBTreeBase<T>* s, IAllocator* pA, const T& data, bool bAllowDuplicates);

template<typename T>
inline Pair<RBNode<T>*, RBNode<T>*>
RBTraverse(
    RBNode<T>* parent,
    RBNode<T>* p,
    bool (*pfn)(RBNode<T>*, RBNode<T>*, void*),
    void* pUserData,
    RB_ORDER order
);

template<typename T>
inline RBNode<T>* RBSearch(RBNode<T>* p, const T& data);

template<typename T>
inline int RBDepth(RBNode<T>* p);

template<typename T>
inline void
RBPrintNodes(
    IAllocator* pA,
    const RBTreeBase<T>* s,
    const RBNode<T>* pNode,
    void (*pfnPrint)(const RBNode<T>*, void*),
    void* pFnData,
    FILE* pF,
    const String sPrefix,
    bool bLeft
);

template<typename T> inline void _RBSetBlackRed(RBNode<T>* black, RBNode<T>* red);
template<typename T> inline void _RBSet(RBNode<T>* elm, RBNode<T>* parent);
template<typename T> inline void _RBSetLinks(RBNode<T>* l, RBNode<T>* r);
template<typename T> inline void _RBRotateLeft(RBTreeBase<T>* s, RBNode<T>* elm);
template<typename T> inline void _RBRotateRight(RBTreeBase<T>* s, RBNode<T>* elm);
template<typename T> inline void _RBInsertColor(RBTreeBase<T>* s, RBNode<T>* elm);
template<typename T> inline void _RBRemoveColor(RBTreeBase<T>* s, RBNode<T>* parent, RBNode<T>* elm);

template<typename T>
inline void
_RBSetLinks(RBNode<T>* l, RBNode<T>* r)
{
    l->left = r->left;
    l->right = r->right;
    l->parent = r->parent;
    l->color = r->color;
}

template<typename T>
inline void
_RBSet(RBNode<T>* elm, RBNode<T>* parent)
{
    elm->parent = parent;
    elm->left = elm->right = nullptr;
    elm->color = RB_COLOR::RED;
}

template<typename T>
inline void
_RBSetBlackRed(RBNode<T>* black, RBNode<T>* red)
{
    black->color = RB_COLOR::BLACK;
    red->color = RB_COLOR::RED;
}

template<typename T>
inline RBNode<T>*
RBRoot(RBTreeBase<T>* s)
{
    return s->pRoot;
}

template<typename T>
inline RBNode<T>*
RBNodeAlloc(IAllocator* pA, const T& data)
{
    auto* r = (RBNode<T>*)alloc(pA, 1, sizeof(RBNode<T>));
    r->data = data;
    return r;
}

template<typename T>
inline bool
RBEmpty(RBTreeBase<T>* s)
{
    return s->pRoot;
}

template<typename T>
inline void
_RBRotateLeft(RBTreeBase<T>* s, RBNode<T>* elm)
{
    auto tmp = elm->right;
    if ((elm->right = tmp->left))
    {
        tmp->left->parent = elm;
    }
    if ((tmp->parent = elm->parent))
    {
        if (elm == elm->parent->left)
            elm->parent->left = tmp;
        else
            elm->parent->right = tmp;
    }
    else
        s->pRoot = tmp;

    tmp->left = elm;
    elm->parent = tmp;
}

template<typename T>
inline void
_RBRotateRight(RBTreeBase<T>* s, RBNode<T>* elm)
{
    auto tmp = elm->left;
    if ((elm->left = tmp->right))
    {
        tmp->right->parent = elm;
    }
    if ((tmp->parent = elm->parent))
    {
        if (elm == elm->parent->left)
            elm->parent->left = tmp;
        else
            elm->parent->right = tmp;
    }
    else
        s->pRoot = tmp;

    tmp->right = elm;
    elm->parent = tmp;
}

template<typename T>
inline void
_RBInsertColor(RBTreeBase<T>* s, RBNode<T>* elm)
{
    RBNode<T>* parent, * gparent, * tmp;
    while ((parent = elm->parent) && parent->color == RB_COLOR::RED)
    {
        gparent = parent->parent;
        if (parent == gparent->left)
        {
            tmp = gparent->right;
            if (tmp && tmp->color == RB_COLOR::RED)
            {
                tmp->color = RB_COLOR::BLACK;
                _RBSetBlackRed(parent, gparent);
                elm = gparent;
                continue;
            }
            if (parent->right == elm)
            {
                _RBRotateLeft(s, parent);
                tmp = parent;
                parent = elm;
                elm = tmp;
            }
            _RBSetBlackRed(parent, gparent);
            _RBRotateRight(s, gparent);
        }
        else
        {
            tmp = gparent->left;
            if (tmp && tmp->color == RB_COLOR::RED)
            {
                tmp->color = RB_COLOR::BLACK;
                _RBSetBlackRed(parent, gparent);
                elm = gparent;
                continue;
            }
            if (parent->left == elm)
            {
                _RBRotateRight(s, parent);
                tmp = parent;
                parent = elm;
                elm = tmp;
            }
            _RBSetBlackRed(parent, gparent);
            _RBRotateLeft(s, gparent);
        }
    }
    s->pRoot->color = RB_COLOR::BLACK;
}

template<typename T>
inline void
_RBRemoveColor(RBTreeBase<T>* s, RBNode<T>* parent, RBNode<T>* elm)
{
    RBNode<T>* tmp;
    while ((elm == nullptr || elm->color == RB_COLOR::BLACK) && elm != s->pRoot)
    {
        if (parent->left == elm)
        {
            tmp = parent->right;
            if (tmp->color == RB_COLOR::RED)
            {
                _RBSetBlackRed(tmp, parent);
                _RBRotateLeft(s, parent);
                tmp = parent->right;
            }
            if ((tmp->left == nullptr || tmp->left->color == RB_COLOR::BLACK) &&
                (tmp->right == nullptr || tmp->right->color == RB_COLOR::BLACK))
            {
                tmp->color = RB_COLOR::RED;
                elm = parent;
                parent = elm->parent;
            }
            else
            {
                if (tmp->right == nullptr || tmp->right->color == RB_COLOR::BLACK)
                {
                    RBNode<T>* oleft;
                    if ((oleft = tmp->left))
                        oleft->color = RB_COLOR::BLACK;
                    tmp->color = RB_COLOR::RED;
                    _RBRotateRight(s, tmp);
                    tmp = parent->right;
                }
                tmp->color = parent->color;
                parent->color = RB_COLOR::BLACK;
                if (tmp->right)
                    tmp->right->color = RB_COLOR::BLACK;
                _RBRotateLeft(s, parent);
                elm = s->pRoot;
                break;
            }
        }
        else
        {
            tmp = parent->left;
            if (tmp->color == RB_COLOR::RED)
            {
                _RBSetBlackRed(tmp, parent);
                _RBRotateRight(s, parent);
                tmp = parent->left;
            }
            if ((tmp->left == nullptr || tmp->left->color == RB_COLOR::BLACK) &&
                (tmp->right == nullptr || tmp->right->color == RB_COLOR::BLACK))
            {
                tmp->color = RB_COLOR::RED;
                elm = parent;
                parent = elm->parent;
            }
            else
            {
                if (tmp->left == nullptr || tmp->left->color == RB_COLOR::BLACK)
                {
                    RBNode<T>* oright;
                    if ((oright = tmp->right))
                        oright->color = RB_COLOR::BLACK;
                    tmp->color = RB_COLOR::RED;
                    _RBRotateLeft(s, tmp);
                    tmp = parent->left;
                }
                tmp->color = parent->color;
                parent->color = RB_COLOR::BLACK;
                if (tmp->left)
                    tmp->left->color = RB_COLOR::BLACK;
                _RBRotateRight(s, parent);
                elm = s->pRoot;
                break;
            }
        }
    }
    if (elm)
        elm->color = RB_COLOR::BLACK;
}

template<typename T>
inline RBNode<T>*
RBRemove(RBTreeBase<T>* s, RBNode<T>* elm)
{
    assert(s->size > 0 && "[RBTree]: empty");

    RBNode<T>* child, * parent, * old = elm;
    enum RB_COLOR color;
    if (elm->left == nullptr)
        child = elm->right;
    else if (elm->right == nullptr)
        child = elm->left;
    else
    {
        RBNode<T>* left;
        elm = elm->right;
        while ((left = elm->left))
            elm = left;
        child = elm->right;
        parent = elm->parent;
        color = elm->color;
        if (child)
            child->parent = parent;
        if (parent)
        {
            if (parent->left == elm)
                parent->left = child;
            else
                parent->right = child;
        }
        else
            s->pRoot = child;
        if (elm->parent == old)
            parent = elm;

        _RBSetLinks(elm, old);

        if (old->parent)
        {
            if (old->parent->left == old)
                old->parent->left = elm;
            else
                old->parent->right = elm;
        }
        else
            s->pRoot = elm;
        old->left->parent = elm;
        if (old->right)
            old->right->parent = elm;
        goto color;
    }
    parent = elm->parent;
    color = elm->color;
    if (child)
        child->parent = parent;
    if (parent)
    {
        if (parent->left == elm)
            parent->left = child;
        else
            parent->right = child;
    }
    else
        s->pRoot = child;
color:
    if (color == RB_COLOR::BLACK)
        _RBRemoveColor(s, parent, child);

    --s->size;
    return (old);
}

template<typename T>
inline void
RBRemoveAndFree(RBTreeBase<T>* s, IAllocator* p, RBNode<T>* elm)
{
    free(p, RBRemove(s, elm));
}

/* create RBNode outside then insert */
template<typename T>
inline RBNode<T>*
RBInsert(RBTreeBase<T>* s, RBNode<T>* elm, bool bAllowDuplicates)
{
    RBNode<T>* parent = nullptr;
    RBNode<T>* tmp = s->pRoot;
    s64 comp = 0;
    while (tmp)
    {
        parent = tmp;
        comp = utils::compare(elm->data, parent->data);

        if (comp == 0)
        {
            /* left case */
            if (bAllowDuplicates) tmp = tmp->left;
            else return tmp;
        }
        else if (comp < 0) tmp = tmp->left;
        else tmp = tmp->right;
    }

    _RBSet(elm, parent);

    if (parent != nullptr)
    {
        if (comp <= 0) parent->left = elm;
        else parent->right = elm;
    }
    else s->pRoot = elm;

    _RBInsertColor(s, elm);
    ++s->size;
    return elm;
}

template<typename T>
inline RBNode<T>*
RBInsert(RBTreeBase<T>* s, IAllocator* pA, const T& data, bool bAllowDuplicates)
{
    RBNode<T>* pNew = RBNodeAlloc(pA, data);
    return RBInsert(s, pNew, bAllowDuplicates);
}

template<typename T>
inline Pair<RBNode<T>*, RBNode<T>*>
RBTraversePRE(
    RBNode<T>* parent,
    RBNode<T>* p,
    bool (*pfn)(RBNode<T>*, RBNode<T>*, void*),
    void* pUserData
)
{
    if (p)
    {
        if (pfn(parent, p, pUserData)) return {parent, p};
        RBTraversePOST(p, p->left, pfn, pUserData);
        RBTraversePOST(p, p->right, pfn, pUserData);
    }

    return {};
}

template<typename T>
inline Pair<RBNode<T>*, RBNode<T>*>
RBTraverseIN(
    RBNode<T>* parent,
    RBNode<T>* p,
    bool (*pfn)(RBNode<T>*, RBNode<T>*, void*),
    void* pUserData
)
{
    if (p)
    {
        RBTraversePOST(p, p->left, pfn, pUserData);
        if (pfn(parent, p, pUserData)) return {parent, p};
        RBTraversePOST(p, p->right, pfn, pUserData);
    }

    return {};
}

template<typename T>
inline Pair<RBNode<T>*, RBNode<T>*>
RBTraversePOST(
    RBNode<T>* parent,
    RBNode<T>* p,
    bool (*pfn)(RBNode<T>*, RBNode<T>*, void*),
    void* pUserData
)
{
    if (p)
    {
        RBTraversePOST(p, p->left, pfn, pUserData);
        RBTraversePOST(p, p->right, pfn, pUserData);
        if (pfn(parent, p, pUserData)) return {parent, p};
    }

    return {};
}

/* early return if pfn returns true */
template<typename T>
inline Pair<RBNode<T>*, RBNode<T>*>
RBTraverse(
    RBNode<T>* parent,
    RBNode<T>* p,
    bool (*pfn)(RBNode<T>*, RBNode<T>*, void*),
    void* pUserData,
    RB_ORDER order
)
{
    switch (order)
    {
        case RB_ORDER::PRE:
        return RBTraversePRE(parent, p, pfn, pUserData);

        case RB_ORDER::IN:
        return RBTraverseIN(parent, p, pfn, pUserData);

        case RB_ORDER::POST:
        return RBTraversePOST(parent, p, pfn, pUserData);
    }

    assert(false && "[RbTree]: incorrect RB_ORDER");
    return {};
}

template<typename T>
inline RBNode<T>*
RBSearch(RBNode<T>* p, const T& data)
{
    // if (p)
    // {
    //     if (data == p->data) return p;
    //     else if (data < p->data) return RBSearch(p->left, data);
    //     else return RBSearch(p->right, data);
    // } else return nullptr;

    auto it = p;
    while (it)
    {
        s64 cmp = utils::compare(data, it->data);
        if (cmp == 0) return it;
        else if (cmp < 0) it = it->left;
        else it = it->right;
    }

    return nullptr;
}

template<typename T>
inline int
RBDepth(RBNode<T>* p)
{
    if (p)
    {
        int l = RBDepth(p->left);
        int r = RBDepth(p->right);
        return 1 + utils::max(l, r);
    } else return 0;
}

template<typename T>
inline void
RBPrintNodes(
    IAllocator* pA,
    const RBTreeBase<T>* s,
    const RBNode<T>* pNode,
    void (*pfnPrint)(const RBNode<T>*, void*),
    void* pFnData,
    FILE* pF,
    const String sPrefix,
    bool bLeft
)
{
    if (pNode)
    {
        fprintf(pF, "%.*s%s", sPrefix.size, sPrefix.pData, bLeft ? "|__" : "\\__");
        pfnPrint(pNode, pFnData);

        String sCat = StringCat(pA, sPrefix, bLeft ? "|   " : "    ");

        RBPrintNodes(pA, s, pNode->left, pfnPrint, pFnData, pF, sCat, true);
        RBPrintNodes(pA, s, pNode->right, pfnPrint, pFnData, pF, sCat, false);

        free(pA, sCat.pData);
    }
}

template<typename T>
inline void
RBDestroy(RBTreeBase<T>* s, IAllocator* pAlloc)
{
    auto pfnFree = +[]([[maybe_unused]] RBNode<T>* pPar, RBNode<T>* p, void* data) -> bool {
        free((IAllocator*)data, p);

        return false;
    };

    RBTraverse({}, s->pRoot, pfnFree, pAlloc, RB_ORDER::POST);
}

template<typename T>
struct RBTree
{
    RBTreeBase<T> base {};
    IAllocator* pAlloc {};
    
    RBTree() = default;
    RBTree(IAllocator* p) : pAlloc(p) {}
};

template<typename T>
inline RBNode<T>* RBRoot(RBTree<T>* s) { return RBRoot<T>(&s->base); }

template<typename T>
inline bool RBEmpty(RBTree<T>* s) { return RBEmpty<T>(&s->base); }

template<typename T>
inline RBNode<T>* RBRemove(RBTree<T>* s, RBNode<T>* elm) { return RBRemove<T>(&s->base, elm); }

template<typename T>
inline RBNode<T>* RBRemove(RBTree<T>* s, const T& x) { return RBRemove<T>(&s->base, RBSearch<T>(s->base.pRoot, x)); }

template<typename T>
inline void RBRemoveAndFree(RBTree<T>* s, RBNode<T>* elm) { RBRemoveAndFree<T>(&s->base, s->pAlloc, elm); }

template<typename T>
inline void RBRemoveAndFree(RBTree<T>* s, const T& x) { RBRemoveAndFree(&s->base, s->pAlloc, RBSearch<T>(s->base.pRoot, x)); }

template<typename T>
inline RBNode<T>* RBInsert(RBTree<T>* s, RBNode<T>* elm, bool bAllowDuplicates) { return RBInsert<T>(&s->base, elm, bAllowDuplicates); }

template<typename T>
inline RBNode<T>* RBInsert(RBTree<T>* s, const T& data, bool bAllowDuplicates) { return RBInsert<T>(&s->base, s->pAlloc, data, bAllowDuplicates); }

template<typename T>
inline void RBDestroy(RBTree<T>* s) { RBDestroy<T>(&s->base, s->pAlloc); }

} /* namespace adt */
