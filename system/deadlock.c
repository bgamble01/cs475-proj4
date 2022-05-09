#include "xinu.h"
#include "deadlock.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

// #define NPROC 20
// #define NLOCK 10
int rag[NLOCK + NPROC][NLOCK + NPROC];
lid32 culprit;

void rag_request(int pid, int lockid)
{
    rag[NLOCK + pid][lockid] = 1;
    // printf("%d %d\n", (NLOCK + pid+1), (lockid+1));
}

void rag_alloc(int pid, int lockid)
{
    rag[NLOCK + pid][lockid] = 0;
    rag[lockid][NLOCK + pid] = 1;
}

void rag_dealloc(int pid, int lockid)
{
    rag[NLOCK + pid][lockid] = 0;
    rag[lockid][NLOCK + pid] = 0;
}

void rag_print()
{
    for (int i = 0; i < NLOCK + NPROC; i++)
    {
        for (int j = 0; j < NLOCK + NPROC; j++)
        {
            kprintf("%d", rag[i][j]);
        }
        kprintf("\n");
    }
}
void deadlock_recover(lid32 culprit)
{
    rag_print();
    int i, j;
    struct lockEntry *lptr = &locktab[culprit];
    for (i = 0; i < NPROC + NLOCK; i++)
    {

        if (rag[culprit][i] == 1)
        {
            kill(i);
            for (j = 0; j < NLOCK; j++)
            {
                remove(i, locktab[j].wait_queue);
            }
            mutex_unlock(lptr);
            for (int x = 0; x < NPROC + NLOCK; x++)
            {
                rag[i][x] = 0;
                rag[x][i] = 0;
            }
            break;
        }
    }

    kprintf("DEADLOCK RECOVER      Killing  pid= %d to release lockid= %d\n", i - NLOCK, culprit);
    rag_print();
    return;
}

void deadlock_detect()
{
    for (int i = 0; i < NLOCK + NPROC; i++)
    {
        for (int j = 0; j < NLOCK + NPROC; j++)
        {
            if (rag[i][j] == 1)
            {
                int arr[1];
                arr[0] = i;
                if (isCycle(arr, j, 1))
                {
                    deadlock_recover(culprit);
                    return;
                }
            }
        }
    }
}

int isCycle(int visited[], int start, int arrSize)
{

    int newSize = arrSize + 1;
    int newVisited[newSize];
    int k;
    culprit = -1;
    // printf("start is %d\n", start);
    // printf(" arr = ");

    for (k = 0; k < arrSize; k++)
    {
        newVisited[k] = visited[k];
        //   printf(" %d ", visited[k]);
    }

    for (int x = 0; x < (newSize); x++)
    {
        if (start == newVisited[x])
        {
            kprintf("DEADLOCK: \n");
            for (int y = 0; y < arrSize; y++)
            {
                if (newVisited[y] < NLOCK)
                {
                    kprintf("lockid = %d ", newVisited[y]);
                    if (culprit == -1)
                    {
                        culprit = newVisited[y];
                    }
                }
                else
                {
                    kprintf("pid = %d ", (newVisited[y] - NLOCK));
                }
            }
            kprintf("\n");

            return 1;
        }
    }

    newVisited[k] = start;

    //  printf("%d", newVisited[k]);
    // printf("\n");

    // printf("truestart: %d, start: %d\n", trueStart, start);
    for (int i = 0; i < NLOCK + NPROC; i++)
    {
        if (rag[start][i] == 1)
        {
            if (isCycle(newVisited, i, newSize))
            {
                return 1;
            }
        }
    }
    return 0;
}

// TODO - add in your functions
