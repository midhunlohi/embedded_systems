/*********************************************************
 * \file gpio_driver.c
 *
 *\details GPIO driver for raspberri pi 3B
 *
 *\author Midhun Lohidakshan
 *
 *********************************************************/
// Header file inclusion

#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/gpio.h>
#include<linux/uaccess.h>
#include<linux/cdev.h>

// LED connected to GPIO 21

#define GPIO_PIN_21 (21)

// Global Variables

dev_t dev = 0;
static struct cdev gpio_cdev;
static struct class *gpio_class;

// Driver Functions
static int gpio_open(struct inode* inode, struct file* file);
static ssize_t gpio_read(struct file* filep, char __user *buf, size_t len, loff_t *off);
static ssize_t gpio_write(struct file* filep, const char __user *buf, size_t len, loff_t *off);
static int gpio_release(struct inode* inode, struct file* file);

// File Operations Structure
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read  = gpio_read,
	.write = gpio_write,
	.open  = gpio_open,
	.release = gpio_release,
};

// Open Function
static int gpio_open(struct inode* inode, struct file* file){
	printk(KERN_INFO " Device File Opened \n ");
	return 0;
}

// Read Function
static ssize_t gpio_read(struct file* filep, char __user *buf, size_t len, loff_t *off){

	uint8_t gpioState = 0;

	// Read GPIO value
	gpioState = gpio_get_value(GPIO_PIN_21);

	// Write to the user
	len = 1;
	if (copy_to_user(buf, &gpioState, len) > 0 ){
		printk(KERN_ERR "ERROR: copy_to_user() failed \n");
	}
	
	// Printing to console
	printk(KERN_INFO "GPIO Value is %d \n", gpioState);
	
	return 0;
}

// Write Function
static ssize_t gpio_write(struct file* filep, const char __user *buf, size_t len, loff_t *off){
	
	uint8_t writeBuf[10] = {0};

	if( copy_from_user( writeBuf, buf, len ) > 0){
		printk(KERN_ERR "ERROR: copy_from_user() failed \n");
	}
	// Printing to console
	printk(KERN_INFO "GPIO Value set to %c \n", writeBuf[0]);

	if(writeBuf[0] == '1'){
		// Set the GPIO value to HIGH
		gpio_set_value(GPIO_PIN_21, 1);

	}else if(writeBuf[0] == '0'){
		// Set the GPIO value to LOW
		gpio_set_value(GPIO_PIN_21, 0);
	}else{
		// Received unknown command
		printk(KERN_ERR "ERROR: unknown command received \n");
	}

	return len;
}

// Release Function
static int gpio_release(struct inode* inode, struct file* file){

	printk(KERN_INFO " Device File Closed \n ");
	return 0;

}

// Exit Function
void __exit gpio_driver_exit(void){

	gpio_unexport(GPIO_PIN_21);
	gpio_free(GPIO_PIN_21);
	device_destroy(gpio_class, dev);
	class_destroy(gpio_class);
	cdev_del(&gpio_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Device Driver Removed \n");

}

// Init Function
static int __init gpio_driver_init(void){

	// Allocating Major Number	
	if(alloc_chrdev_region( &dev, 0, 1, "gpio_device") < 0){
		printk(KERN_ERR "ERROR: Cannot allocate Major Number \n");
		unregister_chrdev_region(dev, 1);
		return -1;
	}
	printk(KERN_INFO "Major = %d, Minor = %d \n", MAJOR(dev), MINOR(dev));

	// Creating cdev structure
	cdev_init( &gpio_cdev, &fops );

	// Adding character device to the system
	if(cdev_add( &gpio_cdev, dev, 1) < 0){
		printk(KERN_ERR "ERROR: Cannot add device to the system \n");
		cdev_del(&gpio_cdev);
		return -1;
	}

	// Creating struct class
	if((gpio_class = class_create(THIS_MODULE, "gpio_class")) == NULL){
		printk(KERN_ERR "ERROR: Cannot create a class \n");
		class_destroy(gpio_class);
		return -1;
	}	

	// Creating device
	if(device_create(gpio_class, NULL, dev, NULL, "gpio_device") == NULL){
		printk(KERN_ERR "ERROR: Cannot create the device\n");
		device_destroy(gpio_class, dev);		
		return -1;
	}

	// Checking the GPIO is valid or not
	if(gpio_is_valid(GPIO_PIN_21) == false){
		printk(KERN_ERR "ERROR: GPIO %d is not valid \n", GPIO_PIN_21);
		device_destroy(gpio_class, dev);
		return -1;	
	}

	// Requesting the GPIO
	if(gpio_request(GPIO_PIN_21, "GPIO_PIN_21") < 0 ){
		printk(KERN_ERR "ERROR: GPIO %d request \n", GPIO_PIN_21);
		gpio_free(GPIO_PIN_21);
		return -1;
	}

	// Configure the GPIO as output
	gpio_direction_output(GPIO_PIN_21 , 0);

	// Export GPIO number to sys/class/		
	gpio_export(GPIO_PIN_21, false);
	
	printk(KERN_INFO "Device Driver Inserted Successfully \n");

	return 0;
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Midhun Lohidakshan");
MODULE_DESCRIPTION("GPIO Driver for Rasperri PI 3B");
MODULE_VERSION("1");
