#include <linux/linkage.h>
#include <linux/moduleloader.h>
//#include <clone.h>
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
#include <asm/uaccess.h>
#include <asm/mmu_context.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h> 
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/pid.h>
#include <trace/events/sched.h>
//#include <trace/events/sched.h>
#include<linux/vector_table.h>
#define CREATE_TRACE_POINTS
#include <trace/events/task.h>

/*
 *  * Minimum number of threads to boot the kernel
 *   */
#define MIN_THREADS 20

/*
 *  * Maximum number of threads
 *   */
#define MAX_THREADS FUTEX_TID_MASK

/*

long _do_fork1(unsigned long clone_flags,
               unsigned long stack_start,
               unsigned long stack_size,
               int __user *parent_tidptr,
               int __user *child_tidptr,
               unsigned long tls,int vector_id)
 {
        struct task_struct *p;
        int trace = 0;
   
        long nr;
       

        if (!(clone_flags & CLONE_UNTRACED)) {
                if (clone_flags & CLONE_VFORK)
                        trace = PTRACE_EVENT_VFORK;
                else if ((clone_flags & CSIGNAL) != SIGCHLD)
                        trace = PTRACE_EVENT_CLONE;
                else
                        trace = PTRACE_EVENT_FORK;

                if (likely(!ptrace_event_enabled(current, trace)))
                        trace = 0;
        }

        p = copy_process(clone_flags, stack_start, stack_size,
                         child_tidptr, NULL, trace, tls);



        if (!IS_ERR(p)) {
                struct completion vfork;
                struct pid *pid;
		trace_sched_process_fork(current, p);

                pid = get_task_pid(p, PIDTYPE_PID);
                nr = pid_vnr(pid);

                if (clone_flags & CLONE_PARENT_SETTID)
                        put_user(nr, parent_tidptr);

                if (clone_flags & CLONE_VFORK) {
                        p->vfork_done = &vfork;
                        init_completion(&vfork);
                        get_task_struct(p);
                }

                wake_up_new_task(p);


                if (unlikely(trace))
                        ptrace_event_pid(trace, pid);

                if (clone_flags & CLONE_VFORK) {
                        if (!wait_for_vfork_done(p, &vfork))
                                ptrace_event_pid(PTRACE_EVENT_VFORK_DONE, pid);
                }

                put_pid(pid);
                if ( clone_flags & CLONE_SYSCALL)
                {

                        p->vt=current->vt;
                        if(current->vt!=NULL)
                        {
                                printk("parent vector table is:%d",current->vt->id);
                                printk("New process vector table is:%d",p->vt->id);
                }       }


        } else {
                nr = PTR_ERR(p);
        }
        return nr;
}



long do_fork1(unsigned long clone_flags,
               unsigned long stack_start,
              unsigned long stack_size,
               int __user *parent_tidptr,
               int __user *child_tidptr,int vector_id)
 {
	printk("The vector id is:%d\n",vector_id);
	return _do_fork1(clone_flags, stack_start, stack_size,
                    parent_tidptr, child_tidptr, 0,vector_id);
         
 }

*/
asmlinkage extern long (*sysptr)(unsigned long clone_flags,unsigned long newsp,uintptr_t parent_tidptr,uintptr_t child_tidptr,unsigned long newtls,int vector_id);

asmlinkage long clone2 (unsigned long clone_flags,unsigned long newsp,uintptr_t parent_tidptr,uintptr_t child_tidptr,unsigned long newtls,int vector_id)
{
	pid_t pid;
	int ret;
	struct pid *pid_struct;
	//copy_from_user(parent_tidptr1, parent_tidptr,sizeof(parent_tidptr));
       // copy_from_user(child_tidptr1, child_tidptr,sizeof(child_tidptr));
//	struct task_struct *task;
	struct task_struct *child;
	printk("Inside clone2\n");
        pid= _do_fork(clone_flags, newsp, 0,
                        (int __user *)parent_tidptr, (int __user *)child_tidptr,0);
	printk("Pid is:%d\n",pid);
	pid_struct = find_get_pid(pid);
	child = pid_task(pid_struct,PIDTYPE_PID);
	ret=change_vt(child,vector_id);
	//printk("Pid of child is :%d\n",child->vt->id);
	
	if(child->vt!=NULL)
	{
		printk("Vector id of child is :%d\n",child->vt->id);

	}
	if(ret<0)
	{
		printk("Error while assigning vt:%d\n",ret);
	}
	//child=find_task(find_vpid(pid), PIDTYPE_PID)
	return pid;
}




static int __init init_sys_clone2(void)
{
       printk("installed new sys_clone2 module\n");
       if (sysptr == NULL)
               sysptr = clone2;
       return 0;
}
static void  __exit exit_sys_clone2(void)
{
       if (sysptr != NULL)
               sysptr = NULL;
       printk("removed sys_clone2 module\n");
}
module_init(init_sys_clone2);
module_exit(exit_sys_clone2);
MODULE_LICENSE("GPL");

