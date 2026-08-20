
/* linklist.h - intrusive linked list data structure macros

DOUBLY-LINKED LIST DOCUMENTATION

  Declare a node type with the following structure:

    typedef struct T T;
    struct T
    {
        // user-defined data here
        ...
        T* prev;
        T* next;
    };

  Declare an empty doubly-linked list:

    T* t_head = NULL;
    T* t_tail = NULL;


    void  DLL_PUSH_BACK      (T* first, T* last, T* node)               - Pushes node onto the back of the list
    void  DLL_PUSH_FRONT     (T* first, T* last, T* node)               - Pushes node onto the front of the list

    void  DLL_INSERT_AFTER   (T* first, T* last, T* ref_node, T* node)  - Inserts node after ref_node
    void  DLL_INSERT_BEFORE  (T* first, T* last, T* ref_node, T* node)  - Inserts node before ref_node

    void  DLL_REMOVE         (T* first, T* last, T* node)               - Removes node from the list
    void  DLL_REMOVE_FIRST   (T* first, T* last)                        - Removes the front node from the list. The removed node is not returned.
    void  DLL_REMOVE_LAST    (T* first, T* last)                        - Removes the back node from the list. The removed node is not returned.


  The macros above assume each node contains pointers named "prev" and
  "next". The macros below allow different field names to be specified.


    void  DLL_PUSH_BACK_NP     (T* first, T* last, T* node, next, prev)
    void  DLL_PUSH_FRONT_NP    (T* first, T* last, T* node, next, prev)

    void  DLL_INSERT_AFTER_NP  (T* first, T* last, T* ref_node, T* node, next, prev)

    void  DLL_INSERT_BEFORE_NP (T* first, T* last, T* ref_node, T* node, next, prev)

    void  DLL_REMOVE_NP        (T* first, T* last, T* node, next, prev)
    void  DLL_REMOVE_FIRST_NP  (T* first, T* last, next, prev)
    void  DLL_REMOVE_LAST_NP   (T* first, T* last, next, prev)


STACK DOCUMENTATION

  Declare a node type with the following structure:

    typedef struct T T;
    struct T
    {
        // user-defined data here
        ...
        T* next;
    };

  Declare an empty stack head:

    T* t_head = NULL;


    void  STACK_PUSH    (T* head, T* node)     - Pushes node onto the top of the stack
    void  STACK_POP     (T* head)              - Removes the top node from the stack. The removed node is not returned.


  The macros above assume each node contains a pointer named "next".
  The macros below allow a different field name to be specified.

    void  STACK_PUSH_N  (T* head, T* node, next)
    void  STACK_POP_N   (T* head, next)


QUEUE DOCUMENTATION

  Declare a node type with the following structure:

    typedef struct T T;
    struct T
    {
        // user-defined data here
        ...
        T* next;
    };

  Declare an empty queue:

    T* t_first = NULL;
    T* t_last  = NULL;


    void  QUEUE_PUSH    (T* first, T* last, T* node)    - Pushes node onto the back of the queue
    void  QUEUE_POP     (T* first, T* last)             - Removes the front node from the queue. The removed node is not returned.


  The macros above assume each node contains a pointer named "next".
  The macros below allow a different field name to be specified.

    void  QUEUE_PUSH_N  (T* first, T* last, T* node, next)
    void  QUEUE_POP_N   (T* first, T* last, next)

*/

#ifndef _LINKLIST_H
#define _LINKLIST_H

#define DLL_PUSH_BACK_NP(first, last, node, next, prev)                     \
    ((first) == NULL ?                                                      \
    ((first) = (last) = (node), (node)->next = (node)->prev = NULL) :       \
    ((last)->next = (node), (node)->prev = (last),                          \
    (last) = (node), (node)->next = NULL))

#define DLL_PUSH_FRONT_NP(first, last, node, next, prev)                    \
    DLL_PUSH_BACK_NP(last, first, node, prev, next)

#define DLL_INSERT_AFTER_NP(first, last, ref_node, node, next, prev)        \
    (((last) == (ref_node)) ?                                               \
    DLL_PUSH_BACK_NP(first, last, node, next, prev) :                       \
    ((node)->prev = (ref_node), (node)->next = (ref_node)->next,            \
    (ref_node)->next->prev = (node), (ref_node)->next = (node)))

#define DLL_INSERT_BEFORE_NP(first, last, ref_node, node, next, prev)       \
    DLL_INSERT_AFTER_NP(last, first, ref_node, node, prev, next)

#define DLL_REMOVE_FIRST_NP(first, last, next, prev)                        \
    (((first) == (last)) ?                                                  \
    (first) = (last) = NULL :                                               \
    ((first) = (first)->next, (first)->prev = NULL))

#define DLL_REMOVE_LAST_NP(first, last, next, prev)                         \
    DLL_REMOVE_FIRST_NP(last, first, prev, next)

#define DLL_REMOVE_NP(first, last, node, next, prev)                        \
    (((first) == (node)) ?                                                  \
    DLL_REMOVE_FIRST_NP(first, last, next, prev) :                          \
    ((last) == (node)) ?                                                    \
    DLL_REMOVE_LAST_NP(first, last, next, prev) :                           \
    ((node)->next->prev = (node)->prev,                                     \
    (node)->prev->next = (node)->next))

#define DLL_PUSH_BACK(first, last, node)                  DLL_PUSH_BACK_NP(first, last, node, next, prev)
#define DLL_PUSH_FRONT(first, last, node)                 DLL_PUSH_FRONT_NP(first, last, node, next, prev)
#define DLL_INSERT_AFTER(first, last, ref_node, node)     DLL_INSERT_AFTER_NP(first, last, ref_node, node, next, prev)
#define DLL_INSERT_BEFORE(first, last, ref_node, node)    DLL_INSERT_BEFORE_NP(first, last, ref_node, node, next, prev)
#define DLL_REMOVE(first, last, node)                     DLL_REMOVE_NP(first, last, node, next, prev)
#define DLL_REMOVE_FIRST(first, last)                     DLL_REMOVE_FIRST_NP(first, last, next, prev)
#define DLL_REMOVE_LAST(first, last)                      DLL_REMOVE_LAST_NP(first, last, next, prev)


#define STACK_PUSH_N(first, node, next)                                     \
    ((node)->next = (first), (first) = (node))

#define STACK_POP_N(first, next)                                            \
    ((first) = (first)->next)

#define STACK_PUSH(first, node)   STACK_PUSH_N(first, node, next)
#define STACK_POP(first)          STACK_POP_N(first, next)


#define QUEUE_PUSH_N(first, last, node, next)                               \
    ((first) == NULL ?                                                      \
    ((first) = (last) = (node), (node)->next = NULL) :                      \
    ((last)->next = (node), (last) = (node), (node)->next = NULL))

#define QUEUE_POP_N(first, last, next)                                      \
    ((first) == (last) ?                                                    \
    (first) = (last) = NULL :                                               \
    ((first) = (first)->next))

#define QUEUE_PUSH(first, last, node)   QUEUE_PUSH_N(first, last, node, next)
#define QUEUE_POP(first, last)          QUEUE_POP_N(first, last, next) 


#endif // _LINKLIST_H
