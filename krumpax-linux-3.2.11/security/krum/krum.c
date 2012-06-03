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

//mmap
#include <linux/slab.h>
#include <linux/backing-dev.h>
#include <linux/mm.h>
#include <linux/shm.h>
#include <linux/mman.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/syscalls.h>
#include <linux/capability.h>
#include <linux/init.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/personality.h>
#include <linux/security.h>
#include <linux/hugetlb.h>
#include <linux/profile.h>
#include <linux/export.h>
#include <linux/mount.h>
#include <linux/mempolicy.h>
#include <linux/rmap.h>
#include <linux/mmu_notifier.h>
#include <linux/perf_event.h>
#include <linux/audit.h>
#include <linux/khugepaged.h>

#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <asm/tlb.h>
#include <asm/mmu_context.h>


#if defined(CONFIG_X86_64) && defined(CONFIG_PAX_MEMORY_UDEREF)
#include <asm/stacktrace.h>
#endif
//end pfault

//mmap
#include <linux/slab.h>
#include <linux/backing-dev.h>
#include <linux/mm.h>
#include <linux/shm.h>
#include <linux/mman.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/syscalls.h>
#include <linux/capability.h>
#include <linux/init.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/personality.h>
#include <linux/security.h>
#include <linux/hugetlb.h>
#include <linux/profile.h>
#include <linux/export.h>
#include <linux/mount.h>
#include <linux/mempolicy.h>
#include <linux/rmap.h>
#include <linux/mmu_notifier.h>
#include <linux/perf_event.h>
#include <linux/audit.h>
#include <linux/khugepaged.h>

#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <asm/tlb.h>
#include <asm/mmu_context.h>
//end mmap


static void krum_impl_test_hook() {
  //printk(KERN_INFO, "krum test hook working\n");
}


static struct security_operations krum_ops = {
  .name = "krum",
  .krum_test_hook = krum_impl_test_hook,
  .krum_do_pfault_check_address = krum_impl_do_pfault_check_address,
  .krum_do_mmap_pgoff = krum_impl_do_mmap_pgoff,
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

static void krum_impl_do_mmap_pgoff(struct mm_struct *mm, vm_flags_t *vm_flags, struct file *file) {
#ifdef CONFIG_PAX_MPROTECT
	if (mm->pax_flags & MF_PAX_MPROTECT) {
		if ((*vm_flags & (VM_WRITE | VM_EXEC)) == (VM_WRITE | VM_EXEC))

#ifdef CONFIG_PAX_EMUPLT
			*vm_flags &= ~VM_EXEC;
#else
			return -EPERM;
#endif

		if (!(*vm_flags & VM_EXEC))
			*vm_flags &= ~VM_MAYEXEC;
		else
			*vm_flags &= ~VM_MAYWRITE;
	}
#endif

#if defined(CONFIG_PAX_PAGEEXEC) && defined(CONFIG_X86_32)
	if ((mm->pax_flags & MF_PAX_PAGEEXEC) && file)
		*vm_flags &= ~VM_PAGEEXEC;
#endif

}

MODULE_LICENSE("GPL");
security_initcall(krum_init);
