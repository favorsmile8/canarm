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

void LEDon(int on);

int main()
{
    unsigned int config1 = 0;
    unsigned int config2 = 0;

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

    /*Set operation mode : loopback to test -> put it in normal when it works*/
    config1 = CAN_IDLE_STOP &
            CAN_REQ_OPERMODE_LOOPBK &
            CAN_CAPTURE_DISABLE &
            CAN_SFR_FILTER_WIN;
    config2 = CAN_CMP_DATABYTE1_BIT7_EID0;

    CAN1SetOperationMode(config1, config2);

    /*Configure FIFO*/

    /*Configure filters*/

    /*Set buffer pointers*/

    /*loop*/
    while(1){
        /*Read CAN bus*/
        
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


