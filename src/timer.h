#ifndef TIMER_H
#define TIMER_H

#include "connection.h"

#define    CON_TIMEOUT      (60)
#define INHEAP(n, x) (((-1) < (x)) && ((x) < (n)))

void c_swap(int x, int y);
void heap_percolate_up(int idx);
void heap_percolate_down(int idx);
int heap_insert(connection_t *c);

int connection_register(connection_t *con);
void connection_unregister(connection_t *con);
void connection_prune();
int connecion_is_expired(connection_t *con);
void connecion_set_reactivated(connection_t *con);
void connecion_set_expired(connection_t *con);
#endif // TIMER_H
