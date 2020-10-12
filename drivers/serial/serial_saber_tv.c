


#include <common.h>
#include <serial.h>
#include <errno.h>
#include <dm.h>

#define TV_ADDR_VCR_OFFSET 0x0
#define TV_ADDR_VDR_OFFSET 0x4

#define TV_SCREEN_W 80
#define TV_SCREEN_H 60

#define TV_COMMAND_SET_CHAR 0b00000001
#define TV_COMMAND_ADVANCE  0b00000010
#define TV_COMMAND_PUT_CHAR 0b00000011
#define TV_COMMAND_SET_X    0b00000100
#define TV_COMMAND_SET_Y    0b00000101
#define TV_COMMAND_NEW_LINE 0b00001000

#define TV_COMMAND_GET_X    0b00010100
#define TV_COMMAND_GET_Y    0b00010101

struct saber_tv_regs {
    u32 command;
	u32 data;
};

struct saber_tv_platdata {
	struct saber_tv_regs regs;
};

static const char* input_buffer = "help\n\r";
static int input_index = 0;

static char _saber_tv_next_input_char(void){
	char next;
	next = input_buffer[input_index];
	if(next == '\0'){
		next = -EAGAIN;
	}else{
		input_index++;
	}
	return next;
}

static inline void _saber_tv_set_data(struct saber_tv_platdata *platdata, u32 data)
{
    writel(data, (u32*)(platdata->regs.data));
}

static inline u32 _saber_tv_get_data(struct saber_tv_platdata *platdata)
{
    return readl((u32*)(platdata->regs.data));
}

static inline void _saber_tv_run_command(struct saber_tv_platdata *platdata, u32 command)
{
    writel(command, (u32*)(platdata->regs.command));
}

static void _saber_tv_new_line(struct saber_tv_platdata *platdata)
{
	int x;
	int y;
	_saber_tv_run_command(platdata, TV_COMMAND_NEW_LINE);

	_saber_tv_run_command(platdata, TV_COMMAND_GET_Y);
	y = _saber_tv_get_data(platdata);

	_saber_tv_set_data(platdata, ' ');
	for(x = 0; x < TV_SCREEN_W; x++){
		_saber_tv_run_command(platdata, TV_COMMAND_PUT_CHAR);
	}
	
	_saber_tv_set_data(platdata, y);
	_saber_tv_run_command(platdata, TV_COMMAND_SET_Y);
}

static int saber_tv_serial_getc(struct udevice *dev)
{
	return _saber_tv_next_input_char();
}

static int saber_tv_serial_putc(struct udevice *dev, const char ch)
{
	struct saber_tv_platdata *platdata = dev_get_platdata(dev);

	switch(ch){
		case '\n': 
			_saber_tv_new_line(platdata);
			break;
		
		case '\r':
			break;

		default:
			if(ch >= ' ' && ch <= '~'){
				_saber_tv_set_data(platdata, ch);
				_saber_tv_run_command(platdata, TV_COMMAND_PUT_CHAR);
			}
			break;
	}

	return 0;
}

static int saber_tv_serial_probe(struct udevice *dev)
{
	// struct saber_tv_platdata *platdata = dev_get_platdata(dev);

	return 0;
}

static int saber_tv_serial_ofdata_to_platdata(struct udevice *dev)
{
	struct saber_tv_platdata *platdata = dev_get_platdata(dev);

	fdt_addr_t address = dev_read_addr(dev);
	if(address == FDT_ADDR_T_NONE){
		// This would happen if it didn't read the device tree correctly
		// Unsure the correct error code for this
		return -EPERM; 
	}
	
	platdata->regs.command = address + TV_ADDR_VCR_OFFSET;
	platdata->regs.data = address + TV_ADDR_VDR_OFFSET;


	return 0;
}


static const struct dm_serial_ops saber_tv_serial_ops = {
	.putc = saber_tv_serial_putc,
	.getc = saber_tv_serial_getc,
};

static const struct udevice_id saber_tv_serial_ids[] = {
	{ .compatible = "saber,saber-tv" },
	{ }
};

U_BOOT_DRIVER(saber_tv_serial) = {
	.name	= "saber_tv_serial",
	.id	= UCLASS_SERIAL,
	.of_match = saber_tv_serial_ids,
	.ofdata_to_platdata = saber_tv_serial_ofdata_to_platdata,
	.platdata_auto_alloc_size = sizeof(struct saber_tv_platdata),
	.probe = saber_tv_serial_probe,
	.ops	= &saber_tv_serial_ops,
};
