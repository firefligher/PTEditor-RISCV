#pragma once

#include "../../types.h"

/*
 * ============================== COMMAND DEVICE ==============================
 *
 * Internal interface for the device that is utilized for communication between
 * the userland library and this kernel module.
 */

typedef long (*ptedit_command_device_handler_t) (unsigned int, unsigned long);

/**
 * Clears all registered command handlers and releases any potential memory
 * that was allocated for their management.
 *
 * Note that this function is not thread-safe. If required, synchronization has
 * to be enforced by the caller.
 */
void ptedit_command_device_clear_commands(void);

/**
 * Installs the communication interface for the userland library.
 *
 * Calling this function multiple times without uninstalling the device in
 * between will only result in one installation. Later calls will still
 * succeed, but have not any effect.
 *
 * @return  Either PTEDIT_STATUS_SUCCESS, if the device is ready for
 *          communication, otherwise PTEDIT_STATUS_ERROR.
 */
ptedit_status_t ptedit_command_device_install(void);

/**
 * Registers the specified handler for handling the incoming ioctl-calls with
 * the specified cmd_number.
 *
 * Note that registering handlers is independent of whether the command device
 * is initialized or not. Also, the registrations are not cleared when the
 * device's initialization changes.
 *
 * This operation may cause memory allocations that should be removed before
 * this module is removed. Otherwise, memory issues may occur.
 *
 * Also note that this function is not thread-safe. If required,
 * synchronization has to be enforced by the caller.
 *
 * @param cmd_number  The ioctl-number that is associated with the specified
 *                    handler.
 *
 * @param handler     The handler that is registered.
 *
 * @return  Either PTEDIT_STATUS_SUCCESS, if registration succeeded, otherwise
 *          PTEDIT_STATUS_ERROR. In the latter case, the specified cmd_number
 *          is occupied.
 */
ptedit_status_t ptedit_command_device_register_command(
  unsigned int cmd_number,
  const ptedit_command_device_handler_t handler
);

/**
 * Uninstalls the communication interface for the userland library.
 *
 * If the corresponding device has not been installed or has been uninstalled
 * previously already, calling this function has no effect.
 */
void ptedit_command_device_uninstall(void);

/* ============================== UMEM DEVICE ============================== */

/**
 * Installs the unprivileged physical memory access.
 *
 * If this function is called although the access has been established already,
 * the function call has no effect.
 *
 * @return  Either PTEDIT_STATUS_SUCCESS, if the access is ready, otherwise
 *          PTEDIT_STATUS_ERROR.
 */
ptedit_status_t ptedit_umem_device_install(void);

/**
 * Uninstalls the unprivileged physical memory access.
 *
 * If this function is called although there is no access installed, the
 * function call has no effect.
 */
void ptedit_umem_device_uninstall(void);
