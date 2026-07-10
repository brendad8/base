
/* stack.h - stack data structure macros

COMPILE-TIME OPTIONS

  #define STACK_UNIT_TESTS

     Defines a function stack_unit_tests() that checks the functionality of the data structure.


DOCUMENTATION

  Declare a node type with the following structure:

    typedef struct T T; 
    struct T
    {
        // user-defined data here
        ...
        T* next;
    };

  Declare an empty head for a stack of type T:

    T* t_head = NULL;


    void  STACK_PUSH    (T* first, T* node)    - Pushes node to front of stack
    void  STACK_POP     (T* first)             - Pops node from front of stack. Node is not returned         

    The macros above assume the nodes reference other nodes with a field named "next".
    The macros below allow for the user to specify the names of the field for "next".

    void  STACK_PUSH_N  (T* first, T* node, Text next)   
    void  STACK_POP_N   (T* first, Text next)            

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

