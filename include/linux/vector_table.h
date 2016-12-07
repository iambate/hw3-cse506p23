#ifndef __LINUX_VECTOR_TABLE_H
#define __LINUX_VECTOR_TABLE_H

#include <linux/types.h>
#include <linux/list.h>

#define GET_FLAG 10
#define SET_FLAG 0

struct sys_vect {
	int sys_no;
	void *sys_func;
};

struct vector_table {
	int id;
	atomic64_t rc;
	struct list_head vt_list;
	int sys_map_size;
	struct sys_vect *sys_map;	
};

struct var_args {
	pid_t process_id;
	int vector_table_id;
};

struct vt_id_list {
	int vt_ids_count;
	int *vt_ids;
};

int register_vt (struct vector_table *vt);
int deregister_vt (struct vector_table *vt);
int deregister_vt_id (int vt_id);
int is_implemented_by_vt (int sys_call_no);
struct vector_table *get_vt (int vt_id);
struct vt_id_list *get_vt_id_list(void);
int change_vt ( struct task_struct *ts, int to_vt_id);
void dec_rc_vt ( struct task_struct *ts );
#endif
