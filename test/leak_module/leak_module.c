#include <linux/module.h>
#include <linux/slab.h>

static int __init leak_init(void) {
    pr_info("[*] Deliberately leaking memory in kmalloc-512...\n");
    for (int i = 0; i < 10000; i++) {
        void *p = kmalloc(512, GFP_KERNEL);
        // intentionally not freeing
    }
    return 0;
}

static void __exit leak_exit(void) {
    pr_info("[*] Leak module unloaded (memory still leaked).\n");
}

module_init(leak_init);
module_exit(leak_exit);
MODULE_LICENSE("GPL");
