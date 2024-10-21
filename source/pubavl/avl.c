#include "pubavl/avl.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

struct avl_stack *avl_stack_init(struct avl_stack *stack)
{
        assert(stack && stack->array);
        (void)memset(stack->array, 0, AVL_STACK_MAX * sizeof(void*));
        stack->size = 0;
        return stack;
}

struct avl_stack *avl_stack_reset(struct avl_stack *stack) 
{
        assert(stack && stack->array && stack->size <= AVL_STACK_MAX);
        (void)memset(stack->array, 0, stack->size * sizeof(void*));
        stack->size = 0;
        return stack;
}

struct avl_stack *avl_stack_push(
        struct avl_stack *stack, 
        struct avl_node *node)
{
        assert(stack && stack->array && stack->size <= AVL_STACK_MAX);
        if(stack->size == AVL_STACK_MAX) {
                return NULL;
        } 
        stack->array[stack->size++] = node;
        return stack;
}

struct avl_node *avl_stack_pop(struct avl_stack *stack)
{
        assert(stack && stack->array && stack->size <= AVL_STACK_MAX);
        if(stack->size == 0) {
                return NULL;
        } 
        const size_t index = stack->size - 1;
        struct avl_node *node = stack->array[index];
        stack->array[index] = NULL;
        stack->size -= 1;
        return node;
}

struct avl_node *avl_stack_peek(struct avl_stack *stack)
{
        assert(stack && stack->array && stack->size <= AVL_STACK_MAX);
        if(stack->size == 0) {
                return NULL;
        } 
        return stack->array[stack->size - 1];
}

struct avl_node *avl_node_init(
        struct avl_node *node,
        struct avl_kv key,
        struct avl_kv value) 
{
        assert(node);
        (void)memset(node, 0, sizeof(struct avl_node));
        node->key = key;
        node->value = value;
        node->height = 1;
        return node;
}

ssize_t avl_node_height(struct avl_node *node)
{
        if(!node) {
                return 0;
        } 
        assert(0 < node->height && node->height <= AVL_STACK_MAX);
        return node->height;
}

struct avl_node *avl_node_update_height(struct avl_node *node) 
{
        assert(node);
        const ssize_t hl = avl_node_height(node->left);
        const ssize_t hr = avl_node_height(node->right);
        node->height = 1 + (hl < hr ? hr : hl);
        return node;
}

ssize_t avl_node_balance_factor(struct avl_node *node) 
{
        assert(node);
        return avl_node_height(node->right) - avl_node_height(node->left);
}

struct avl_node *avl_node_rotate_right(struct avl_node *node)
{
        assert(node && node->left);
        struct avl_node *x = node->left;
        struct avl_node *t = x->right;
        x->right = node;
        node->left = t;
        (void)avl_node_update_height(node);
        (void)avl_node_update_height(x);
        return x;
}

struct avl_node *avl_node_rotate_left(struct avl_node *node)
{
        assert(node && node->right);
        struct avl_node *y = node->right;
        struct avl_node *t = y->left;
        y->left = node;
        node->right = t;
        (void)avl_node_update_height(node);
        (void)avl_node_update_height(y);
        return y;
} 

struct avl_node *avl_node_rebalance(struct avl_node *node) 
{
        const ssize_t balance = avl_node_balance_factor(node);
        assert(-3 < balance && balance < 3);
        if(balance > 1) {
                if(avl_node_balance_factor(node->right) < 0) {
                        node->right = avl_node_rotate_right(node->right);
                        assert(node->right);
                        return avl_node_rotate_left(node);
                } else {
                        return avl_node_rotate_left(node);
                }
        } else if(balance < -1) {
                if(avl_node_balance_factor(node->left) > 0) {
                        node->left = avl_node_rotate_left(node->left);
                        assert(node->left);
                        return avl_node_rotate_right(node);
                } else {
                        return avl_node_rotate_right(node);
                }
        } else {
                return node;
        }
}

struct avl_node *avl_stack_rebalance(
        struct avl_stack *stack,
        const size_t nsteps,
        struct avl_node *top)
{
        assert(top && nsteps <= stack->size && stack->size <= AVL_STACK_MAX);
        struct avl_node *next, *new_top = NULL;
        new_top = avl_node_rebalance(avl_node_update_height(top));
        for(size_t i = 0; i < nsteps; ++i) {
                next = avl_stack_pop(stack);
                assert(next && (next->left == top || next->right == top));
                if(next->left == top){
                        next->left = new_top;
                } else if(next->right == top) {
                        next->right = new_top;
                } else {
                        assert(0);
                }
                top = next;
                new_top = avl_node_rebalance(avl_node_update_height(next));
        }
        return new_top; 
}

struct avl_node *avl_node_add(
        struct avl_node *const root,
        struct avl_stack *stack,
        struct avl_kv key,
        struct avl_kv value,
        avl_cmp_t cmp,
        avl_alloc_t alloc,
        void *state,
        struct avl_node **result)
{
        struct avl_node *new_node, *top, *srch, **addr = NULL;
        assert(alloc && result);
        if(!root) {
                new_node = alloc(state);
                if(!new_node) {
                        goto FAILURE;
                } else {
                        *result = avl_node_init(new_node, key, value);
                        return new_node;
                }
        }
        (void)avl_stack_reset(stack);
        srch = root;
        for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(!srch) {
                        goto FINISH;
                } else if(cmp(key, srch->key)) {
                        if(!avl_stack_push(stack, srch)) {
                                goto FAILURE;
                        } else {
                                addr = &srch->left;
                                srch = srch->left;
                        }
                } else if(cmp(srch->key, key)) {
                        if(!avl_stack_push(stack, srch)) {
                                goto FAILURE;
                        } else {
                                addr = &srch->right;
                                srch = srch->right;
                        }
                } else {
                        goto FAILURE;
                }
        }
        assert(0);
        FAILURE:
        *result = NULL;
        return root;
        FINISH:
        assert(addr);
        new_node = alloc(state);
        if(!new_node) {
                goto FAILURE;
        }
        *addr = *result = avl_node_init(new_node, key, value);
        top = avl_stack_pop(stack);
        assert(top && (top->left == new_node || top->right == new_node));
        return avl_stack_rebalance(stack, stack->size, top);
}

struct avl_node *avl_node_get(
        struct avl_node *node, 
        struct avl_kv key,
        avl_cmp_t cmp)
{
        assert(cmp);
        for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(!node) {
                        return NULL;
                } else if(cmp(key, node->key)) {
                        node = node->left;
                } else if(cmp(node->key, key)) {
                        node = node->right;
                } else {
                        return node;
                }
        }
        assert(0);
        return NULL;
}

struct avl_node *avl_node_remove_ent_nary(
        struct avl_node *root,
        struct avl_stack *stack,
        struct avl_node **addr,
        struct avl_node **entry)
{
        struct avl_node *top = NULL;
        struct avl_node *succ = (*entry)->right;
        const size_t saved_size = stack->size;
        for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(!succ->left) {
                        goto FINISH;
                } else if(!avl_stack_push(stack, succ)) {
                        goto FAILURE;
                } else {
                        succ = succ->left;
                }
        }
        assert(0);
        FAILURE:
        *entry = NULL;
        return root;
        FINISH:
        top = avl_stack_pop(stack);
        assert(stack->size >= saved_size && top && top->left == succ);
        top->left = succ->right;
        succ->right = avl_stack_rebalance(
                stack, stack->size - saved_size, top);
        succ->left = (*entry)->left;
        succ = avl_node_rebalance(avl_node_update_height(succ));
        if(!addr) {
                assert(!stack->size);
                return succ;
        } 
        *addr = succ;
        top = avl_stack_pop(stack);
        assert(top && (top->left == succ || top->right == succ));
        return avl_stack_rebalance(stack, stack->size, top);
}

struct avl_node *avl_node_remove_ent(
        struct avl_node *root,
        struct avl_stack *stack,
        struct avl_node **addr,
        struct avl_node **entry)
{
        struct avl_node *patch, *top = NULL;
        struct avl_node *ent = *entry;
        assert(ent);
        if(!ent->left) {
                patch = ent->right;
        } else if(!ent->right) {
                patch = ent->left;
        } else if(!ent->right->left) {
                ent->right->left = ent->left;
                patch = avl_node_rebalance(
                        avl_node_update_height(ent->right));
        } else {
                return avl_node_remove_ent_nary(root, stack, addr, entry);
        } 
        if(!addr) {
                assert(!stack->size);
                return patch;
        } 
        *addr = patch;
        top = avl_stack_pop(stack);
        assert(top && (top->left == patch || top->right == patch));
        return avl_stack_rebalance(stack, stack->size, top);
}

struct avl_node *avl_node_remove(
        struct avl_node *root,
        struct avl_stack *stack,
        struct avl_kv key, 
        avl_cmp_t cmp,
        struct avl_node **entry)
{
        assert(cmp && entry);
        if(!root) {
                goto FAILURE;
        } 
        (void)avl_stack_reset(stack);
        struct avl_node *srch = root;
        struct avl_node **addr = NULL;
        for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(!srch) {
                        goto FAILURE;
                } else if(cmp(key, srch->key)) {
                        if(!avl_stack_push(stack, srch)) {
                                goto FAILURE;
                        } else {
                                addr = &srch->left;
                                srch = srch->left;
                        }
                } else if(cmp(srch->key, key)) {
                        if(!avl_stack_push(stack, srch)) {
                                goto FAILURE;
                        } else {
                                addr = &srch->right;
                                srch = srch->right;
                        }
                } else {
                        *entry = srch;
                        return avl_node_remove_ent(root, stack, addr, entry);
                }
        }
        assert(0);
        FAILURE:
        *entry = NULL;
        return root;
}

struct avl_node *avl_node_min(struct avl_node *node)
{
        if(!node) {
                return NULL;
        } else for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(!node->left) {
                        return node;
                }
                node = node->left;
        } 
        assert(0);
        return NULL;
}

struct avl_node *avl_node_max(struct avl_node *node)
{
        if(!node) {
                return NULL;
        } else for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(!node->right) {
                        return node;
                } 
                node = node->right;
        }
        assert(0);
        return NULL;
}

struct avl_stack *avl_node_reversed(
        struct avl_node *node, 
        struct avl_stack *stack)
{
        (void)avl_stack_reset(stack);
        for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(!node) {
                        return stack;
                } else if(!avl_stack_push(stack, node)) {
                        return NULL;
                }
                node = node->right;
        }
        assert(0);
        return NULL;
}

struct avl_stack *avl_node_traverse(
        struct avl_node *node, 
        struct avl_stack *stack)
{
        (void)avl_stack_reset(stack);
        for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(!node) {
                        return stack;
                } else if(!avl_stack_push(stack, node)) {
                        return NULL;
                }
                node = node->left;
        }
        assert(0);
        return NULL;
}

struct avl_node *avl_node_remove_first(
        struct avl_node *const root, 
        struct avl_stack *stack,
        struct avl_node **min)
{
        assert(min);
        if(!root) {
                goto FAILURE;
        }
        (void)avl_stack_reset(stack);
        struct avl_node *srch = root;
        struct avl_node **addr = NULL;
        for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(!srch->left) {
                        *min = srch;
                        return avl_node_remove_ent(root, stack, addr, min);
                } else if(!avl_stack_push(stack, srch)) {
                        goto FAILURE;
                } else {
                        addr = &srch->left;
                        srch = srch->left;
                } 
        }
        assert(0);
        FAILURE:
        *min = NULL;
        return root;
}

struct avl_node *avl_node_remove_last(
        struct avl_node *root, 
        struct avl_stack *stack,
        struct avl_node **max)
{
        assert(max);
        if(!root) {
                goto FAILURE;
        }
        (void)avl_stack_reset(stack);
        struct avl_node *srch = root;
        struct avl_node **addr = NULL;
        for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(!srch->right) {
                        *max = srch;
                        return avl_node_remove_ent(root, stack, addr, max);
                } else if(!avl_stack_push(stack, srch)) {
                        goto FAILURE;
                } else {
                        addr = &srch->right;
                        srch = srch->right;
                } 
        }
        assert(0);
        FAILURE:
        *max = NULL;
        return root;
}

struct avl_stack *avl_node_upper(
        struct avl_node *node, 
        struct avl_stack *stack,
        struct avl_kv key, 
        avl_cmp_t cmp)
{
        (void)avl_stack_reset(stack);
        for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(cmp(key, node->key)) {
                        if(!avl_stack_push(stack, node)) {
                                return NULL;
                        } else if(node->left == NULL) {
                                return stack;
                        } 
                        node = node->left;
                } else if(cmp(node->key, key)) {
                        if(node->right == NULL) {
                                return stack;
                        } 
                        node = node->right;
                } else {
                        if(!avl_stack_push(stack, node)) {
                                return NULL;
                        } 
                        return stack;
                }
        }
        assert(0);
        return NULL;
}

struct avl_stack *avl_node_lower(
        struct avl_node *node, 
        struct avl_kv key, 
        avl_cmp_t cmp,
        struct avl_stack *stack)
{
        (void)avl_stack_reset(stack);
        for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(cmp(key, node->key)) {
                        if(node->left == NULL) {
                                return stack;
                        } 
                        node = node->left;
                } else if(cmp(node->key,  key)) {
                        if(!avl_stack_push(stack, node)) {
                                return NULL;
                        } else if(node->right == NULL) {
                                return stack;
                        }
                        node = node->right;
                } else {
                        if(!avl_stack_push(stack, node)) {
                                return NULL;
                        } 
                        return stack;
                }
        }
        assert(0);
        return NULL;
}

struct avl_tree *avl_tree_init(
        struct avl_tree *tree,
        avl_cmp_t cmp,
        avl_alloc_t alloc,
        avl_free_t free,
        void *state)
{
        assert(tree);
        tree->size = 0;
        tree->root = NULL;
        tree->cmp = cmp;
        tree->alloc = alloc;
        tree->free = free;
        tree->heap = state;
        return tree;
}

void avl_free_nodes(struct avl_tree *tree, struct avl_stack *stack)
{
        struct avl_node *node;
        (void)avl_stack_reset(stack);
        avl_traverse(tree, stack);
        for(size_t n = 0; n < tree->size; ++n) {
                if(!avl_next(stack, &node)) {
                        return;
                }
                tree->free(tree->heap, node);
        }
}

struct avl_node *avl_add(
        struct avl_tree *tree, 
        struct avl_stack *stack,
        struct avl_kv key, 
        struct avl_kv value)
{
        struct avl_node *result = NULL;
        tree->root = avl_node_add(
                tree->root,
                stack,  
                key, 
                value, 
                tree->cmp, 
                tree->alloc, 
                tree->heap,
                &result);
        if(result) {
                tree->size += 1;
                return result;
        } else {
                return NULL;
        }
}

struct avl_tree *avl_remove(
        struct avl_tree *tree, 
        struct avl_stack *stack, 
        struct avl_kv key, 
        struct avl_kv *rkey, 
        struct avl_kv *rvalue)
{
        struct avl_node *result = NULL;
        tree->root = avl_node_remove(
                tree->root, 
                stack,
                key, 
                tree->cmp, 
                &result);
        if(result) {
                tree->size -= 1;
                if(rkey) {
                        *rkey = result->key;
                }
                if(rvalue) {
                        *rvalue = result->value;
                }
                tree->free(tree->heap, result);
                return tree;
        } else {
                return NULL;
        }
}

struct avl_node *avl_get(
        struct avl_tree *tree, 
        struct avl_kv key)
{
        return avl_node_get(tree->root, key, tree->cmp);
}

struct avl_node *avl_min(struct avl_tree *tree)
{
        return avl_node_min(tree->root);
}

struct avl_node *avl_max(struct avl_tree *tree)
{
        return avl_node_max(tree->root);
}

struct avl_stack *avl_reversed(
        struct avl_tree *tree, 
        struct avl_stack *stack)
{
        struct avl_node *node = tree->root;
        (void)avl_stack_reset(stack);
        if(!node) {
                return stack;
        } else for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(!avl_stack_push(stack, node)) {
                        return NULL;
                } else if(!node->right) {
                        return stack;
                } 
                node = node->right;
        }
        assert(0);
        return NULL;
}

struct avl_stack *avl_traverse(
        struct avl_tree *tree, 
        struct avl_stack *stack)
{
        struct avl_node *node = tree->root;
        (void)avl_stack_reset(stack);
        if(!node) {
                return stack;
        } else for(size_t I = 0; I < AVL_STACK_MAX; ++I) {
                if(!avl_stack_push(stack, node)) {
                        return NULL;
                } else if(!node->left) {
                        return stack;
                } 
                node = node->left;
        }
        assert(0);
        return NULL;
}

struct avl_tree *avl_remove_min(
        struct avl_tree *tree,
        struct avl_stack *stack,
        struct avl_kv *rkey,
        struct avl_kv *rvalue)
{
        struct avl_node *result = NULL;
        tree->root = avl_node_remove_first(tree->root, stack, &result);
        if(result) {
                tree->size -= 1;
                if(rkey) {
                        *rkey = result->key;
                }
                if(rvalue) {
                        *rvalue = result->value;
                }
                tree->free(tree->heap, result);
                return tree;
        } else {
                return NULL;
        }
}

struct avl_tree *avl_remove_max(
        struct avl_tree *tree,
        struct avl_stack *stack,
        struct avl_kv *rkey,
        struct avl_kv *rvalue)
{
        struct avl_node *result = NULL;
        tree->root = avl_node_remove_last(tree->root, stack, &result);
        if(result) {
                tree->size -= 1;
                if(rkey) {
                        *rkey = result->key;
                }
                if(rvalue) {
                        *rvalue = result->value;
                }
                tree->free(tree->heap, result);
                return tree;
        } else {
                return NULL;
        }
}

int avl_next(
        struct avl_stack *stack,
        struct avl_node **result)
{
        struct avl_node *node = avl_stack_pop(stack);
        if(!node) {
                return 0;
        } else if(node->right) {
                for(struct avl_node *n = node->right; n; n = n->left) {
                        if(!avl_stack_push(stack, n)) {
                                return 0;
                        }
                }
        }
        *result = node;
        return 1;
}

int avl_prior(
        struct avl_stack *stack,
        struct avl_node **result)
{
        struct avl_node *node = avl_stack_pop(stack);
        if(!node) {
                return 0;
        } else if(node->left) {
                for(struct avl_node *n = node->left; n; n = n->right) {
                        if(!avl_stack_push(stack, n)) {
                                return 0;
                        }
                }
        }
        *result = node;
        return 1;
}

struct avl_stack *avl_upper(
        struct avl_tree *tree, 
        struct avl_stack *stack,
        struct avl_kv key)
{
        return avl_node_upper(tree->root, stack, key, tree->cmp);
}

struct avl_stack *avl_lower(
        struct avl_tree *tree, 
        struct avl_stack *stack,
        struct avl_kv key)
{
        return avl_node_lower(tree->root, key, tree->cmp, stack);
}