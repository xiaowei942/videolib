#ifndef _LIST_H
#define _LIST_H

struct list_head
{
	struct list_head *prev;
	struct list_head *next;
};

static inline void init_list_head(struct list_head *head)
{
	head->next = head;
	head->prev = head;
}
static inline void __list_add(struct list_head *new , struct list_head *prev , struct list_head *next)
{
	new->next = next;
	new->prev = prev;
	prev->next = new;
	next->prev = new;
}

static inline void list_push_back(struct list_head *new , struct list_head *head)
{
	__list_add(new , head->prev , head);
}

static inline void list_push_front(struct list_head *new , struct list_head *head)
{
	__list_add(new , head , head->next);
}

static inline void __list_del(struct list_head *prev , struct list_head *next)
{
	prev->next = next;
	next->prev = prev;
}

static inline void list_del(struct list_head *node)
{
	__list_del(node->prev , node->next);
}

static inline int list_empty(const struct list_head *head)
{
	return (head == head->next);
}
#define offset_of(type , member) (size_t)(&((type *)0)->member)

#define container_of(ptr , type , member) \
	({ \
	 (type *)( (char *)ptr - offset_of(type , member)); \
	 })

#define list_entry(ptr , type , member) \
	container_of(ptr , type , member)

#define list_first_entry(ptr , type , member) \
	list_entry((ptr)->next , type , member)

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); \
			pos = pos->next)

#endif// _LIST_H
