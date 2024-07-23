#include "ili9341_lib.h"

#define AUTHOR      "cuongnd56 ngoduycuong0506@gmail.com"
#define VERSION     "1.0"
#define DESCRIPTION "Bring up ILI9341"

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

struct ILI9341_Driver_t *ili9341_t;

static struct of_device_id ili9341_device_ids[] = {
    { .compatible = "of,ili9341" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ili9341_device_ids);

static struct spi_device_id ili9341_spi_device_ids[] = {
	{"ili9341_device", 0},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(spi, ili9341_spi_device_ids);

static struct spi_driver ili9341_driver = {
	.probe = ili9341_probe,
	.remove = ili9341_remove,
	.id_table = ili9341_spi_device_ids,
	.driver = {
		.name = "ili9341_device",
		.of_match_table = ili9341_device_ids,
	},
};

static struct file_operations fops = {
    .owner      = THIS_MODULE,
    .write      = m_write,
    .open       = m_open,
    .release    = m_release,
};












