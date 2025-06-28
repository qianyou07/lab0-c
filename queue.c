#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *tmp = malloc(sizeof(struct list_head));
    if (tmp)
        INIT_LIST_HEAD(tmp);
    return tmp;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node, *safe;
    list_for_each_safe(node, safe, head) {
        element_t *tmp = list_entry(node, element_t, list);
        free(tmp->value);
        free(tmp);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *tmp = malloc(sizeof(element_t));
    if (!tmp)
        return false;

    tmp->value = strdup(s);
    if (!tmp->value) {
        free(tmp);
        return false;
    }

    list_add(&tmp->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *tmp = malloc(sizeof(element_t));
    if (!tmp)
        return false;

    tmp->value = strdup(s);
    if (!tmp->value) {
        free(tmp);
        return false;
    }

    list_add_tail(&tmp->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;
    element_t *tmp = list_first_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, tmp->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del_init(&tmp->list);
    return tmp;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;
    element_t *tmp = list_last_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, tmp->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del_init(&tmp->list);
    return tmp;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each(li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || head->next == head)
        return false;

    struct list_head *slow = head->next, *fast = slow->next;
    while (fast != head && fast != head->prev) {
        slow = slow->next;
        fast = fast->next->next;
    }
    list_del(slow);
    element_t *tmp = list_entry(slow, element_t, list);
    free(tmp->value);
    free(tmp);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || head->next == head)
        return false;
    element_t *start = list_first_entry(head, element_t, list), *end, *safe;
    int len = 1;
    list_for_each_entry_safe(end, safe, head, list) {
        if (strcmp(start->value, end->value) != 0) {
            struct list_head *before = start->list.prev, *after = &end->list;
            if (len > 2) {
                // delete [start, end)
                struct list_head *node = &start->list, *safe_next = node->next;
                while (node != after) {
                    node->prev->next = node->next;
                    node->next->prev = node->prev;
                    element_t *tmp = list_entry(node, element_t, list);
                    free(tmp->value);
                    free(tmp);
                    node = safe_next;
                    safe_next = safe_next->next;
                }
                before->next = after;
                after->prev = before;
            }
            start = end;
            len = 2;
        } else if (end->list.next == head && len >= 2) {
            struct list_head *before = start->list.prev,
                             *after = end->list.next;

            // delete [start, end]
            struct list_head *node = &start->list, *safe_next = node->next;
            while (node != after) {
                node->prev->next = node->next;
                node->next->prev = node->prev;
                element_t *tmp = list_entry(node, element_t, list);
                free(tmp->value);
                free(tmp);
                node = safe_next;
                safe_next = safe_next->next;
            }
            before->next = after;
            after->prev = before;

        } else {
            len++;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;
    struct list_head *before = head, *node = head->next, *neighbor = node->next,
                     *after = neighbor->next;

    while (node != head && neighbor != head) {
        before->next = neighbor;
        neighbor->prev = before;
        neighbor->next = node;
        node->prev = neighbor;
        node->next = after;
        after->prev = node;
        before = node;
        node = after;
        neighbor = node->next;
        after = neighbor->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node, *safe;
    list_for_each_safe(node, safe, head) {
        node->next = node->prev;
        node->prev = safe;
    }
    safe = head->prev;
    head->prev = head->next;
    head->next = safe;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || head->next == head || k == 1)
        return;

    int cnt = 1;

    struct list_head *start = head->next, *cur = head->next;
    while (cur != head) {
        if (cnt == k) {
            struct list_head *before, *after;
            before = start->prev;
            after = cur->next;
            for (struct list_head *node = start, *safe = node->next;
                 node != after; node = safe, safe = safe->next) {
                node->next = node->prev;
                node->prev = safe;
            }
            start->next = after;
            cur->prev = before;
            before->next = cur;
            after->prev = start;
            start = after;
            cur = after;
            cnt = 1;
        } else {
            cnt++;
            cur = cur->next;
        }
    }
}

void quick_sort(struct list_head *start, struct list_head *end, bool descend)
{
    if (start == end || list_is_singular(start))
        return;

    // 計算區間大小
    int size = 1;
    struct list_head *tmp;
    for (tmp = start; tmp != end; tmp = tmp->next)
        size++;

    // 隨機選擇 pivot
    srand(time(NULL));
    int pivot_index = rand() % size;
    struct list_head *pivot = start;
    for (int i = 0; i < pivot_index; i++)
        pivot = pivot->next;

    const element_t *pivot_ele = list_entry(pivot, element_t, list);

    struct list_head *before = start->prev, *after = end->next;

    // 建立三個 list: smaller, equal, larger
    LIST_HEAD(small);
    LIST_HEAD(equal);
    LIST_HEAD(large);

    tmp = start;
    struct list_head *next;
    while (tmp != after) {
        next = tmp->next;
        const element_t *ele = list_entry(tmp, element_t, list);
        int cmp = strcmp(ele->value, pivot_ele->value);
        if ((cmp < 0 && !descend) || (cmp > 0 && descend)) {
            list_del(tmp);
            list_add_tail(tmp, &small);
        } else if (cmp == 0) {
            list_del(tmp);
            list_add_tail(tmp, &equal);
        } else {
            list_del(tmp);
            list_add_tail(tmp, &large);
        }
        tmp = next;
    }

    // 把 small, equal, large 接回原本位置
    struct list_head *cur = before;
    if (!list_empty(&small)) {
        cur->next = small.next;
        small.next->prev = cur;
        cur = small.prev;
    }

    if (!list_empty(&equal)) {
        cur->next = equal.next;
        equal.next->prev = cur;
        cur = equal.prev;
    }

    if (!list_empty(&large)) {
        cur->next = large.next;
        large.next->prev = cur;
        cur = large.prev;
    }

    cur->next = after;
    after->prev = cur;

    // 排序左右兩邊
    if (!list_empty(&small)) {
        quick_sort(small.next, small.prev, descend);
    }
    if (!list_empty(&large)) {
        quick_sort(large.next, large.prev, descend);
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head)
        return;

    quick_sort(head->next, head->prev, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || head->next == head)
        return 0;

    struct list_head *tail = head->next;
    int size = 1;
    for (struct list_head *cur = tail->next, *safe = cur->next; cur != head;
         cur = safe, safe = safe->next) {
        const element_t *tail_element = list_entry(tail, element_t, list);
        const element_t *cur_element = list_entry(cur, element_t, list);
        int cmp = strcmp(tail_element->value, cur_element->value);
        while (cmp > 0) {
            tail = tail->prev;
            size--;
            element_t *be_freed = list_entry(tail->next, element_t, list);
            free(be_freed->value);
            free(be_freed);
            if (tail == head)
                break;
            tail_element = list_entry(tail, element_t, list);
            cmp = strcmp(tail_element->value, cur_element->value);
        }
        tail->next = cur;
        cur->prev = tail;
        tail = tail->next;
        size++;
    }
    tail->next = head;
    return size;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || head->next == head)
        return 0;

    struct list_head *tail = head->next;
    int size = 1;
    for (struct list_head *cur = tail->next, *safe = cur->next; cur != head;
         cur = safe, safe = safe->next) {
        const element_t *tail_element = list_entry(tail, element_t, list);
        const element_t *cur_element = list_entry(cur, element_t, list);
        int cmp = strcmp(tail_element->value, cur_element->value);
        while (cmp < 0) {
            tail = tail->prev;
            size--;
            element_t *be_freed = list_entry(tail->next, element_t, list);
            free(be_freed->value);
            free(be_freed);
            if (tail == head)
                break;
            tail_element = list_entry(tail, element_t, list);
            cmp = strcmp(tail_element->value, cur_element->value);
        }
        tail->next = cur;
        cur->prev = tail;
        tail = tail->next;
        size++;
    }
    tail->next = head;
    return size;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    queue_contex_t *head_queue_element =
        (queue_contex_t *) ((char *) head->next -
                            offsetof(queue_contex_t, chain));

    int size = 0;
    int first_list_size = 0;
    for (struct list_head *cur = head->next; cur != head; cur = cur->next) {
        const queue_contex_t *tmp =
            (queue_contex_t *) ((char *) cur - offsetof(queue_contex_t, chain));
        if (tmp->id == 0)
            first_list_size = tmp->size;
        size += tmp->size;
    }
    for (int i = 0; i < size; i++) {
        queue_contex_t *local_extremum = NULL;
        for (struct list_head *cur = head->next; cur != head; cur = cur->next) {
            queue_contex_t *tmp =
                (queue_contex_t *) ((char *) cur -
                                    offsetof(queue_contex_t, chain));
            if ((tmp->id == 0 && first_list_size > 0) ||
                (tmp->id != 0 && tmp->q && tmp->q->next != tmp->q)) {
                if (local_extremum == NULL) {
                    local_extremum = tmp;
                } else {
                    const element_t *local_element =
                        list_first_entry(local_extremum->q, element_t, list);
                    const element_t *tmp_element =
                        list_first_entry(tmp->q, element_t, list);
                    int cmp = strcmp(local_element->value, tmp_element->value);
                    if ((descend && cmp < 0) || (!descend && cmp > 0)) {
                        local_extremum = tmp;
                    }
                }
            }
        }

        if (local_extremum->id == 0) {
            first_list_size--;
        }
        struct list_head *node = local_extremum->q->next;
        list_del(node);
        list_add_tail(node, head_queue_element->q);
    }
    return size;
}
