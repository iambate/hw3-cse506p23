#ifndef __LINUX_VECTOR_TABLE_H
#define __LINUX_VECTOR_TABLE_H

#include <linux/types.h>
#include <linux/list.h>

struct vector_table {
	unsigned long id;
	atomic64_t rc;
	struct list_head vt_list;
	int sys_map_size;
	int **sys_map;	
	long (* call_back)(int sys_call_no, int param_nos, ...);

};

int register_vt (struct vector_table *vt);
int deregister_vt (struct vector_table *vt);
int deregister_vt_id (int vt_id);
#endif
