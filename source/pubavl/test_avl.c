
#include "pubavl/avl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define AVL_TEST(expr) if(!(expr)) { \
        fprintf(stderr, "TEST:%i:%s\r\n", __LINE__, __func__); \
        abort(); \
}

int cmp_i64(struct avl_kv a, struct avl_kv b) 
{
        return a.u.i64 < b.u.i64;
}

struct avl_node *alloc_node(void *heap) 
{
        return malloc(sizeof(struct avl_node));
}

void free_node(void *heap, struct avl_node *node)
{
        (void)free(node);
}

int cmp_rand(const void *a, const void *b)
{
        return (rand() % 3) - 1;
}

void init_keys(int64_t *array, const int count)
{
        for(int n = 0; n < count; ++n) {
                array[n] = n;
        }
        (void)qsort(array, (size_t)count, sizeof(int64_t), cmp_rand);
}

void add_all(
        struct avl_tree *tree, 
        struct avl_stack *stk, 
        int64_t *array, 
        const int count)
{
        (void)avl_stack_reset(stk);
        for(int64_t i = 0; i < count; ++i) {
                const int64_t r = array[i];
                AVL_TEST(avl_add(tree, stk, AVL_KV(i64, r), AVL_KV(i64, r)));
        }
}

void test_add()
{
        (void)puts("test_add()");
        const int COUNT = 1000;
        int64_t keys[COUNT];
        struct avl_tree tree;
        struct avl_stack stack;
        struct avl_node *node;
        (void)init_keys(keys, COUNT);
        (void)avl_tree_init(&tree, cmp_i64, alloc_node, free_node, NULL); 
        (void)avl_stack_init(&stack);
        (void)add_all(&tree, &stack, keys, COUNT);
        AVL_TEST(avl_traverse(&tree, &stack));
        for(int64_t j = 0; j < COUNT; ++j) {
                AVL_TEST(avl_next(&stack, &node));
                AVL_TEST(node->key.u.i64 == j);
        }
        (void)avl_free_nodes(&tree, &stack);
}

void test_remove()
{
        (void)puts("test_remove()");
        const int COUNT = 100;
        int64_t keys[COUNT];
        struct avl_tree tree;
        struct avl_stack stack;
        (void)init_keys(keys, COUNT);
        (void)avl_tree_init(&tree, cmp_i64, alloc_node, free_node, NULL);
        (void)avl_stack_init(&stack);
        (void)add_all(&tree, &stack, keys, COUNT);
        for(int64_t i = 0; i < COUNT; ++i) {
                printf("iter=%zi\r\n", i);
                const int64_t k = keys[i];
                AVL_TEST(avl_get(&tree, AVL_KV(i64, k)));
                AVL_TEST(avl_remove(&tree, &stack, AVL_KV(i64, k), NULL, NULL));
                AVL_TEST(!avl_get(&tree, AVL_KV(i64, k)));
        }
        AVL_TEST(tree.size == 0);
}

void test_prior()
{
        (void)puts("test_prior()");
        const int COUNT = 1000;
        int64_t keys[COUNT];
        struct avl_tree tree;
        struct avl_stack stack;
        struct avl_node *node;
        (void)init_keys(keys, COUNT);
        (void)avl_tree_init(&tree, cmp_i64, alloc_node, free_node, NULL);
        (void)avl_stack_init(&stack);
        (void)add_all(&tree, &stack, keys, COUNT);
        AVL_TEST(avl_reversed(&tree, &stack));
        for(int64_t j = COUNT - 1; j > 0; --j) {
                AVL_TEST(avl_prior(&stack, &node));
                AVL_TEST(node->key.u.i64 == j);
        }
        (void)avl_free_nodes(&tree, &stack);
}

void test_remove_min()
{
        (void)puts("test_remove_min()");
        const int COUNT = 10000;
        int64_t keys[COUNT];
        struct avl_tree tree;
        struct avl_stack stack;
        struct avl_kv key, value;
        (void)init_keys(keys, COUNT);
        (void)avl_tree_init(&tree, cmp_i64, alloc_node, free_node, NULL);
        (void)avl_stack_init(&stack);
        (void)add_all(&tree, &stack, keys, COUNT);
        for(int64_t i = 0; i < COUNT; ++i) {
                struct avl_node *node = avl_min(&tree);
                AVL_TEST(node->key.u.i64 == i);
                AVL_TEST(avl_remove_min(&tree, &stack, &key, &value));
        }
}

void test_remove_max()
{
        (void)puts("test_remove_max()");
        const int COUNT = 10000;
        int64_t keys[COUNT];
        struct avl_tree tree;
        struct avl_stack stack;
        struct avl_kv key, value;
        (void)init_keys(keys, COUNT);
        (void)avl_tree_init(&tree, cmp_i64, alloc_node, free_node, NULL);
        (void)avl_stack_init(&stack);
        (void)add_all(&tree, &stack, keys, COUNT);
        for(int64_t i = COUNT - 1; i >= 0; --i) {
                struct avl_node *node = avl_max(&tree);
                AVL_TEST(node->key.u.i64 == i);
                AVL_TEST(avl_remove_max(&tree, &stack, &key, &value));
        }
}

void test_upper()
{
        (void)puts("test_upper()");
        const int COUNT = 1000;
        int64_t keys[COUNT];
        struct avl_tree tree;
        struct avl_stack stack;
        struct avl_node *node;
        (void)init_keys(keys, COUNT);
        (void)avl_tree_init(&tree, cmp_i64, alloc_node, free_node, NULL);
        (void)avl_stack_init(&stack);
        (void)add_all(&tree, &stack, keys, COUNT);
        for(int64_t i = 0; i < COUNT; ++i) {
                AVL_TEST(avl_upper(&tree, &stack, AVL_KV(i64, i)))
                for(int64_t j = i; j < COUNT; ++j) {
                        AVL_TEST(avl_next(&stack, &node));
                        AVL_TEST(node->key.u.i64 == j);
                }
        }
        (void)avl_free_nodes(&tree, &stack);
}

void test_lower()
{
        (void)puts("test_lower()");
        const int COUNT = 1000;
        int64_t keys[COUNT];
        struct avl_tree tree;
        struct avl_stack stack;
        struct avl_node *node;
        (void)init_keys(keys, COUNT);
        (void)avl_tree_init(&tree, cmp_i64, alloc_node, free_node, NULL);
        (void)avl_stack_init(&stack);
        (void)add_all(&tree, &stack, keys, COUNT);
        for(int64_t i = 0; i < COUNT; ++i) {
                AVL_TEST(avl_lower(&tree, &stack, AVL_KV(i64, i)))
                for(int64_t j = i; j >= 0; --j) {
                        AVL_TEST(avl_prior(&stack, &node));
                        AVL_TEST(node->key.u.i64 == j);
                }
        }
        (void)avl_free_nodes(&tree, &stack);
}

int main(int argc, char **args) 
{
        test_add();
        test_remove();
        test_prior();
        test_remove_min();
        test_remove_max();
        test_upper();
        test_lower();
        return EXIT_SUCCESS;
}