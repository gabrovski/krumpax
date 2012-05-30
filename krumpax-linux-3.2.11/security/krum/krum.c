#include <linux/security.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/current.h>
#include "include/krum.h"

//pfault files
#include <linux/magic.h>		/* STACK_END_MAGIC		*/
#include <linux/sched.h>		/* test_thread_flag(), ...	*/
#include <linux/kdebug.h>		/* oops_begin/end, ...		*/
#include <linux/module.h>		/* search_exception_table	*/
#include <linux/bootmem.h>		/* max_low_pfn			*/
#include <linux/kprobes.h>		/* __kprobes, ...		*/
#include <linux/mmiotrace.h>		/* kmmio_handler, ...		*/
#include <linux/perf_event.h>		/* perf_sw_event		*/
#include <linux/hugetlb.h>		/* hstate_index_to_shift	*/
#include <linux/prefetch.h>		/* prefetchw			*/
#include <linux/unistd.h>
#include <linux/compiler.h>

#include <asm/traps.h>			/* dotraplinkage, ...		*/
#include <asm/pgalloc.h>		/* pgd_*(), ...			*/
#include <asm/kmemcheck.h>		/* kmemcheck_*(), ...		*/
#include <asm/fixmap.h>			/* VSYSCALL_START		*/
#include <asm/tlbflush.h>

#if defined(CONFIG_X86_64) && defined(CONFIG_PAX_MEMORY_UDEREF)
#include <asm/stacktrace.h>
#endif
//end pfault

static void krum_impl_test_hook() {
  //printk(KERN_INFO, "krum test hook working\n");
}


static struct security_operations krum_ops = {
  .name = "krum",
  .krum_test_hook = krum_impl_test_hook,
  .krum_do_pfault_check_address = krum_impl_do_pfault_check_address,
};


static int __init krum_init(void) {
  int error;
  printk(KERN_INFO "trying to load krum\n");

  error = register_security(&krum_ops);
  if (error) 
    printk(KERN_INFO "could not load krum\n");

  return error;
}

void cleanup_module() { }

static int krum_impl_do_pfault_check_address(struct pt_regs *regs, unsigned long error_code,
					 unsigned long *address) {
#if defined(CONFIG_X86_64) && defined(CONFIG_PAX_MEMORY_UDEREF)
	if (!user_mode(regs) && *address < 2 * PAX_USER_SHADOW_BASE) {
		if (!search_exception_tables(regs->ip)) {
			bad_area_nosemaphore(regs, error_code, *address);
			return -1; // should return
		}
		if (*address < PAX_USER_SHADOW_BASE) {
			printk(KERN_ERR "PAX: please report this to pageexec@freemail.hu\n");
			printk(KERN_ERR "PAX: faulting IP: %pS\n", (void *)regs->ip);
			show_trace_log_lvl(NULL, NULL, (void *)regs->sp, regs->bp, KERN_ERR);
		} else
			*address -= PAX_USER_SHADOW_BASE;
	}
#endif
	return 0;
}

MODULE_LICENSE("GPL");
security_initcall(krum_init);
