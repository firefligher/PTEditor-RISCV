#pragma once

/* ======================== COMMAND DEVICE HANDLERS ======================== */

/*
long ptedit_command_get_page_size(
  unsigned int ioctl_num,
  unsigned long ioctl_param
);

long ptedit_command_get_pat(
  unsigned int ioctl_num,
  unsigned long ioctl_param
);

long ptedit_command_get_root(
  unsigned int ioctl_num,
  unsigned long ioctl_param
);

long ptedit_command_read_page(
  unsigned int ioctl_num,
  unsigned long ioctl_param
);

long ptedit_command_set_pat(
  unsigned int ioctl_num,
  unsigned long ioctl_param
);

long ptedit_command_set_root(
  unsigned int ioctl_num,
  unsigned long ioctl_param
);

long ptedit_command_switch_tlb_invalidation(
  unsigned int ioctl_num,
  unsigned long ioctl_param
);
*/

long ptedit_command_vm_lock(
  unsigned int ioctl_num,
  unsigned long ioctl_param
);

long ptedit_command_vm_resolve(
  unsigned int ioctl_num,
  unsigned long ioctl_param
);

long ptedit_command_vm_unlock(
  unsigned int ioctl_num,
  unsigned long ioctl_param
);

/*
long ptedit_command_vm_update(
  unsigned int ioctl_num,
  unsigned long ioctl_param
);

long ptedit_command_write_page(
  unsigned int ioctl_num,
  unsigned long ioctl_param
);
*/
