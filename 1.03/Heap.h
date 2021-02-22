#ifndef HEAP_H
# define HEAP_H

struct heap_node;

typedef struct heap_node heap_node_t;

typedef struct heap {
  heap_node_t *min;
  int size;
} heap_t;
   
#endif
