#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <asm/io.h>

#define JAKESTERING_MAX_USER_SIZE 1024

#define BCM2835_GPIO_ADDRESS 0x20200000

static struct proc_dir_entry *jakestering_proc = NULL;

static char data_buffer[JAKESTERING_MAX_USER_SIZE+1] = { 0 };

static unsigned int* gpio_registers = NULL;

ssize_t jakestering_read( struct file* file, char __user* user, size_t size, loff_t* off )
{
  return copy_to_user( user, "Hello\n", 7 ) ? 0 : 7;
}

ssize_t jakestering_write( struct file* file, const char __user* user, size_t size, loff_t* off )
{
  unsigned int pin = UINT_MAX;
  unsigned int value = UINT_MAX;

  memset( data_buffer, 0x0, sizeof( data_buffer ) );
  if ( size > JAKESTERING_MAX_USER_SIZE )
  {
    size = JAKESTERING_MAX_USER_SIZE;
  }
  
  if ( copy_from_user( data_buffer, user, size ) )
  {
    return 0;
  }

  printk( "%s\n", data_buffer );

  if ( sscanf( data_buffer, "%d,%d", &pin, &value ) != 2 )
  {
    printk("Improper data format\n");
    return size;
  }

  if ( pin > 27 || pin < 0 )
  {
    printk( "Invalid pin number\n" );
    return size;
  }

  if ( value != 0 && value != 1 )
  {
    printk("Invalid value number\n");
    return size;
  }

  printk( "pin %d, value %d", pin, value );
  
  if ( value == 1 )
  {
    gpio_set_value( pin, value );
  }

  else if ( value == 0 )
  {
    gpio_set_value( pin, value );
  }

  return size;
}

static const struct proc_ops jakestering_proc_fops =
{
  .proc_read = jakestering_read,
  .proc_write = jakestering_write,
};

static int __init gpio_driver_init( void )
{
  printk( "Jakestering gpio driver installed\n" );
  
  gpio_registers = (int*)ioremap(BCM2835_GPIO_ADDRESS, PAGE_SIZE);
  if ( gpio_registers == NULL )
  {
    printk( "Failed to map GPIO memory to driver\n" );
    return -1;
  }
  
  printk( "Successfully mapped into GPIO memeory\n" );

  jakestering_proc = proc_create( "jakestering-gpio", 0666, NULL, &jakestering_proc_fops );
  if ( jakestering_proc == NULL )
  {
    return -1;
  }

  return 0;
}

static void __exit gpio_driver_exit( void )
{
  printk( "Jakestering gpio driver successfully uninstalled\n" );
  iounmap( gpio_registers );
  proc_remove( jakestering_proc );
  return;
}

module_init( gpio_driver_init );
module_exit( gpio_driver_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Jakester: jkellum819@gmail.com" );
MODULE_DESCRIPTION( "gpio test driver" );
MODULE_VERSION( "1.0" );
