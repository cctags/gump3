/********************************************************************
 * The doubly linked list is from the 'linux/list.h'                *
 ********************************************************************/

struct list_head
{
    struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define list_entry(ptr, type, member) \
    (type *)((char *)ptr - ((size_t)&((type *)0)->member))

static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

static inline void __list_add(
        struct list_head *node, struct list_head *prev, struct list_head *next)
{
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

static inline void __list_del(struct list_head * prev, struct list_head * next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_add(struct list_head *node, struct list_head *head)
{
    __list_add(node, head, head->next);
}

static inline void list_add_tail(struct list_head *node, struct list_head *head)
{
    __list_add(node, head->prev, head);
}

static inline void list_del(struct list_head *node)
{
    __list_del(node->prev, node->next);
    node->next = (struct list_head *)0x1001;
    node->prev = (struct list_head *)0x2002;
}

static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}
