#include <asm/page.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <linux/vector_table.h>

struct vector_table *vt, *vt2, *vt3, *vt4, *vt5, *vt6, *vt7;
static int __init init_sys_mergesort(void)
{
	int rc;
	vt = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
	vt2 = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
	vt3 = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
	vt4 = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
	vt5 = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
	vt6 = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
	vt7 = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
	printk("installed new sys_mergesort module\n");
	if (current->vt == NULL)
		printk("VT is null");
	rc = register_vt(vt);
	rc = register_vt(vt2);
	rc = register_vt(vt3);
	rc = register_vt(vt4);
	rc = register_vt(vt5);
	rc = register_vt(vt6);
	rc = register_vt(vt7);
	printk("register_vt returned: %d", rc);
	return 0;
}
static void  __exit exit_sys_mergesort(void)
{
	deregister_vt(vt);
	deregister_vt(vt2);
	deregister_vt(vt3);
	deregister_vt(vt4);
	deregister_vt(vt5);
	deregister_vt(vt6);
	deregister_vt(vt7);
	kfree(vt);
	kfree(vt2);
	kfree(vt3);
	kfree(vt4);
	kfree(vt5);
	kfree(vt6);
	kfree(vt7);
	printk("removed sys_merge module\n");
}
module_init(init_sys_mergesort);
module_exit(exit_sys_mergesort);
MODULE_LICENSE("GPL");
