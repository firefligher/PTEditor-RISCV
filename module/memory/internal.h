#pragma once

#include "../types.h"

enum internal_release_mm_status {
    INTERNAL_RELEASE_FAILED = 0,
    INTERNAL_RELEASE_WAS_INTERNAL_ONLY,
    INTERNAL_RELEASE_UNLOCKED_GLOBALLY
};

struct mm_struct *internal_acquire_mm(pid_t pid);
enum internal_release_mm_status internal_release_mm(pid_t pid);
