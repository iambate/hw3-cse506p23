#include <asm/page.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <linux/vector_table.h>

struct vector_table *vt[7];
static int __init init_sys_mergesort(void)
{
	int rc, i;
	struct vector_table *tmp_vt;
	printk("Is implemented: %d\n", is_implemented_by_vt(__NR_read));
	for(i = 0;i < 7; i++){
		vt[i] = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
		vt[i]->id = i;
	}
	printk("installed new sys_mergesort module\n");
	if (current->vt == NULL)
		printk("VT is null\n");
	for(i = 0;i < 7; i++){
		rc = register_vt(vt[i]);
		printk("register_vt returned: %d\n id: %lu\n", rc, vt[i]->id);
		tmp_vt = get_vt(rc);
		if (tmp_vt != NULL && tmp_vt->id == rc)
			printk("VT found");
	}
	return 0;
}
static void  __exit exit_sys_mergesort(void)
{
	int i, rc;
	for(i = 0;i < 7; i++){
		//rc = deregister_vt(vt[i]);
		//printk("deregister_vt returned: %d\n", rc);
		rc = deregister_vt(vt[i]);
		printk("deregister_vt returned: %d\n", rc);
		kfree(vt[i]);
	}
	printk("removed sys_merge module\n");
}
module_init(init_sys_mergesort);
module_exit(exit_sys_mergesort);
MODULE_LICENSE("GPL");
