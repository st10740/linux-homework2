#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"

typedef struct {
    struct list_head list;
    long value;
} node_t;

struct list_head *list_new()
{
    struct list_head *l = malloc(sizeof(struct list_head));
    if (!l)
        return NULL;
    INIT_LIST_HEAD(l);
    return l;
}

void list_add_node(struct list_head *head, struct list_head *node)
{
    if (!(head) || !node)
        return;
    list_del(node);
    list_add(node, head);
}

int list_length(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
    return len;
}

void list_construct(struct list_head *head, int n)
{
    if (!head)
        return;
    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node)
        return;
    new_node->value = n;
    list_add(&new_node->list, head);
}

void list_free(struct list_head *head)
{
    node_t *item, *tmp;
    list_for_each_entry_safe (item, tmp, head, list) {
        free(item);
    }
    free(head);
}

/* Verify if list is order */
static bool list_is_ordered(struct list_head *head)
{
    int value;
    node_t *item;
    list_for_each_entry (item, head, list) {
        if (&item->list != head->next && item->value < value)
            return false;
        value = item->value;
    }
    return true;
}

/* shuffle array, only work if n < RAND_MAX */
void shuffle(int *array, size_t n)
{
    if (n <= 0)
        return;

    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

void quick_sort(struct list_head **list)
{
    int n = list_length(*list);
    int value;
    int i = 0;
    int max_level = 2 * n;
    bool first = true;
    struct list_head *heads[max_level];

    struct list_head *result = list_new();
    struct list_head *left = list_new();
    struct list_head *right = list_new();
    struct list_head *pivot = list_new();

    heads[0] = *list;

    while (i >= 0) {
        struct list_head *head = heads[i];
        if (!list_empty(head) && !list_is_singular(head)) {
            struct list_head *pivot_node = head->next;
            value = list_entry(pivot_node, node_t, list)->value;
            struct list_head *p = pivot_node->next;
            list_add_node(pivot, pivot_node);

            while (p != head) {
                struct list_head *n = p;
                p = p->next;
                list_add_node(list_entry(n, node_t, list)->value > value
                                  ? right
                                  : left,
                              n);
            }
            free(head);

            heads[i] = left;
            heads[i + 1] = pivot;
            heads[i + 2] = right;

            left = list_new();
            right = list_new();
            pivot = list_new();

            i += 2;
        } else {
            if (!list_empty(head))
                list_add_node(result, head->next);
            i--;
        }
    }
    *list = result;
}

int main(int argc, char **argv)
{
    struct list_head *list = list_new();

    size_t count = 100000;
    int *test_arr = malloc(sizeof(int) * count);

    for (int i = 0; i < count; ++i)
        test_arr[i] = i;
    shuffle(test_arr, count);

    while (count--)
        list_construct(list, test_arr[count]);

    quick_sort(&list);
    assert(list_is_ordered(list));

    list_free(list);
    free(test_arr);

    return 0;
}