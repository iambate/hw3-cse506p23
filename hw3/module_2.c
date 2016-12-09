#include <asm/page.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <stdarg.h>
#include <asm/unistd.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>

#define VT_1_NUMBER 5
#define VT_2_NUMBER 5

#define Debug 0

struct vector_table *vt_1;
struct vector_table *vt_2;

long read_logger(unsigned int fd, char __user *buf, size_t count)
{
	int ret = 0;

#if Debug
	printk(KERN_INFO "read_logger fd = %u, buf = %p, count = %lu\n",
		fd, buf, (unsigned long)count);
#endif

	ret = sys_read(fd, buf, count);
	if (fd > 2)
		printk(KERN_INFO "Logger: READ sys_call with fd = %u,"
			" buf = %p, size = %lu, return value = %d,"
			" by process with pid = %d\n", fd, buf,
			(unsigned long)count, ret, current->pid);
#if Debug
	printk(KERN_INFO "ret from read_sys logger is %d\n", ret);
#endif
	return ret;
}

long write_logger(unsigned int fd, const char __user * buf, size_t count)
{
	int ret = 0;

#if Debug
	printk(KERN_INFO "write_logger fd = %u, buf = %p, count = %lu\n",
		fd, buf, (unsigned long)count);
#endif
	ret = sys_write(fd, buf, count);

	if (fd > 2)
		printk(KERN_INFO "Logger: WRITE sys_call with fd = %u,"
			" buf = %p, size = %lu, return value = %d"
			" by process with pid = %d\n", fd, buf,
			(unsigned long)count, ret, current->pid);

#if Debug
	printk(KERN_INFO "ret from write_sys in write_logger is %d\n", ret);
#endif
	return ret;
}

long mkdir_logger(const char __user * pathname, umode_t mode)
{
	int err = 0;
	char *path = NULL;
	size_t lenstr = strlen_user(pathname);

	if (lenstr > 4096) {
		printk(KERN_ERR "Path Length is greater than Max length");
		err = -EINVAL;
		goto out;
	}

	path = kmalloc(sizeof(char)*lenstr, GFP_KERNEL);
	if (!path) {
		printk(KERN_ERR "kmalloc failed\n");
		err = -ENOMEM;
		goto out;
	}

	if (copy_from_user(path, pathname, lenstr*sizeof(char))) {
		printk(KERN_ERR "error in copy_from_user\n");
		err = -EFAULT;
		goto out;
	}

	err = sys_mkdir(pathname, mode);
out:
	printk(KERN_INFO "Logger: MKDIR sys_call with pathname = %s,"
		" mode = %lu returned with %d by process with pid =%d\n"
		, path, (unsigned long)mode, err, current->pid);

	kfree(path);
	return err;
}

long rmdir_logger(const char __user * pathname)
{
	int err = 0;
	char *path = NULL;
	size_t lenstr = strlen_user(pathname);

	if (lenstr > 4096) {
		printk(KERN_ERR "Path Length is greater than Max length");
		err = -EINVAL;
		goto out;
	}

	path = kmalloc(sizeof(char)*lenstr, GFP_KERNEL);
	if (!path) {
		printk(KERN_ERR "kmalloc failed\n");
		err = -ENOMEM;
		goto out;
	}

	if (copy_from_user(path, pathname, lenstr*sizeof(char))) {
		printk(KERN_ERR "error in copy_from_user\n");
		err = -EFAULT;
		goto out;
	}

	err = sys_rmdir(pathname);
out:
	printk(KERN_INFO "Logger: RMDIR sys_call with pathname = %s,"
		" returned with %d by process with pid = %d\n", path, err,
		current->pid);
	kfree(path);
	return err;
}

void delete_sys_vector(struct vector_table *vt)
{
	if (vt && vt->sys_map)
		kfree(vt->sys_map);
	kfree(vt);
	vt = NULL;
}

void create_sys_vector_1(void)
{
	int err = 1;

	vt_1 = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
	vt_1->sys_map_size = VT_1_NUMBER;
	vt_1->sys_map = kmalloc(sizeof(struct sys_vect)*VT_1_NUMBER, GFP_KERNEL);

	vt_1->sys_map[0].sys_no = __NR_read;
	vt_1->sys_map[0].sys_func = read_logger;
	vt_1->sys_map[1].sys_no = __NR_mkdir;
	vt_1->sys_map[1].sys_func = mkdir_logger;
	vt_1->sys_map[2].sys_no = __NR_link;
	vt_1->sys_map[2].sys_func = NULL;
	vt_1->sys_map[3].sys_no = __NR_rmdir;
	vt_1->sys_map[3].sys_func = rmdir_logger;
	vt_1->sys_map[4].sys_no = __NR_write;
	vt_1->sys_map[4].sys_func = write_logger;

	vt_1->module_ref = THIS_MODULE;
#if Debug
	printk(KERN_INFO "value of this module is %p\n", THIS_MODULE);
#endif
	err = register_vt(vt_1);
#if Debug
	printk(KERN_INFO "err from register_vt = %d\n", err);
#endif
	if (err <= 0) {
		delete_sys_vector(vt_1);
	}
	printk(KERN_INFO "Registering vector 1 =  %p of module 2 "
		"with id = %d\n", vt_1, err);
}

void deregister_sys_vector(struct vector_table *vt)
{
#if Debug
	printk("deregister vector = %p\n", vt);
#endif
	if (vt) {
		deregister_vt(vt);
		delete_sys_vector(vt);
	}
}

void create_sys_vector_2(void)
{
	int err = 1;

	vt_2 = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
	vt_2->sys_map_size = VT_2_NUMBER;
	vt_2->sys_map = kmalloc(sizeof(struct sys_vect)*VT_2_NUMBER, GFP_KERNEL);

	vt_2->sys_map[0].sys_no = __NR_read;
	vt_2->sys_map[0].sys_func = NULL;
	vt_2->sys_map[1].sys_no = __NR_mkdir;
	vt_2->sys_map[1].sys_func = NULL;
	vt_2->sys_map[2].sys_no = __NR_write;
	vt_2->sys_map[2].sys_func = NULL;
	vt_2->sys_map[3].sys_no = __NR_rmdir;
	vt_2->sys_map[3].sys_func = NULL;
	vt_2->sys_map[4].sys_no = __NR_link;
	vt_2->sys_map[4].sys_func = NULL;

	vt_2->module_ref = THIS_MODULE;
#if Debug
	printk("value of this module in vect2 is %p\n", THIS_MODULE);
#endif	
	err = register_vt(vt_2);
#if Debug
	printk(KERN_INFO"err from register_vt2 = %d\n", err);
#endif
	if (err <= 0) {
		delete_sys_vector(vt_2);
	}
	printk(KERN_INFO "Registering vector 2 =  %p of module 2 "
		"with id = %d\n", vt_2, err);
}

void test_function(void)
{
	char *tp = NULL;
	unsigned int fd = 999, fd1 = 999;
	int err = 0;
	mm_segment_t oldfs;
	long (*read_func)(unsigned int fd, char __user *buf, size_t count); 
	long (*write_func)(unsigned int fd, const char __user *buf, size_t count);

	oldfs = get_fs();
	set_fs(get_ds());
	fd = sys_open("test.txt", O_RDONLY, 0);
	fd1 = sys_open("test_1.txt", O_WRONLY, 0);
	printk("fd = %u\n", fd);
	if (fd >= 0) {
		tp = kmalloc(sizeof(char)*16, GFP_KERNEL);
		if (err < 0) {
			printk("error opening %s:%d", "./var_arg.c", err);
		}
		read_func = vt_2->sys_map[0].sys_func;
		printk("value of cb using void is = %ld, sys_no is: %d, original_no is:%d\n",
			read_func(fd, tp, 5), vt_1->sys_map[0].sys_no, __NR_write);
		tp[5] = '\0';
		printk("string is %s\n", tp);
		write_func = vt_2->sys_map[2].sys_func;
		printk("value of write is = %ld, sys_no is: %d, original_no is:%d\n",
			write_func(fd1, tp, 5), vt_2->sys_map[2].sys_no, __NR_write);
		tp[5] = '\0';
		printk("string is %s\n", tp);
		
		printk(" value of module ref is %p\n", vt_2->module_ref);
		sys_close(fd);
		kfree(tp);
	}
	set_fs(oldfs);
}

void register_all_sys_vectors(void)
{
	create_sys_vector_1();
	create_sys_vector_2();
	//test_function();
}

void deregister_all_sys_vectors(void)
{
	deregister_sys_vector(vt_1);
	deregister_sys_vector(vt_2);
}

static int __init init_module_2(void)
{
	printk("installed  module_2\n");
	register_all_sys_vectors();
	return 0;
}
static void  __exit exit_module_2(void)
{
	deregister_all_sys_vectors();
	printk("removed module_2\n");
}
module_init(init_module_2);
module_exit(exit_module_2);
MODULE_LICENSE("GPL");
