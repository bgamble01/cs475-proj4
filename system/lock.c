#include <xinu.h>

local lid32 newlock(void);

/**
 * Create a new semaphore and return the ID to the caller
 * @return ID of the mutex to caller, or SYSERR
 */
lid32 lock_create()
{
	intmask mask; /* saved interrupt mask	*/
	lid32 lockid; /* lock ID to return	*/

	mask = disable();

	if ((lockid = newlock()) == SYSERR)
	{
		restore(mask);
		return SYSERR;
	}

	restore(mask);
	return lockid;
}

/**
 * Allocate an unused lock and return its index
 * @return	ID of free lock
 */
local lid32 newlock(void)
{
	static lid32 nextlock = 0; /* next lockid to try	*/
	lid32 lockid;			   /* ID to return	*/
	int32 i;				   /* iterate through # entries	*/
	struct lockentry *lptr;

	// TODO START

	// TODO - loop through each element in the lock table.

	// TODO - and find a lock that is free to use

	// TODO - set its state to used, and reset the mutex to FALSE

	// TODO - return its lockid

	// TODO - if there is no such lock, return SYSERR

	// TODO END

	for (i = 0; i < NLOCK; i++)
	{
		lptr = &locktab[i];
		if (lptr->state == LOCK_FREE)
		{
			lptr->state = LOCK_USED;
			lptr->lock = FALSE;
			lockid = i;
			return lockid;
		}
	}
	return SYSERR;
}

/**
 * Delete a lock by releasing its table entry
 * @param ID of lock to release
 */
syscall lock_delete(lid32 lockid)
{
	intmask mask;			/* saved interrupt mask		*/
	struct lockentry *lptr; /* ptr to lock table entry	*/

	mask = disable();
	if (isbadlock(lockid))
	{
		restore(mask);
		return SYSERR;
	}

	lptr = &locktab[lockid];
	if (lptr->state == LOCK_FREE)
	{
		restore(mask);
		return SYSERR;
	}

	// TODO START

	// TODO - reset lock's state to free and the mutex to FALSE

	// TODO - remove all processes waiting on its queue, and send them to the ready queue

	// TODO (RAG) - remove all RAG edges to and from this lock

	// TODO END
	locktab[lockid].state = LOCK_FREE;
	locktab[lockid].lock = FALSE;

	struct queue *lockQ = locktab[lockid].wait_queue;
	pid32 currProc = dequeue(lockQ);

	while (currProc != EMPTY)
	{
		rag_dealloc(currProc, lockid);
		enqueue(currProc, readyqueue, proctab[currProc].prprio);
		currProc = dequeue(lockQ);
	}
	enqueue(currProc, readyqueue, proctab[currProc].prprio);

	resched();
	restore(mask);
	return OK;
}

/**
 * Acquires the given lock, or cause current process to wait until acquired
 * @param lockid	lock on which to wait
 * @return status
 */
syscall acquire(lid32 lockid)
{
	intmask mask;			// saved interrupt mask
	struct lockentry *lptr; // ptr to sempahore table entry

	mask = disable();
	if (isbadlock(lockid))
	{
		restore(mask);
		return SYSERR;
	}

	lptr = &locktab[lockid];
	if (lptr->state == LOCK_FREE)
	{
		restore(mask);
		return SYSERR;
	}

	// TODO START
	// TODO - enqueue the current process ID on the lock's wait queue
	//  replaced locktab[lockid].wait_queue with lptr
	enqueue(currpid, lptr->wait_queue, proctab[currpid].prprio);
	// TODO (RAG) - add a request edge in the RAG
	rag_request(currpid, lockid);
	// TODO END

	restore(mask); // reenable interrupts

	// TODO START
	// TODO - lock the mutex!
	mutex_lock(&lptr->lock);
	// lptr->lock = TRUE;
	//  TODO END

	mask = disable(); // disable interrupts

	// TODO START
	// TODO (RAG) - we reache this point. Must've gotten the lock! Transform request edge to allocation edge
	rag_alloc(currpid, lockid);
	// TODO END

	restore(mask); // reenable interrupts
	return OK;
}

/**
 * Unlock, releasing a process if one is waiting
 * @param lockid ID of lock to unlock
 * @return status
 */
syscall release(lid32 lockid)
{
	intmask mask;			/* saved interrupt mask		*/
	struct lockentry *lptr; /* ptr to lock table entry	*/

	mask = disable();
	if (isbadlock(lockid))
	{
		restore(mask);
		return SYSERR;
	}
	lptr = &locktab[lockid];
	if (lptr->state == LOCK_FREE)
	{
		restore(mask);
		return SYSERR;
	}

	// TODO START
	// TODO - remove current process' ID from the lock's queue
	remove(currpid, lptr->wait_queue);

	// TODO - unlock the mutex

	// lptr->lock = FALSE; // changed to false
	mutex_unlock(&lptr->lock);
	// TODO (RAG) - remove allocation edge from RAG
	rag_dealloc(currpid, lockid);
	// TODO END

	restore(mask);
	return OK;
}
