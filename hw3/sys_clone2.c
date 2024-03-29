#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include<linux/sched.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/unistd.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/completion.h>
#include <linux/personality.h>
#include <linux/mempolicy.h>
#include <linux/sem.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/iocontext.h>
#include <linux/key.h>
#include <linux/binfmts.h>
#include <linux/mman.h>
#include <linux/mmu_notifier.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/vmacache.h>
#include <linux/nsproxy.h>
#include <linux/capability.h>
#include <linux/cpu.h>
#include <linux/cgroup.h>
#include <linux/security.h>
#include <linux/hugetlb.h>
#include <linux/seccomp.h>
#include <linux/swap.h>
#include <linux/syscalls.h>
#include <linux/jiffies.h>
#include <linux/futex.h>
#include <linux/compat.h>
#include <linux/kthread.h>
#include <linux/task_io_accounting_ops.h>
#include <linux/rcupdate.h>
#include <linux/ptrace.h>
#include <linux/mount.h>
#include <linux/audit.h>
#include <linux/memcontrol.h>
#include <linux/ftrace.h>
#include <linux/proc_fs.h>
#include <linux/profile.h>
#include <linux/rmap.h>
#include <linux/ksm.h>
#include <linux/acct.h>
#include <linux/tsacct_kern.h>
#include <linux/cn_proc.h>
#include <linux/freezer.h>
#include <linux/delayacct.h>
#include <linux/taskstats_kern.h>
#include <linux/random.h>
#include <linux/tty.h>
#include <linux/blkdev.h>
#include <linux/fs_struct.h>
#include <linux/magic.h>
#include <linux/perf_event.h>
#include <linux/posix-timers.h>
#include <linux/user-return-notifier.h>
#include <linux/oom.h>
#include <linux/khugepaged.h>
#include <linux/signalfd.h>
#include <linux/uprobes.h>
#include <linux/aio.h>
#include <linux/compiler.h>
#include <linux/sysctl.h>
#include <linux/kcov.h>
#include<linux/kernel.h>
#include<linux/sched.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <asm/mmu_context.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/pid.h>
#include <trace/events/sched.h>
#include <linux/vector_table.h>
#define CREATE_TRACE_POINTS
#include <trace/events/task.h>
asmlinkage extern long (*sysptr)
		(unsigned long clone_flags, unsigned long newsp,
		uintptr_t parent_tidptr, uintptr_t child_tidptr,
		unsigned long newtls, int vector_id);

asmlinkage long clone2(unsigned long clone_flags,
			unsigned long newsp, uintptr_t parent_tidptr,
			uintptr_t child_tidptr, unsigned long newtls,
			int vector_id)
{
	pid_t pid;
	int ret;
	struct pid *pid_struct;
	struct task_struct *child;

	pid = _do_fork(clone_flags, newsp, 0,
			(int __user *)parent_tidptr,
			(int __user *)child_tidptr,
				0);
	pid_struct = find_get_pid(pid);
	child = pid_task(pid_struct, PIDTYPE_PID);
	ret = change_vt(child, vector_id);
	if (ret < 0)
		printk(KERN_INFO "Error while assigning vt:%d\n", ret);
	return pid;
}




static int __init init_sys_clone2(void)
{
	printk(KERN_INFO "installed new sys_clone2 module\n");
	if (sysptr == NULL)
		sysptr = clone2;
	return 0;
}
static void  __exit exit_sys_clone2(void)
{
	if (sysptr != NULL)
		sysptr = NULL;
	printk(KERN_INFO "removed sys_clone2 module\n");
}
module_init(init_sys_clone2);
module_exit(exit_sys_clone2);
MODULE_LICENSE("GPL");

