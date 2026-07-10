


//***************************************************************************
//          DOCUMENTATION
//***************************************************************************

// void  QUEUE_PUSH  (T* first, T* last, T* node)   Pushes node to end of queue
// void  QUEUE_POP   (T* first, T* last)            Pops node from start of queue. Node is not returned.


// void  QUEUE_PUSH_N  (T* first, T* last, T* node, Text next)   
// void  QUEUE_POP_N   (T* first, T* last, Text next)            
 
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
