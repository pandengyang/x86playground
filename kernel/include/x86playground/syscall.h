#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "x86playground/type.h"

typedef void (*syscall_func)(void *arg1);

#define SYSCALL_NUM 1

#define SYSCALL_NUM_PUTS 0

#endif
