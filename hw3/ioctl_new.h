#define GET_FLAG 10
#define SET_FLAG 0

struct var_args {
	pid_t process_id;
	int vector_table_id;
};

struct vt_id_list {
	int vt_ids_count;
	int *vt_ids;
};
