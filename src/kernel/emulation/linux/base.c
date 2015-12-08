#define BUILDING_BASE_C
#include "base.h"
#include <asm/unistd.h>
#include "simple.h"
#include "duct_errno.h"
#include "syscalls.h"

#ifdef __x86_64__
__attribute__((naked))
long linux_syscall(long a1, long a2, long a3, long a4, long a5, long a6, int nr)
{
	__asm__ ("movl 8(%rsp), %eax\n"
			"movq %rcx, %r10\n"
			"syscall\n"
			"ret");
}
#elif defined(__i386__)
__attribute__((naked))
long linux_syscall(long a1, long a2, long a3, long a4, long a5, long a6, int nr)
{
	/*
	__asm__("push %%ecx\n"
			"push %%edx\n"
			"push %%ebp\n"
			"movl %%esp, %%ebp\n"
			"sysenter\n"
			"ret\n"
			:: "a"(nr), "b" (a1), "c"(a2), "d"(a3),
				"S"(a4), "D"(a5), "ebp"(a6));
	__builtin_unreachable();
	*/
	__asm__("push %ebp\n"
			"call 1f\n"
			"1:\n"
			"pop %ebp\n"
			"addl $(2f-1b), %ebp\n"
			"push %ebp\n"
			"push %ecx\n"
			"push %edx\n"
			"movl 28+16(%esp), %eax\n"
			"movl 4+16(%esp), %ebx\n"
			"movl 8+16(%esp), %ecx\n"
			"movl 12+16(%esp), %edx\n"
			"movl 16+16(%esp), %esi\n"
			"movl 20+16(%esp), %edi\n"
			"push %ebp\n"
			"movl 24+16(%esp), %ebp\n"
			"movl %ebp, -4(%esp)\n"
			"movl %esp, %ebp\n"
			"sysenter\n"
			"2:\n"
			"pop %ebp\n"
			"ret"
		   );
}
#else
#	error Unsupported platform!
#endif

long __unknown_syscall(int nr, ...)
{
	__simple_printf("Unimplemented syscall (%d)\n", nr);
	return -ENOSYS;
}

int __linux_syscall(int nr, long a1, long a2, long a3, long a4, long a5, long a6)
{
	return linux_syscall(a1, a2, a3, a4, a5, a6, nr);
}

#ifdef __TESTING
void _start()
{
#ifdef __x86_64__
	__asm__ ("andq    $-16, %rsp");
#elif defined(__i386__
	__asm__ ("andl    $-16, %esp");
#endif
	__simple_printf("Hello world\n");
	LINUX_SYSCALL1(__NR_exit, 0);
}
#endif

