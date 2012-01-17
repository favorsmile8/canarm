/* 
 * can.c 
 *  
 *  
 * CAN BUS test  
 */  
  
#include <sys/types.h>  
#include <sys/unistd.h>  
#include <sys/stat.h>  
#include <linux/fcntl.h>  
#include <linux/ioctl.h>  
#include <pthread.h>  
  
#include "can.h"  
  
static void can_show_header(struct can_frame_header *header)  
{  
    printf("\nHeader: id=%d srr=%d ide=%d eid=%d rtr=%d rb1=%d rb0=%d dlc=%d\n",  
       header->id,  
       header->srr,  
       header->ide,  
       header->eid,  
       header->rtr,  
       header->rb1,  
       header->rb0,  
       header->dlc);  
}  
  
static int device = -1;  
static int device1 = -1;  
  
static void * can_read(void * th)   
{  
    struct can_frame frame_recv;  
    static sid = 0;  
    int i;  
    while(1) {  
    memset(&frame_recv, 0, sizeof(struct can_frame));  
    if (device1 > 0)  
        read(device1, &frame_recv, sizeof(struct can_frame));  
    else  
        read(device, &frame_recv, sizeof(struct can_frame));  
      
    if (frame_recv.header.id != sid) {  
        can_show_header(&frame_recv.header);  
        printf("RECV: \"");  
    }  
    for (i=0; i<frame_recv.header.dlc; i++)  
        printf("%c", frame_recv.data[i]);  
    if (frame_recv.header.id != sid)  
        printf("\"\n");  
      
    sid = frame_recv.header.id;  
    }  
    return NULL;  
}  
  
int main(int argc, char *argv[])  
{  
    if (argc < 2) {  
    printf("Useage: %s dev [[dev1 [-f]] [-f]]\n", argv[0]);  
    printf("   dev dev1 --- can device\n");  
    printf("   -f       --- filter mode\n");  
    exit(0);  
    }  
      
    static char buf[256];  
    static char *pbuf=buf;  
    struct can_frame frame;  
      
    struct can_filter filter = {  
    .fid = {  
        {3,0,0,1},  
        {4,1,9,1},  
        {5,0,0,1},  
        {6,1,13,1},  
        {7,0,0,1},  
        {8,1,17,1},  
    },  
    .sidmask = 0x0f,  
    .eidmask = 0x00,  
    .mode = 3,  
    };  
      
    struct can_filter not_filter = {  
    .sidmask = 0,  
    .eidmask = 0,  
    .mode = 0,  
    };  
      
    device = open(argv[1], O_RDWR);  
    if (device ==  0){  
    perror("open:device");  
    exit(-1);  
    }  
    if (argc > 2) {  
    if (strcmp(argv[2], "-f") != 0) {  
        device1 = open(argv[2], O_RDWR);  
        if (device1 ==  0) {  
        perror("open:device1");  
        exit(-1);  
        }  
    }  
    }     
      
    int i,can_id = 1;  
    int rate;  
    int len;  
      
    ioctl(device, CAN_IOCSFILTER, &filter);  
    ioctl(device, CAN_IOCSRATE, 250000);  
    ioctl(device, CAN_IOCGRATE, &rate);  
    printf("%s Work Rate: %dbps\n", argv[1], rate);  
      
    if (device1 > 0) {  
    	ioctl(device1, CAN_IOCSFILTER, &filter);  
    	ioctl(device1, CAN_IOCSRATE, 250000);  
    	if (argc > 3) {  
        	if (strcmp(argv[3], "-f") == 0) {  
        		ioctl(device1, CAN_IOCSFILTER, &filter);  
        	}  
    	}  
    	printf("%s Work Rate: %dbps\n", argv[2], rate);  
    } else {      
    	if (argc > 2) {  
        	if (strcmp(argv[2], "-f") == 0) {  
        		ioctl(device, CAN_IOCSFILTER, &filter);  
        	}  
    	}  
    	ioctl(device, CAN_IOCTLOOPBACKMODE);  
    	printf("%s work on loopback mode.\n", argv[1]);  
    }  
      
    pthread_attr_t attr;  
    pthread_t thread_recv;  
    pthread_attr_init(&attr);  
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);  
    if (pthread_create(&thread_recv, NULL, can_read, 0)) {  
    perror("thread_create");  
    exit(-1);  
    }  
      
    if (((argc == 3) && (strcmp(argv[2], "-f") == 0)) || ((argc > 3) && (strcmp(argv[3], "-f") == 0))) {  
    printf("\nTest in Filter mode. \n");  
    printf("Only receive the frame that it's id & 0xf == [3...8]\n");   
    printf("The frame data=id(id=1...2031), and eid=id*2+1\n");  
    }  
    else {  
    printf("\nPress \"\\q\" to quit! \n");  
    printf("Input the message and press Enter to send!\n");  
    printf("The frame id=1...2031, and eid=id*2+1\n");  
    }  
      
    while(1)  
    {  
    	//if (getchar() == 'q') break;  
              
	//printf("on va passer le memset connard\n");
   	memset(&frame, 0, sizeof(struct can_frame));
	//printf("youpi\n");
    	frame.header.id = can_id;  
      
    	if ((can_id % 2) == 0) {  
        	frame.header.ide = 1;  
        	frame.header.eid = can_id * 2 + 1;  
    	}else{  
        	frame.header.ide = 0;  
        	frame.header.eid = 0;  
    	}  

	//printf("blop\n");
      
      
    	if (((argc > 2) && (strcmp(argv[2], "-f") == 0)) || ((argc > 3) && (strcmp(argv[3], "-f") == 0))) {  
        	sprintf(frame.data, "%d", can_id);  
        	frame.header.dlc = strlen(frame.data);  
		printf("yaourt !\n");
        	write(device, &frame, sizeof(struct can_frame));  
		printf("blourp\n");
        	sleep(1);  
    	}  
    	else {  
        	printf("\nCanTest> ");  
        	scanf("%s", buf);  
        	if (strcmp(buf, "\\q") == 0)  
        	break;  
          
        	len = strlen(buf);  
        	pbuf = buf;  
		printf("%s \n", pbuf);
        	for (len; len>CAN_FRAME_MAX_DATA_LEN; len-=CAN_FRAME_MAX_DATA_LEN) {  
			//printf("Dans le for ! %s \n", pbuf);
        		memcpy(frame.data, pbuf, CAN_FRAME_MAX_DATA_LEN);  
        		frame.header.dlc = CAN_FRAME_MAX_DATA_LEN;  
        		write(device, &frame, sizeof(struct can_frame));  
        		pbuf += CAN_FRAME_MAX_DATA_LEN;  
        	}  
        	if (len > 0) {  
        		frame.header.dlc = len;  
			memcpy(frame.data, pbuf, len);  
			//printf("%s \n", frame.data);
        		write(device, &frame, sizeof(struct can_frame));  
        	}  
    	}  
    	can_id++;  
    	if (can_id > 2031) {  
        	can_id = 1;  
    	}  
    }  
      
    close(device);  
    if (device1 > 0)  
    	close(device1);  
    return 0;  
}
