/* SlidePot.cpp
 * Selina Song
 * Modified: [date]
 * 12-bit ADC input on ADC1 channel 5, PB18
 */
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"
#include "../inc/SlidePot.h"
#define ADCVREF_VDDA 0x000
#define ADCVREF_INT  0x200

void SlidePot::Init(void){
  ADC1->ULLMEM.GPRCM.RSTCTL = 0xB1000003; // 1) reset
  ADC1->ULLMEM.GPRCM.PWREN = 0x26000001;  // 2) activate
  Clock_Delay(24);                        // 3) wait
  ADC1->ULLMEM.GPRCM.CLKCFG = 0xA9000000; // 4) ULPCLK
  ADC1->ULLMEM.CLKFREQ = 7;               // 5) 40-48 MHz
  ADC1->ULLMEM.CTL0 = 0x03010000;         // 6) divide by 8
  ADC1->ULLMEM.CTL1 = 0x00000000;         // 7) mode
  ADC1->ULLMEM.CTL2 = 0x00000000;         // 8) MEMRES
  ADC1->ULLMEM.MEMCTL[0] = 5;             // 9) channel 5 is PB18
  ADC1->ULLMEM.SCOMP0 = 0;                // 10) 8 sample clocks
  ADC1->ULLMEM.CPU_INT.IMASK = 0;         // 11) no interrupt
}

uint32_t SlidePot::In(void){
  ADC1->ULLMEM.CTL0 |= 0x00000001;             // 1) enable conversions
  ADC1->ULLMEM.CTL1 |= 0x00000100;             // 2) start ADC
  uint32_t volatile delay=ADC1->ULLMEM.STATUS; // 3) time to let ADC start
  while((ADC1->ULLMEM.STATUS&0x01)==0x01){}    // 4) wait for completion
  return ADC1->ULLMEM.MEMRES[0];               // 5) 12-bit result
}

// constructor
SlidePot::SlidePot(uint32_t m, uint32_t b){
  slope = m;
  offset = b;
  data = 0;
  flag = 0;
  distance = 0;
}

void SlidePot::Save(uint32_t n){
  data = n;       // store ADC sample in mailbox
  distance = Convert(n);  // optional: precompute distance
  flag = 1;       // set semaphore
}

uint32_t SlidePot::Convert(uint32_t n){
  uint32_t position;
  position = ((slope * n) >> 12) + offset;
  if(position > 2000){
    position = 2000;
  }
  return position;
}

float SlidePot::FloatConvert(uint32_t input){
  return 0.00048828125*input - 0.0001812345;
}

void SlidePot::Sync(void){
  while(flag == 0){
  }
  flag = 0;
}

uint32_t SlidePot::Distance(void){
  return Convert(data);
}