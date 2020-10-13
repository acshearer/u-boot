


#include <common.h>
#include <serial.h>
#include <errno.h>
#include <asm/sbi.h>
#include <dm.h>

struct saber_sbi_platdata {

};

static void _saber_sbi_putc(const char ch){
	sbi_console_putchar(ch);
}

static int _saber_sbi_getc(void){
	return sbi_console_getchar();
}

static int saber_sbi_serial_getc(struct udevice *dev)
{	
	int c = _saber_sbi_getc();
	if(c < 0){
		return '\0';
	}
	return (int)c;

	// In general return -EAGAIN if there is something pending, but this driver has no buffer.
}

static int saber_sbi_serial_putc(struct udevice *dev, const char ch)
{
	_saber_sbi_putc(ch);
	return 0;
}

static int saber_sbi_serial_probe(struct udevice *dev)
{
	// struct saber_sbi_platdata *platdata = dev_get_platdata(dev);

	return 0;
}

static int saber_sbi_serial_ofdata_to_platdata(struct udevice *dev)
{
	// struct saber_sbi_platdata *platdata = dev_get_platdata(dev);

	fdt_addr_t address = dev_read_addr(dev);
	if(address == FDT_ADDR_T_NONE){
		// This would happen if it didn't read the device tree correctly
		// Unsure the correct error code for this
		return -EPERM; 
	}
	
	// platdata->regs.command = address + TV_ADDR_VCR_OFFSET;
	// platdata->regs.data = address + TV_ADDR_VDR_OFFSET;


	return 0;
}


static const struct dm_serial_ops saber_sbi_serial_ops = {
	.putc = saber_sbi_serial_putc,
	.getc = saber_sbi_serial_getc,
};

static const struct udevice_id saber_sbi_serial_ids[] = {
	{ .compatible = "saber,saber-sbi-serial" },
	{ }
};

U_BOOT_DRIVER(saber_sbi_serial) = {
	.name	= "saber_sbi_serial",
	.id	= UCLASS_SERIAL,
	.of_match = saber_sbi_serial_ids,
	.ofdata_to_platdata = saber_sbi_serial_ofdata_to_platdata,
	.platdata_auto_alloc_size = sizeof(struct saber_sbi_platdata),
	.probe = saber_sbi_serial_probe,
	.ops	= &saber_sbi_serial_ops,
};
