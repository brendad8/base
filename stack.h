
/* stack.h - stack data structure macros

COMPILE-TIME OPTIONS

  #define STACK_UNIT_TESTS

     Defines stack_unit_tests() which verifies the functionality of the data structure.


DOCUMENTATION

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

*/

#ifndef _STACK_H
#define _STACK_H

//***************************************************************************
//          INCLUDE FILES
//***************************************************************************

// TODO(bcall): stack_unit_tests()

//***************************************************************************
//          FUNCTION PROTOTYPES
//***************************************************************************

// TODO(bcall): stack_unit_tests()

//***************************************************************************
//          MACROS
//***************************************************************************

#define STACK_PUSH_N(first, node, next)                                     \
    ((node)->next = (first), (first) = (node))

#define STACK_POP_N(first, next)                                            \
    ((first) = (first)->next)

#define STACK_PUSH(first, node)   STACK_PUSH_N(first, node, next)
#define STACK_POP(first)          STACK_POP_N(first, next)


#endif // _STACK_H

