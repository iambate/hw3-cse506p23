#include <linux/types.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/rwlock_types.h>
#include <linux/vector_table.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/export.h>

DEFINE_RWLOCK(vt_rwlock);
static struct vector_table vt_head = {
	.id = 0,
	.vt_list = LIST_HEAD_INIT(vt_head.vt_list)
};

/*
 * return 0 or negative number on error
 * else return the unique vector ID
 */
int register_vt (struct vector_table *vt)
{
	static unsigned long counter = 1;
	if (vt == NULL) {
		return 0;
	}
	write_lock(&vt_rwlock);
	vt->id = counter++;
	list_add(&(vt->vt_list), &(vt_head.vt_list));
	write_unlock(&vt_rwlock);
	return vt->id;
}

EXPORT_SYMBOL(register_vt);

/*
 * return 0 or negative number on error
 * 	-1: still is use
 * else return the unique vector ID of the vector to be deleted
 */
int deregister_vt (struct vector_table *vt)
{
	int err = 0;
	struct list_head *pos, *tmp_lh;
	struct vector_table *tmp_vt;
	if (vt == NULL) {
		return 0;
	}
	write_lock(&vt_rwlock);
	list_del(&vt->vt_list);
	/*
	list_for_each_safe(pos, tmp_lh, &(vt_head.vt_list)) {
		tmp_vt = list_entry(pos, struct vector_table, vt_list);
		printk("\ntmp_vt id: %lu\n", tmp_vt->id);
		printk("tmp_vt: %p\n", tmp_vt);
		if (tmp_vt->id == vt->id) {
			if (atomic64_read(&vt->rc) != 0) {
				err = -1;
				goto out;
			} else {
				printk("Deleting now\n");
			}
		}
	}*/
out:
	write_unlock(&vt_rwlock);
	return err;
}
EXPORT_SYMBOL(deregister_vt);

/*
 * return 0 or negative number on error
 * 	-1: still is use
 * else return the unique vector ID of the vector to be deleted
 */
int deregister_vt_id (int vt_id)
{
	int err = 0;
	struct list_head *pos, *tmp_lh;
	struct vector_table *tmp_vt;
	if (vt_id < 1) {
		return 0;
	}
	write_lock(&vt_rwlock);
	list_for_each_safe(pos, tmp_lh, &(vt_head.vt_list)) {
		tmp_vt = list_entry(pos, struct vector_table, vt_list);
		if (tmp_vt->id == vt_id) {
			if (atomic64_read(&tmp_vt->rc) != 0) {
				err = -1;
				goto out;
			} else {
				list_del(&tmp_vt->vt_list);
				break;
			}
		}
	}
out:
	write_unlock(&vt_rwlock);
	return err;
}
EXPORT_SYMBOL(deregister_vt_id);

/*
 * Tells is a syscall is implemented by current's vector table
 * 1 : default
 * 0 : call the vector table's callback function
 * -1: not implemented sys_call
 */
int is_implemented_by_vt (int sys_call_no){
	int **sys_map, sys_map_size, i;
	if (current->vt != NULL) {
		sys_map = current->vt->sys_map;
		sys_map_size = current->vt->sys_map_size;
		for (i = 0; i < sys_map_size; i++) {
			if (sys_call_no == sys_map[i][0]) {
				return sys_map[i][1];
			}
		}
	}
	return 1;
}
EXPORT_SYMBOL(is_implemented_by_vt);
