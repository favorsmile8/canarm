/**
 * Header file for signal formation
 */

#ifndef _SIGNAL_FORM_H_
#define _SIGNAL_FORM_H_

#include<stdio.h>
#include<stdlib.h>


/*Structure containing information needed by the PIC
 *an given by IA*/
typedef struct{
  unsigned int
    error :1, /*an error occured*/
    OVT :1, /*over temperature*/
    unused :14, /*not used bits*/
    Isign :1, /*Sign of the current (0 positive, 1 negative)*/
    Isc :15; /*Current in the device*/
  short int Vsc; /*tension super capa*/
  short int Vdc; /*tension bus super capa*/
}PICtoARM;

/*Measurement mabe by the PIC and needed by IA*/
typedef struct{
  float current; /*current to take from or give to the device*/
}ARMtoPIC;




#endif /*_SIGNAL_FORM_H_*/
