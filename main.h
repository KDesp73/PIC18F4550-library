#include <18F4550.h>
#DEVICE HIGH_INTS=TRUE
#device adc=10

#FUSES NOWDT                     //No Watch Dog Timer
#FUSES WDT32768                    //Watch Dog Timer uses 1:128 Postscale
#FUSES HSPLL                     //High Speed Crystal/Resonator with PLL enabled
#FUSES NOPROTECT                 //Code not protected from reading
#FUSES BROWNOUT                //No brownout reset
//#FUSES BORV20                    //Brownout reset at 2.0V
#FUSES NOPUT                     //No Power Up Timer
#FUSES NOCPD                     //No EE protection
#FUSES STVREN                    //Stack full/underflow will cause reset
#FUSES NODEBUG                   //No Debug mode for ICD
#FUSES NOLVP                     //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOWRT                     //Program memory not write protected
#FUSES NOWRTD                    //Data EEPROM not write protected
#FUSES NOIESO                      //Internal External Switch Over mode enabled
#FUSES NOFCMEN                     //Fail-safe clock monitor enabled
#FUSES NOPBADEN                 ///PORTB pins are configured as analog input channels on RESET
#FUSES NOWRTC                    //configuration not registers write protected
#FUSES NOWRTB                    //Boot block not write protected
#FUSES NOEBTR                    //Memory not protected from table reads
#FUSES NOEBTRB                   //Boot block not protected from table reads
#FUSES NOCPB                     //No Boot Block code protection
#FUSES MCLR                      //Master Clear pin enabled
#FUSES NOLPT1OSC                   //Timer1 configured for low-power operation
#FUSES NOXINST                   //Extended set extension and Indexed Addressing mode disabled (Legacy mode)
#FUSES PLL5                      //Divide By 12(48MHz oscillator input)
#FUSES CPUDIV1                   //System Clock by 2
#FUSES USBDIV                    //USB clock source comes from PLL divide by 2
#FUSES VREGEN                    //USB voltage regulator enabled
#FUSES ICPRT                     //ICPRT enabled
#FUSES WRTB
#FUSES NOICPRT 
#FUSES NOLPT1OSC
#FUSES CCP2B3 
#FUSES NOCPB
#FUSES NOCPD
#FUSES NOWRTC
#FUSES NOWRTD
#FUSES NOEBTR
#FUSES NOEBTRB              

/* systm clock is 48 MHz */
#use delay(clock=48000000,RESTART_WDT)
/* --- BEGIN: changes required for bootloader ------------------------------ */

/* ------------------------------------------------------------------------- */
/* map reset vector and interrupt vector                                     */
/* 0x000-0x7FF is used by the bootloader. The bootloader maps the original   */
/* reset vecotr (0x000) to 0x800 and the reset vector (0x008) to 0x800.      */
/* ------------------------------------------------------------------------- */
#build (reset=0x800, interrupt=0x808)

/* ------------------------------------------------------------------------- */
/* reserve boot block area                                                   */
/* This memory range is used by the bootloader, so the application must not  */
/* use this area.                                                            */
/* ------------------------------------------------------------------------- */
#org 0, 0x7FF {}

/* --- END: changes required for bootloader -------------------------------- */




