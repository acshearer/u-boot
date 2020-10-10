
#include <common.h>
#include <irq_func.h>

int cleanup_before_linux(void)
{
	disable_interrupts();

	return 0;
}
