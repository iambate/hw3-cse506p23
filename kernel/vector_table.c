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
#include <linux/module.h>

DECLARE_RWSEM(vt_rwlock);
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
	down_write(&vt_rwlock);
	vt->id = counter++;
	list_add(&(vt->vt_list), &(vt_head.vt_list));
	up_write(&vt_rwlock);
	return vt->id;
}
EXPORT_SYMBOL(register_vt);

long sys_getvtbyid(void)
{

        if(current->vt==NULL)
                return 0;
        else
                return current->vt->id;

}

EXPORT_SYMBOL(sys_getvtbyid);

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
	down_write(&vt_rwlock);
	list_for_each_safe(pos, tmp_lh, &(vt_head.vt_list)) {
		tmp_vt = list_entry(pos, struct vector_table, vt_list);
		if (tmp_vt->id == vt->id) {
			if (atomic64_read(&vt->rc) != 0) {
				printk("RC not zero: not deleting\n");
				err = -1;
				goto out;
			} else {
				list_del(&vt->vt_list);
				printk("Deleting now\n");
			}
		}
	}
out:
	up_write(&vt_rwlock);
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
	down_write(&vt_rwlock);
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
	up_write(&vt_rwlock);
	return err;
}
EXPORT_SYMBOL(deregister_vt_id);

/*
 * Tells if a syscall is implemented by current's vector table
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
	down_read(&vt_rwlock);
	list_for_each_entry(tmp_vt, &(vt_head.vt_list), vt_list) {
		if (tmp_vt->id == vt_id) {
			rc_vt = tmp_vt;
		}
	}
	up_read(&vt_rwlock);
	return rc_vt;
}
EXPORT_SYMBOL(get_vt);


/*
 * return a user array for ioctl of maximum 1024 length
 */
int get_vt_id_list(struct vt_id_list *_vt_id_list)
{
	/*
	 * vt_ids can be max 1024
	 */
	char *vt_ids = NULL, *tmp = NULL;
	int retval = 0, count = 0, tmp_len = 0, vt_ids_len = 0;
	struct vector_table *tmp_vt;
	vt_ids = kmalloc(_vt_id_list->vt_ids_info_len, GFP_KERNEL);
	if (vt_ids == NULL) {
		retval = -ENOMEM;
		goto err;
	}
	memset(vt_ids, 0, _vt_id_list->vt_ids_count);

	tmp = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (tmp == NULL) {
		retval = -ENOMEM;
		goto err;
	}
	memset(tmp, 0, PAGE_SIZE);

	down_read(&vt_rwlock);
	list_for_each_entry(tmp_vt, &(vt_head.vt_list), vt_list) {
		count++;
		if (tmp_len + vt_ids_len +1 < _vt_id_list->vt_ids_info_len) {
			sprintf(tmp,"Vector ID: %d and reference count: %ld\n",
				tmp_vt->id, atomic64_read(&tmp_vt->rc));
			tmp_len = strlen(tmp);
			vt_ids_len += tmp_len;
			strcat(vt_ids, tmp);
		}
	}
	up_read(&vt_rwlock);

	if (count == 0) {
		goto out;
	}
	copy_to_user(_vt_id_list->vt_ids_info, vt_ids, vt_ids_len+1);
	_vt_id_list->vt_ids_info_len = vt_ids_len;
out:
	_vt_id_list->vt_ids_count = count;
err:
	if (vt_ids != NULL) {
		kfree(vt_ids);
	}
	if (tmp != NULL) {
		kfree(tmp);
	}
	return retval;
}
EXPORT_SYMBOL(get_vt_id_list);


/*
 * Changes the vt of process from ts->vt to vt having to_vt_id
 */
int change_vt ( struct task_struct *ts, int to_vt_id)
{
	struct vector_table *tmp_vt, *from_vt = NULL, *to_vt = NULL;
	int from_vt_id = 0, rc = 0;
	down_write(&vt_rwlock);
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
	if (to_vt_id != 0 && to_vt == NULL) {
		rc = -EINVAL;
		goto out;
	}
	if (from_vt != NULL) {
		atomic64_dec(&from_vt->rc);
		if (from_vt->module_ref ) {
			if (atomic64_read(&from_vt->rc) == 0) {
				printk(KERN_DEBUG "decreasing module ref of %p\n", from_vt->module_ref);
				module_put(from_vt->module_ref);
			}
		}
		else {
			rc = -EFAULT;
			goto out;
		}
	}
	if (to_vt == NULL) {
		ts->vt = NULL;
		goto out;
	}
	if (to_vt->module_ref ) {
		if (atomic64_read(&to_vt->rc) == 0) {
			try_module_get(to_vt->module_ref);
			printk(KERN_DEBUG "increased module ref of %p\n", to_vt->module_ref);
		}
	}
	else {
		rc = -EFAULT;
		goto out;
	}
	atomic64_inc(&to_vt->rc);
	ts->vt = to_vt;
out:
	up_write(&vt_rwlock);
	return rc;
}
EXPORT_SYMBOL(change_vt);

/*
 * Reduce the vector_table
 */
void dec_rc_vt ( struct task_struct *ts )
{
	if (ts->vt != NULL) {
		printk(KERN_DEBUG "Decrement RC\n");
		atomic64_dec(&(ts->vt->rc));
		if (ts->vt->module_ref ) {
			if (atomic64_read(&(ts->vt->rc)) == 0) {
				printk(KERN_INFO "decreasing module ref of %p inside dec\n", ts->vt->module_ref);
				module_put(ts->vt->module_ref);
			}
		}
		ts->vt = NULL;
	}
}
EXPORT_SYMBOL(dec_rc_vt);

/*
 * Take read lock on vt
 */
void vt_read_lock(void) {
	down_read(&vt_rwlock);
}
EXPORT_SYMBOL(vt_read_lock);

/*
 * Release read lock on vt
 */
void vt_read_unlock(void) {
	up_read(&vt_rwlock);
}
EXPORT_SYMBOL(vt_read_unlock);
