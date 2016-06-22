//******************************************************************************
//  usart1_spi_2_wire_init.c
//  MSP430F169 - USART1, SPI Interface 2 wire + FRAME + DATAVALIDE
//
//  Description: USART1 in SPI mode interface 3 wire .
//  USART1 is used to transmit data, software generated frame sync
//  pulse, with SMCLK
//
//  Using DAC12.0 and 2.5V ADC12REF reference with a gain of 1,
//  output 2V on P6.6. Output accuracy is specified by that of the ADC12REF.
//
//
//  //* SWRST** please see MSP430x1xx Users Guide for description *//
//
//
//               MSP430F169
//            -----------------
//        /|\|                 |
//         | |                 |
//         --|RST              |       --------------
//           |                 |       |             |
//           |       SIMO1/P5.1|------>|DIN          |
//           |       UCLK1/P5.3|------>|SCLK         |
//           |             P5.6|------>|DATAVALID    |
//           |                 |       |             |
//           |                 |       |             |
//           |        DAC0/P6.6|--> Out Ananlog
//     Vin-->|P6.3/ADC3        |
//            P6.4/ADC4
//            P6.5/ADC5
//  olivier lopez
//  sep 2008
//  IAR Embedded Workbench KickStart for MSP430 Version: 4.11A
//******************************************************************************

#include  <msp430x16x.h>


void msp430_init(void)
{
  //Pins config: O means GPIO, 1 means use pin's special function
  P1SEL = 0x00;                             // P1 I/O select
  P2SEL = 0x00;                             // P2 I/O select
  P3SEL = 0x30;                             // P3 4,5 usart0 select
  P4SEL = 0x00;                             // P4 I/O select
  P5SEL = 0x1A;                             // P5.1,3 SPI option select P5.4
  P6SEL = 0xB8;                             // P6.3 P6.4 P6.5 ADC DAC_1 options select
  
  //Direction: 0 = input, 1 = output
  P1DIR = 0xFF;                             // P1 output direction
  P2DIR = 0xFF;                             // P2 output direction
  P3DIR = 0xDF;                             // P3 output direction
  P4DIR = 0xFF;                             // P4 output direction
  P5DIR = 0xFF;                             // P5 output direction
  P6DIR = 0xCF;                             // P6.4,5 input/other output
  
  //Module enable register 2
  ME2 |= USPIE1;                            // Enable USART1 SPI
  
  UCTL1 |= CHAR + SYNC + MM;                // 8-bit SPI Master **SWRST**
  UTCTL1 = CKPH + SSEL1 + STC;              // SMCLK delayed, 3-pin
  UBR01 = 0x2;                              // ACLK/2 for baud rate
  UBR11 = 0x0;                              // ACLK/2 for baud rate
  UMCTL1 = 0x0;                             // Clear modulation
  UCTL1 &= ~SWRST;                          // Initialize USART state machine

  // init UART0

  ME1 |= UTXE0 + URXE0;                     // Enable USART0 TXD/RXD
  UCTL0 |= CHAR;                            // 8-bit character
  UTCTL0 |= SSEL0;                          // UCLK= ACLK
  //UBR00=0x41;                               // 8MHz 9600
  //UBR10=0x03;                               // 8MHz 9600
  //UMCTL0=0x09;                              // 8MHz 9600  modulation
  //UBR00=0x8E;                               // 8MHz 56000
  //UBR10=0x00;                               // 8MHz 56000
  //UMCTL0=0xF7;                              // 8MHz 56000  modulation
  UBR00=0xA0; // 8MHz 192000
  UBR10=0x01; // 8MHz 192000
  UMCTL0=0x5B; // 8MHz 192000 modulation
  UCTL0 &= ~SWRST;                          // Initialize USART0 state machine
  IE1 |= URXIE0 + UTXIE0;                   // Enable USART0 RX/TX interrupt
  IFG1 &= ~UTXIFG0;                         // Clear inital flag on POR
  BCSCTL1 |= XTS;                           // ACLK = LFXT1 = HF XTAL
  BCSCTL2 |= SELM_3;                        // MCLK= LFXT1 (safe)


}