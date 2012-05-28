#include <linux/security.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/current.h>
#include "include/krum.h"

static struct security_operations krum_ops {
  .name = "krum",
};

static int __init krum_init(void) {
  int error;

  error = register_security(&krum_ops);
  if (error) 
    printk(KERN_INFO "could not load krum\n");

  return error;
}

void cleanup_module() { }
