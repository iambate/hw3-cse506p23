#include <linux/moduleloader.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <stdarg.h>

#define Debug 1

struct my_def {
	unsigned short a;
	int b;
	double c;
	char d;
	float e;
};

char *hello;
struct my_def *x;

int call_var(int num, ...)
{
	va_list valist;
	struct my_def *y;
	int g;
	char *tmp;
	unsigned long h;
	/*float i;
	int z;*/
	char j;

	va_start(valist, num);
	g = va_arg(valist, int);
	tmp = va_arg(valist, char*);
	h = va_arg(valist, unsigned long);
	y = va_arg(valist, struct my_def*);
	/*i = (float)va_arg(valist, double);*/
	j = (char)va_arg(valist, int);

	printk("g = %d\n, tmp=%s\n, h = %lu\n, j = %c\n", g, tmp, h, j);
	printk("struct y: a = %hu\t, b = %d\t, d = %c\n", y->a, y->b, y->d);
	va_end(valist);
	return 2;
}

void initialize_var(void)
{

	int err = 0;
#if Debug
	printk(KERN_DEBUG "inside initialize\n");
#endif
	x = kmalloc(sizeof(struct my_def), GFP_KERNEL);
	hello = kmalloc(sizeof(char)*100, GFP_KERNEL);
	strcpy(hello, "This is in initialize\n");
	x->a = 1;
	x->b = -12;
	x->c = 10.5;
	x->d = 'k';
	x->e = 0.75;
	err = call_var(5, 15, hello, -1, x, 'a');
	printk("error from call_var: %d\n", err);

}

static int __init init_var_arg(void)
{
#if Debug
	printk(KERN_DEBUG "installed new var_arg module\n");
#endif
	initialize_var();
	return 0;
}
static void  __exit exit_var_arg(void)
{
	kfree(hello);
	kfree(x);
#if Debug
	printk(KERN_DEBUG "removed new_arg module\n");
#endif
}
module_init(init_var_arg);
module_exit(exit_var_arg);
MODULE_LICENSE("GPL");
