#define _PIC33FJ64_

#ifdef _PIC33FJ64_
#include <p33FJ64MC802.h>
#else
#include <p33FJ128MC802.h>
#endif /*_PIC33FJ64_*/

#include<ecan.h>
#include<dma.h>
#include<string.h>
#include"ecan_dma.h"

/*Configure the oscillator because we have to know it frequency for
 *bit timing configuration
 *Frequency is set to 40MHz*/
_FOSCSEL( FNOSC_FRC )/* Oscillateur interne FRC avec PLL*/
_FOSC( FCKSM_CSECMD & IOL1WAY_OFF & POSCMD_NONE & OSCIOFNC_ON )
_FPOR( FPWRT_PWR128 )/* Tempo 128ms à la mise sous tension*/
_FICD(ICS_PGD1)


/* ECAN message buffer length */
#define ECAN1_MSG_BUF_LENGTH 4
typedef unsigned int ECAN1MSGBUF[ECAN1_MSG_BUF_LENGTH][8];

/**
 * DMA buffers for ECAN
 */
ECAN1MSGBUF ecan1msgBuf __attribute__((space(dma),aligned(ECAN1_MSG_BUF_LENGTH*16)));

mID canRxMessage;

/* Interrupt Service Routine 1
 No fast context save, and no variables stacked */
void __attribute__((interrupt, no_auto_psv))_C1Interrupt(void)
{
    /* check to see if the interrupt is caused by receive */
    if(C1INTFbits.RBIF)
    {
        /*check to see if buffer 1 is full */
        if(C1RXFUL1bits.RXFUL1)
        {
            /* set the buffer full flag and the buffer received flag */
            canRxMessage.buffer_status=CAN_BUF_FULL;
            canRxMessage.buffer=1;
        }
        /* check to see if buffer 2 is full */
        else if(C1RXFUL1bits.RXFUL2)
        {
            /* set the buffer full flag and the buffer received flag */
            canRxMessage.buffer_status=CAN_BUF_FULL;
            canRxMessage.buffer=2;
        }
        /* check to see if buffer 3 is full */
        else if(C1RXFUL1bits.RXFUL3)
        {
            /* set the buffer full flag and the buffer received flag */
            canRxMessage.buffer_status=CAN_BUF_FULL;
            canRxMessage.buffer=3;
        }

        /* clear flag */
        C1INTFbits.RBIF = 0;
    }
    else if(C1INTFbits.TBIF)
    {
        /* clear flag */
        C1INTFbits.TBIF = 0;
    }
    else;
    /* clear interrupt flag */
    IFS2bits.C1IF = 0;
}

/**
 * Prototypes
 */
void rxECAN(mID * message);
void LEDon(int on);
void initOSC();
void initECAN();
void initDMA();
void initInterrupt();
void disableUserInterrupt();

int main()
{
    /*Save state of the DMA buffer for test purpose*/
    char tmp[8];

    /*Write message*/
    mID msg;
    mID* message = &msg;
    unsigned long word0 = 0;
    unsigned long word1 = 0;
    unsigned long word2 = 0;

    initOSC();
    
    LEDon(0);

    initECAN();

    initDMA();
    
    initInterrupt();
    
    memset(msg.data, 42, 8);
    msg.data_length = 8;
    msg.id = 0x123;
    msg.message_type = CAN_MSG_DATA;
    msg.frame_type = CAN_FRAME_STD;
    msg.buffer = 0;

    /* check to see if the message has an extended ID */
    if(message->frame_type==CAN_FRAME_EXT)
    {
        /* get the extended message id EID28..18*/
        word0=(message->id & 0x1FFC0000) >> 16;
        /* set the SRR and IDE bit */
        word0=word0+0x0003;
        /* the the value of EID17..6 */
        word1=(message->id & 0x0003FFC0) >> 6;
        /* get the value of EID5..0 for word 2 */
        word2=(message->id & 0x0000003F) << 10;
    }
    else
    {
        /* get the SID */
        word0=((message->id & 0x000007FF) << 2);
    }
    /* check to see if the message is an RTR message */
    if(message->message_type==CAN_MSG_RTR)
    {
        if(message->frame_type==CAN_FRAME_EXT)
        word2=word2 | 0x0200;
        else
        word0=word0 | 0x0002;
        ecan1msgBuf[message->buffer][0]=word0;
        ecan1msgBuf[message->buffer][1]=word1;
        ecan1msgBuf[message->buffer][2]=word2;
    }
    else
    {
        word2=word2+(message->data_length & 0x0F);
        ecan1msgBuf[message->buffer][0]=word0;
        ecan1msgBuf[message->buffer][1]=word1;
        ecan1msgBuf[message->buffer][2]=word2;
        /* fill the data */
        ecan1msgBuf[message->buffer][3]=((message->data[1] << 8) + message->data[0]);
        ecan1msgBuf[message->buffer][4]=((message->data[3] << 8) + message->data[2]);
        ecan1msgBuf[message->buffer][5]=((message->data[5] << 8) + message->data[4]);
        ecan1msgBuf[message->buffer][6]=((message->data[7] << 8) + message->data[6]);
    }

    /* set the message for transmission*/
    C1TR01CONbits.TXREQ0=1;
    while(C1TR01CONbits.TXREQ0 == 1);

    while(C1RXFUL1bits.RXFUL1 == 0);

    memcpy(tmp, &ecan1msgBuf[1][3], 8);

    disableUserInterrupt();

    return 0;
}

void LEDon(int on)
{
    TRISBbits.TRISB15 = 0;

    LATBbits.LATB15 = on;
}

void rxECAN(mID * message)
{
    unsigned int ide=0;
    unsigned int rtr=0;
    unsigned long id=0;
    /* check to see what type of message it is
    */
    /* message is standard identifier
    */
    if(ide==0)
    {
        message->id=(ecan1msgBuf[message->buffer][0] & 0x1FFC) >> 2;
        message->frame_type=CAN_FRAME_STD;
        rtr=ecan1msgBuf[message->buffer][0] & 0x0002;
    }
    /* mesage is extended identifier */
    else
    {
        id=ecan1msgBuf[message->buffer][0] & 0x1FFC;
        message->id=id << 16;
        id=ecan1msgBuf[message->buffer][1] & 0x0FFF;
        message->id=message->id+(id << 6);
        id=(ecan1msgBuf[message->buffer][2] & 0xFC00) >> 10;
        message->id=message->id+id;
        message->frame_type=CAN_FRAME_EXT;
        rtr=ecan1msgBuf[message->buffer][2] & 0x0200;
    }
    /* check to see what type of message it is */
    /* RTR message */
    if(rtr==1)
    {
        message->message_type=CAN_MSG_RTR;
    }
    /* normal message */
    else
    {
        message->message_type=CAN_MSG_DATA;
        message->data[0]=(unsigned char)ecan1msgBuf[message->buffer][3];
        message->data[1]=(unsigned char)((ecan1msgBuf[message->buffer][3] & 0xFF00) >> 8);
        message->data[2]=(unsigned char)ecan1msgBuf[message->buffer][4];
        message->data[3]=(unsigned char)((ecan1msgBuf[message->buffer][4] & 0xFF00) >> 8);
        message->data[4]=(unsigned char)ecan1msgBuf[message->buffer][5];
        message->data[5]=(unsigned char)((ecan1msgBuf[message->buffer][5] & 0xFF00) >> 8);
        message->data[6]=(unsigned char)ecan1msgBuf[message->buffer][6];
        message->data[7]=(unsigned char)((ecan1msgBuf[message->buffer][6] & 0xFF00) >> 8);
        message->data_length=(unsigned char)(ecan1msgBuf[message->buffer][2] & 0x000F);
    }
}

void initOSC()
{
    /*Configurer l'oscillateur pour 39,614 MIPS,~40Mhz
     * Fosc= Fin*M/(N1*N2), Fcy=Fosc/2, où M = PLLFBD + 2,  N1 = 2,  N2 = 2
     * Fosc= 7.37*(43)/(2*2)=79.225 Mhz pour Fosc, Fcy = 39.614 Mhz (39613750)
     */
    PLLFBD = 41;/* Facteur de la PLL*/
    CLKDIVbits.PLLPOST = 0;/* Pas de division d'horloge*/
    CLKDIVbits.PLLPRE = 0;
}

void initECAN()
{
    C1CTRL1bits.WIN = 0x0;

    C1CTRL1bits.REQOP=0x4;
    while (C1CTRL1bits.OPMODE!=0x4);

    /* FCAN is selected to be FCY */
    /* FCAN = FCY = 40 MHz */
    C1CTRL1bits.CANCKS = 0x1;
    /*
    Bit Time = (Sync Segment + Propagation Delay + Phase Segment 1 + Phase Segment 2) = 20 * TQ
    Phase Segment 1 = 8 TQ
    Phase Segment 2 = 6 TQ
    Propagation Delay = 5 TQ
    Sync Segment = 1 TQ
    CiCFG1<BRP> =(FCAN/(2 * N * FBAUD))- 1
    BIT RATE OF 1 Mbps
    */
    C1CFG1bits.BRP = BRP_VAL;
    /* Synchronization Jump Width set to 4 TQ */
    C1CFG1bits.SJW = 0x3;
    /* Phase Segment 1 time is 8 TQ */
    C1CFG2bits.SEG1PH = 0x7;
    /* Phase Segment 2 time is set to be programmable */
    C1CFG2bits.SEG2PHTS = 0x1;
    /* Phase Segment 2 time is 6 TQ */
    C1CFG2bits.SEG2PH = 0x5;
    /* Propagation Segment time is 5 TQ */
    C1CFG2bits.PRSEG = 0x4;
    /* Bus line is sampled three times at the sample point */
    C1CFG2bits.SAM = 0x1;

    /*Assign 4 buffers*/
    C1FCTRLbits.DMABS = 0x0;

    C1CTRL1bits.WIN = 0x1;

    /* Select acceptance mask 0 filter 0 buffer 1 */
    C1FMSKSEL1bits.F0MSK = 0;
    /* Configure acceptance mask - match the ID in filter 0 */
    /* setup the mask to check every bit of the standard message, the macro when called as */
    /* CAN_FILTERMASK2REG_SID(0x7FF) will write the register C1RXM0SID to include every bit in */
    /* filter comparison */
    C1RXM0SID=CAN_FILTERMASK2REG_SID(0x7FF);
    /* Configure acceptance filter 0 */
    /* Set up the filter to accept a standard ID of 0x123, the macro when called as */
    /* CAN_FILTERMASK2REG_SID(0x123) will write the register C1RXF0SID to only accept standard */
    /* ID of 0x123 */
    C1RXF0SID=CAN_FILTERMASK2REG_SID(0x123);
    /* Set filter to check for standard ID and accept standard ID only */
    CAN_SETMIDE(C1RXM0SID);
    CAN_FILTERSTD(C1RXF0SID);
    /* Acceptance filter to use buffer 1 for incoming messages */
    C1BUFPNT1bits.F0BP = 0x1;
    /* Enable filter 0 */
    C1FEN1bits.FLTEN0 = 1;

    C1CTRL1bits.WIN = 0x0;

    /*Put the module in normal mode*/
    C1CTRL1bits.REQOP = 0x2;
    while(C1CTRL1bits.OPMODE != 0x2);

    C1TR01CONbits.TXEN0 = 1;/* ECAN1, Buffer 0 is a Transmit Buffer */
    C1TR01CONbits.TXEN1 = 0;/* ECAN1, Buffer 1 is a Receive Buffer */
    C1TR01CONbits.TX0PRI = 0x3;/* Message Buffer 0 Priority Level */
}

void initDMA()
{
    /* Initialize the DMA channel 0 for ECAN TX and clear the colission flags */
    DMACS0 = 0;
    /* Set up Channel 0 for peripheral indirect addressing mode normal operation, word operation */
    /* and select TX to peripheral */
    DMA0CON = 0x2020;
    /*Set the peripheral IRQ number*/
    DMA0REQbits.IRQSEL = 0x46;
    /* Set up the address of the peripheral ECAN1 (C1TXD) */
    DMA0PAD = 0x0442;
    /* Set the data block transfer size of 8 */
    DMA0CNT = 7;
    /* Automatic DMA TX initiation by DMA request */
    DMA0REQ = 0x0046;
    /* DPSRAM atart address offset value */
    DMA0STA = __builtin_dmaoffset(&ecan1msgBuf);
    /* Enable the channel */
    DMA0CONbits.CHEN = 1;
    /* Initialize DMA Channel 2 for ECAN RX and clear the collision flags */
    DMACS0 = 0;
    /* Set up Channel 2 for Peripheral Indirect addressing mode (normal operation, word operation */
    /* and select as RX to peripheral */
    DMA2CON = 0x0020;
    /*Set the peripheral IRQ number*/
    DMA2REQbits.IRQSEL = 0x22;
    /* Set up the address of the peripheral ECAN1 (C1RXD) */
    DMA2PAD = 0x0440;
    /* Set the data block transfer size of 8 */
    DMA2CNT = 7;
    /* Automatic DMA Rx initiation by DMA request */
    DMA2REQ = 0x0022;
    /* DPSRAM atart address offset value */
    DMA2STA = __builtin_dmaoffset(&ecan1msgBuf);
    /* Enable the channel */
    DMA2CONbits.CHEN = 1;
}

void initInterrupt()
{
    /*Enable user interrupt*/
    SRbits.IPL = 0;

    /*Set the priority of interrupt*/
    IPC17bits.C1TXIP = 0x6;
    IPC8bits.C1RXIP = 0x6;

    /* clear flag */
    C1INTFbits.RBIF = 0;
    C1INTFbits.TBIF = 0;
    IFS2bits.C1IF = 0;

    /* Enable ECAN1 interrupt */
    IEC2bits.C1IE = 1;
    /* Enable transmit interrupt */
    C1INTEbits.TBIE = 1;
    /* Enable ECAN1 receive Interrupt */
    C1INTEbits.RBIE = 1;
}

void disableUserInterrupt()
{
    /*Disable user interrupts*/
    SRbits.IPL = 7;
}

