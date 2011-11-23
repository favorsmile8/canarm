/**
 * Source file for CAN bus relative functions
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>

/**
 * Open the driver for the mcp2515 module and initialise the CAN bus
 * This function returns the file descriptor of the CAN driver when no 
 *  problem occured 
 * A negative value when an error occured
 */
int can_init()
{
  /*Since the drivers are files on linux we have to open it before all
   *We wish to read from the bus, but also write on it, so we open in 
   *read/write mode
   */
  return open("/dev/mcp2515", O_RDWR, 0);
}

/*This function must be called to close the file at the end of the use of the 
 *driver given the filde descriptor of the file
 *The return value is 0 when no error occured, it is -1 otherwise
 */
int can_release(int fd)
{
  return close(fd);
}
