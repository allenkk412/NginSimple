#include "timer.h"

#include <stdio.h>

connection_t *ns_connections[MAXCONNECTION] = {0};         // 存储着指向所有连接的指针数组，最小堆
static int heap_size = 0;

void c_swap(int x, int y)
{
    if( x >=0 && x < heap_size && y >= 0 && y < heap_size)
    {
        connection_t *tmp = ns_connections[x];
        ns_connections[x] = ns_connections[y];
        ns_connections[y] = tmp;

        // 更新heap_idx
        ns_connections[x]->heap_idx = x;
        ns_connections[y]->heap_idx = y;
    }
    else
    {
        printf("ERROR: connections heap c_swap() error.");
        fflush(stdout);
    }

}

void heap_percolate_up(int idx)
{
    while(idx/2 >= 0)
    {
        int fidx = idx/2;    // idx的父节点
        connection_t *c   = ns_connections[idx];
        connection_t *fc  = ns_connections[fidx];
        if(c->active_time >= fc->active_time)
            break;
        c_swap(idx, fidx);
        idx = fidx;
    }
}

void heap_percolate_down(int idx)
{

    while (1)
    {
        int proper_child;
        int lchild = INHEAP(heap_size, (idx * 2 + 1) ) ? (idx * 2 + 1)  : (heap_size + 1);
        int rchild = INHEAP(heap_size, (idx * 2    ) ) ? (idx * 2     ) : (heap_size + 1);

        if(lchild > heap_size && rchild > heap_size)
        { // 无左右子节点
          break;
        }
        else if(lchild < heap_size && rchild < heap_size)  //(INHEAP(heap_size, lchild) && INHEAP(heap_size, rchild))
        { // 有左右子节点
          proper_child = (ns_connections[lchild]->active_time <  ns_connections[rchild]->active_time) ? lchild : rchild;
        }
        else if (lchild > heap_size)
        {
          proper_child = rchild;
        }
        else
        {
          proper_child = lchild;
        }
        // idx is the smaller than children
        if (ns_connections[idx]->active_time <= ns_connections[proper_child]->active_time)
            break;

        c_swap(idx, proper_child);
        idx = proper_child;
  }
}

int heap_insert(connection_t *con)
{
    if (heap_size >= MAXCONNECTION)
        return -1;
    ns_connections[heap_size++] = con;
    con->heap_idx = heap_size - 1;
    heap_percolate_up(con->heap_idx);

    printf("heap_size: %d\n", heap_size);

    return 0;
}

int connection_register(connection_t *con)
{
    if (heap_size >= MAXCONNECTION)
    {
        printf("ERROR:heap is full.\n");
        fflush(stdout);
        return -1;
    }

    return heap_insert(con);
}

void connection_unregister(connection_t *con)
{
    // heap_size-- 和 heap_percolate_down()完成DeleteMin操作
    if(heap_size >= 1)
    {
        ns_connections[con->heap_idx] = ns_connections[heap_size - 1];
        ns_connections[con->heap_idx]->heap_idx = con->heap_idx;
        heap_size--;
        heap_percolate_down(con->heap_idx);
    }
    else
    {
        printf("ERROR:heap_size <= 1.\n");
        fflush(stdout);
    }
}

void connection_prune()
{
    while (heap_size > 0)
    {
        connection_t *c = ns_connections[0];
        if (time(NULL) - c->active_time >= CON_TIMEOUT)
            connection_close(c);
        else
            break;
  }
}

int connection_is_expired(connection_t *con)
{
    return ( (time(NULL) - con->active_time ) > CON_TIMEOUT);
}

void connection_set_reactivated(connection_t *con)
{
    con->active_time = time(NULL);
    heap_percolate_down(con->heap_idx);
}

void connection_set_expired(connection_t *con)
{
    con->active_time = 0; // very old time
    heap_percolate_up(con->heap_idx);
}
