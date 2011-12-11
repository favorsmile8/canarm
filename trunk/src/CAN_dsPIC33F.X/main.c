#define _PIC33FJ64_

#ifdef _PIC33FJ64_
#include <p33FJ64MC802.h>
/*Configure the oscillator because we have to know it frequency for
 *bit timing configuration
 *Frequency is set to 40MHz*/
_FOSCSEL(FNOSC_PRI);
_FOSC(POSCMD_HS);
#else
#include <p33FJ128MC802.h>
#endif

#include<ecan.h>
#include<string.h>

void LEDon(int on);

int main()
{
    unsigned int config1 = 0;
    unsigned int config2 = 0;
    unsigned char data[8];
    unsigned char dataTX[8];
    int i = 0;

    memset(data, 0, 8);
    memset(dataTX, 'a', 8);

    /*Initialise ECAN module
     *With ecan.h functions I hope DMA is initialised
     *by the library
     */
    config1 = CAN_SYNC_JUMP_WIDTH4 & CAN_BAUD_PRE_SCALE(4);
    config2 = CAN_WAKEUP_BY_FILTER_DIS &
            CAN_PROPAGATIONTIME_SEG_TQ(8) &
            CAN_PHASE_SEG1_TQ(8) &
            CAN_PHASE_SEG2_TQ(8) &
            CAN_SEG2_TIME_LIMIT_SET &
            CAN_SAMPLE1TIME;

    CAN1Initialize(config1, config2);

    /*Configure FIFO*/
    config1 = CAN_DMA_BUF_SIZE_32 & CAN_FIFO_AREA_RB9;

    CAN1FIFOCon(config1);

    /*Set FIFO buffer pointers
     *Each message types needs 1 TX buffer and 1 RX buffer
     */
    CAN1SetBUFPNT1(0x1122);
    CAN1SetBUFPNT2(0x3344);
    CAN1SetBUFPNT3(0x5566);
    CAN1SetBUFPNT4(0x7788);

    /*Set the direction of each buffer*/
    for(i=0 ; i<16 ; i++){
        /*buffer 0 : RX
         *buffer 1 : TX
         *...
         */
        CAN1SetTXRXMode(i, i%2);
    }

    /*Configure filters, use of filter 1
     *For other devices type, if the information to get or set are not the same
     *other filter numbers could be used
     */
    config1 = CAN_FILTER_SID(1) & CAN_RX_EID_DIS;
    config2 = CAN_FILTER_EID(0) & CAN_FILTER_EID(1);
    CAN1SetFilter(1, config1, config2);

    /*Set operation mode : loopback to test -> put it in normal when it works*/
    config1 = CAN_IDLE_STOP &
            CAN_REQ_OPERMODE_LOOPBK &
            CAN_CAPTURE_DISABLE &
            CAN_SFR_FILTER_WIN;
    config2 = CAN_CMP_DATABYTE1_BIT7_EID0;

    CAN1SetOperationMode(config1, config2);

    /*loop*/
    while(1){
        /*Send message for test purpose*/
        while(!CAN1IsTXReady(1));
        /*send a message
         *length 8 bytes, filter 1, Standard ID, data frame,
         *DMAptr, buffer from which to send
         */
        CAN1SendMessage(dataTX, 8, 1, 0, 0, C1TXD, 1);

        /*Read CAN bus*/
        while(!CAN1IsRXReady(0));
        /*Receive buffer is full*/
        CAN1ReceiveMessage(data, 8, C1RXD);

        /*Test if the message transmission is successful
         *If yes turn the led on
         */
        if(memcmp(data, dataTX, 8) == 0){
            LEDon(1);
        }

        /*If we have a request for informations*/

        /*If we have a request to change some settings on device*/
    }

    return 0;
}

void LEDon(int on)
{
    TRISBbits.TRISB15 = 0;

    LATBbits.LATB15 = on;
}


