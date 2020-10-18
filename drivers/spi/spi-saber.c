
#include <common.h>
#include <dm.h>
#include <errno.h>
#include <log.h>
#include <malloc.h>
#include <fdtdec.h>
#include <spi.h>
#include <asm/io.h>
#include <linux/bitops.h>

// Control register flags
#define SPI_SPE  0b01000000
#define SPI_MSTR 0b00010000
#define SPI_SPR1 0b00000010
#define SPI_SPR0 0b00000001

// SPI_SPR1, SPI_SPR0
#define SPI_SPEED_1_4   0b00
#define SPI_SPEED_1_8   0b01
#define SPI_SPEED_1_64  0b10
#define SPI_SPEED_1_128 0b11

#define SPI_SPEED_SLOW SPI_SPEED_1_128
#define SPI_SPEED_FAST SPI_SPEED_1_4

// Status register flags
#define SPI_SPIF 0b10000000

struct saber_spi_regs {
	u32* data;
	u32* status;
	u32* control;
    u32* cs;
};

struct saber_spi_platdata {
	struct saber_spi_regs *regs;
    bool enabled;
};

static void saber_spi_activate(struct udevice *dev) {
    struct saber_spi_platdata *plat = dev_get_platdata(dev);

    if(!plat->enabled){
//        printf("SPI FIRST TIME ACTIVATE\n");
        uint32_t speed = SPI_SPEED_FAST;
        uint32_t flags = SPI_SPE | SPI_MSTR | (speed & (SPI_SPR1 | SPI_SPR0));

        writel(flags, plat->regs->control);

        plat->enabled = true;
    }

    writel(0, plat->regs->cs);
    // u32 cs = readl(plat->regs->cs);
//    printf("WR TEST: should be zero: %x\n", cs);
}

static void saber_spi_deactivate(struct udevice *dev) {
    struct saber_spi_platdata *plat = dev_get_platdata(dev);

    writel(1, plat->regs->cs);
}


static int saber_spi_xfer(struct udevice *dev, unsigned int bitlen, const void *dout, void *din, unsigned long flags) {
    struct saber_spi_platdata *plat = dev_get_platdata(dev->parent);

//    printf("Data   : %x@%x\n", readl(plat->regs->data), (u32)plat->regs->data);
//    printf("Status : %x@%x\n", readl(plat->regs->status), (u32)plat->regs->status);
//    printf("Control: %x@%x\n", readl(plat->regs->control), (u32)plat->regs->control);
//    printf("CS     : %x@%x\n", readl(plat->regs->cs), (u32)plat->regs->cs);

//	printf(__FILE__" saber_spi_xfer\n");

    unsigned int bytes = bitlen / 8;
    const unsigned char *txp = dout;
	unsigned char *rxp = din;
    uint32_t data;

    if(bitlen % 8){
        return -1;
    }

    if(flags & SPI_XFER_BEGIN){
//        printf("SPI ACTIVATE\n");
        saber_spi_activate(dev->parent);
        // u32 cs = readl(plat->regs->cs);
//        printf("2nd WR TEST: should be zero: %x\n", cs);
    }

    
//    printf("Data   : %x@%x\n", readl(plat->regs->data), (u32)plat->regs->data);
//    printf("Status : %x@%x\n", readl(plat->regs->status), (u32)plat->regs->status);
//    printf("Control: %x@%x\n", readl(plat->regs->control), (u32)plat->regs->control);
//    printf("CS     : %x@%x\n", readl(plat->regs->cs), (u32)plat->regs->cs);


    while(bytes--){

        if(txp){
            data = *txp;
            txp++;
        }else{
			data = 0xff;
//            printf("!!");
        }

//        printf("Sending: %x\n", data);

        writel(data, plat->regs->data);

        while(!(readl(plat->regs->status) & SPI_SPIF));

        data = readl(plat->regs->data);

        if (rxp){
			*rxp = data & 0xff;
            rxp++;
        }else{
//            printf("!!");
        }

//        printf("Recieved: %x\n", data);
    }

    if(flags & SPI_XFER_END){
//        printf("SPI DEACTIVATE\n");
        saber_spi_deactivate(dev->parent);
    }

    return 0;
}


static int saber_spi_set_speed(struct udevice *dev, uint speed) {
    struct saber_spi_platdata *plat = dev_get_platdata(dev);
    
    // fake clock speeds, does not conform to HZ requested.
    // these constants are taken from drivers/mmc/mmc_spi.c
    uint32_t speed_code = SPI_SPEED_SLOW;
    if(speed == 400000){
        speed_code = SPI_SPEED_SLOW;
    }else if(speed == 25000000){
        speed_code = SPI_SPEED_FAST;
    }

    uint32_t flags = readl(plat->regs->control);

    flags &= ~(SPI_SPR1 | SPI_SPR0);
    flags |= (speed_code & (SPI_SPR1 | SPI_SPR0));

    writel(flags, plat->regs->control);

    
    return 0;
}


static int saber_spi_set_mode(struct udevice *dev, uint mode) {
    return 0;
}

static int saber_spi_probe(struct udevice *dev) {
	return 0;
}

static int saber_spi_ofdata_to_platdata(struct udevice *dev) {
	struct saber_spi_platdata *plat = dev_get_platdata(dev);

	void* addr = (void *)(ulong)dev_remap_addr(dev);

    plat->regs->data = addr + 0x0;
    plat->regs->status = addr + 0x4;
    plat->regs->control = addr + 0x8;
    plat->regs->cs = addr + 0xc;

    plat->enabled = false;

	return 0;
}


static const struct dm_spi_ops saber_spi_ops = {
    .xfer        = saber_spi_xfer,
    .set_speed   = saber_spi_set_speed,
    .set_mode    = saber_spi_set_mode
};

static const struct udevice_id saber_spi_ids[] = {
    { .compatible = "saber,saber-spi" },
    {}
};

U_BOOT_DRIVER(saber_spi) = {
    .name                     = "saber_spi",
    .id                       = UCLASS_SPI,
    .of_match                 = saber_spi_ids,
    .ops                      = &saber_spi_ops,
    .ofdata_to_platdata       = saber_spi_ofdata_to_platdata,
    .platdata_auto_alloc_size = sizeof(struct saber_spi_platdata),
    .probe                    = saber_spi_probe,
    .flags                    = DM_UC_FLAG_SEQ_ALIAS
};