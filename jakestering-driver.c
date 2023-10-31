#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <asm/io.h>

#define JAKESTERING_MAX_USER_SIZE 1024

#define BCM2835_GPIO_ADDRESS 0x20200000

static struct proc_dir_entry *jakestering_proc = NULL;

static char data_buffer[JAKESTERING_MAX_USER_SIZE+1] = { 0 };

static unsigned int* gpio_registers = NULL;

static unsigned int returnValue;

static void gpio_write_pin( unsigned int pin, unsigned int value )
{
  if ( value == 1 )
  {
    unsigned int* gpio_on_register = ( unsigned int* )( ( char* )gpio_registers + 0x1c ); //hardcoded offset to GPSETn
    *gpio_on_register |= ( 0b1 << pin );
  
    return;
  }
  
  else
  {
    unsigned int* gpio_off_register = ( unsigned int* )( ( char* )gpio_registers + 0x28 ); //hardcode offset to GPCLRn
    *gpio_off_register |= ( 0b1 << pin );
  
    return;
  }
}


static void gpio_set_pinMode( unsigned int pin, unsigned int mode )
{
  unsigned int fsel_index  = pin / 10;
  unsigned int fsel_bitpos = pin % 10;
  unsigned int* gpio_fsel  = gpio_registers + fsel_index;
  if (mode == 1)
  {
    *gpio_fsel &= ~( 0b111 << ( fsel_bitpos * 3 ) ); //clear the last value in registers
    *gpio_fsel |=  ( 0b1 << ( fsel_bitpos * 3 ) ); //set pin to output
  }
  
  else 
  {
    *gpio_fsel &= ~( 0b111 << ( fsel_bitpos * 3 ) ); //setting value to 0 
  }
    
}

static void gpio_read_pin( unsigned int pin )
{
  unsigned int* gpio_lev = ( unsigned int* )( ( char* )gpio_registers + 0x34 ); //hardcoded offset for GPLEVn
  returnValue = 0;
  returnValue = ( *gpio_lev >> pin ) & 1;
  return;
}

ssize_t jakestering_read( struct file* file, char __user* user, size_t size, loff_t* off )
{
  char buffer[ 2 ];
  unsigned int data = returnValue;

  snprintf( buffer, sizeof( buffer ), "%d", data );

  if ( copy_to_user( user, buffer, sizeof( buffer ) ) != 0)
  {
    return 0;
  }

  return sizeof( buffer );
//  return copy_to_user( user, "Hello\n", 6 ) ? 0 : 6;
}

ssize_t jakestering_write( struct file* file, const char __user* user, size_t size, loff_t* off )
{
  unsigned int pin = UINT_MAX;
  unsigned int value = UINT_MAX;
  unsigned int mode = UINT_MAX;

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

  if ( sscanf( data_buffer, "%d,%d,%d", &mode, &pin, &value ) != 3 )
  {
    printk("Improper data format\n");
    return size;
  }

  if ( mode > 4 || mode < 0 )
  {
    printk( "Invalid mode number\n" );
    return size;
  }

  if ( pin > 27 || pin < 0 )
  {
    printk( "Invalid pin number\n" );
    return size;
  }

  if ( value > 3 || value < 0 )
  {
    printk("Invalid value number\n");
    return size;
  }

  printk( "mode: %d, pin %d, value %d", mode, pin, value );
  
  if ( mode == 0 || mode == 1)
  {
    gpio_set_pinMode( pin, mode );
  } 
  
  else if ( mode == 2 )
  {
    gpio_read_pin( pin );
    printk( "read pin: %d, %d\n", pin, returnValue );
  }
  
  else if ( mode == 3 )
  {
    gpio_write_pin( pin, value );
  } 

  else if ( mode == 4)
  {
    printk( "PUD_CONTROL\n" );
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
