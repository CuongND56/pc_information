#include "ili9341_lib.h"

#define AUTHOR      "cuongnd56 ngoduycuong0506@gmail.com"
#define VERSION     "1.0"
#define DESCRIPTION "Bring up ILI9341"

#define DC_GPIO 31

static int __init initialize(void);
static void __exit deinitialize(void);

/* Declate the probe and remove functions */
static int ili9341_probe(struct spi_device *client);
static int ili9341_remove(struct spi_device *client);

static ssize_t m_read (struct file *mf, char __user *user_buffer, size_t length, loff_t *offset);
static ssize_t m_write (struct file *mf, const char __user *user_buffer, size_t length, loff_t *offset);
static int m_open (struct inode *mInode, struct file *mf);
static int m_release (struct inode *mInode, struct file *mf);


struct ILI9341_Driver_t {
    dev_t dev_num;
    struct class *m_class;
    struct cdev m_cdev;
    struct ili9341_device *device;
    int size;
}

static struct ILI9341_Driver_t ili9341_t;

static struct of_device_id ili9341_device_ids[] = {
    { .compatible = "of,ili9341" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ili9341_device_ids);

// static struct spi_device_id ili9341_spi_device_ids[] = {
// 	{"ili9341_device", 0},
// 	{ /* sentinel */ },
// };
// MODULE_DEVICE_TABLE(spi, ili9341_spi_device_ids);

static struct spi_driver ili9341_driver = {
	.probe = ili9341_probe,
	.remove = ili9341_remove,
	// .id_table = ili9341_spi_device_ids,
	.driver = {
		.name = "ili9341_screen",
		.of_match_table = ili9341_device_ids,
        .owner = THIS_MODULE,
	},
};

static struct file_operations fops = {
    .owner      = THIS_MODULE,
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
    
    if ((mdev.m_class = class_create(THIS_MODULE, "ili9341_class")) == NULL){
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
    cdev_del(&ili9341_t->m_cdev);
rm_class:
    class_destroy(ili9341_t->m_class);
rm_dev_num:
    unregister_chrdev_region(ili9341_t->dev_num, 1);

    return -1;
}

static void __exit deinitialize(void) {

    pr_info("Deinitialize: Start...\n");
    spi_unregister_driver(&ili9341_driver);
    cdev_del(&ili9341_t->m_cdev);
    class_destroy(ili9341_t->m_class);
    unregister_chrdev_region(ili9341_t->dev_num, 1);
}

static int ili9341_probe(struct spi_device *client) {

    pr_info("PROBE...\n")
    ili9341_t->device = kmalloc(sizeof(struct ili9341_device), GFP_KERNEL);
    if (ili9341->device == NULL) {
        pr_err("kmalloc failed\n");
    }
    ili9341_t->device->client = client;
    ili9341_t->device->dc_gpio = DC_GPIO;

    gpio_request(DC_GPIO, "gpio0_30");
    gpio_direction_output(DC_GPIO, LOW);

    ili9341_init(ili9341_t->device);
    fillScreen(ili9341_t->device, ILI9341_PINK);
}

static int ili9341_remove(struct spi_device *client) {

    gpio_free(DC_GPIO);
}







