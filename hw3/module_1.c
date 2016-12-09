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

int EndsWith(const char *str, const char *suffix)
{
    	size_t lenstr;
	size_t lensuffix;

	if (!str || !suffix)
		return 0;
	lenstr = strlen(str);
	lensuffix = strlen(suffix);
	if (lensuffix >  lenstr)
	 	return 0;
	return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

long protected_unlink(const char __user *pathname)
{
	int ret;
	char *path = NULL;
	size_t lenstr = strlen_user(pathname);

	if (lenstr > 4096) {
		printk(KERN_ERR "Path Length is greater than Max length");
		ret = -EINVAL;
		goto out;
	}

	path = kmalloc(sizeof(char)*lenstr, GFP_KERNEL);
	if (!path) {
		printk(KERN_ERR "kmalloc failed\n");
		ret = -ENOMEM;
		goto out;
	}

	if (copy_from_user(path, pathname, lenstr*sizeof(char))) {
		printk(KERN_ERR "error in copy_from_user\n");
		ret = -EFAULT;
		goto out;
	}

	if(EndsWith(path, ".protected"))
	{
		printk(KERN_ERR "Operation not permitted\n");
		ret = -EINVAL;
	}
	else
	{
		ret = sys_unlink(pathname);
	}
	printk(KERN_INFO "Protected_Unlink returns with %d\n", ret);
out:
	kfree(path);
	return ret;
}

long verify_read_6(unsigned int fd, char __user *buf, size_t count)
{
	int ret = 0;
	char *ptr = NULL;
	char *buf_6 = NULL;

#if Debug
	printk(KERN_INFO "verify_read_6 fd = %u, buf = %p, count = %lu\n",
		fd, buf, (unsigned long)count);
#endif

	ret = sys_read(fd, buf, count);
	if(ret > 0 && fd > 2)
	{
		buf_6 = kmalloc(sizeof(char)*6, GFP_KERNEL);
		if (!buf_6) {
			printk(KERN_ERR "kmalloc failed\n");
			ret = -ENOMEM;
			goto out;
		}

		if (copy_from_user(buf_6, buf, 6*sizeof(char))) {
			printk(KERN_ERR "error in copy_from_user\n");
			ret = -EFAULT;
			goto out;
		}
		
		ptr = strnstr(buf_6, "Virus", 6);
#if Debug
		printk("%s\n",ptr);
#endif
		if(ptr != NULL)
		{
			printk(KERN_ERR "There is virus in the file\n");
		}
out:
		kfree(buf_6);
	}
#if Debug
	printk(KERN_INFO "ret from verify_read_6 is %d\n", ret);
#endif
	return ret;
}

long restrictive_read(unsigned int fd, char __user *buf, size_t count)
{
	char *path_buf = NULL, *name_buf = NULL;
	struct file *fcmdline = NULL;
	int err = 0, readnums;

#if Debug
	printk(KERN_INFO "restrictive_read fd = %u, buf = %p, count = %lu\n",
		fd, buf, (unsigned long)count);
#endif
	if (fd < 3)
		goto direct;

	path_buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (path_buf == NULL) {
		err = -ENOMEM;
		goto out;
	}
	sprintf(path_buf, "/proc/%d/cmdline", current->pid);
	name_buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (name_buf == NULL) {
		err = -ENOMEM;
		goto out;
	}
	fcmdline = filp_open(path_buf, O_RDONLY, 0);
	if (IS_ERR(fcmdline)) {
		err = PTR_ERR(fcmdline);
		goto out;
	}
	readnums = kernel_read(fcmdline, 0, name_buf, PAGE_SIZE);
	if (readnums <= 0) {
		err = readnums;
		goto out;
	}
	if (strstr(name_buf, "httpd") != NULL) {
		err = -ENOTSUPP;
		goto out;
	}
direct:
	err = sys_read(fd, buf, count);
#if Debug
	printk(KERN_INFO "RESTRICTIVE_READ sys_call with fd = %u,"
		" buf = %p, size = %lu, return value = %d",
		fd, buf, (unsigned long)count, err);
#endif
out:
	if ( fcmdline != NULL)
		filp_close(fcmdline, NULL);
	if ( path_buf != NULL)
		kfree(path_buf);
	if ( name_buf != NULL)
		kfree(name_buf);

#if Debug
	printk(KERN_INFO "ret from read in restrictive_read is %d\n", err);
#endif
	return err;
}

long restrictive_write(unsigned int fd, const char __user * buf, size_t count)
{
	char *path_buf = NULL, *name_buf = NULL;
	struct file *fcmdline = NULL;
	int err = 0, readnums;

#if Debug
	printk(KERN_INFO "restrictive_write fd = %u, buf = %p, count = %lu\n",
		fd, buf, (unsigned long)count);
#endif
	if (fd < 3)
		goto direct;
	path_buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (path_buf == NULL) {
		err = -ENOMEM;
		goto out;
	}
	sprintf(path_buf, "/proc/%d/cmdline", current->pid);
	name_buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (name_buf == NULL) {
		err = -ENOMEM;
		goto out;
	}
	fcmdline = filp_open(path_buf, O_RDONLY, 0);
	if (IS_ERR(fcmdline)) {
		err = PTR_ERR(fcmdline);
		goto out;
	}
	readnums = kernel_read(fcmdline, 0, name_buf, PAGE_SIZE);
	if (readnums <= 0) {
		err = readnums;
		goto out;
	}
	if (strstr(name_buf, "httpd") != NULL) {
		err = -ENOTSUPP;
		goto out;
	}
direct:
	err = sys_write(fd, buf, count);
#if Debug
	printk(KERN_INFO "RESTRICTIVE_WRITE sys_call with fd = %u,"
		" buf = %p, size = %lu, return value = %d",
		fd, buf, (unsigned long)count, err);
	printk(KERN_INFO "ret in restrictive_write is %d\n", err);
#endif
out:
	if ( fcmdline != NULL)
		filp_close(fcmdline, NULL);
	if ( path_buf != NULL)
		kfree(path_buf);
	if ( name_buf != NULL)
		kfree(name_buf);
	return err;
}

long restrictive_mkdir(const char __user * pathname, umode_t mode)
{
	char *path_buf = NULL, *name_buf = NULL;
	struct file *fcmdline = NULL;
	int err = 0, readnums;

	path_buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (path_buf == NULL) {
		err = -ENOMEM;
		goto out;
	}
	sprintf(path_buf, "/proc/%d/cmdline", current->pid);
	name_buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (name_buf == NULL) {
		err = -ENOMEM;
		goto out;
	}
	fcmdline = filp_open(path_buf, O_RDONLY, 0);
	if (IS_ERR(fcmdline)) {
		err = PTR_ERR(fcmdline);
		goto out;
	}
	readnums = kernel_read(fcmdline, 0, name_buf, PAGE_SIZE);
	if (readnums <= 0) {
		err = readnums;
		goto out;
	}
	if (strstr(name_buf, "httpd") != NULL) {
		err = -ENOTSUPP;
		goto out;
	}
	err = sys_mkdir(pathname, mode);
#if Debug
	printk(KERN_INFO "RESTRICTIVE_MKDIR sys_call with pathname = %s,"
		" mode = %lu returned with %d\n", pathname, (unsigned long)mode, err);
#endif
out:
	if ( fcmdline != NULL)
		filp_close(fcmdline, NULL);
	if ( path_buf != NULL)
		kfree(path_buf);
	if ( name_buf != NULL)
		kfree(name_buf);
	return err;
}

long restrictive_rmdir(const char __user * pathname)
{
	char *path_buf = NULL, *name_buf = NULL;
	struct file *fcmdline = NULL;
	int err = 0, readnums;
	path_buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (path_buf == NULL) {
		err = -ENOMEM;
		goto out;
	}
	sprintf(path_buf, "/proc/%d/cmdline", current->pid);
	name_buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (name_buf == NULL) {
		err = -ENOMEM;
		goto out;
	}
	fcmdline = filp_open(path_buf, O_RDONLY, 0);
	if (IS_ERR(fcmdline)) {
		err = PTR_ERR(fcmdline);
		goto out;
	}
	readnums = kernel_read(fcmdline, 0, name_buf, PAGE_SIZE);
	if (readnums <= 0) {
		err = readnums;
		goto out;
	}
	if (strstr(name_buf, "httpd") != NULL) {
		err = -ENOTSUPP;
		goto out;
	}
	err = sys_rmdir(pathname);
#if Debug
	printk(KERN_INFO "RESTRICTIVE_RMDIR sys_call with pathname = %s,"
		" returned with %d\n", pathname, err);
#endif
out:
	if ( fcmdline != NULL)
		filp_close(fcmdline, NULL);
	if ( path_buf != NULL)
		kfree(path_buf);
	if ( name_buf != NULL)
		kfree(name_buf);
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
	if (!vt_1) {
		printk(KERN_ERR "kmalloc failed\n");
		err = -ENOMEM;
		goto out;
	}

	vt_1->sys_map_size = VT_1_NUMBER;
	vt_1->sys_map = kmalloc(sizeof(struct sys_vect)*VT_1_NUMBER, GFP_KERNEL);
	if (!vt_1->sys_map) {
		printk(KERN_ERR "kmalloc failed\n");
		err = -ENOMEM;
		goto out;
	}

	vt_1->sys_map[0].sys_no = __NR_read;
	vt_1->sys_map[0].sys_func = restrictive_read;
	vt_1->sys_map[1].sys_no = __NR_mkdir;
	vt_1->sys_map[1].sys_func = restrictive_mkdir;
	vt_1->sys_map[2].sys_no = __NR_write;
	vt_1->sys_map[2].sys_func = restrictive_write;
	vt_1->sys_map[3].sys_no = __NR_rmdir;
	vt_1->sys_map[3].sys_func = restrictive_rmdir;
	vt_1->sys_map[4].sys_no = __NR_unlink;
	vt_1->sys_map[4].sys_func = protected_unlink;

	vt_1->module_ref = THIS_MODULE;
#if Debug
	printk("value of this module is %p\n", THIS_MODULE);
#endif
	err = register_vt(vt_1);
out:
#if Debug
	printk(KERN_INFO"err from register_vt = %d\n", err);
#endif
	if (err <= 0 && vt_1) {
		delete_sys_vector(vt_1);
	}
	printk(KERN_INFO "Registering vector 1 =  %p of module 1 "
		"with id = %d\n", vt_1, err);
}

void deregister_sys_vector(struct vector_table *vt)
{
	printk("deregister vector in module 1 = %p\n", vt);
	if (vt) {
		deregister_vt(vt);
		delete_sys_vector(vt);
	}
}

void create_sys_vector_2(void)
{
	int err = 1;

	vt_2 = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
	if (!vt_2) {
		printk(KERN_ERR "kmalloc failed\n");
		err = -ENOMEM;
		goto out;
	}

	vt_2->sys_map_size = VT_2_NUMBER;
	vt_2->sys_map = kmalloc(sizeof(struct sys_vect)*VT_2_NUMBER, GFP_KERNEL);
	if (!vt_2->sys_map) {
		printk(KERN_ERR "kmalloc failed\n");
		err = -ENOMEM;
		goto out;
	}

	vt_2->sys_map[0].sys_no = __NR_read;
	vt_2->sys_map[0].sys_func = verify_read_6;
	vt_2->sys_map[1].sys_no = __NR_unlink;
	vt_2->sys_map[1].sys_func = NULL;
	vt_2->sys_map[2].sys_no = __NR_write;
	vt_2->sys_map[2].sys_func = restrictive_write;
	vt_2->sys_map[3].sys_no = __NR_rmdir;
	vt_2->sys_map[3].sys_func = restrictive_mkdir;
	vt_2->sys_map[4].sys_no = __NR_link;
	vt_2->sys_map[4].sys_func = NULL;

	vt_2->module_ref = THIS_MODULE;
#if Debug
	printk("value of this module in vect2 is %p\n", THIS_MODULE);
#endif	
	err = register_vt(vt_2);
out:
#if Debug
	printk(KERN_INFO"err from register_vt2 = %d\n", err);
#endif
	if (err <= 0 && vt_2 ) {
		delete_sys_vector(vt_2);
	}
	printk(KERN_INFO "Registering vector 2 =  %p of module 1 "
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

static int __init init_module_1(void)
{
	printk("installed  module_1\n");
	register_all_sys_vectors();
	return 0;
}
static void  __exit exit_module_1(void)
{
	deregister_all_sys_vectors();
	printk("removed module_1\n");
}
module_init(init_module_1);
module_exit(exit_module_1);
MODULE_LICENSE("GPL");
