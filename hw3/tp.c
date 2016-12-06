#include <asm/page.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <linux/vector_table.h>
DEFINE_RWLOCK(vt_rwlock);
static struct vector_table vt_head = {
        .id = 0,
        .vt_list = LIST_HEAD_INIT(vt_head.vt_list)
};

static int remove_vt(struct vector_table *vt){
        int err = 0;
        struct list_head *pos, *tmp_lh;
        struct vector_table *tmp_vt;
        if (vt == NULL) {
                return 0;
        }
        write_lock(&vt_rwlock);
        list_for_each_safe(pos, tmp_lh, &(vt_head.vt_list)) {
                tmp_vt = list_entry(pos, struct vector_table, vt_list);
                printk("\ntmp_vt id: %lu\n", tmp_vt->id);
                printk("tmp_vt: %p\n", tmp_vt);
                if (tmp_vt->id == vt->id) {
                        if (atomic64_read(&vt->rc) != 0) {
                                err = -1;
                                goto out;
                        } else {
				list_del(pos);
                                printk("Deleting now\n");
                        }
                }
        }
out:
        write_unlock(&vt_rwlock);
        return err;

}

struct vector_table *vt[7];
static int __init init_sys_mergesort(void)
{
	int i;
	printk("Is implemented: %d\n", is_implemented_by_vt(__NR_read));
	for(i = 0;i < 7; i++){
		vt[i] = kmalloc(sizeof(struct vector_table), GFP_KERNEL);
		vt[i]->id = i;
	}
	printk("installed new sys_mergesort module\n");
	if (current->vt == NULL)
		printk("VT is null\n");
	for(i = 0;i < 7; i++){
		write_lock(&vt_rwlock);
		list_add(&(vt[i]->vt_list), &(vt_head.vt_list));
		write_unlock(&vt_rwlock);
		//rc = register_vt(vt[i]);
		//printk("register_vt returned: %d\n", rc);
	}
	return 0;
}
static void  __exit exit_sys_mergesort(void)
{
	int i;
	for(i = 0;i < 7; i++){
		//rc = deregister_vt(vt[i]);
		//printk("deregister_vt returned: %d\n", rc);
		remove_vt(vt[i]);
		kfree(vt[i]);
	}
	printk("removed sys_merge module\n");
}
module_init(init_sys_mergesort);
module_exit(exit_sys_mergesort);
MODULE_LICENSE("GPL");
