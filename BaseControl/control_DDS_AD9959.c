/****************************************************************/
/* FILENAME: control_DDS_AD9959.c                               */
/* DESCRIPTION: This program recieves commands from             */
/* serial port on MSP430F169 and pushes info to                 */
/*    DDS_9959 via 3 wire spi port                              */
/*    AUTHORS: WF & ST, LPL                          */
/*    CREATED june 2016                                         */
/*    VERSION: 02                                               */
/* IAR Embedded Workbench KickStart for MSP430 Version: 4.11A   */
/****************************************************************/

#include <msp430x16x.h>
#include "control_DDS_AD9959.h"

/**************** Function prototypes ***********************************/
/*                                                                      */
void init_sys(void);                // MSP430 Initialisation routine
void ad9959_init(void);                 // AD9959 Initialisation routine
void tempo_loop(int loop_number);             // wait
void write_serial(int instruction, long data, int num_byte);
void refresh_ad9959(void);
void write_to_channel_0(long freq);
void write_to_channel_1(long freq);
unsigned long chars_to_long(unsigned char input[]);
unsigned long input_freq, command_form_freq;
void write_immediate(void);

/************************************************************************/
/*                                                                      */
/* main() variable declarations                                         */
/*                                                                      */

short FR1_ADRESS = 0x01;                  // AD9959 FR1 adresss Byte
long FR1_NUM_BYTE = 0x03;                  // AD9959 byte number by FR1  */
long FR1 = 0xD00000;                      // VCO high gain, PLLx20
//long FR1 = 0xA00000;                      // VCO high gain, PLLx8
                    /* DDS AD9959 */
//CSR: CHANNEL SELECT REGISTER in AD9959
short CSR_ADRESS = 0x00;                  // AD9959 CSR adresss Byte
long CSR0 = 0x10;                         // AD9959 CH0  MSB first
long CSR1 = 0x20;                         // AD9959 CH1  MSB first
//long CSR2 = 0x40;                         // AD9959 CH2  MSB first
//long CSR3 = 0x80;                         // AD9959 CH3  MSB first
int CSR_NUM_BYTE = 0x01;                  // AD9911 byte number by CSR


short FTW_ADRESS = 0x04;                  // AD9959 FTW adresss Byte
long FTW0 = 0x51EB851;                  // AD9959 Frequency Tuning Word_0 10 MHz @Clk=500MHz
long FTW1 = 0x51EB851;                  // AD9959 Frequency Tuning Word_1 10 MHz
//long FTW2 = 0x51EB851;                // AD9959 Frequency Tuning Word_2 20 MHz
//long FTW3 = 0x51EB851;                // AD9959 Frequency Tuning Word_3 10 MHz
int FTW_NUM_BYTE = 0x04;                  // AD9959 byte number by FTW
unsigned char temp[4];
int t = 0;
char i = 0;
char j = 0;

void main(void)
{
  init_sys();                   // Initialise the MSP430 & AD9959
  __bis_SR_register(LPM0_bits + GIE);// Enter LPM0/interrupt
}
/************************************************************/
/* Prototype - init_sys                                     */
/*                                                          */
/*  Description                                             */
/*  This prototype initialises the MSP430F169               */
/************************************************************/
void init_sys(void)
{
  WDTCTL = WDTPW + WDTHOLD;   // stop Watch Dog Timer
  msp430_init();   // init usart1, spi mode, 2wire
  tempo_loop(1000);                    // wait loop
  
  //Set other ports to 0;
  P4OUT = 0x00;                         
  P3OUT = 0x00;
  
  ad9959_init();    //initialise ad9959
  
}
/************************************************************/
/* Prototype - init_ad9959                                     */
/*                                                          */
/*  Description                                             */
/*  Initialises the ad9959 DDS chip               */
/************************************************************/
void ad9959_init(void)
{
  
  refresh_ad9959();                     // Reset AD9959
  tempo_loop(1000);                    // wait loop
  
  //P4OUT =0x08;       // CS2 = 1 CS1 = 0 select AD9959
  write_serial(FR1_ADRESS, FR1, FR1_NUM_BYTE); // write FR1 to AD9911
  tempo_loop(10);                       // wait loop
  refresh_ad9959();
    
  write_to_channel_0(FTW0);
  tempo_loop(1000);                    // wait loop
  
  write_to_channel_1(FTW1);
  tempo_loop(1000);                    // wait loop

  refresh_ad9959();

}
/************************************************************/
/* Prototype - refresh_ad9959                           */
/*                                                          */
/*  Description                                             */
/*  Sends a pulse to refresh parameters on ad9959   */
/************************************************************/
void refresh_ad9959(void)
{
  //validation des 4 channel de l'AD9559
  P5OUT = 0x40;                         // AD9959 I/O update
  P5OUT = 0x00; 
}
/************************************************************/
/* Prototype - write_to_channel_0                           */
/*                                                          */
/*  Description                                             */
/*  writes frequency to channel 0 of AD9959   */
/************************************************************/
void write_to_channel_0(long freq)
{
//configuration du DDS AD9959 channel 0
  write_serial(CSR_ADRESS, CSR0, CSR_NUM_BYTE);
                                        // write CSR0 to AD9959
  write_serial(FTW_ADRESS, freq, FTW_NUM_BYTE);
                                        // write frequency to AD9959
}
/************************************************************/
/* Prototype - write_to_channel_1                           */
/*                                                          */
/*  Description                                             */
/*  writes frequency to channel 1 of AD9959   */
/************************************************************/
void write_to_channel_1(long freq)
{
//configuration du DDS AD9959 channel 1
  write_serial(CSR_ADRESS, CSR1, CSR_NUM_BYTE);
                                        // write CSR0 to AD9959
  write_serial(FTW_ADRESS, freq, FTW_NUM_BYTE);
                                        // write frequency to AD9959
}
/************************************************************/
/* Prototype - write_serial                           */
/*                                                          */
/*  Description                                             */
/*  This prototype write the AD9959 Frequency Tuning Word   */
/*  on USART1                                               */
/************************************************************/
void write_serial(int instruction, long data, int num_byte)
{
  int i=0;

  TXBUF1 = instruction;
  do
  {
    TXBUF1 = data >>(num_byte-1-i)*8;
    i++;
  }
  while (i < num_byte);
}

/************************************************************/
/* Prototype - tempo_loop                                   */
/*                                                          */
/*  Description                                             */
/*  This prototype wait i loop                              */
/************************************************************/
void tempo_loop(int loop_number)
{
  int i;
  int j;
  for(i = 0; i < loop_number; ++i)          // wait loop
  {
    j=j+i;
  }
}

/************************************************************/
/* Prototype - Incoming message interrupt                                   */
/*                                                          */
/*  Description                                             */
/*  Treat incoming message from serial port                             */
/************************************************************/
// UART0 RX ISR
#pragma vector=USART0RX_VECTOR
__interrupt void usart0_rx (void)
{
    temp[t++] = RXBUF0; //Read from buffer
    if(t > 3)
    {
      write_immediate();      
      t = 0; //Reset read length
    }
}

/************************************************************/
/* Prototype - write_immediate                              */
/*                                                          */
/*  Description                                             */
/*  Reads incoming frequency and send to DDS immediately    */
/************************************************************/
void write_immediate()
{
  input_freq = (unsigned long) temp[3] << 24 | (unsigned long) temp[2] << 16 | (unsigned long) temp[1] << 8 | temp[0]; //Convert to unsigned long
  command_form_freq = input_freq * 4294967296 / 500000000; //Convert to command for DDS
  write_to_channel_0(command_form_freq); //Issue command
  //tempo_loop(10);                       // wait loop
  refresh_ad9959(); //Execute
}