#include "ili9341_lib.h"

#define AUTHOR      "cuongnd56 ngoduycuong0506@gmail.com"
#define VERSION     "1.0"
#define DESCRIPTION "Bring up ILI9341"

#define RING_METER  "ring_meter"

static int __init initialize(void);
static void __exit deinitialize(void);

/* Declate the probe and remove functions */
static int ili9341_probe(struct spi_device *client);
static int ili9341_remove(struct spi_device *client);

static ssize_t m_read(struct file *mf, char __user *user_buffer, size_t length, loff_t *offset);
static ssize_t m_write(struct file *mf, const char __user *user_buffer, size_t length, loff_t *offset);
static int m_open(struct inode *mInode, struct file *mf);
static int m_release(struct inode *mInode, struct file *mf);


char kernel_buff[50];

struct ILI9341_Driver_t {
    dev_t dev_num;
    struct class *m_class;
    struct cdev m_cdev;
    struct ili9341_device *device;
    int size;
};

static struct ILI9341_Driver_t ili9341_t;

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

static struct file_operations fops = {
    .owner      = THIS_MODULE,
    .read       = m_read,
    .write      = m_write,
    .open       = m_open,
    .release    = m_release,
};

static int __init initialize(void) {

    if (alloc_chrdev_region(&ili9341_t.dev_num, 0, 1, "ili9341-cdev")) {
        pr_err("ERROR: Can not make a number device\n");
        return -1;
    }
    pr_info("Initializing...\n");
    pr_info("Major : %d    Minor : %d\n", MAJOR(ili9341_t.dev_num), MINOR(ili9341_t.dev_num));
    
    if ((ili9341_t.m_class = class_create(THIS_MODULE, "ili9341_class")) == NULL){
        pr_err("ERROR: Can not create class ili9341\n");
        goto rm_dev_num;
    }
    pr_info("Initialize: Class created\n");

    if (device_create(ili9341_t.m_class, NULL, ili9341_t.dev_num, NULL, "ili9341_device") == NULL) {
        pr_err("ERROR: Can not create device ili9341\n");
        goto rm_class;

    }
    pr_info("Initialize: Device created\n");

    pr_info("Initialize: Creating cdev structure\n");
    cdev_init(&ili9341_t.m_cdev, &fops);
    if (cdev_add(&ili9341_t.m_cdev, ili9341_t.dev_num, 1) < 0) {
        pr_err("ERROR: Can not add device\n");
        goto rm_cdev_add;
    }
    pr_info("Initialize: file operations added to device file\n");

    if (spi_register_driver(&ili9341_driver) < 0) {
        pr_err("ERROR: Can not register spi\n");
        goto rm_cdev_add;
    }
    pr_info("Initialize: register spi driver\n");
    
    return 0;

rm_cdev_add:
    cdev_del(&ili9341_t.m_cdev);
rm_class:
    class_destroy(ili9341_t.m_class);
rm_dev_num:
    unregister_chrdev_region(ili9341_t.dev_num, 1);

    return -1;
}

static void __exit deinitialize(void) {

    pr_info("Deinitialize: Start...\n");
    spi_unregister_driver(&ili9341_driver);
    device_destroy(ili9341_t.m_class, ili9341_t.dev_num);          
    class_destroy(ili9341_t.m_class);
    cdev_del(&ili9341_t.m_cdev);
    unregister_chrdev_region(ili9341_t.dev_num, 1);
}

static int ili9341_probe(struct spi_device *client) {

    struct device *dev = &client->dev;

    pr_info("PROBE...\n");
    if (dev != NULL && dev->init_name != NULL) {
        pr_info("%s, %d, device name: %s", __func__, __LINE__, dev->init_name);
    }

    ili9341_t.device = kmalloc(sizeof(struct ili9341_device), GFP_KERNEL);
    if (ili9341_t.device == NULL) {
        pr_err("kmalloc failed\n");
        return -1;
    }

    ili9341_t.device->spi_dev.client = client;

    ili9341_t.device->spi_dev.rs_gpio =  gpiod_get(dev, "rs", GPIOD_OUT_HIGH);
    ili9341_t.device->spi_dev.dc_gpio =  gpiod_get(dev, "dc", GPIOD_OUT_HIGH);
    ili9341_t.device->spi_dev.cs_gpio =  gpiod_get(dev, "chs", GPIOD_OUT_LOW);

    ili9341_init(ili9341_t.device);

    // ringMeter1(ili9341_t.device, 500, 0, 1020, 10, 30, 40, 10, ILI9341_YELLOW, RED2RED);
    // ringMeter1(ili9341_t.device, 750, 0, 1020, 10, 110, 40,10, ILI9341_YELLOW, BLUE2BLUE);
    // fillScreen(ili9341_t.device, ILI9341_GREENYELLOW);
    drawFastHLine(ili9341_t.device, 10, 30, 110, ILI9341_GREENYELLOW);
    drawFastHLine(ili9341_t.device, 10, 110, 110, ILI9341_ORANGE);
    pr_info("Initialize: ili9341 init 3\n"); 
    
    return 0;
}

static int ili9341_remove(struct spi_device *client) {

    pr_info("Remove: Start...\n");
    if (ili9341_t.device != NULL) {
        gpiod_put(ili9341_t.device->spi_dev.rs_gpio);
        gpiod_put(ili9341_t.device->spi_dev.dc_gpio);
        gpiod_put(ili9341_t.device->spi_dev.cs_gpio);
        kfree(ili9341_t.device);
    }

    return 0;
}

static ssize_t m_read (struct file *mf, char __user *user_buffer, size_t length, loff_t *offset) {

    pr_info("Read system call\n");
    return length;
}

static ssize_t m_write (struct file *mf, const char __user *user_buffer, size_t length, loff_t *offset) {

    int status;
    char command[50] = {0};
    uint16_t x0, y0, x1, y1, x2, y2, color;

    pr_info("Write system call\n");
    
    /* Copy the buffer from user */
    status = copy_from_user(kernel_buff, user_buffer, length);
    if (status) {
        pr_err("%s - copy_from_user failed\n", __func__);
        return -EFAULT;
    }

    /* If the string is "clear", clear the display */
    if (!strncmp(RING_METER, kernel_buff, strlen(RING_METER))) {
        sscanf(kernel_buff, "%s %hd %hd %hd %hd %hd %hd %hd", command, &x0, &y0, &x1, &y1, &x2, &y2, &color);
        drawFillTriangle(ili9341_t.device, x0, y0, x1, y1, x2, y2, color);
    } else {
        pr_info("message: %s\n", kernel_buff);
    }

    /* Make the buffer empty */
    memset(kernel_buff, 0, sizeof(kernel_buff));
    
    return length;
}

static int m_open (struct inode *mInode, struct file *mf) {
    
    pr_info("Open system call\n");
    return 0;
}

static int m_release (struct inode *mInode, struct file *mf) {
    pr_info("Release system call\n");
    return 0;
}

// unsigned long __stack_chk_guard;
// void __stack_chk_guard_setup(void)
// {
//      __stack_chk_guard = 0xBAAAAAAD;//provide some magic numbers
// }

// void __stack_chk_fail(void)                         
// {
//  /* Error message */                                 
// }// will be called when guard variable is corrupted 

module_init(initialize);
module_exit(deinitialize);

// module_spi_driver(ili9341_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_VERSION(VERSION);





