/**
 * Source file for signal formation suited to the CAN bus
 * given the basics informations needed by and received 
 * from IA
 */

#include<stdio.h>
#include<stdlib.h>
#include"signalForm.h"
#include"canTransfert.h"

/**Return the received from PIC signal
 *fd : file descriptor of the CQN module returned previously by can_init
 *err : error bit
 *OVT : over temperature on the device
 *Isc : current in the device
 *Vsc : tension super capa
 *Vdc : tension on the capa bus
 *return value : 0 on success, -1 otherwise
 */
int recv_signal(int fd, char* err, char* OVT, float* Isc, float* Vsc, float* Vdc)
{
  int ret = -1;
  PICtoARM buf;

  if(can_read(&buf, sizeof(PICtoARM), fd) == sizeof(PICtoARM))
    ret = 0;
  else
    ret = -1;

  *err = (char)buf.error;
  *OVT = (char)buf.OVT;

  /*Maybe we'll have to change it to adapt the structure to the PIC*/
  *Isc = buf.Isc;
  /*Check the sign of the current*/
  if(buf.Isign == 1)
    *Isc = 0 - *Isc;

  *Vsc = buf.Vsc;
  *Vdc = buf.Vdc;
  /*\changes*/

  return ret;
}

int send_signal()
{
  int ret = -1;

  return ret;
}
