/**
 * Header file containing the CAN bus relative functions
 */

#ifndef _CAN_TRANSFERT_H__
#define _CAN_TRANSFERT_H__

#define CAN_DATA_SIZE 8

int can_init();
int can_release(int fd);
int can_write(char* buf, int len, int fd);
int can_read(void* buf, int len, int fd);

#endif /*_CAN_TRANSFERT_H__*/
