#ifndef __ECAN_DMA_H__
#define __ECAN_DMA_H__


/* CAN Baud Rate Configuration */
#define FCAN 40000000
#define BITRATE 1000000
#define NTQ 20 /*20 time quanta in a bit time*/
#define BRP_VAL ((FCAN/(2*NTQ*BITRATE))-1)


/* Filter and mask defines */
/* Macros used to write filter/mask ID to Register CiRXMxSID and CiRXFxSID */
/* For example, to set up the filter to accept a value of 0x123, the macro when called as */
/* CAN_FILTERMASK2REG_SID(0x123) will write the register space to accept message with ID 0x123 */
/* Use for Standard Messages Only */
#define CAN_FILTERMASK2REG_SID(x) ((x & 0x07FF)<<5)
/* The Macro will set the MIDE bit in CiRXMxSID */
#define CAN_SETMIDE(sid) (sid|0x0008)
/* The Macro will set the EXIDE bit in CiRXFxSID to only accept extended messages */
#define CAN_FILTERXTD(sid) (sid|0x0008)
/* The macro will clear the EXIDE bit in CiRXFxSID to only accept standard messages */
#define CAN_FILTERSTD(sid) (sid & 0xFFF7)
/* Macro used to write filter/mask ID to Register CiRXMxSID, CiRXMxEID, CiRXFxSID and CiRXFxEID */
/* For example, to set up the filter to accept a value of 0x123, the macro when called as */
/* CAN_FILTERMASK2REG_SID(0x123) will write the register space to accept message with ID 0x123 */
/* Use for Extended Messages only*/
#define CAN_FILTERMASK2REG_EID0(x) (x & 0xFFFF)
#define CAN_FILTERMASK2REG_EID1(x) (((x & 0x1FFC)<<3)|(x & 0x3))


/* message structure in RAM for CAN */
typedef struct{
    /* Keep track of the buffer status */
    unsigned char buffer_status;
    /* RTR message or data message */
    unsigned char message_type;
    /* Frame type extended or standard */
    unsigned char frame_type;
    /* Buffer being used to send and receive messages */
    unsigned char buffer;
    /* 29 bit ID max of 0x1FFF FFFF */
    /* 11 bit ID max of 0x7FF */
    unsigned long id;
    unsigned char data[8];
    unsigned char data_length;
}mID;


/* ECAN message type identifiers */
#define CAN_BUF_EMPTY 0x06
#define CAN_MSG_DATA 0x01
#define CAN_MSG_RTR 0x02
#define CAN_FRAME_EXT 0x03
#define CAN_FRAME_STD 0x04
#define CAN_BUF_FULL 0x05

/*Usefull modes of the can bus*/
#define CAN_CONFIGURATION_MODE 0x4
#define CAN_LOOPBACK_MODE 0x2
#define CAN_NORMAL_MODE 0x0

#endif /*__ECAN_DMA_H__*/
