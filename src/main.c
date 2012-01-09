#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ioctl.h>
#include "lib/can/mcp251x.h"
#include "lib/can/can.h"

int main(int argc, char** argv)
{

  /*
  struct id_filter idf = {
    .id = 1,
    .ide = 0,
    .eid = 0,
    .active = 1
  };
*/

  static char buf[4];
  static char *pbuf=buf;
  struct can_frame frame;
  struct can_frame frame_rcv;

  struct can_filter filter = {
    .fid = {
      {3, 0 , 0, 1},
      {4, 1, 9, 1},
      {5, 0, 0, 1},
      {6, 1, 13, 1},
      {7, 0, 0, 1},
      {8, 1, 17, 1},
    },
    .sidmask = 0,
    .eidmask = 0,
    .mode = 0,
  };

  int can_id = 1;
  int fd = can_init();
  int len;
  
//  ioctl(fd, CAN_IOCTLOOPBACKMODE); // Loopback mode
  ioctl(fd, CAN_IOCSFILTER, &filter);  
  ioctl(fd, CAN_IOCSRATE, 250000);


  memset(&frame, 0, sizeof(struct can_frame));

  frame.header.id = can_id;
  frame.header.ide = 0;
  frame.header.eid = 0;


  sprintf(buf, "BLOP");
  len = strlen(buf);
  pbuf = buf;

  for (len; len>CAN_FRAME_MAX_DATA_LEN; len-=CAN_FRAME_MAX_DATA_LEN)
  {
    memcpy(frame.data, buf, CAN_FRAME_MAX_DATA_LEN);
    frame.header.dlc = CAN_FRAME_MAX_DATA_LEN;
    printf("Write-> %d \n", write(fd, &frame, sizeof(struct can_frame)));
    pbuf += CAN_FRAME_MAX_DATA_LEN;
  }

  printf("LEN : %d \n", len);

  if (len > 0)
  {
    memcpy(frame.data, buf, CAN_FRAME_MAX_DATA_LEN);
    frame.header.dlc = CAN_FRAME_MAX_DATA_LEN;
    printf("Write-> %d \n", write(fd, &frame, sizeof(struct can_frame)));
    //printf("Read-> %d \n", read(fd, &frame_rcv, sizeof(struct can_frame)));
  }

  printf("CACA");

  printf("Release :%d \n", close(fd));

  //printf("%s \n", frame_rcv.data);

  return 0;
}
