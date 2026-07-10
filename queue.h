

/* queue.h - queue data structure macros

COMPILE-TIME OPTIONS

  #define QUEUE_UNIT_TESTS

     Defines a function queue_unit_tests() that checks the functionality of the data structure.


DOCUMENTATION

  Declare a node type with the following structure:

    typedef struct T T; 
    struct T
    {
        // user-defined data here
        ...
        T* next;
    };

  Declare an empty head and tail for a queue of type T:

    T* t_head = NULL;
    T* t_tail = NULL;


    void  QUEUE_PUSH  (T* first, T* last, T* node)    - Pushes node to end of queue
    void  QUEUE_POP   (T* first, T* last)             - Pops node from start of queue. Node is not returned.


    The macros above assume the nodes reference other nodes with a field named "next".
    The macros below allow for the user to specify the names of the field for "next".


    void  QUEUE_PUSH_N  (T* first, T* last, T* node, Text next)   
    void  QUEUE_POP_N   (T* first, T* last, Text next)            

*/

 
#ifndef _QUEUE_H
#define _QUEUE_H

//***************************************************************************
//          MACROS
//***************************************************************************

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

#endif // _QUEUE_H
      



