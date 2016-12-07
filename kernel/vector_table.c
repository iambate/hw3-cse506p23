#include <asm-generic/uaccess.h>
#include <linux/types.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/rwlock_types.h>
#include <linux/vector_table.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/export.h>
#include <linux/vmalloc.h>
#include <linux/limits.h>
#include <linux/mm.h>

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

	if (vt_head.id != 0) {
		printk("ID of head is not zero\n");
		return 0;
	}
	atomic64_set(&(vt->rc), 0);
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
	list_for_each_safe(pos, tmp_lh, &(vt_head.vt_list)) {
		tmp_vt = list_entry(pos, struct vector_table, vt_list);
		if (tmp_vt->id == vt->id) {
			if (atomic64_read(&vt->rc) != 0) {
				err = -1;
				goto out;
			} else {
				list_del(&vt->vt_list);
				printk("Deleting now\n");
			}
		}
	}
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
				list_del(pos);
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
 * INT_MAX : default
 * Any Positive Number : call the vector table's callback function
 * Any Negative Number : not implemented sys_call/error
 */
int is_implemented_by_vt (int sys_call_no){
	struct sys_vect *map = NULL;
	int sys_map_size, i, err = INT_MAX;
	if (current->vt != NULL && current->vt->sys_map != NULL) {
		map = current->vt->sys_map;
		sys_map_size = current->vt->sys_map_size;
		for (i = 0; i < sys_map_size; i++) {
			if (sys_call_no == map[i].sys_no) {
				if (map[i].sys_func == NULL) {
					err = -ENOTSUPP;
					break;
				} else if (map[i].sys_func) {
					err = i;
					break;
				} else {
					err = -EFAULT;
					break;
				}
			}
		}
	}
	return err;
}
EXPORT_SYMBOL(is_implemented_by_vt);

/*
 * return NULL on error
 * else return the unique vector having the vector ID
 */
struct vector_table *get_vt (int vt_id)
{
	struct vector_table *tmp_vt, *rc_vt = NULL;
	if (vt_id < 1) {
		return NULL;
	}
	read_lock(&vt_rwlock);
	list_for_each_entry(tmp_vt, &(vt_head.vt_list), vt_list) {
		if (tmp_vt->id == vt_id) {
			rc_vt = tmp_vt;
		}
	}
	read_unlock(&vt_rwlock);
	return rc_vt;
}
EXPORT_SYMBOL(get_vt);


/*
 * return a user array for ioctl of maximum 1024 length
 */
struct vt_id_list *get_vt_id_list(void)
{
	/*
	 * vt_ids can be max 1024
	 */
	int count = 0, retval = 0, *vt_ids = NULL;
	struct vt_id_list *_vt_id_list = NULL;
	struct vector_table *tmp_vt;
	vt_ids = kmalloc(sizeof(int)*1024, GFP_KERNEL);
	if (vt_ids == NULL) {
		retval = -ENOMEM;
		goto err;
	}
	_vt_id_list = kmalloc(sizeof(struct vt_id_list), GFP_KERNEL);
	if (_vt_id_list == NULL) {
		retval = -ENOMEM;
		goto err;
	}

	read_lock(&vt_rwlock);
	list_for_each_entry(tmp_vt, &(vt_head.vt_list), vt_list) {
		vt_ids[count] = tmp_vt->id;
		count ++;
		if (count >= 1024)
			break;
	}
	read_unlock(&vt_rwlock);

	if (count == 0) {
		kfree(vt_ids);
		goto out;
	}
	_vt_id_list->vt_ids = vt_ids;

out:
	_vt_id_list->vt_ids_count = count;
	return _vt_id_list;
err:
	if (vt_ids != NULL) {
		kfree(vt_ids);
	}
	return ERR_PTR(retval);
}
EXPORT_SYMBOL(get_vt_id_list);


/*
 * Changes the vt of process from ts->vt to vt having to_vt_id
 */
int change_vt ( struct task_struct *ts, int to_vt_id)
{
	struct vector_table *tmp_vt, *from_vt = NULL, *to_vt = NULL;
	int from_vt_id = 0, rc = 0;
	write_lock(&vt_rwlock);
	if (ts->vt != NULL) {
		from_vt_id = ts->vt->id;
	}
	list_for_each_entry(tmp_vt, &(vt_head.vt_list), vt_list) {
		if (tmp_vt->id == from_vt_id) {
			from_vt = tmp_vt;
		} else if (tmp_vt->id == to_vt_id) {
			to_vt = tmp_vt;
		}
	}
	if (to_vt == NULL) {
		rc = -EINVAL;
		goto out;
	}
	if (from_vt != NULL) {
		atomic64_dec(&from_vt->rc);
	}
	atomic64_inc(&to_vt->rc);
	ts->vt = to_vt;
out:
	write_unlock(&vt_rwlock);
	return rc;
}
EXPORT_SYMBOL(change_vt);
