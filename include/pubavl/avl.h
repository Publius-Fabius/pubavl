#ifndef PUBAVL_AVL_H
#define PUBAVL_AVL_H

#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>
#include <float.h>

#if UINTPTR_MAX <= (255)
#define AVL_STACK_MAX 8
#elif UINTPTR_MAX <= (65535)
#define AVL_STACK_MAX 16
#elif UINTPTR_MAX <= (4294967295U)
#define AVL_STACK_MAX 32
#elif UINTPTR_MAX <= (18446744073709551615UL)
#define AVL_STACK_MAX 64
#else 
#error "Unable to determine machine's bit depth"
#endif

#ifndef AVL_KV_HINT
#define AVL_KV_HINT AVL_STACK_MAX
#endif

/** AVL Tree Data */
struct avl_kv {
        union {
                void *ptr;
                unsigned char u8;
                signed char i8;

#if AVL_KV_HINT >= 16
#ifdef UINT16_MAX
                uint16_t u16;
#endif
#ifdef INT16_MAX
                int16_t i16;
#endif
#endif

#if AVL_KV_HINT >= 32
#ifdef UINT32_MAX
                uint32_t u32;
#endif
#ifdef INT32_MAX
                int32_t i32;
#endif
#ifdef FLT_MAX 
                float f32;
#endif
#endif

#if AVL_KV_HINT >= 64
#ifdef UINT64_MAX
                uint64_t u64;
#endif
#ifdef INT64_MAX
                int64_t i64;
#endif
#ifdef DBL_MAX
                double f64;
#endif
#endif 
        } u;
};

/** AVL Tree Node */
struct avl_node {
        struct avl_kv key;
        struct avl_kv value;
        struct avl_node *left;
        struct avl_node *right;
        ssize_t height;
};

/** AVL Node Allocation */
typedef struct avl_node *(*avl_alloc_t)(void *heap);

/** AVL Node Free */
typedef void (*avl_free_t)(void *heap, struct avl_node *node);

/** AVL Tree Comparison Function */
typedef int (*avl_cmp_t)(struct avl_kv a, struct avl_kv b);

/** Balanced Binary Search Tree */
struct avl_tree {
        struct avl_node *root;
        size_t size;
        avl_cmp_t cmp;
        avl_alloc_t alloc;
        avl_free_t free;
        void *heap;
};

/** Stack for AVL Trees */
struct avl_stack {
        struct avl_node *array[AVL_STACK_MAX];
        size_t size;
};

/** AVL_DATA Constant */
#define AVL_KV(FIELD, VALUE) (struct avl_kv) { .u.FIELD = VALUE }

/** Initialize the avl_stack */
struct avl_stack *avl_stack_init(struct avl_stack *stack);

/** Reset the avl_stack */
struct avl_stack *avl_stack_reset(struct avl_stack *stack);

/** Initialize an avl_tree. */
struct avl_tree *avl_tree_init(
        struct avl_tree *tree,
        avl_cmp_t cmp,
        avl_alloc_t alloc,
        avl_free_t free,
        void *state);

/** Free all the tree's nodes. */
void avl_free_nodes(struct avl_tree *tree, struct avl_stack *stack);

/** Add a new entry unless it already exists. */
struct avl_node *avl_add(
        struct avl_tree *tree, 
        struct avl_stack *stack,
        struct avl_kv key, 
        struct avl_kv value);

/** Remove the entry with the given key. */
struct avl_tree *avl_remove(
        struct avl_tree *tree, 
        struct avl_stack *stack,
        struct avl_kv key,
        struct avl_kv *rkey,
        struct avl_kv *rvalue);

/** Look up the key's node. */
struct avl_node *avl_get(
        struct avl_tree *tree, 
        struct avl_kv key);

/** Get the tree's minimum node */
struct avl_node *avl_min(struct avl_tree *tree);

/** Get the tree's maximum node. */
struct avl_node *avl_max(struct avl_tree *tree);

/** Remove the first entry. */
struct avl_tree *avl_remove_min(
        struct avl_tree *tree,
        struct avl_stack *stack,
        struct avl_kv *rkey,
        struct avl_kv *rvalue);

/** Remove the last entry. */
struct avl_tree *avl_remove_max(
        struct avl_tree *tree,
        struct avl_stack *stack,
        struct avl_kv *rkey,
        struct avl_kv *rvalue);

/** Traverse in descending order. */
struct avl_stack *avl_reversed(
        struct avl_tree *tree, 
        struct avl_stack *stack);

/** Traverse in ascending order. */
struct avl_stack *avl_traverse(
        struct avl_tree *tree, 
        struct avl_stack *stack);

/** Iterate forward one step.  Can only be used with ascending traversal. */
int avl_next(
        struct avl_stack *stack,
        struct avl_node **result);

/** Iterate backward one step. Can only be used with descending traversal. */
int avl_prior(
        struct avl_stack *stack,
        struct avl_node **result);

/** Ascending traversal from entries equal to or greater than the key. */
struct avl_stack *avl_upper(
        struct avl_tree *tree, 
        struct avl_stack *stack,
        struct avl_kv key);

/** Descending traversal from entries equal to or less than the key. */
struct avl_stack *avl_lower(
        struct avl_tree *tree, 
        struct avl_stack *stack,
        struct avl_kv key);

#endif