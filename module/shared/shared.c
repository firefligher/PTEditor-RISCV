#include <asm/current.h>
#include <linux/pid.h>
#include <linux/sched.h>

#include "shared.h"

struct mm_struct *ptedit_shared_get_mm(size_t pid) {
  struct task_struct *task;
  struct pid* vpid;

  /* Find mm */
  task = current;
  if(pid != 0) {
    vpid = find_vpid(pid);
    if(!vpid) return NULL;
    task = pid_task(vpid, PIDTYPE_PID);
    if(!task) return NULL;
  }
  if(task->mm) {
      return task->mm;
  } else {
      return task->active_mm;
  }
  return NULL;
}
