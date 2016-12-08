#define GET_FLAG 10
#define SET_FLAG 0
#define GET_VT 11

struct var_args {
	pid_t process_id;
	int vector_table_id;
};

struct vt_id_list {
	int vt_ids_info_len;
	int vt_ids_count;
	char *vt_ids_info;
};
