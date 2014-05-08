#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
	
static int khello_init(void){
		printk("khello_init, \n");
			
			return 0;
}

static void khello_exit(void){
		printk("khello_exit, \n");
			
}

module_init(khello_init);
module_exit(khello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("khello");

