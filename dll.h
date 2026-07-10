
/* dll.h - doubly-linked list macros

COMPILE-TIME OPTIONS

  #define DLL_UNIT_TESTS

     Defines a function dll_unit_tests() that checks the functionality of the data structure.


DOCUMENTATION

  Declare a node type with the following structure:

    typedef struct T T; 
    struct T
    {
        // user-defined data here
        ...
        T* prev;
        T* next;
    };

  Declare an empty head and tail for a doubly-linked list of type T:

    T* t_head = NULL;
    T* t_tail = NULL;


  void  DLL_PUSH_BACK         (T* first, T* last, T* node)                 - Pushes node to end of list
  void  DLL_PUSH_FRONT        (T* first, T* last, T* node)                 - Pushes node to front of list
 
  void  DLL_INSERT_AFTER      (T* first, T* last, T* ref_node, T* node)    - Inserts node after reference node in list
  void  DLL_INSERT_BEFORE     (T* first, T* last, T* ref_node, T* node)    - Inserts node before reference node in list
 
  void  DLL_REMOVE            (T* first, T* last, T* node)                 - Removes node from list
  void  DLL_REMOVE_FIRST      (T* first, T* last)                          - Removes node from front of list
  void  DLL_REMOVE_LAST       (T* first, T* last)                          - Removes node from end of list


  The macros above assume the nodes reference other nodes with fields named "prev" and "next".
  The macros below allow for the user to specify the names of the fields for "next" and "prev".


  void  DLL_PUSH_BACK_NP      (T* first, T* last, T* node, Text next, Text prev)                  
  void  DLL_PUSH_FRONT_NP     (T* first, T* last, T* node, Text next, Text prev)                 
 
  void  DLL_INSERT_AFTER_NP   (T* first, T* last, T* ref_node, T* node, Text next, Text prev)    
  void  DLL_INSERT_BEFORE_NP  (T* first, T* last, T* ref_node, T* node, Text next, Text prev)    
 
  void  DLL_REMOVE_NP         (T* first, T* last, T* node, Text next, Text prev)                 
  void  DLL_REMOVE_FIRST_NP   (T* first, T* last, Text next, Text prev)                          
  void  DLL_REMOVE_LAST_NP    (T* first, T* last, Text next, Text prev)                          

*/

#ifndef _DLL_H
#define _DLL_H

//***************************************************************************
//          INCLUDE FILES
//***************************************************************************

// TODO(bcall): dll_unit_tests()

//***************************************************************************
//          FUNCTION PROTOTYPES
//***************************************************************************

// TODO(bcall): dll_unit_tests()

//***************************************************************************
//          MACROS
//***************************************************************************

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

#endif // _DLL_H
