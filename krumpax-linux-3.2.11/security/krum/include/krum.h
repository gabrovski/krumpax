#ifndef _KRUM_SECURITY
#define _KRUM_SECURITY

#include <linux/export.h>
#include <linux/security.h>

static int krum_impl_do_pfault_check_address(struct pt_regs *regs, unsigned long error_code, unsigned long *address);


#endif
