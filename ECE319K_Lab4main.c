/* ECE319K_Lab4main.c
 * Traffic light FSM
 * ECE319H students must use pointers for next state
 * ECE319K students can use indices or pointers for next state
 * Put your names here or look silly
  */

#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/UART.h"
#include "../inc/Timer.h"
#include "../inc/Dump.h"  // student's Lab 3
#include <stdio.h>
#include <string.h>
// put your EID in the next line
const char EID1[] = "SJS5984"; //  ;replace abc123 with your EID
// Hint implement Traffic_Out before creating the struct, make struct match your Traffic_Out
struct State {
  char Name[12];
  uint32_t Output;
  uint32_t TimeDelay;
  const struct State *Next[8];};

#define goS &FSM[0] //use &FSM[0] to get the address of the first state in the array
#define southyellow &FSM[1] //
#define allredsouth &FSM[2]
#define walkon &FSM[3]
#define walkro1 &FSM[4]
#define roff &FSM[5]
#define walkro2 &FSM[6]
#define roff2 &FSM[7]
#define allredwalk &FSM[8]
#define goW &FSM[9]
#define westyellow &FSM[10]
#define allredwest &FSM[11]

typedef const struct State State_t;
State_t FSM[12] ={
  {"goS", 0x04000101, 200, {goS, goS, southyellow, southyellow, southyellow, southyellow, southyellow, southyellow}},
  {"southyellow", 0x04000102, 50, {allredsouth, allredsouth, allredsouth, allredsouth, allredsouth, allredsouth, allredsouth, allredsouth}},
  {"allredsouth", 0x04000104, 50, {walkon, goS, goW, goW, walkon, walkon, walkon, walkon}},
  {"walkon", 0x0C400104, 200, {walkro1, walkro1, walkro1, walkro1, walkro1, walkro1, walkro1, walkro1}},
  {"walkro1", 0x04000104, 50, {roff, roff, roff, roff, roff, roff, roff, roff}},
  {"roff", 0x00000104, 50, {walkro2, walkro2, walkro2, walkro2, walkro2, walkro2, walkro2, walkro2}},
  {"walkro2", 0x04000104, 50, {roff2, roff2, roff2, roff2, roff2, roff2, roff2, roff2}},
  {"roff2", 0x00000104, 50, {allredwalk, allredwalk, allredwalk, allredwalk, allredwalk, allredwalk, allredwalk, allredwalk}},
  {"allredwalk", 0x04000104, 50, {allredwest, allredwest, goW, goW, goW, allredwest, goW, goW}},
  {"goW", 0x04000044, 200, {westyellow, westyellow, westyellow, westyellow, westyellow, westyellow, westyellow, westyellow}},
  {"westyellow", 0x04000084, 50, {allredwest, allredwest, allredwest, allredwest, allredwest, allredwest, allredwest, allredwest}},
  {"allredwest", 0x04000104, 50, {goS, goS, goW, goS, walkon, goS, walkon, goS}}};

State_t *Pt; // state pointer

// initialize all 6 LED outputs and 3 switch inputs
// assumes LaunchPad_Init resets and powers A and B
void Traffic_Init(void){ // assumes LaunchPad_Init resets and powers A and B
 // write this 
  IOMUX->SECCFG.PINCM[PB0INDEX] = 0x00000081; 
  IOMUX->SECCFG.PINCM[PB1INDEX] = 0x00000081; 
  IOMUX->SECCFG.PINCM[PB2INDEX] = 0x00000081; 

  IOMUX->SECCFG.PINCM[PB6INDEX] = 0x00000081; 
  IOMUX->SECCFG.PINCM[PB7INDEX] = 0x00000081; 
  IOMUX->SECCFG.PINCM[PB8INDEX] = 0x00000081; 


  IOMUX->SECCFG.PINCM[PB15INDEX] = 0x00040081; 
  IOMUX->SECCFG.PINCM[PB16INDEX] = 0x00040081; 
  IOMUX->SECCFG.PINCM[PB17INDEX] = 0x00040081; 

  GPIOB->DOE31_0 |= 0x0C4001C7;
}
/* Activate LEDs
* Inputs: west is 3-bit value to three east/west LEDs
*         south is 3-bit value to three north/south LEDs
*         walk is 3-bit value to 3-color positive logic LED on PB22,PB26,PB27
* Output: none
* - west =1 sets west green
* - west =2 sets west yellow
* - west =4 sets west red
* - south =1 sets south green
* - south =2 sets south yellow
* - south =4 sets south red
* - walk=0 to turn off LED
* - walk bit 22 sets blue color
* - walk bit 26 sets red color
* - walk bit 27 sets green color
* Feel free to change this. But, if you change the way it works, change the test programs too
* Be friendly*/
void Traffic_Out(uint32_t west, uint32_t south, uint32_t walk){
  const uint32_t westTable[8] = {
    0,(1<<6),(1<<7),0,(1<<8),0,0,0
  };
  const uint32_t southTable[8] = {
    0,(1<<0),(1<<1),0,(1<<2),0,0,0
  };
  const uint32_t walkTable[8] = {
    0,(1<<22),(1<<26),(1<<22)|(1<<26),(1<<27),(1<<22)|(1<<27),(1<<26)|(1<<27),(1<<22)|(1<<26)|(1<<27)        
  };
  GPIOB->DOUT31_0 &= ~0x0C4001C7;
  GPIOB->DOUT31_0 |= westTable[west] | southTable[south] | walkTable[walk];
}

/* Read sensors
 * Input: none
 * Output: sensor values
 * - bit 0 is west car sensor
 * - bit 1 is south car sensor
 * - bit 2 is walk people sensor
* Feel free to change this. But, if you change the way it works, change the test programs too
 */
uint32_t Traffic_In(void){
  return (GPIOB->DIN31_0 >> 15) & 0x07; //reads the input in DIN and bit shift it right to least significant bit and mask it to extract the bits that are on
}
// use main1 to determine Lab4 assignment
void Lab4Grader(int mode);
void Grader_Init(void);
int main1(void){ // main1
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Lab4Grader(0); // print assignment, no grading
  while(1){
  }
}
// use main2 to debug LED outputs
// at this point in ECE319K you need to be writing your own test functions
// modify this program so it tests your Traffic_Out  function
int main2(void){ // main2
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Grader_Init(); // execute this line before your code
  LaunchPad_LED1off();
  Traffic_Init(); // your Lab 4 initialization
  if((GPIOB->DOE31_0 & 0x20)==0){
    UART_OutString("access to GPIOB->DOE31_0 should be friendly.\n\r");
  }
  UART_Init();
  UART_OutString("Lab 4, Spring 2026, Step 1. Debug LEDs\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  Debug_Init();
  SysTick_Init();
  while(1){
  Traffic_Out(0, 1, 0);
  SysTick_Wait10ms(200);

  Traffic_Out(0, 2, 0);
  SysTick_Wait10ms(50);

  Traffic_Out(0, 4, 0);
  SysTick_Wait10ms(50);

  Traffic_Out(1, 0, 0);
  SysTick_Wait10ms(200);

  Traffic_Out(2, 0, 0);
  SysTick_Wait10ms(50);

  Traffic_Out(4, 0, 0);
  SysTick_Wait10ms(50);

  Traffic_Out(0, 4, 7);
  SysTick_Wait10ms(50);

  Traffic_Out(0, 4, 2);
  SysTick_Wait10ms(50);

  Traffic_Out(0, 0, 0);
  SysTick_Wait10ms(50);
}
    if((GPIOB->DOUT31_0&0x20) == 0){
      UART_OutString("DOUT not friendly\n\r");
    }
  }
// use main3 to debug the three input switches
// at this point in ECE319K you need to be writing your own test functions
// modify this program so it tests your Traffic_In  function
int main3(void){ // main3
  uint32_t last=0,now;
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Traffic_Init(); // your Lab 4 initialization
  Debug_Init();   // Lab 3 debugging
  UART_Init();
  __enable_irq(); // UART uses interrupts
  UART_OutString("Lab 4, Spring 2026, Step 2. Debug switches\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
  while(1){
    now = Traffic_In(); // Your Lab4 input
    if(now != last){ // change
      UART_OutString("Switch= 0x"); UART_OutUHex(now); UART_OutString("\n\r");
      Debug_Dump(now);
      last = now;
    }
    Clock_Delay(800000); // 10ms, to debounce switch
  }
}
// use main4 to debug using your dump
// proving your machine cycles through all states
int main4(void){// main4
uint32_t input;
  Clock_Init80MHz(0);
  LaunchPad_Init();
  LaunchPad_LED1off();
  Traffic_Init(); // your Lab 4 initialization
 // set initial state
  Debug_Init();   // Lab 3 debugging
  UART_Init();
  __enable_irq(); // UART uses interrupts
  UART_OutString("Lab 4, Spring 2026, Step 3. Debug FSM cycle\n\r");
  UART_OutString("EID1= "); UART_OutString((char*)EID1); UART_OutString("\n\r");
// initialize your FSM
  SysTick_Init();   // Initialize SysTick for software waits
  Pt = goS;
  while(1){
      // 1) output depending on state using Traffic_Out
    GPIOB->DOUT31_0 &= ~0x0C4001C7;
    GPIOB->DOUT31_0 |= Pt->Output;
      // call your Debug_Dump logging your state number and output
    uint32_t stateNumber = 0;
    for(int i=0; i<12; i++){
      if(Pt == &FSM[i]){
        stateNumber = i;
        break;
      }
    }
    uint32_t logValue =  (stateNumber << 24) |
    (((Pt->Output >> 6) & 0x07) << 16) |   // west bits (PB6-8)
    (((Pt->Output >> 0) & 0x07) << 8 ) |   // south bits (PB0-2)
    (((Pt->Output >> 22) & 0x07));         // walk bits (PB22,26,27 mapped into 3 bits)
    Debug_Dump(logValue);
      // 2) wait depending on state
    SysTick_Wait10ms(Pt->TimeDelay);
      // 3) hard code this so input always shows all switches pressed
    uint32_t input = 7;
      // 4) next depends on state and input
    Pt = Pt->Next[input];
  }
}
// use main5 to grade
int main(void){// main5
  Clock_Init80MHz(0);
  LaunchPad_Init();
  Grader_Init(); // execute this line before your code
  LaunchPad_LED1off();
  Traffic_Init(); // your Lab 4 initialization
// initialize your FSM
  SysTick_Init();   // Initialize SysTick for software waits
  // initialize your FSM
  Lab4Grader(1); // activate UART, grader and interrupts
  Pt = goS;  //initialize the pointer from 
  while(1){
      // 1) output depending on state using Traffic_Out
      GPIOB->DOUT31_0 &= ~0x0C4001C7; //bit clears all of the output bits (PBs)
      GPIOB->DOUT31_0 |= Pt->Output; //
      // call your Debug_Dump logging your state number and output
      // 2) wait depending on state
      SysTick_Wait10ms(Pt -> TimeDelay);
      // 3) input from switches
       uint32_t in = Traffic_In();
      // 4) next depends on state and input
      Pt = Pt->Next[in];
  }
}

