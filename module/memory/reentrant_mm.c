#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0)
  #include <linux/rwsem.h>
#else
  #include <linux/mmap_lock.h>
#endif

#include <linux/mm_types.h>
#include <linux/mutex.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include "../config.h"
#include "internal.h"

/* --------------------- MM-ENTRY-RELATED DEFINITIONS ---------------------- */

/*
 * Since Linux' internal memory locking mechanism may not be reentrant-safe,
 * we need to keep track of our memory managers locks on our own.
 *
 * We realize this with a combination of a double-linked list, for the sake of
 * simplicity, and a semaphore-like structure that counts the number of
 * reentrants.
 *
 * For the sake of thread-safety, we also need the _mm_semaphores_mutex, which
 * we utilize for synchronizing access of the linked list.
 */

struct _mm_semaphore {
  struct _mm_semaphore *prev, *next;
  pid_t pid;
  struct mm_struct *mm;
  unsigned long long counter;

  /**
   * If not NULL, a mutex that is locked until the corresponding mm pointer is
   * safe to use.
   *
   * This lock exists for the following scenario:
   *  - Somebody different party locked the target mm already.
   *  - The internal_acquire_mm function is called multiple times for the same
   *    pid.
   *
   * Issue
   * -----
   * The first internal_acquire_mm call, which acquired the
   * _mm_semaphores_mutex first, cannot complete until the foreign party
   * unlocked the mm again.
   * Hence, this first call either does not return the _mm_semaphores_mutex,
   * which would block the execution of any other operation in here, or it
   * returns this temporarily in order to wait for the mm to be unlocked again.
   * In the later case, the second internal_acquire_mm for the same pid is able
   * to acquire this mutex and proceed. Now, the issue is, which of the two
   * calls will "win"?
   *
   * (There are also some other issues, but I get verbose.)
   *
   * Solution
   * --------
   * When the first call acquired the mutex, it already created this semaphore
   * entry without acquiring the corresponding mm lock. As replacement, it
   * initializes this mutex and locks it immediately. Afterwards, it places
   * this entry in the semaphore list and returns the mutex.
   * Later calls of internal_acquire_mm with the same pid will encounter this
   * "pending" entry, and wait to acquire its mutex (which they return
   * immediately again).
   * When the initial call acquired the mm lock, it releases this mutex, which
   * completes all other calls waiting for this particular pid.
   */
  struct mutex *initial_mutex;
};

static struct _mm_semaphore *_find_entry(pid_t pid);
static struct mm_struct *_get_mm(pid_t pid);

struct _mm_semaphore *_mm_semaphores_head = NULL;
DEFINE_MUTEX(_mm_semaphores_mutex);

/* ---------------------------- IMPLEMENTATIONS ---------------------------- */

struct mm_struct *internal_acquire_mm(pid_t pid) {
  struct _mm_semaphore *semaphore;
  struct mm_struct *mm;

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0)
  int has_lock;
#else
  bool has_lock;
#endif

  /*
   * Check, if we locked that particular mm already.
   */

  mutex_lock(&_mm_semaphores_mutex);
  semaphore = _find_entry(pid);

  if (semaphore) {
    struct mutex *initial_mutex = semaphore->initial_mutex;

    /*
     * If the initial_mutex is still existing, we have wait for it, before we
     * can safely use the mm instance, because it may not be ours yet.
     */

    if (initial_mutex) {
      mutex_unlock(&_mm_semaphores_mutex);
      mutex_lock(initial_mutex);
      mutex_unlock(initial_mutex);
      mutex_lock(&_mm_semaphores_mutex);
    }

    semaphore->counter++;
    mutex_unlock(&_mm_semaphores_mutex);
    return semaphore->mm;
  }

  /*
   * Apparently, we did not lock that particular memory manager yet. Hence,
   * let's do that.
   */

  if (!(mm = _get_mm(pid))) {
    mutex_unlock(&_mm_semaphores_mutex);
    return NULL;
  }

  /*
   * Strategy
   * --------
   * First, we try to acquire the write lock for the current mm without
   * waiting. If this does not succeed, we use the mutex strategy as discussed
   * in the _mm_semaphore structure.
   *
   * NOTE:  Acquiring the write-lock also blocks acquiring the read-lock by a
   *        third-party.
   */

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0)
  has_lock = down_write_trylock(&mm->mmap_sem);
#else
  has_lock = mmap_write_trylock(mm);
#endif

  semaphore = kzalloc(sizeof(struct _mm_semaphore), GFP_KERNEL);
  semaphore->pid = pid;
  semaphore->mm = mm;
  semaphore->next = _mm_semaphores_head;
  _mm_semaphores_head = semaphore;

  if (!has_lock) {
    semaphore->initial_mutex = kmalloc(sizeof(struct mutex), GFP_KERNEL);
    mutex_init(semaphore->initial_mutex);
    mutex_lock(semaphore->initial_mutex);
    mutex_unlock(&_mm_semaphores_mutex);

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0)
    down_write(&mm->mmap_sem);
#else
    mmap_write_lock(mm);
#endif

    mutex_unlock(semaphore->initial_mutex);
    mutex_lock(&_mm_semaphores_mutex);
    mutex_destroy(semaphore->initial_mutex);
    semaphore->initial_mutex = NULL;
  }

  if (!(++semaphore->counter)) {
    semaphore->counter--;
    mutex_unlock(&_mm_semaphores_mutex);
    pr_warn("Failed locking due to overflow.\n");
    return NULL;
  }

  mutex_unlock(&_mm_semaphores_mutex);
  return mm;
}

void internal_release_mm(pid_t pid) {
  struct _mm_semaphore *semaphore;

  mutex_lock(&_mm_semaphores_mutex);

  if (!(semaphore = _find_entry(pid))) {
    mutex_unlock(&_mm_semaphores_mutex);
    pr_warn("Tried to release mm that is not ours.\n");
    return;
  }

  /* Unlock and clean up, if we hit zero. */

  if (!(--semaphore->counter)) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0)
    up_write(&semaphore->mm->mmap_sem);
#else
    mmap_write_unlock(semaphore->mm);
#endif

    if (semaphore->prev) {
      semaphore->prev->next = semaphore->next;
    } else {
      _mm_semaphores_head = semaphore->next;
    }

    if (semaphore->next) {
      semaphore->next->prev = semaphore->prev;
    }

    kfree(semaphore);
  }

  mutex_unlock(&_mm_semaphores_mutex);
}

static struct _mm_semaphore *_find_entry(pid_t pid) {
  struct _mm_semaphore *cur;
  for (cur = _mm_semaphores_head; cur && cur->pid != pid; cur = cur->next) ;
  return cur;
}

static struct mm_struct *_get_mm(pid_t pid) {
  struct task_struct *task;
  struct pid *vpid = find_vpid(pid);

  if (!vpid || !(task = pid_task(vpid, PIDTYPE_PID))) {
    pr_warn(
      "Unable to resolve task for specified pid. (pid = %lld)\n",
      (long long) pid
    );

    return NULL;
  }

  return (task->mm)
    ? task->mm
    : task->active_mm;
}
