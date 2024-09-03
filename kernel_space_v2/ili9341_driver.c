/*
 * ili9342 LCD for Raspberry Pi Model B rev2
 */
 
#include "ili9341_driver.h"


#define BLOCKSIZE (4*1024)
#define ORIENTATION 0 //0=LANDSCAPE 1=PORTRAIT  

#define DISPLAY_WIDTH   320
#define DISPLAY_HEIGHT  240

#define DISPLAY_BPP     16

static struct device_t ili9341_device = {0};

// initialization of GPIO
static void tft_init_board(struct fb_info *info)
{
	gpiod_set_value(ili9341_device.cs_gpio, 0);
}

// hard reset of the graphic controller and the tft
static void tft_hard_reset(void)
{
    gpiod_set_value(ili9341_device.rs_gpio, 0);
    msleep(120);
    gpiod_set_value(ili9341_device.rs_gpio, 1);
    msleep(120);
}

// write command
static void tft_command_write(char command)
{
    gpiod_set_value(ili9341_device.dc_gpio, 0);
    spi_write(ili9341_device.client, (uint8_t *)&command, 1);
}

// write data
static void tft_data_write(char data)
{
    gpiod_set_value(ili9341_device.dc_gpio, 1);
    spi_write(ili9341_device.client, (uint8_t *)&data, 1);
}

// initialization of ili9341
static void tft_init(struct fb_info *info)
{
	
	tft_hard_reset();
	
	tft_command_write(0x28); //display OFF
	tft_command_write(0x11); //exit SLEEP mode
	tft_data_write(0x00);
	tft_command_write(0xCB); //Power Control A
	tft_data_write(0x39); //always 0x39
	tft_data_write(0x2C); //always 0x2C
	tft_data_write(0x00); //always 0x
	tft_data_write(0x34); //Vcore = 1.6V
	tft_data_write(0x02); //DDVDH = 5.6V
	tft_command_write(0xCF); //Power Control B
	tft_data_write(0x00); //always 0x
	tft_data_write(0x81); //PCEQ off
	tft_data_write(0x30); //ESD protection
	tft_command_write(0xE8); //Driver timing control A
	tft_data_write(0x85); //non‐overlap
	tft_data_write(0x01); //EQ timing
	tft_data_write(0x79); //Pre‐charge timing
	tft_command_write(0xEA); //Driver timing control B
	tft_data_write(0x00); //Gate driver timing
	tft_data_write(0x00); //always 0x
	tft_command_write(0xED); //Power‐On sequence control
	tft_data_write(0x64); //soft start
	tft_data_write(0x03); //power on sequence
	tft_data_write(0x12); //power on sequence
	tft_data_write(0x81); //DDVDH enhance on
	tft_command_write(0xF7); //Pump ratio control
	tft_data_write(0x20); //DDVDH=2xVCI
	tft_command_write(0xC0); //power control 1
	tft_data_write(0x26);
	tft_data_write(0x04); //second parameter for ILI9340 (ignored by ILI9341)
	tft_command_write(0xC1); //power control 2
	tft_data_write(0x11);
	tft_command_write(0xC5); //VCOM control 1
	tft_data_write(0x35);
	tft_data_write(0x3E);
	tft_command_write(0xC7); //VCOM control 2
	tft_data_write(0xBE);
	tft_command_write(0x36); //memory access control = BGR
	tft_data_write(0x88);
	tft_command_write(0xB1); //frame rate control
	tft_data_write(0x00);
	tft_data_write(0x10);
	tft_command_write(0xB6); //display function control
	tft_data_write(0x0A);
	tft_data_write(0xA2);
	tft_command_write(0x3A); //pixel format = 16 bit per pixel
	tft_data_write(0x55);
	tft_command_write(0xF2); //3G Gamma control
	tft_data_write(0x02); //off
	tft_command_write(0x26); //Gamma curve 3
	tft_data_write(0x01);
	tft_command_write(0x2A); //column address set
	tft_data_write(0x00);
	tft_data_write(0x00); //start 0x00
	tft_data_write(0x00);
	tft_data_write(0xEF); //end 0xEF
	tft_command_write(0x2B); //page address set
	tft_data_write(0x00);
	tft_data_write(0x00); //start 0x00
	tft_data_write(0x01);
	tft_data_write(0x3F); //end 0x013F
	
	tft_command_write(0x29); //display ON

}

// write memory to TFT
static void ili9341_update_display_area(const struct fb_image *image)
{
	int x,y;
	
	// set column
	(ORIENTATION) ? tft_command_write(0x2B) : tft_command_write(0x2A);
	
	tft_data_write(image->dx >> 8);
	tft_data_write(image->dx);
	
	tft_data_write((image->dx + image->width) >> 8);
	tft_data_write(image->dx + image->width);
	// set row
	(ORIENTATION) ? tft_command_write(0x2A) : tft_command_write(0x2B);
	
	tft_data_write(image->dy >> 8);
	tft_data_write(image->dy);
	
	tft_data_write((image->dy + image->height) >> 8);
	tft_data_write(image->dy + image->height);
		
	tft_command_write(0x2C); //Memory Write
	
	if(ORIENTATION == 0){
		for(y=0;y < image->width ;y++){
			for(x=0;x < image->height ;x++){
				tft_data_write(image->data[(image->dx * (2 * image->width)) + (image->dy * 2) + 1]);
				tft_data_write(image->data[(image->dx * (2 * image->width)) + (image->dy * 2) + 2]);
			}
		}
	}else{
		for(y=0;y < image->width ;y++){
			for(x=0;x < image->height ;x++){
				tft_data_write(image->data[(image->dx * (2 * image->width)) + (image->dy * 2) + 1]);
				tft_data_write(image->data[(image->dx * (2 * image->width)) + (image->dy * 2) + 2]);
			}
		}
	}

	tft_command_write(0x29); //display ON	
}


static void ili9341_update_display_color_area(const struct fb_fillrect *rect)
{
	int x,y;
	// set column
	(ORIENTATION) ? tft_command_write(0x2B) : tft_command_write(0x2A);
	
	tft_data_write(rect->dx >> 8);
	tft_data_write(rect->dx);
	
	tft_data_write((rect->dx + rect->width) >> 8);
	tft_data_write(rect->dx + rect->width);
	// set row
	
	(ORIENTATION) ? tft_command_write(0x2A) : tft_command_write(0x2B);
	
	tft_data_write(rect->dy >> 8);
	tft_data_write(rect->dy);
	
	tft_data_write((rect->dy + rect->height) >> 8);
	tft_data_write(rect->dy + rect->height);
		
	tft_command_write(0x2C); //Memory Write
	
	if(ORIENTATION == 0){
		for(y=0;y < rect->width ;y++){
			for(x=0;x < rect->height ;x++){
				tft_data_write(rect->color);
				tft_data_write(rect->color >> 8);
			}
		}
	}else{
		for(y=0;y < rect->height ;y++){
			for(x=0;x < rect->width ;x++){
				tft_data_write(rect->color);
				tft_data_write(rect->color >> 8);
			}
		}
	}
	
	tft_command_write(0x29); //display ON	
}

static void ili9341_update_display(const struct fb_info *info)
{
	int x,y;
	
	tft_command_write(0x2C); //Memory Write
	
	if(ORIENTATION == 0){
		for(y=0;y < DISPLAY_WIDTH ;y++){
			for(x=0;x < DISPLAY_HEIGHT ;x++){
				tft_data_write(info->screen_base[(x * (2 * DISPLAY_WIDTH)) + (y * 2) + 1]);
				tft_data_write(info->screen_base[(x * (2 * DISPLAY_WIDTH)) + (y * 2) + 2]);
			}
		}
	}else{
		for(y=(DISPLAY_HEIGHT - 1);y >= 0 ;y--){
			for(x=0;x < DISPLAY_WIDTH ;x++){
				tft_data_write(info->screen_base[(y * (2 * DISPLAY_WIDTH)) + (x * 2) + 1]);
				tft_data_write(info->screen_base[(y * (2 * DISPLAY_WIDTH)) + (x * 2) + 2]);
			}
		}
	}
	tft_command_write(0x29); //display ON
}

static void ili9341_fillrect(struct fb_info *info, const struct fb_fillrect *rect)
{
	//printk(KERN_INFO "fb%d: ili9341_fillrect\n", info->node);
    //ili9341_update_display_color_area(rect);
    ili9341_update_display(info);
}

static void ili9341_copyarea(struct fb_info *info, const struct fb_copyarea *area)
{
	//printk(KERN_INFO "fb%d: ili9341_copyarea\n", info->node);
    ili9341_update_display(info);
}

static void ili9341_imageblit(struct fb_info *info, const struct fb_image *image)
{
   //printk(KERN_INFO "fb%d: ili9341_imageblit\n", info->node);
   //ili9341_update_display_area(image);
   ili9341_update_display(info);
}

static ssize_t ili9341_write(struct fb_info *info, const char __user *buf, size_t count, loff_t *ppos)
{
	unsigned long p = *ppos;
	void *dst;
	int err = 0;
	unsigned long total_size;

	if (info->state != FBINFO_STATE_RUNNING)
		return -EPERM;

	total_size = info->screen_size;

	if (total_size == 0)
		total_size = info->fix.smem_len;

	if (p > total_size)
		return -EFBIG;

	if (count > total_size) {
		err = -EFBIG;
		count = total_size;
	}

	if (count + p > total_size) {
		if (!err)
			err = -ENOSPC;

		count = total_size - p;
	}

	dst = (void __force *) (info->screen_base + p);

	if (info->fbops->fb_sync)
		info->fbops->fb_sync(info);

	if (copy_from_user(dst, buf, count))
		err = -EFAULT;

	if  (!err)
		*ppos += count;

	ili9341_update_display(info);

	return (err) ? err : count;   
}

static ssize_t ili9341_read(struct fb_info *info, char __user *buf, size_t count, loff_t *ppos)
{
	unsigned long p = *ppos;
	void *dst;
	int err = 0;
	unsigned long total_size;

	if (info->state != FBINFO_STATE_RUNNING)
		return -EPERM;

	total_size = info->screen_size;

	if (total_size == 0)
		total_size = info->fix.smem_len;

	if (p > total_size)
		return -EFBIG;

	if (count > total_size) {
		err = -EFBIG;
		count = total_size;
	}

	if (count + p > total_size) {
		if (!err)
			err = -ENOSPC;

		count = total_size - p;
	}

	dst = (void __force *) (info->screen_base + p);

	if (info->fbops->fb_sync)
		info->fbops->fb_sync(info);

	if (copy_from_user(dst, buf, count))
		err = -EFAULT;

	if  (!err)
		*ppos += count;

	return (err) ? err : count;
}

static void ili9341_deferred_io(struct fb_info *info, struct list_head *pagelist)
{
    ili9341_update_display(info);
}



static struct fb_fix_screeninfo ili9341_fix = {
    .id             = "ili9341",
    .type           = FB_TYPE_PACKED_PIXELS,
    .visual         = FB_VISUAL_TRUECOLOR,
    .accel          = FB_ACCEL_NONE,
    .xpanstep       = 0,
    .ypanstep       = 0,
    .ywrapstep      = 0,
    .line_length    = DISPLAY_WIDTH * DISPLAY_BPP / 8,
};


static struct fb_var_screeninfo ili9341_var = {
    .width          = DISPLAY_WIDTH,
    .height         = DISPLAY_HEIGHT,
    .bits_per_pixel = DISPLAY_BPP,
    .xres           = DISPLAY_WIDTH,
    .yres           = DISPLAY_HEIGHT,
    .xres_virtual   = DISPLAY_WIDTH,
    .yres_virtual   = DISPLAY_HEIGHT,
    .activate       = FB_ACTIVATE_NOW,
    .vmode          = FB_VMODE_NONINTERLACED,

    .nonstd         = 0,
    .red.offset     = 11,
    .red.length     = 5,
    .green.offset   = 5,
    .green.length   = 6,
    .blue.offset    = 0,
    .blue.length    = 5,
    .transp.offset  = 0,
    .transp.length  = 0,
};


static struct fb_ops ili9341_ops = {
    .owner          = THIS_MODULE,
    .fb_read        = ili9341_read,
    .fb_write       = ili9341_write,
    .fb_fillrect    = ili9341_fillrect,
    .fb_copyarea    = ili9341_copyarea,
    .fb_imageblit   = ili9341_imageblit,
};

static struct fb_deferred_io ili9341_defio = {
    .delay          = HZ/25,
    .deferred_io    = ili9341_deferred_io,
};

static unsigned int fps;

static int ili9341_probe(struct spi_device *spi)
{   
    struct fb_info *info;
    int retval = -ENOMEM;
    int vmem_size;
    unsigned char *vmem;
    struct device *dev = &spi->dev;

    vmem_size = ili9341_var.width * ili9341_var.height * ili9341_var.bits_per_pixel/8;
    vmem = vzalloc(vmem_size);
    if (!vmem) {
        return -ENOMEM;
    }
    memset(vmem, 0, vmem_size);

    ili9341_device.client = spi;
    ili9341_device.rs_gpio =  gpiod_get(dev, "rs", GPIOD_OUT_HIGH);
    ili9341_device.dc_gpio =  gpiod_get(dev, "dc", GPIOD_OUT_HIGH);
    ili9341_device.cs_gpio =  gpiod_get(dev, "chs", GPIOD_OUT_LOW);

    info = framebuffer_alloc(0, &spi->dev);
    if (!info) {
        vfree(vmem);
        return -ENOMEM;
    }

    info->screen_base = (char __iomem *)vmem;
    info->screen_size = vmem_size;
    info->fbops = &ili9341_ops;
    info->fix = ili9341_fix;
    info->fix.smem_start = (unsigned long)vmem;
    info->fix.smem_len = vmem_size;
    info->var = ili9341_var;
    info->flags = FBINFO_DEFAULT | FBINFO_VIRTFB;

    info->fbdefio = &ili9341_defio;
    if (0 < fps) {
        info->fbdefio->delay = HZ/fps;
    }

    fb_deferred_io_init(info);

    retval = register_framebuffer(info);
    if (retval < 0) {
        printk(KERN_INFO "%s: retval: %d\n", __func__, retval);
        framebuffer_release(info);
        vfree(vmem);
        return retval;
    }

    spi_set_drvdata(spi, info);

    tft_init_board(info);

    tft_hard_reset();

    tft_init(info);


    printk(KERN_INFO "fb%d: ili9341 LCD framebuffer device\n", info->node);
    return 0;
}


static int ili9341_remove(struct spi_device *spi)
{
    struct fb_info *info = spi_get_drvdata(spi);

    if (info) {
        unregister_framebuffer(info);
        fb_deferred_io_cleanup(info);
        vfree((void __force *)info->screen_base);

        framebuffer_release(info);
    }
    return 0;
}


static struct of_device_id ili9341_of_match[] = {
    { .compatible = "tft-c,ili9341-c" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ili9341_of_match);

static struct spi_device_id ili9341_id[] = {
	{"ili9341-c", 0},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(spi, ili9341_id);

static struct spi_driver ili9341_driver = {
	.probe = ili9341_probe,
	.remove = ili9341_remove,
	.id_table = ili9341_id,
	.driver = {
		.name = "ili9341_screen",
		.of_match_table = ili9341_of_match,
	},
};

unsigned long __stack_chk_guard;
void __stack_chk_guard_setup(void)
{
     __stack_chk_guard = 0xBAAAAAAD;//provide some magic numbers
}

void __stack_chk_fail(void)                         
{
 /* Error message */                                 
}// will be called when guard variable is corrupted 

module_param(fps, uint, 0);
MODULE_PARM_DESC(fps, "Frames per second (default 25)");

module_spi_driver(ili9341_driver);

MODULE_DESCRIPTION("ili9341 LCD framebuffer driver");
MODULE_AUTHOR("sammyizimmy");
MODULE_LICENSE("GPL");