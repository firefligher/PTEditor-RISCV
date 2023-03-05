#pragma once

struct mm_struct *internal_acquire_mm(pid_t pid);
void internal_release_mm(pid_t pid);
