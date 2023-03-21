#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/types.h>

#include "../config.h"
#include "userland.h"

/* ------------------- COMMAND-ENTRY-RELATED DEFINITIONS ------------------- */

struct _cmd_entry {
  unsigned int number;
  ptedit_command_device_handler_t handler;
};

static size_t _cmd_find_slot(int cmd_number);

/*
 * _cmd_entries is an array list that is sorted by the number of its entries in
 * ascending order.
 *
 * _cmd_entries_capacity: The allocated capacity of _cmd_entries; unit is
 *                        elements.
 *
 * _cmd_entries_limit:    The number of valid entries in _cmd_entries; unit is
 *                        elements; offset is zero.
 *
 * Intuition: _cmd_entries_limit <= _cmd_entries_capacity
 */

static size_t _cmd_entries_capacity = 0;
static size_t _cmd_entries_limit = 0;
static struct _cmd_entry *_cmd_entries = NULL;

#define _CMD_ENTRIES_INITIAL_SIZE ((size_t) 16)

/* ----------------------- DEVICE-RELATED DEFINITIONS ---------------------- */

enum _device_status {
  _DEVICE_STATUS_UNINITIALIZED = 0,
  _DEVICE_STATUS_AVAILABLE,
  _DEVICE_STATUS_OCCUPIED
};

static long _device_ioctl(
  struct file *file,
  unsigned int ioctl_num,
  unsigned long ioctl_param
);

static int _device_open(struct inode *inode, struct file *file);
static int _device_release(struct inode *inode, struct file *file);

static struct file_operations _device_operations = {
  .open = _device_open,
  .owner = THIS_MODULE,
  .release = _device_release,
  .unlocked_ioctl = _device_ioctl
};

static struct miscdevice _device = {
  .fops = &_device_operations,
  .minor = MISC_DYNAMIC_MINOR,
  .mode = S_IRWXUGO,
  .name = PTEDITOR_DEVICE_NAME
};

/*
 * NOTE:  Since multiple userland processes may open our device concurrently,
 *        but we only allow one client at the same time, we utilize
 *        _device_status as an occupation indicator.
 *        Due to thread-safety, we need the _device_status_mutex in order to
 *        synchronize our accesses to _device_status.
 */

static enum _device_status _device_status = _DEVICE_STATUS_UNINITIALIZED;
DEFINE_MUTEX(_device_status_mutex);

/* ---------------------------- IMPLEMENTATIONS ---------------------------- */

void ptedit_command_device_clear_commands(void) {
  if (_cmd_entries) {
    kfree(_cmd_entries);
    _cmd_entries = NULL;
  }

  _cmd_entries_capacity = 0;
  _cmd_entries_limit = 0;
}

ptedit_status_t ptedit_command_device_install(void) {
  int error;

  mutex_lock(&_device_status_mutex);

  if (_device_status != _DEVICE_STATUS_UNINITIALIZED) {
    mutex_unlock(&_device_status_mutex);
    return PTEDIT_STATUS_SUCCESS;
  }

  if ((error = misc_register(&_device))) {
    pr_alert("Failed installing command device with. (error = %d)\n", error);
    mutex_unlock(&_device_status_mutex);
    return PTEDIT_STATUS_ERROR;
  }

  _device_status = _DEVICE_STATUS_AVAILABLE;
  mutex_unlock(&_device_status_mutex);
  pr_info("Successfully installed command device.\n");

  return PTEDIT_STATUS_SUCCESS;
}

ptedit_status_t ptedit_command_device_register_command(
  unsigned int cmd_number,
  const ptedit_command_device_handler_t handler
) {
  size_t slot_index;

  /*
   * Check, if there is already an entry with the same cmd_number and determine
   * the slot for the new entry.
   */

  slot_index = _cmd_find_slot(cmd_number);

  if (_cmd_entries && _cmd_entries[slot_index].number == cmd_number) {
    pr_warn(
      "Attempted to register command for command device with same cmd_number "
      "multiple times. (cmd_number = %u)\n",
      cmd_number
    );

    return PTEDIT_STATUS_ERROR;
  }

  /* Check, if we have to enlarge _cmd_entries for the next entry. */

  if (!_cmd_entries) {
    _cmd_entries = kmalloc_array(
      _CMD_ENTRIES_INITIAL_SIZE,
      sizeof(struct _cmd_entry),
      GFP_KERNEL
    );

    _cmd_entries_capacity = _CMD_ENTRIES_INITIAL_SIZE;
  }

  if (_cmd_entries_limit == _cmd_entries_capacity) {
    struct _cmd_entry *old_cmd_entries = _cmd_entries;

    _cmd_entries_capacity *= 2;
    _cmd_entries = kmalloc_array(
      _cmd_entries_capacity,
      sizeof(struct _cmd_entry),
      GFP_KERNEL
    );

    memcpy(
      _cmd_entries,
      old_cmd_entries,
      sizeof(struct _cmd_entry) * _cmd_entries_limit
    );

    kfree(old_cmd_entries);
  }

  /*
   * Ensure that the slot that the slot for the new entry is unoccupied, while
   * our sorting-invariant is preserved (aka we may need to move all entries
   * that will follow the new entry by one element unit to the right).
   *
   * NOTE:  We could already consider this when enlaring memory, but this would
   *        increase the code complexity (subjectively), while benefit seems to
   *        be very small due to the rarity of that operation.
   */

   if (slot_index < _cmd_entries_limit) {
    memmove(
      &_cmd_entries[slot_index + 1],
      &_cmd_entries[slot_index],
      _cmd_entries_limit - slot_index
    );
   }

   /* Insert the entry. */

   _cmd_entries[slot_index].number = cmd_number;
   _cmd_entries[slot_index].handler = handler;
   _cmd_entries_limit++;

   return PTEDIT_STATUS_SUCCESS;
}

void ptedit_command_device_uninstall(void) {
  mutex_lock(&_device_status_mutex);

  if (_device_status == _DEVICE_STATUS_UNINITIALIZED) {
    mutex_unlock(&_device_status_mutex);
    return;
  }

  misc_deregister(&_device);
  _device_status = _DEVICE_STATUS_UNINITIALIZED;
  mutex_unlock(&_device_status_mutex);
  pr_info("Successfully uninstalled command device.\n");
}

static size_t _cmd_find_slot(int cmd_number) {
  size_t left = 0, right = 0, cursor;

  /*
   * NOTE:  Since 'right' is unsigned, we need to do a zero-check, before
   *        computing the right boundary. Otherwise, the decrementation may
   *        result in an overflow.
   */

  if (_cmd_entries_limit) {
    right = _cmd_entries_limit - 1;
  }

  for (cursor = 0; left < right; cursor = left + (right - left) / 2) {
    if (_cmd_entries[cursor].number < cmd_number) {
      left = cursor + 1;
      continue;
    }

    if (_cmd_entries[cursor].number > cmd_number) {
      right = (cursor > 0)
        ? cursor - 1
        : 0;

      continue;
    }

    break;
  }

  if (
    cursor == _cmd_entries_limit - 1 &&
    _cmd_entries[cursor].number < cmd_number
  ) {
    return _cmd_entries_limit;
  }

  return cursor;
}

static long _device_ioctl(
  struct file *file,
  unsigned int ioctl_num,
  unsigned long ioctl_param
) {
  size_t slot_index;

  if (!_cmd_entries) {
    goto error_no_entry;
  }

  slot_index = _cmd_find_slot(ioctl_num);

  if (slot_index >= _cmd_entries_limit) {
    goto error_no_entry;
  }

  return _cmd_entries[slot_index].handler(ioctl_num, ioctl_param);

error_no_entry:
  pr_warn(
    "Client specified command with undefined ioctl number. (number = %u) \n",
    ioctl_num
  );

  return -1;
}

static int _device_open(struct inode *inode, struct file *file) {
  mutex_lock(&_device_status_mutex);

  if (_device_status != _DEVICE_STATUS_AVAILABLE) {
    mutex_unlock(&_device_status_mutex);
    pr_info(
      "Attempted to lock command device although it has been locked already.\n"
    );

    return -EBUSY;
  }

  _device_status = _DEVICE_STATUS_OCCUPIED;
  mutex_unlock(&_device_status_mutex);
  pr_info("Client successfully occupied command device.\n");

  return 0;
}

static int _device_release(struct inode *inode, struct file *file) {
  mutex_lock(&_device_status_mutex);
  _device_status = _DEVICE_STATUS_AVAILABLE;
  mutex_unlock(&_device_status_mutex);

  pr_info("Client released command device.\n");

  return 0;
}
