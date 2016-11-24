#include <linux/types.h>
#include <linux/list.h>

struct vector_table {
	unsigned long id;
	atomic64_t rc;
	struct list_head vt_list;
	int (* call_back)(int sys_call_no, int param_nos, ...);
	int **new_sys_call_nos;
};
