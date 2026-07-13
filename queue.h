
/* queue.h - queue data structure macros

COMPILE-TIME OPTIONS

  #define QUEUE_UNIT_TESTS

     Defines queue_unit_tests() which verifies the functionality of the data structure.


DOCUMENTATION

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

#ifndef _QUEUE_H
#define _QUEUE_H

//***************************************************************************
//          INCLUDE FILES
//***************************************************************************

// TODO(bcall): queue_unit_tests()

//***************************************************************************
//          FUNCTION PROTOTYPES
//***************************************************************************

// TODO(bcall): queue_unit_tests()

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
