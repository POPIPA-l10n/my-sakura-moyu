/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
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

#ifndef _SYS_TREE_H_
#define _SYS_TREE_H_

#include <sys/cdefs.h>

/*
 * This file defines data structures for different types of trees:
 * splay trees, rank-balanced and sorted digital search trees.
 *
 * A splay tree is a self-organizing data structure.  Every operation
 * on the tree causes a splay to happen.  The splay moves the requested
 * node to the root of the tree and partly rebalances it.
 *
 * This has the benefit that request locality causes faster lookups as
 * the requested nodes move to the top of the tree.  On the other hand,
 * every lookup causes memory writes.
 *
 * The Balance Theorem bounds the total access time for m operations
 * and n inserts on an initially empty tree as O((m + n)lg n).  The
 * amortized cost for a sequence of m accesses to a splay tree is O(lg n);
 *
 * A rank-balanced tree is a binary search tree with an integer
 * rank-difference as an attribute of each pointer from parent to child.
 * The sum of the rank-differences on any path from a node down to null is
 * the same, and defines the rank of that node. The rank of the null node
 * is -1.
 *
 * Different additional conditions define different sorts of balanced
 * trees, including "red-black" and "AVL" trees.  The set of conditions
 * applied here are the "weak-AVL" conditions of Haeupler, Sen and Tarjan:
 *      - every rank-difference is 1 or 2.
 *      - the rank of any leaf is 1.
 *
 * For historical reasons, rank differences that are even are associated
 * with the color red (Rank-Even-Difference), and the child that a red edge
 * points to is called a red child.
 *
 * Every operation on a rank-balanced tree is bounded as O(lg n).
 * The maximum height of a rank-balanced tree is 2lg (n+1).
 *
 * A sorted digital-search tree is a binary search tree with the binary
 * bits of the node key as branch selector. The maximum height of a sorted
 * digital-search tree cannot exceed the binary length of the key.
 */
#ifdef __cplusplus
/*
 * In C++ there can be structure trees and class trees:
 * <type> becomes a direct type specifier.
 */
#define TREE_TYPE_ALIAS 1
#endif

#ifdef TREE_TYPE_ALIAS
#define TREE(type) type
#else
#define TREE(type) struct type
#endif /* TREE_TYPE_ALIAS */

#define SPLAY_HEAD(name, type)                                          \
struct name {                                                           \
        TREE(type) *sph_root; /* root of the tree */                    \
}

#define SPLAY_INITIALIZER(root)                                         \
        { NULL }

#define SPLAY_INIT(root) do {                                           \
        (root)->sph_root = NULL;                                        \
} while (0)

#define SPLAY_ENTRY(type)                                               \
struct {                                                                \
        TREE(type) *spe_left; /* left element */                        \
        TREE(type) *spe_right; /* right element */                      \
}

#define SPLAY_LEFT(elm, field)          (elm)->field.spe_left
#define SPLAY_RIGHT(elm, field)         (elm)->field.spe_right
#define SPLAY_ROOT(head)                (head)->sph_root
#define SPLAY_EMPTY(head)               (SPLAY_ROOT(head) == NULL)

/* SPLAY_ROTATE_{LEFT,RIGHT} expect that tmp hold SPLAY_{RIGHT,LEFT} */
#define SPLAY_ROTATE_RIGHT(head, tmp, field) do {                       \
        SPLAY_LEFT((head)->sph_root, field) = SPLAY_RIGHT(tmp, field);  \
        SPLAY_RIGHT(tmp, field) = (head)->sph_root;                     \
        (head)->sph_root = tmp;                                         \
} while (0)

#define SPLAY_ROTATE_LEFT(head, tmp, field) do {                        \
        SPLAY_RIGHT((head)->sph_root, field) = SPLAY_LEFT(tmp, field);  \
        SPLAY_LEFT(tmp, field) = (head)->sph_root;                      \
        (head)->sph_root = tmp;                                         \
} while (0)

#define SPLAY_LINKLEFT(head, tmp, field) do {                           \
        SPLAY_LEFT(tmp, field) = (head)->sph_root;                      \
        tmp = (head)->sph_root;                                         \
        (head)->sph_root = SPLAY_LEFT((head)->sph_root, field);         \
} while (0)

#define SPLAY_LINKRIGHT(head, tmp, field) do {                          \
        SPLAY_RIGHT(tmp, field) = (head)->sph_root;                     \
        tmp = (head)->sph_root;                                         \
        (head)->sph_root = SPLAY_RIGHT((head)->sph_root, field);        \
} while (0)

#define SPLAY_ASSEMBLE(head, node, left, right, field) do {             \
        SPLAY_RIGHT(left, field) = SPLAY_LEFT((head)->sph_root, field); \
        SPLAY_LEFT(right, field) = SPLAY_RIGHT((head)->sph_root, field);\
        SPLAY_LEFT((head)->sph_root, field) = SPLAY_RIGHT(node, field); \
        SPLAY_RIGHT((head)->sph_root, field) = SPLAY_LEFT(node, field); \
} while (0)

/* Generates prototypes and inline functions */

#define SPLAY_PROTOTYPE(name, type, field, cmp)                         \
void name##_SPLAY(struct name *, TREE(type) *);                         \
void name##_SPLAY_MINMAX(struct name *, int);                           \
TREE(type) *name##_SPLAY_INSERT(struct name *, TREE(type) *);           \
TREE(type) *name##_SPLAY_REMOVE(struct name *, TREE(type) *);           \
                                                                        \
/* Finds the node with the same key as elm */                           \
static __unused __inline TREE(type) *                                   \
name##_SPLAY_FIND(struct name *head, TREE(type) *elm)                   \
{                                                                       \
        if (SPLAY_EMPTY(head))                                          \
                return(NULL);                                           \
        name##_SPLAY(head, elm);                                        \
        if ((cmp)(elm, (head)->sph_root) == 0)                          \
                return (head->sph_root);                                \
        return (NULL);                                                  \
}                                                                       \
                                                                        \
static __unused __inline TREE(type) *                                   \
name##_SPLAY_PREV(struct name *head, TREE(type) *elm)                   \
{                                                                       \
        name##_SPLAY(head, elm);                                        \
        if (SPLAY_LEFT(elm, field) != NULL) {                           \
                elm = SPLAY_LEFT(elm, field);                           \
                while (SPLAY_RIGHT(elm, field) != NULL) {               \
                        elm = SPLAY_RIGHT(elm, field);                  \
                }                                                       \
        } else                                                          \
                elm = NULL;                                             \
        return (elm);                                                   \
}                                                                       \
                                                                        \
static __unused __inline TREE(type) *                                   \
name##_SPLAY_NEXT(struct name *head, TREE(type) *elm)                   \
{                                                                       \
        name##_SPLAY(head, elm);                                        \
        if (SPLAY_RIGHT(elm, field) != NULL) {                          \
                elm = SPLAY_RIGHT(elm, field);                          \
                while (SPLAY_LEFT(elm, field) != NULL) {                \
                        elm = SPLAY_LEFT(elm, field);                   \
                }                                                       \
        } else                                                          \
                elm = NULL;                                             \
        return (elm);                                                   \
}                                                                       \
                                                                        \
static __unused __inline TREE(type) *                                   \
name##_SPLAY_MIN_MAX(struct name *head, int val)                        \
{                                                                       \
        name##_SPLAY_MINMAX(head, val);                                 \
        return (SPLAY_ROOT(head));                                      \
}

/* Main splay operation.
 * Moves node close to the key of elm to top
 */
#define SPLAY_GENERATE(name, type, field, cmp)                          \
TREE(type) *                                                            \
name##_SPLAY_INSERT(struct name *head, TREE(type) *elm)                 \
{                                                                       \
    if (SPLAY_EMPTY(head)) {                                            \
            SPLAY_LEFT(elm, field) = SPLAY_RIGHT(elm, field) = NULL;    \
    } else {                                                            \
            int __comp;                                                 \
            name##_SPLAY(head, elm);                                    \
            __comp = (cmp)(elm, (head)->sph_root);                      \
            if(__comp < 0) {                                            \
                    SPLAY_LEFT(elm, field) = SPLAY_LEFT((head)->sph_root, field);\
                    SPLAY_RIGHT(elm, field) = (head)->sph_root;         \
                    SPLAY_LEFT((head)->sph_root, field) = NULL;         \
            } else if (__comp > 0) {                                    \
                    SPLAY_RIGHT(elm, field) = SPLAY_RIGHT((head)->sph_root, field);\
                    SPLAY_LEFT(elm, field) = (head)->sph_root;          \
                    SPLAY_RIGHT((head)->sph_root, field) = NULL;        \
            } else                                                      \
                    return ((head)->sph_root);                          \
    }                                                                   \
    (head)->sph_root = (elm);                                           \
    return (NULL);                                                      \
}                                                                       \
                                                                        \
TREE(type) *                                                            \
name##_SPLAY_REMOVE(struct name *head, TREE(type) *elm)                 \
{                                                                       \
        TREE(type) *__tmp;                                              \
        if (SPLAY_EMPTY(head))                                          \
                return (NULL);                                          \
        name##_SPLAY(head, elm);                                        \
        if ((cmp)(elm, (head)->sph_root) == 0) {                        \
                if (SPLAY_LEFT((head)->sph_root, field) == NULL) {      \
                        (head)->sph_root = SPLAY_RIGHT((head)->sph_root, field);\
                } else {                                                \
                        __tmp = SPLAY_RIGHT((head)->sph_root, field);   \
                        (head)->sph_root = SPLAY_LEFT((head)->sph_root, field);\
                        name##_SPLAY(head, elm);                        \
                        SPLAY_RIGHT((head)->sph_root, field) = __tmp;   \
                }                                                       \
                return (elm);                                           \
        }                                                               \
        return (NULL);                                                  \
}                                                                       \
                                                                        \
void                                                                    \
name##_SPLAY(struct name *head, TREE(type) *elm)                        \
{                                                                       \
        TREE(type) __node, *__left, *__right, *__tmp;                   \
        int __comp;                                                     \
                                                                        \
        SPLAY_LEFT(&__node, field) = SPLAY_RIGHT(&__node, field) = NULL;\
        __left = __right = &__node;                                     \
                                                                        \
        while ((__comp = (cmp)(elm, (head)->sph_root)) != 0) {          \
                if (__comp < 0) {                                       \
                        __tmp = SPLAY_LEFT((head)->sph_root, field);    \
                        if (__tmp == NULL)                              \
                                break;                                  \
                        if ((cmp)(elm, __tmp) < 0){                     \
                                SPLAY_ROTATE_RIGHT(head, __tmp, field); \
                                if (SPLAY_LEFT((head)->sph_root, field) == NULL)\
                                        break;                          \
                        }                                               \
                        SPLAY_LINKLEFT(head, __right, field);           \
                } else if (__comp > 0) {                                \
                        __tmp = SPLAY_RIGHT((head)->sph_root, field);   \
                        if (__tmp == NULL)                              \
                                break;                                  \
                        if ((cmp)(elm, __tmp) > 0){                     \
                                SPLAY_ROTATE_LEFT(head, __tmp, field);  \
                                if (SPLAY_RIGHT((head)->sph_root, field) == NULL)\
                                        break;                          \
                        }                                               \
                        SPLAY_LINKRIGHT(head, __left, field);           \
                }                                                       \
        }                                                               \
        SPLAY_ASSEMBLE(head, &__node, __left, __right, field);          \
}                                                                       \
                                                                        \
/* Splay with either the minimum or the maximum element                 \
 * Used to find minimum or maximum element in tree.                     \
 */                                                                     \
void name##_SPLAY_MINMAX(struct name *head, int __comp)                 \
{                                                                       \
        TREE(type) __node, *__left, *__right, *__tmp;                   \
                                                                        \
        SPLAY_LEFT(&__node, field) = SPLAY_RIGHT(&__node, field) = NULL;\
        __left = __right = &__node;                                     \
                                                                        \
        while (1) {                                                     \
                if (__comp < 0) {                                       \
                        __tmp = SPLAY_LEFT((head)->sph_root, field);    \
                        if (__tmp == NULL)                              \
                                break;                                  \
                        if (__comp < 0){                                \
                                SPLAY_ROTATE_RIGHT(head, __tmp, field); \
                                if (SPLAY_LEFT((head)->sph_root, field) == NULL)\
                                        break;                          \
                        }                                               \
                        SPLAY_LINKLEFT(head, __right, field);           \
                } else if (__comp > 0) {                                \
                        __tmp = SPLAY_RIGHT((head)->sph_root, field);   \
                        if (__tmp == NULL)                              \
                                break;                                  \
                        if (__comp > 0) {                               \
                                SPLAY_ROTATE_LEFT(head, __tmp, field);  \
                                if (SPLAY_RIGHT((head)->sph_root, field) == NULL)\
                                        break;                          \
                        }                                               \
                        SPLAY_LINKRIGHT(head, __left, field);           \
                }                                                       \
        }                                                               \
        SPLAY_ASSEMBLE(head, &__node, __left, __right, field);          \
}

#define SPLAY_NEGINF    -1
#define SPLAY_INF       1

#define SPLAY_INSERT(name, x, y)        name##_SPLAY_INSERT(x, y)
#define SPLAY_REMOVE(name, x, y)        name##_SPLAY_REMOVE(x, y)
#define SPLAY_FIND(name, x, y)          name##_SPLAY_FIND(x, y)
#define SPLAY_PREV(name, x, y)          name##_SPLAY_PREV(x, y)
#define SPLAY_NEXT(name, x, y)          name##_SPLAY_NEXT(x, y)
#define SPLAY_MIN(name, x)              (SPLAY_EMPTY(x) ? NULL          \
                                        : name##_SPLAY_MIN_MAX(x, SPLAY_NEGINF))
#define SPLAY_MAX(name, x)              (SPLAY_EMPTY(x) ? NULL          \
                                        : name##_SPLAY_MIN_MAX(x, SPLAY_INF))

#define SPLAY_FOREACH(x, name, head)                                    \
        for ((x) = SPLAY_MIN(name, head);                               \
             (x) != NULL;                                               \
             (x) = SPLAY_NEXT(name, head, x))

/* Macros that define a rank-balanced tree */
#define RB_HEAD(name, type)                                             \
struct name {                                                           \
        TREE(type) *rbh_root; /* root of the tree */                    \
}

#define RB_INITIALIZER(root)                                            \
        { NULL }

#define RB_INIT(root) do {                                              \
        (root)->rbh_root = NULL;                                        \
} while (0)

#define RB_ENTRY(type)                                                  \
struct {                                                                \
        TREE(type) *rbe_left;          /* left element */               \
        TREE(type) *rbe_right;         /* right element */              \
        TREE(type) *rbe_parent;        /* parent element */             \
}

#define RB_LEFT(elm, field)             (elm)->field.rbe_left
#define RB_RIGHT(elm, field)            (elm)->field.rbe_right

/*
 * With the expectation that any object of TREE(type) has an
 * address that is a multiple of 4, and that therefore the
 * 2 least significant bits of a pointer to TREE(type) are
 * always zero, this implementation sets those bits to indicate
 * that the left or right child of the tree node is "red".
 */
#define RB_UP(elm, field)               (elm)->field.rbe_parent
#define RB_BITS(elm, field)             (*(__uintptr_t *)&RB_UP(elm, field))
#define RB_RED_L                        ((__uintptr_t)1)
#define RB_RED_R                        ((__uintptr_t)2)
#define RB_RED_MASK                     ((__uintptr_t)3)
#define RB_FLIP_LEFT(elm, field)        (RB_BITS(elm, field) ^= RB_RED_L)
#define RB_FLIP_RIGHT(elm, field)       (RB_BITS(elm, field) ^= RB_RED_R)
#define RB_RED_LEFT(elm, field)         ((RB_BITS(elm, field) & RB_RED_L) != 0)
#define RB_RED_RIGHT(elm, field)        ((RB_BITS(elm, field) & RB_RED_R) != 0)
#define RB_PARENT(elm, field)           ((__typeof(RB_UP(elm, field)))  \
                                         (RB_BITS(elm, field) & ~RB_RED_MASK))
#define RB_ROOT(head)                   (head)->rbh_root
#define RB_EMPTY(head)                  (RB_ROOT(head) == NULL)

#define RB_SET_PARENT(dst, src, field) do {                             \
        RB_BITS(dst, field) &= RB_RED_MASK;                             \
        RB_BITS(dst, field) |= (__uintptr_t)src;                        \
} while (0)

#define RB_SET(elm, parent, field) do {                                 \
        RB_UP(elm, field) = parent;                                     \
        RB_LEFT(elm, field) = RB_RIGHT(elm, field) = NULL;              \
} while (0)

#define RB_COLOR(elm, field)    (RB_PARENT(elm, field) == NULL ? 0 :    \
                                RB_LEFT(RB_PARENT(elm, field), field) == elm ?\
                                RB_RED_LEFT(RB_PARENT(elm, field), field) :\
                                RB_RED_RIGHT(RB_PARENT(elm, field), field))

/*
 * Something to be invoked in a loop at the root of every modified subtree,
 * from the bottom up to the root, to update augmented node data.
 */
#ifndef RB_AUGMENT
#define RB_AUGMENT(x)   break
#endif

#define RB_SWAP_CHILD(head, out, in, field) do {                        \
        if (RB_PARENT(out, field) == NULL)                              \
                RB_ROOT(head) = (in);                                   \
        else if ((out) == RB_LEFT(RB_PARENT(out, field), field))        \
                RB_LEFT(RB_PARENT(out, field), field) = (in);           \
        else                                                            \
                RB_RIGHT(RB_PARENT(out, field), field) = (in);          \
} while (0)

#define RB_ROTATE_LEFT(head, elm, tmp, field) do {                      \
        (tmp) = RB_RIGHT(elm, field);                                   \
        if ((RB_RIGHT(elm, field) = RB_LEFT(tmp, field)) != NULL) {     \
                RB_SET_PARENT(RB_RIGHT(elm, field), elm, field);        \
        }                                                               \
        RB_SET_PARENT(tmp, RB_PARENT(elm, field), field);               \
        RB_SWAP_CHILD(head, elm, tmp, field);                           \
        RB_LEFT(tmp, field) = (elm);                                    \
        RB_SET_PARENT(elm, tmp, field);                                 \
        RB_AUGMENT(elm);                                                \
} while (0)

#define RB_ROTATE_RIGHT(head, elm, tmp, field) do {                     \
        (tmp) = RB_LEFT(elm, field);                                    \
        if ((RB_LEFT(elm, field) = RB_RIGHT(tmp, field)) != NULL) {     \
                RB_SET_PARENT(RB_LEFT(elm, field), elm, field);         \
        }                                                               \
        RB_SET_PARENT(tmp, RB_PARENT(elm, field), field);               \
        RB_SWAP_CHILD(head, elm, tmp, field);                           \
        RB_RIGHT(tmp, field) = (elm);                                   \
        RB_SET_PARENT(elm, tmp, field);                                 \
        RB_AUGMENT(elm);                                                \
} while (0)

/* Generates prototypes and inline functions */
#define RB_PROTOTYPE(name, type, field, cmp)                            \
        RB_PROTOTYPE_INTERNAL(name, type, field, cmp,)
#define RB_PROTOTYPE_STATIC(name, type, field, cmp)                     \
        RB_PROTOTYPE_INTERNAL(name, type, field, cmp, __unused static)
#define RB_PROTOTYPE_INTERNAL(name, type, field, cmp, attr)             \
        RB_PROTOTYPE_INSERT_COLOR(name, type, attr);                    \
        RB_PROTOTYPE_REMOVE_COLOR(name, type, attr);                    \
        RB_PROTOTYPE_INSERT(name, type, attr);                          \
        RB_PROTOTYPE_REMOVE(name, type, attr);                          \
        RB_PROTOTYPE_FIND(name, type, attr);                            \
        RB_PROTOTYPE_NFIND(name, type, attr);                           \
        RB_PROTOTYPE_NEXT(name, type, attr);                            \
        RB_PROTOTYPE_PREV(name, type, attr);                            \
        RB_PROTOTYPE_MINMAX(name, type, attr);                          \
        RB_PROTOTYPE_REINSERT(name, type, attr);
#define RB_PROTOTYPE_INSERT_COLOR(name, type, attr)                     \
        attr void name##_RB_INSERT_COLOR(struct name *, TREE(type) *)
#define RB_PROTOTYPE_REMOVE_COLOR(name, type, attr)                     \
        attr void name##_RB_REMOVE_COLOR(struct name *,                 \
            TREE(type) *, TREE(type) *)
#define RB_PROTOTYPE_REMOVE(name, type, attr)                           \
        attr TREE(type) *                                               \
        name##_RB_REMOVE(struct name *, TREE(type) *)
#define RB_PROTOTYPE_INSERT(name, type, attr)                           \
        attr TREE(type) *                                               \
        name##_RB_INSERT(struct name *, TREE(type) *)
#define RB_PROTOTYPE_FIND(name, type, attr)                             \
        attr TREE(type) *                                               \
        name##_RB_FIND(struct name *, TREE(type) *)
#define RB_PROTOTYPE_NFIND(name, type, attr)                            \
        attr TREE(type) *                                               \
        name##_RB_NFIND(struct name *, TREE(type) *)
#define RB_PROTOTYPE_NEXT(name, type, attr)                             \
        attr TREE(type) *name##_RB_NEXT(TREE(type) *)
#define RB_PROTOTYPE_PREV(name, type, attr)                             \
        attr TREE(type) *name##_RB_PREV(TREE(type) *)
#define RB_PROTOTYPE_MINMAX(name, type, attr)                           \
        attr TREE(type) *name##_RB_MINMAX(struct name *, int)
#define RB_PROTOTYPE_REINSERT(name, type, attr)                         \
        attr TREE(type) *                                               \
        name##_RB_REINSERT(struct name *, TREE(type) *)

/* Main rb operation.
 * Moves node close to the key of elm to top
 */
#define RB_GENERATE(name, type, field, cmp)                             \
        RB_GENERATE_INTERNAL(name, type, field, cmp,)
#define RB_GENERATE_STATIC(name, type, field, cmp)                      \
        RB_GENERATE_INTERNAL(name, type, field, cmp, __unused static)
#define RB_GENERATE_INTERNAL(name, type, field, cmp, attr)              \
        RB_GENERATE_INSERT_COLOR(name, type, field, attr)               \
        RB_GENERATE_REMOVE_COLOR(name, type, field, attr)               \
        RB_GENERATE_INSERT(name, type, field, cmp, attr)                \
        RB_GENERATE_REMOVE(name, type, field, attr)                     \
        RB_GENERATE_FIND(name, type, field, cmp, attr)                  \
        RB_GENERATE_NFIND(name, type, field, cmp, attr)                 \
        RB_GENERATE_NEXT(name, type, field, attr)                       \
        RB_GENERATE_PREV(name, type, field, attr)                       \
        RB_GENERATE_MINMAX(name, type, field, attr)                     \
        RB_GENERATE_REINSERT(name, type, field, cmp, attr)

#define RB_GENERATE_INSERT_COLOR(name, type, field, attr)               \
attr void                                                               \
name##_RB_INSERT_COLOR(struct name *head, TREE(type) *elm)              \
{                                                                       \
        TREE(type) *child, *parent;                                     \
        while ((parent = RB_PARENT(elm, field)) != NULL) {              \
                if (RB_LEFT(parent, field) == elm) {                    \
                        if (RB_RED_LEFT(parent, field)) {               \
                                RB_FLIP_LEFT(parent, field);            \
                                return;                                 \
                        }                                               \
                        RB_FLIP_RIGHT(parent, field);                   \
                        if (RB_RED_RIGHT(parent, field)) {              \
                                elm = parent;                           \
                                continue;                               \
                        }                                               \
                        if (!RB_RED_RIGHT(elm, field)) {                \
                                RB_FLIP_LEFT(elm, field);               \
                                RB_ROTATE_LEFT(head, elm, child, field);\
                                if (RB_RED_LEFT(child, field))          \
                                        RB_FLIP_RIGHT(elm, field);      \
                                else if (RB_RED_RIGHT(child, field))    \
                                        RB_FLIP_LEFT(parent, field);    \
                                elm = child;                            \
                        }                                               \
                        RB_ROTATE_RIGHT(head, parent, elm, field);      \
                } else {                                                \
                        if (RB_RED_RIGHT(parent, field)) {              \
                                RB_FLIP_RIGHT(parent, field);           \
                                return;                                 \
                        }                                               \
                        RB_FLIP_LEFT(parent, field);                    \
                        if (RB_RED_LEFT(parent, field)) {               \
                                elm = parent;                           \
                                continue;                               \
                        }                                               \
                        if (!RB_RED_LEFT(elm, field)) {                 \
                                RB_FLIP_RIGHT(elm, field);              \
                                RB_ROTATE_RIGHT(head, elm, child, field);\
                                if (RB_RED_RIGHT(child, field))         \
                                        RB_FLIP_LEFT(elm, field);       \
                                else if (RB_RED_LEFT(child, field))     \
                                        RB_FLIP_RIGHT(parent, field);   \
                                elm = child;                            \
                        }                                               \
                        RB_ROTATE_LEFT(head, parent, elm, field);       \
                }                                                       \
                RB_BITS(elm, field) &= ~RB_RED_MASK;                    \
                break;                                                  \
        }                                                               \
}

#define RB_GENERATE_REMOVE_COLOR(name, type, field, attr)               \
attr void                                                               \
name##_RB_REMOVE_COLOR(struct name *head,                               \
    TREE(type) *parent, TREE(type) *elm)                                \
{                                                                       \
        TREE(type) *sib;                                                \
        if (RB_LEFT(parent, field) == elm &&                            \
            RB_RIGHT(parent, field) == elm) {                           \
                RB_BITS(parent, field) &= ~RB_RED_MASK;                 \
                elm = parent;                                           \
                parent = RB_PARENT(elm, field);                         \
                if (parent == NULL)                                     \
                        return;                                         \
        }                                                               \
        do  {                                                           \
                if (RB_LEFT(parent, field) == elm) {                    \
                        if (!RB_RED_LEFT(parent, field)) {              \
                                RB_FLIP_LEFT(parent, field);            \
                                return;                                 \
                        }                                               \
                        if (RB_RED_RIGHT(parent, field)) {              \
                                RB_FLIP_RIGHT(parent, field);           \
                                elm = parent;                           \
                                continue;                               \
                        }                                               \
                        sib = RB_RIGHT(parent, field);                  \
                        if ((~RB_BITS(sib, field) & RB_RED_MASK) == 0) {\
                                RB_BITS(sib, field) &= ~RB_RED_MASK;    \
                                elm = parent;                           \
                                continue;                               \
                        }                                               \
                        RB_FLIP_RIGHT(sib, field);                      \
                        if (RB_RED_LEFT(sib, field))                    \
                                RB_FLIP_LEFT(parent, field);            \
                        else if (!RB_RED_RIGHT(sib, field)) {           \
                                RB_FLIP_LEFT(parent, field);            \
                                RB_ROTATE_RIGHT(head, sib, elm, field); \
                                if (RB_RED_RIGHT(elm, field))           \
                                        RB_FLIP_LEFT(sib, field);       \
                                if (RB_RED_LEFT(elm, field))            \
                                        RB_FLIP_RIGHT(parent, field);   \
                                RB_BITS(elm, field) |= RB_RED_MASK;     \
                                sib = elm;                              \
                        }                                               \
                        RB_ROTATE_LEFT(head, parent, sib, field);       \
                } else {                                                \
                        if (!RB_RED_RIGHT(parent, field)) {             \
                                RB_FLIP_RIGHT(parent, field);           \
                                return;                                 \
                        }                                               \
                        if (RB_RED_LEFT(parent, field)) {               \
                                RB_FLIP_LEFT(parent, field);            \
                                elm = parent;                           \
                                continue;                               \
                        }                                               \
                        sib = RB_LEFT(parent, field);                   \
                        if ((~RB_BITS(sib, field) & RB_RED_MASK) == 0) {\
                                RB_BITS(sib, field) &= ~RB_RED_MASK;    \
                                elm = parent;                           \
                                continue;                               \
                        }                                               \
                        RB_FLIP_LEFT(sib, field);                       \
                        if (RB_RED_RIGHT(sib, field))                   \
                                RB_FLIP_RIGHT(parent, field);           \
                        else if (!RB_RED_LEFT(sib, field)) {            \
                                RB_FLIP_RIGHT(parent, field);           \
                                RB_ROTATE_LEFT(head, sib, elm, field);  \
                                if (RB_RED_LEFT(elm, field))            \
                                        RB_FLIP_RIGHT(sib, field);      \
                                if (RB_RED_RIGHT(elm, field))           \
                                        RB_FLIP_LEFT(parent, field);    \
                                RB_BITS(elm, field) |= RB_RED_MASK;     \
                                sib = elm;                              \
                        }                                               \
                        RB_ROTATE_RIGHT(head, parent, sib, field);      \
                }                                                       \
                break;                                                  \
        } while ((parent = RB_PARENT(elm, field)) != NULL);             \
}

#define RB_GENERATE_REMOVE(name, type, field, attr)                     \
attr TREE(type) *                                                       \
name##_RB_REMOVE(struct name *head, TREE(type) *elm)                    \
{                                                                       \
        TREE(type) *child, *old, *parent, *right;                       \
                                                                        \
        old = elm;                                                      \
        parent = RB_PARENT(elm, field);                                 \
        right = RB_RIGHT(elm, field);                                   \
        if (RB_LEFT(elm, field) == NULL)                                \
                elm = child = right;                                    \
        else if (right == NULL)                                         \
                elm = child = RB_LEFT(elm, field);                      \
        else {                                                          \
                if ((child = RB_LEFT(right, field)) == NULL) {          \
                        child = RB_RIGHT(right, field);                 \
                        RB_RIGHT(old, field) = child;                   \
                        parent = elm = right;                           \
                } else {                                                \
                        do                                              \
                                elm = child;                            \
                        while ((child = RB_LEFT(elm, field)) != NULL);  \
                        child = RB_RIGHT(elm, field);                   \
                        parent = RB_PARENT(elm, field);                 \
                        RB_LEFT(parent, field) = child;                 \
                        RB_SET_PARENT(RB_RIGHT(old, field), elm, field);\
                }                                                       \
                RB_SET_PARENT(RB_LEFT(old, field), elm, field);         \
                elm->field = old->field;                                \
        }                                                               \
        RB_SWAP_CHILD(head, old, elm, field);                           \
        if (child != NULL)                                              \
                RB_SET_PARENT(child, parent, field);                    \
        if (parent != NULL)                                             \
                name##_RB_REMOVE_COLOR(head, parent, child);            \
        while (parent != NULL) {                                        \
                RB_AUGMENT(parent);                                     \
                parent = RB_PARENT(parent, field);                      \
        }                                                               \
        return (old);                                                   \
}

#define RB_GENERATE_INSERT(name, type, field, cmp, attr)                \
/* Inserts a node into the RB tree */                                   \
attr TREE(type) *                                                       \
name##_RB_INSERT(struct name *head, TREE(type) *elm)                    \
{                                                                       \
        TREE(type) *tmp;                                                \
        TREE(type) *parent = NULL;                                      \
        int comp = 0;                                                   \
        tmp = RB_ROOT(head);                                            \
        while (tmp) {                                                   \
                parent = tmp;                                           \
                comp = (cmp)(elm, parent);                              \
                if (comp < 0)                                           \
                        tmp = RB_LEFT(tmp, field);                      \
                else if (comp > 0)                                      \
                        tmp = RB_RIGHT(tmp, field);                     \
                else                                                    \
                        return (tmp);                                   \
        }                                                               \
        RB_SET(elm, parent, field);                                     \
        if (parent == NULL)                                             \
                RB_ROOT(head) = elm;                                    \
        else if (comp < 0)                                              \
                RB_LEFT(parent, field) = elm;                           \
        else                                                            \
                RB_RIGHT(parent, field) = elm;                          \
        name##_RB_INSERT_COLOR(head, elm);                              \
        while (elm != NULL) {                                           \
                RB_AUGMENT(elm);                                        \
                elm = RB_PARENT(elm, field);                            \
        }                                                               \
        return (NULL);                                                  \
}

#define RB_GENERATE_FIND(name, type, field, cmp, attr)                  \
/* Finds the node with the same key as elm */                           \
attr TREE(type) *                                                       \
name##_RB_FIND(struct name *head, TREE(type) *elm)                      \
{                                                                       \
        TREE(type) *tmp = RB_ROOT(head);                                \
        int comp;                                                       \
        while (tmp) {                                                   \
                comp = cmp(elm, tmp);                                   \
                if (comp < 0)                                           \
                        tmp = RB_LEFT(tmp, field);                      \
                else if (comp > 0)                                      \
                        tmp = RB_RIGHT(tmp, field);                     \
                else                                                    \
                        return (tmp);                                   \
        }                                                               \
        return (NULL);                                                  \
}

#define RB_GENERATE_NFIND(name, type, field, cmp, attr)                 \
/* Finds the first node greater than or equal to the search key */      \
attr TREE(type) *                                                       \
name##_RB_NFIND(struct name *head, TREE(type) *elm)                     \
{                                                                       \
        TREE(type) *tmp = RB_ROOT(head);                                \
        TREE(type) *res = NULL;                                         \
        int comp;                                                       \
        while (tmp) {                                                   \
                comp = cmp(elm, tmp);                                   \
                if (comp < 0) {                                         \
                        res = tmp;                                      \
                        tmp = RB_LEFT(tmp, field);                      \
                }                                                       \
                else if (comp > 0)                                      \
                        tmp = RB_RIGHT(tmp, field);                     \
                else                                                    \
                        return (tmp);                                   \
        }                                                               \
        return (res);                                                   \
}

#define RB_GENERATE_NEXT(name, type, field, attr)                       \
/* ARGSUSED */                                                          \
attr TREE(type) *                                                       \
name##_RB_NEXT(TREE(type) *elm)                                         \
{                                                                       \
        if (RB_RIGHT(elm, field)) {                                     \
                elm = RB_RIGHT(elm, field);                             \
                while (RB_LEFT(elm, field))                             \
                        elm = RB_LEFT(elm, field);                      \
        } else {                                                        \
                if (RB_PARENT(elm, field) &&                            \
                    (elm == RB_LEFT(RB_PARENT(elm, field), field)))     \
                        elm = RB_PARENT(elm, field);                    \
                else {                                                  \
                        while (RB_PARENT(elm, field) &&                 \
                            (elm == RB_RIGHT(RB_PARENT(elm, field), field)))\
                                elm = RB_PARENT(elm, field);            \
                        elm = RB_PARENT(elm, field);                    \
                }                                                       \
        }                                                               \
        return (elm);                                                   \
}

#define RB_GENERATE_PREV(name, type, field, attr)                       \
/* ARGSUSED */                                                          \
attr TREE(type) *                                                       \
name##_RB_PREV(TREE(type) *elm)                                         \
{                                                                       \
        if (RB_LEFT(elm, field)) {                                      \
                elm = RB_LEFT(elm, field);                              \
                while (RB_RIGHT(elm, field))                            \
                        elm = RB_RIGHT(elm, field);                     \
        } else {                                                        \
                if (RB_PARENT(elm, field) &&                            \
                    (elm == RB_RIGHT(RB_PARENT(elm, field), field)))    \
                        elm = RB_PARENT(elm, field);                    \
                else {                                                  \
                        while (RB_PARENT(elm, field) &&                 \
                            (elm == RB_LEFT(RB_PARENT(elm, field), field)))\
                                elm = RB_PARENT(elm, field);            \
                        elm = RB_PARENT(elm, field);                    \
                }                                                       \
        }                                                               \
        return (elm);                                                   \
}

#define RB_GENERATE_MINMAX(name, type, field, attr)                     \
attr TREE(type) *                                                       \
name##_RB_MINMAX(struct name *head, int val)                            \
{                                                                       \
        TREE(type) *tmp = RB_ROOT(head);                                \
        TREE(type) *parent = NULL;                                      \
        while (tmp) {                                                   \
                parent = tmp;                                           \
                if (val < 0)                                            \
                        tmp = RB_LEFT(tmp, field);                      \
                else                                                    \
                        tmp = RB_RIGHT(tmp, field);                     \
        }                                                               \
        return (parent);                                                \
}

#define RB_GENERATE_REINSERT(name, type, field, cmp, attr)              \
attr TREE(type) *                                                       \
name##_RB_REINSERT(struct name *head, TREE(type) *elm)                  \
{                                                                       \
        TREE(type) *cmpelm;                                             \
        if (((cmpelm = RB_PREV(name, head, elm)) != NULL &&             \
            cmp(cmpelm, elm) >= 0) ||                                   \
            ((cmpelm = RB_NEXT(name, head, elm)) != NULL &&             \
            cmp(elm, cmpelm) >= 0)) {                                   \
                /* XXXLAS: Remove/insert is heavy handed. */            \
                RB_REMOVE(name, head, elm);                             \
                return (RB_INSERT(name, head, elm));                    \
        }                                                               \
        return (NULL);                                                  \
}                                                                       \

#define RB_NEGINF       -1
#define RB_INF          1

#define RB_INSERT(name, x, y)   name##_RB_INSERT(x, y)
#define RB_REMOVE(name, x, y)   name##_RB_REMOVE(x, y)
#define RB_FIND(name, x, y)     name##_RB_FIND(x, y)
#define RB_NFIND(name, x, y)    name##_RB_NFIND(x, y)
#define RB_NEXT(name, x, y)     name##_RB_NEXT(y)
#define RB_PREV(name, x, y)     name##_RB_PREV(y)
#define RB_MIN(name, x)         name##_RB_MINMAX(x, RB_NEGINF)
#define RB_MAX(name, x)         name##_RB_MINMAX(x, RB_INF)
#define RB_REINSERT(name, x, y) name##_RB_REINSERT(x, y)

#define RB_FOREACH(x, name, head)                                       \
        for ((x) = RB_MIN(name, head);                                  \
             (x) != NULL;                                               \
             (x) = name##_RB_NEXT(x))

#define RB_FOREACH_FROM(x, name, y)                                     \
        for ((x) = (y);                                                 \
            ((x) != NULL) && ((y) = name##_RB_NEXT(x), (x) != NULL);    \
             (x) = (y))

#define RB_FOREACH_SAFE(x, name, head, y)                               \
        for ((x) = RB_MIN(name, head);                                  \
            ((x) != NULL) && ((y) = name##_RB_NEXT(x), (x) != NULL);    \
             (x) = (y))

#define RB_FOREACH_REVERSE(x, name, head)                               \
        for ((x) = RB_MAX(name, head);                                  \
             (x) != NULL;                                               \
             (x) = name##_RB_PREV(x))

#define RB_FOREACH_REVERSE_FROM(x, name, y)                             \
        for ((x) = (y);                                                 \
            ((x) != NULL) && ((y) = name##_RB_PREV(x), (x) != NULL);    \
             (x) = (y))

#define RB_FOREACH_REVERSE_SAFE(x, name, head, y)                       \
        for ((x) = RB_MAX(name, head);                                  \
            ((x) != NULL) && ((y) = name##_RB_PREV(x), (x) != NULL);    \
             (x) = (y))

/* Marcos for a Sorted Digital Search Tree - DS */
#define TREE_SWAP(type, a, b) do {                                      \
        TREE(type) *__swap;                                             \
        __swap = (a); (a) = (b); (b) = __swap;                          \
} while (0)

#define DS_HEAD(name, type, ktype)                                      \
struct name {                                                           \
        TREE(type) *dsh_root; /* root of the tree */                    \
        ktype              dsh_sbit;                                    \
}

#define DS_INITIALIZER(root)                                            \
        { NULL, 0 }

#define DS_INIT(root, sbit) do {                                        \
        (root)->dsh_root = NULL;                                        \
        (root)->dsh_sbit = (sbit);                                      \
} while (0)

#define DS_ENTRY(type)                                                  \
struct {                                                                \
        TREE(type) *dse_left; /* left element */                        \
        TREE(type) *dse_right; /* right element */                      \
        TREE(type) *dse_parent; /* parent element */                    \
}

#define DS_LEFT(elm, field)             (elm)->field.dse_left
#define DS_RIGHT(elm, field)            (elm)->field.dse_right
#define DS_PARENT(elm, field)           (elm)->field.dse_parent
#define DS_KEY(elm, kfield)             (elm)->kfield
#define DS_ROOT(head)                   (head)->dsh_root
#define DS_SBIT(head)                   (head)->dsh_sbit
#define DS_EMPTY(head)                  (DS_ROOT(head) == NULL)

#define DS_SWAP_CHILD(head, out, in, field) do {                        \
        if (!DS_PARENT(out, field)) {                                   \
                DS_ROOT(head) = (in);                                   \
        } else {                                                        \
                if (out == DS_LEFT(DS_PARENT(out, field), field)) {     \
                        DS_LEFT(DS_PARENT(out, field), field) = (in);   \
                } else {                                                \
                /* out == DS_RIGHT(DS_PARENT(out, field), field) */     \
                        DS_RIGHT(DS_PARENT(out, field), field) = (in);  \
                }                                                       \
        }                                                               \
} while (0)

#define DS_SWAP_PARENT(out, in, field) do {                             \
        if (DS_LEFT(out, field)) {                                      \
                DS_PARENT(DS_LEFT(out, field), field) = (in);           \
        }                                                               \
        if (DS_RIGHT(out, field)) {                                     \
                DS_PARENT(DS_RIGHT(out, field), field) = (in);          \
        }                                                               \
} while (0)

#define DS_REPLACE(head, out, in, field) do {                           \
        DS_SWAP_CHILD(head, out, in, field);                            \
        DS_SWAP_PARENT(out, in, field);                                 \
        DS_LEFT(in, field) = DS_LEFT(out, field);                       \
        DS_RIGHT(in, field) = DS_RIGHT(out, field);                     \
        DS_PARENT(in, field) = DS_PARENT(out, field);                   \
} while (0)

#define DS_SWAP(type, head, out, in, field) do {                        \
        DS_SWAP_CHILD(head, in, out, field);                            \
        DS_SWAP_CHILD(head, out, in, field);                            \
        TREE_SWAP(type, DS_PARENT(in, field), DS_PARENT(out, field));   \
                                                                        \
        DS_SWAP_PARENT(in, out, field);                                 \
        DS_SWAP_PARENT(out, in, field);                                 \
        TREE_SWAP(type, DS_LEFT(in, field), DS_LEFT(out, field));       \
        TREE_SWAP(type, DS_RIGHT(in, field), DS_RIGHT(out, field));     \
} while (0)

/*
 *      ktype:  signed __typeof(<key>)
 *      kfield: the key associated with <elm>
 */
#define DS_PROTOTYPE(name, type, field, ktype, kfield)                  \
        DS_PROTOTYPE_INTERNAL(name, type, field, ktype, kfield,)
#define DS_PROTOTYPE_STATIC(name, type, field, ktype, kfield)           \
        DS_PROTOTYPE_INTERNAL(name, type, field, ktype, kfield,         \
                              __unused static)
#define DS_PROTOTYPE_INTERNAL(name, type, field, ktype, kfield, attr)   \
        DS_PROTOTYPE_INSERT(name, type, field, ktype, kfield, attr);    \
        DS_PROTOTYPE_REMOVE(name, type, field, attr);                   \
        DS_PROTOTYPE_FIND(name, type, field, ktype, kfield, attr);      \
        DS_PROTOTYPE_NFIND(name, type, field, ktype, kfield, attr);     \
        DS_PROTOTYPE_NEXT(name, type, field, attr);                     \
        DS_PROTOTYPE_PREV(name, type, field, attr);                     \
        DS_PROTOTYPE_MIN(name, type, field, attr);
#define DS_PROTOTYPE_INSERT(name, type, field, ktype, kfield, attr)     \
        attr TREE(type) *                                               \
        name##_DS_INSERT(struct name *, TREE(type) *)
#define DS_PROTOTYPE_REMOVE(name, type, field, attr)                    \
        void name##_DS_REMOVE(struct name *, TREE(type) *)
#define DS_PROTOTYPE_FIND(name, type, field, ktype, kfield, attr)       \
        attr TREE(type) *name##_DS_FIND(struct name *, ktype)
#define DS_PROTOTYPE_NFIND(name, type, field, ktype, kfield, attr)      \
        attr TREE(type) *name##_DS_NFIND(struct name *, ktype)
#define DS_PROTOTYPE_NEXT(name, type, field, attr)                      \
        attr TREE(type) *name##_DS_NEXT(TREE(type) *)
#define DS_PROTOTYPE_PREV(name, type, field, attr)                      \
        attr TREE(type) *name##_DS_PREV(TREE(type) *)
#define DS_PROTOTYPE_MIN(name, type, field, attr)                       \
        attr TREE(type) *name##_DS_MIN(struct name *)

#define DS_GENERATE(name, type, field, ktype, kfield)                   \
        DS_GENERATE_INTERNAL(name, type, field, ktype, kfield,)
#define DS_GENERATE_STATIC(name, type, field, ktype, kfield)            \
        DS_GENERATE_INTERNAL(name, type, field, ktype, kfield,          \
                             __unused static)
#define DS_GENERATE_INTERNAL(name, type, field, ktype, kfield, attr)    \
        DS_GENERATE_INSERT(name, type, field, ktype, kfield, attr)      \
        DS_GENERATE_REMOVE(name, type, field, attr)                     \
        DS_GENERATE_FIND(name, type, field, ktype, kfield, attr)        \
        DS_GENERATE_NFIND(name, type, field, ktype, kfield, attr)       \
        DS_GENERATE_NEXT(name, type, field, attr)                       \
        DS_GENERATE_PREV(name, type, field, attr)                       \
        DS_GENERATE_MIN(name, type, field, attr)

#define DS_GENERATE_INSERT(name, type, field, ktype, kfield, attr)      \
/* Inserts a node into the DS tree */                                   \
attr TREE(type) *                                                       \
name##_DS_INSERT(struct name *head, TREE(type) *elm)                    \
{                                                                       \
        TREE(type) *ins = elm, *tmp = DS_ROOT(head), *parent = NULL;    \
        ktype sbit = DS_SBIT(head);                                     \
        ktype comp, tbit;                                               \
        while (tmp) {                                                   \
                comp = DS_KEY(ins, kfield) - DS_KEY(tmp, kfield);       \
                tbit = DS_KEY(ins, kfield) & sbit;                      \
                if (comp == 0) {                                        \
                        return (tmp);                                   \
                }                                                       \
                if (!tbit) {                                            \
                        if (comp > 0) {                                 \
                                DS_REPLACE(head, tmp, ins, field);      \
                                TREE_SWAP(type, tmp, ins);              \
                        }                                               \
                        parent = tmp;                                   \
                        tmp = DS_LEFT(tmp, field);                      \
                } else {                                                \
                        if (comp < 0) {                                 \
                                DS_REPLACE(head, tmp, ins, field);      \
                                TREE_SWAP(type, tmp, ins);              \
                        }                                               \
                        parent = tmp;                                   \
                        tmp = DS_RIGHT(tmp, field);                     \
                }                                                       \
                sbit >>= 1;                                             \
        }                                                               \
        if (!parent) {                                                  \
                DS_ROOT(head) = ins;                                    \
        } else if (!tbit) {                                             \
                DS_LEFT(parent, field) = ins;                           \
        } else {                                                        \
                DS_RIGHT(parent, field) = ins;                          \
        }                                                               \
        DS_PARENT(ins, field) = parent;                                 \
        return (NULL);                                                  \
}

#define DS_GENERATE_REMOVE(name, type, field, attr)                     \
/* Removes a node from the DS tree */                                   \
void                                                                    \
name##_DS_REMOVE(struct name *head, TREE(type) *elm)                    \
{                                                                       \
        TREE(type) *rem = elm, *tmp = NULL;                             \
        while (DS_LEFT(rem, field) || DS_RIGHT(rem, field)) {           \
                if (DS_LEFT(rem, field)) {                              \
                        tmp = DS_LEFT(rem, field);                      \
                        while (DS_RIGHT(tmp, field)) {                  \
                                tmp = DS_RIGHT(tmp, field);             \
                        }                                               \
                } else {                                                \
                        tmp = DS_RIGHT(rem, field);                     \
                        while (DS_LEFT(tmp, field)) {                   \
                                tmp = DS_LEFT(tmp, field);              \
                        }                                               \
                }                                                       \
                DS_SWAP(type, head, rem, tmp, field);                   \
        }                                                               \
        DS_SWAP_CHILD(head, rem, NULL, field);                          \
}

#define DS_GENERATE_FIND(name, type, field, ktype, kfield, attr)        \
/* Find the node equal to the search key */                             \
attr TREE(type) *                                                       \
name##_DS_FIND(struct name *head, ktype key)                            \
{                                                                       \
        TREE(type) *tmp = DS_ROOT(head);                                \
        ktype comp;                                                     \
        while (tmp) {                                                   \
                comp = key - DS_KEY(tmp, kfield);                       \
                if (comp == 0) {                                        \
                        return (tmp);                                   \
                }                                                       \
                if (comp < 0) {                                         \
                        tmp = DS_LEFT(tmp, field);                      \
                } else {                                                \
                        tmp = DS_RIGHT(tmp, field);                     \
                }                                                       \
        }                                                               \
        return (NULL);                                                  \
}

#define DS_GENERATE_NFIND(name, type, field, ktype, kfield, attr)       \
/* Finds the first node greater than or equal to the search key */      \
attr TREE(type) *                                                       \
name##_DS_NFIND(struct name *head, ktype key)                           \
{                                                                       \
        TREE(type) *tmp = DS_ROOT(head), *res = NULL;                   \
        ktype comp;                                                     \
        while (tmp) {                                                   \
                comp = key - DS_KEY(tmp, kfield);                       \
                if (comp == 0) {                                        \
                        return (tmp);                                   \
                }                                                       \
                if (comp < 0) {                                         \
                        res = tmp;                                      \
                        tmp = DS_LEFT(tmp, field);                      \
                } else {                                                \
                        tmp = DS_RIGHT(tmp, field);                     \
                }                                                       \
        }                                                               \
        return (res);                                                   \
}

#define DS_GENERATE_NEXT(name, type, field, attr)                       \
/* ARGSUSED */                                                          \
attr TREE(type) *                                                       \
name##_DS_NEXT(TREE(type) *elm)                                         \
{                                                                       \
        if (DS_RIGHT(elm, field)) {                                     \
                elm = DS_RIGHT(elm, field);                             \
                while (DS_LEFT(elm, field))                             \
                        elm = DS_LEFT(elm, field);                      \
        } else {                                                        \
                if (DS_PARENT(elm, field) &&                            \
                    (elm == DS_LEFT(DS_PARENT(elm, field), field)))     \
                        elm = DS_PARENT(elm, field);                    \
                else {                                                  \
                        while (DS_PARENT(elm, field) &&                 \
                            (elm == DS_RIGHT(DS_PARENT(elm, field), field)))\
                                elm = DS_PARENT(elm, field);            \
                        elm = DS_PARENT(elm, field);                    \
                }                                                       \
        }                                                               \
        return (elm);                                                   \
}

#define DS_GENERATE_PREV(name, type, field, attr)                       \
/* ARGSUSED */                                                          \
attr TREE(type) *                                                       \
name##_DS_PREV(TREE(type) *elm)                                         \
{                                                                       \
        if (DS_LEFT(elm, field)) {                                      \
                elm = DS_LEFT(elm, field);                              \
                while (DS_RIGHT(elm, field))                            \
                        elm = DS_RIGHT(elm, field);                     \
        } else {                                                        \
                if (DS_PARENT(elm, field) &&                            \
                    (elm == DS_RIGHT(DS_PARENT(elm, field), field)))    \
                        elm = DS_PARENT(elm, field);                    \
                else {                                                  \
                        while (DS_PARENT(elm, field) &&                 \
                            (elm == DS_LEFT(DS_PARENT(elm, field), field)))\
                                elm = DS_PARENT(elm, field);            \
                        elm = DS_PARENT(elm, field);                    \
                }                                                       \
        }                                                               \
        return (elm);                                                   \
}

#define DS_GENERATE_MIN(name, type, field, attr)                        \
/* ARGSUSED */                                                          \
attr TREE(type) *                                                       \
name##_DS_MIN(struct name *head)                                        \
{                                                                       \
        TREE(type) *tmp = DS_ROOT(head), *res = NULL;                   \
        while (tmp) {                                                   \
                res = tmp;                                              \
                tmp = DS_LEFT(tmp, field);                              \
        }                                                               \
        return (res);                                                   \
}

#define DS_INSERT(name, head, elm)      name##_DS_INSERT(head, elm)
#define DS_REMOVE(name, head, elm)      name##_DS_REMOVE(head, elm)
#define DS_FIND(name, head, key)        name##_DS_FIND(head, key)
#define DS_NFIND(name, head, key)       name##_DS_NFIND(head, key)
#define DS_NEXT(name, elm)              name##_DS_NEXT(elm)
#define DS_PREV(name, elm)              name##_DS_PREV(elm)
#define DS_MIN(name, head)              name##_DS_MIN(head)

#define DS_FOREACH(x, name, head)                                       \
        for ((x) = name##_DS_MIN(head);                                 \
             (x) != NULL;                                               \
             (x) = name##_DS_NEXT(x))

#endif  /* _SYS_TREE_H_ */

