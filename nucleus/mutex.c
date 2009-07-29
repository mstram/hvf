#include <mutex.h>

/* slow-path mutex locking */
void __mutex_lock(mutex_t *lock)
{
	spin_lock(&lock->queue_lock);

	if (unlikely(atomic_add_unless(&lock->state, -1, 0))) {
		spin_unlock(&lock->queue_lock);
		return; /* aha! someone released it & it's ours now! */
	}

	list_add_tail(&current->blocked_list, &lock->queue);

	spin_unlock(&lock->queue_lock);

	schedule_blocked();
}