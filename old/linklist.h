
/* linklist.h - intrusive linked list data structure macros

   To use this library, do this:
      #include "base/linklist.h"
*/

#ifndef LINKLIST_H
#define LINKLIST_H

#define dll_push_back_np(first, last, node, next, prev)                     \
    ((first) == NULL ?                                                      \
    ((first) = (last) = (node), (node)->next = (node)->prev = NULL) :       \
    ((last)->next = (node), (node)->prev = (last),                          \
    (last) = (node), (node)->next = NULL))

#define dll_push_front_np(first, last, node, next, prev)                    \
    dll_push_back_np(last, first, node, prev, next)

#define dll_insert_after_np(first, last, ref_node, node, next, prev)        \
    (((last) == (ref_node)) ?                                               \
    dll_push_back_np(first, last, node, next, prev) :                       \
    ((node)->prev = (ref_node), (node)->next = (ref_node)->next,            \
    (ref_node)->next->prev = (node), (ref_node)->next = (node)))

#define dll_insert_before_np(first, last, ref_node, node, next, prev)       \
    dll_insert_after_np(last, first, ref_node, node, prev, next)

#define dll_remove_first_np(first, last, next, prev)                        \
    (((first) == (last)) ?                                                  \
    (first) = (last) = null :                                               \
    ((first) = (first)->next, (first)->prev = null))

#define dll_remove_last_np(first, last, next, prev)                         \
    dll_remove_first_np(last, first, prev, next)

#define dll_remove_np(first, last, node, next, prev)                        \
    (((first) == (node)) ?                                                  \
    dll_remove_first_np(first, last, next, prev) :                          \
    ((last) == (node)) ?                                                    \
    dll_remove_last_np(first, last, next, prev) :                           \
    ((node)->next->prev = (node)->prev,                                     \
    (node)->prev->next = (node)->next))

#define dll_push_back(first, last, node)                  dll_push_back_np(first, last, node, next, prev)
#define dll_push_front(first, last, node)                 dll_push_front_np(first, last, node, next, prev)
#define dll_insert_after(first, last, ref_node, node)     dll_insert_after_np(first, last, ref_node, node, next, prev)
#define dll_insert_before(first, last, ref_node, node)    dll_insert_before_np(first, last, ref_node, node, next, prev)
#define dll_remove(first, last, node)                     dll_remove_np(first, last, node, next, prev)
#define dll_remove_first(first, last)                     dll_remove_first_np(first, last, next, prev)
#define dll_remove_last(first, last)                      dll_remove_last_np(first, last, next, prev)


#define stack_push_n(first, node, next)                                     \
    ((node)->next = (first), (first) = (node))

#define stack_pop_n(first, next)                                            \
    ((first) = (first)->next)

#define stack_push(first, node)   stack_push_n(first, node, next)
#define stack_pop(first)          stack_pop_n(first, next)


#define queue_push_n(first, last, node, next)                               \
    ((first) == null ?                                                      \
    ((first) = (last) = (node), (node)->next = null) :                      \
    ((last)->next = (node), (last) = (node), (node)->next = null))

#define queue_pop_n(first, last, next)                                      \
    ((first) == (last) ?                                                    \
    (first) = (last) = null :                                               \
    ((first) = (first)->next))

#define queue_push(first, last, node)   queue_push_n(first, last, node, next)
#define queue_pop(first, last)          queue_pop_n(first, last, next) 

#endif // LINKLIST_H
