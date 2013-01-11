#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <linux/syscalls.h>
#include <linux/delay.h>    // loops_per_jiffy

#define CR0_WP 0x00010000   // Write Protect Bit (CR0:16)

/* Just so we do not taint the kernel */
MODULE_LICENSE("GPL");

void **syscall_table;
unsigned long **find_sys_call_table(void);

long (*orig_sys_open)(const char __user *filename, int flags, int mode);

/**
 * Addresses in my System.map file
 * ffffffff81175dc0 T sys_close
 * ffffffff81801300 R sys_call_table
 * ffffffff81c0f3a0 D loops_per_jiffy
 */
unsigned long **find_sys_call_table() {
    
    unsigned long ptr;
    unsigned long *p;

    for (ptr = (unsigned long)sys_close;
         ptr < (unsigned long)&loops_per_jiffy;
         ptr += sizeof(void *)) {
             
        p = (unsigned long *)ptr;

        if (p[__NR_close] == (unsigned long)sys_close) {
            printk(KERN_DEBUG "Found the sys_call_table!!!\n");
            return (unsigned long **)p;
        }
    }
    
    return NULL;
}

long my_sys_open(const char __user *filename, int flags, int mode) {
    long ret;

    ret = orig_sys_open(filename, flags, mode);
    printk(KERN_DEBUG "file %s has been opened with mode %d\n", filename, mode);
    
    return ret;
}

static int __init syscall_init(void)
{
    int ret;
    unsigned long addr;
    unsigned long cr0;
  
    syscall_table = (void **)find_sys_call_table();

    if (!syscall_table) {
        printk(KERN_DEBUG "Cannot find the system call address\n"); 
        return -1;
    }

    cr0 = read_cr0();
    write_cr0(cr0 & ~CR0_WP);

    addr = (unsigned long)syscall_table;
    ret = set_memory_rw(PAGE_ALIGN(addr) - PAGE_SIZE, 3);
    if(ret) {
        printk(KERN_DEBUG "Cannot set the memory to rw (%d) at addr %16lX\n", ret, PAGE_ALIGN(addr) - PAGE_SIZE);
    } else {
        printk(KERN_DEBUG "3 pages set to rw");
    }
    
    orig_sys_open = syscall_table[__NR_open];
    syscall_table[__NR_open] = my_sys_open;

    write_cr0(cr0);
  
    return 0;
}

static void __exit syscall_release(void)
{
    unsigned long cr0;
    
    cr0 = read_cr0();
    write_cr0(cr0 & ~CR0_WP);
    
    syscall_table[__NR_open] = orig_sys_open;
    
    write_cr0(cr0);
}

module_init(syscall_init);
module_exit(syscall_release);

