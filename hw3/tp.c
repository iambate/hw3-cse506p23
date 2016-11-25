#include <asm/page.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <linux/vector_table.h>

struct vector_table *vt, *vt2;
static int __init init_sys_mergesort(void)
{
	int rc;
	vt = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
	printk("installed new sys_mergesort module\n");
	if (current->vt == NULL)
		printk("VT is null");
	rc = register_vt(vt);
	printk("register_vt returned: %d", rc);
	return 0;
}
static void  __exit exit_sys_mergesort(void)
{
	deregister_vt(vt);
	printk("removed sys_merge module\n");
}
module_init(init_sys_mergesort);
module_exit(exit_sys_mergesort);
MODULE_LICENSE("GPL");
