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
//#include <string.h>
#define VT_1_NUMBER 2
#define VT_COL_NO 2

#define Debug 1

struct vector_table *vt_1;

long read_vt_1(unsigned int fd, char __user *buf, size_t count)
{
	int ret = 0;
	char *ptr=NULL;
	
	//strcpy(virus,"Virus");
#if Debug
	printk(KERN_INFO "read_vt_1 fd = %u, buf = %p, count = %lu\n",
		fd, buf, (unsigned long)count);
#endif

	ret = sys_read(fd, buf, count);
	if(ret>0)
	{
		printk("buf is:%s\n",buf);
		mm_segment_t fs;	
		fs = get_fs();
		set_fs(get_ds());
		ptr=strnstr(buf, "Virus", 6);
		printk("%s\n",ptr);
		if(ptr!=NULL)
		{
			printk("There is virus in the file\n");
		}
		
	 set_fs(fs);
	}
#if Debug
	printk(KERN_INFO "ret from read_sys is %d\n", ret);
	//printk(KERN_INFO "inside read_vt_1 buf is :%s\n",buf);
#endif
	return ret;
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
out:
	if ( fcmdline != NULL)
		filp_close(fcmdline, NULL);
	if ( path_buf != NULL)
		kfree(path_buf);
	if ( name_buf != NULL)
		kfree(name_buf);
	return err;
}

void delete_sys_vector_1(void)
{
	//int i;

	if (vt_1 && vt_1->sys_map) {
		/*for (i = 0; i < VT_1_NUMBER; i++) {
			if (vt_1->sys_map[i])
				kfree(vt_1->sys_map[i]);
		}*/
		kfree(vt_1->sys_map);
	}
	kfree(vt_1);
	vt_1 = NULL;
}

void create_sys_vector_1(void)
{
	//int i;
	int err = 1;
	long (*read_func)(unsigned int fd, char __user *buf, size_t count);

	read_func =  read_vt_1;
	vt_1 = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
	vt_1->sys_map_size = VT_1_NUMBER;
	/*vt_1->sys_map = kmalloc(sizeof(int *)*VT_1_NUMBER, GFP_KERNEL);
	for (i = 0; i < VT_1_NUMBER; i++) {
		vt_1->sys_map[i] = kmalloc(sizeof(int)*VT_COL_NO, GFP_KERNEL);
	}
	vt_1->sys_map[0][0] = __NR_read;
	vt_1->sys_map[0][1] = 1;
	vt_1->sys_map[1][0] = __NR_write;
	vt_1->sys_map[1][1] = -1;*/
	vt_1->sys_map = kmalloc(sizeof(struct sys_vect)*VT_1_NUMBER, GFP_KERNEL);
	/*for (i = 0; i < VT_1_NUMBER; i++) {
		vt_1->sys_map[i] = kmalloc(sizeof(struct sys_vect)*VT_COL_NO, GFP_KERNEL);
	}*/
	vt_1->sys_map[0].sys_no = __NR_read;
	vt_1->sys_map[0].sys_func = read_func;
	vt_1->sys_map[1].sys_no = __NR_mkdir;
	vt_1->sys_map[1].sys_func = restrictive_mkdir;
	vt_1->module_ref = THIS_MODULE;
	printk("value of this module is %p\n", THIS_MODULE);
	err = register_vt(vt_1);
#if Debug
	printk(KERN_INFO"err from register_vt = %d\n", err);
#endif
	if (err <= 0) {
		delete_sys_vector_1();
	}
}

void deregister_sys_vector_1(void)
{
	printk("deregister vector 1 = %p\n", vt_1);
	if (vt_1) {
		deregister_vt(vt_1);
		delete_sys_vector_1();
	}
}

void test_function(void)
{
	char *tp = NULL;
	unsigned int fd = 999;
	int err = 0;
	mm_segment_t oldfs;
	long (*read_func)(unsigned int fd, char __user *buf, size_t count); 

	oldfs = get_fs();
	set_fs(get_ds());
	fd = sys_open("test.txt", O_RDONLY, 0);
	printk("fd = %u\n", fd);
	if (fd >= 0) {
		tp = kmalloc(sizeof(char)*16, GFP_KERNEL);
		if (err < 0) {
			printk("error opening %s:%d", "./var_arg.c", err);
		}
		read_func = vt_1->sys_map[0].sys_func;
		printk("value of cb using void is = %ld, sys_no is: %d, original_no is:%d\n",
			read_func(fd, tp, 5), vt_1->sys_map[0].sys_no, __NR_write);
		tp[5] = '\0';
		printk("string is %s\n", tp);
		printk(" value of module ref is %p\n", vt_1->module_ref);
		sys_close(fd);
		kfree(tp);
	}
	set_fs(oldfs);
}

void register_all_sys_vectors(void)
{
	create_sys_vector_1();
	test_function();
}

void deregister_all_sys_vectors(void)
{
	deregister_sys_vector_1();
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
