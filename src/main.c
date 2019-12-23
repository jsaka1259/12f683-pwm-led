#include <xc.h>
#include <stdint.h>

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Detect (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif

uint8_t cnt  = 0;
uint8_t duty = 255;
int8_t  step = -1;

void __interrupt() isr(void) {
  if (T1IF) {
    TMR1H = 0xec;
    TMR1L = 0x78;
    step  = (duty == 0) ? 1 : (duty == 255) ? -1 : step;
    duty += step;
    T1IF  = 0;
  }

  if (T2IF) {
    cnt++;
    GP0  = (cnt <= duty) ? 1 : 0;
    T2IF = 0;
  }
}

void main(void) {
  OSCCON = 0x70;
  GPIO   = 0x01;
  TRISIO = 0x08;
  ANSEL  = 0x00;
  CMCON0 = 0x07;
  WPU    = 0x00;
  nGPPU  = 1;

  T1CON = 0x00;
  TMR1H = 0xec;
  TMR1L = 0x78;

  T2CON = 0x00;
  PR2   = 0x63;

  TMR1IE = 1;
  TMR2IE = 1;
  PEIE   = 1;
  GIE    = 1;

  TMR1ON = 0;
  TMR2ON = 1;

  while(1) {
    if (GP3 == 0) {
      TMR1ON = TMR1ON ^ 1;
      while (GP3 == 0);
      __delay_ms(50);
    }
  }
}

