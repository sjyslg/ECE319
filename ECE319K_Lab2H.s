// ****************** ECE319K_Lab2H.s ***************
// Your solution to Lab 2 in assembly code
// Author: Selina Song
// Last Modified: 2/1/2026
// ECE319H Spring 2026 (ECE319K students do Lab2)

    .include "../inc/msp.s"

        .data
        .align 2
// Declare global variables here if needed
// with the .space assembly directive

        .text
        .thumb
        .align 2
        .global EID
EID:    .string "sjs5984" // replace ZZZ123 with your EID here
        .align 2
  .equ dot,100
  .equ dash,(3*dot)
  .equ shortgap,(2*dot)  // because it will send an interelement too
  .equ interelement,dot
  .equ N100ms, 2000000
  .equ N300ms, 6000000
  Morse:
  .long  dot,  dash,    0,    0, 0 // A
  .long  dash,  dot,  dot,  dot, 0 // B
  .long  dash,  dot, dash,  dot, 0 // C
  .long  dash,  dot,  dot,    0, 0 // D
  .long  dot,     0,    0,    0, 0 // E
  .long  dot,   dot, dash,  dot, 0 // F
  .long  dash, dash,  dot,    0, 0 // G
  .long  dot,   dot,  dot,  dot, 0 // H
  .long  dot,   dot,    0,    0, 0 // I
  .long  dot,  dash, dash, dash, 0 // J
  .long  dash,  dot, dash,    0, 0 // K
  .long  dot,  dash,  dot,  dot, 0 // L
  .long  dash, dash,    0,    0, 0 // M
  .long  dash,  dot,    0,    0, 0 // N
  .long  dash, dash, dash,    0, 0 // O
  .long  dot,  dash, dash,  dot, 0 // P
  .long  dash, dash,  dot, dash, 0 // Q
  .long  dot,  dash,  dot,    0, 0 // R
  .long  dot,   dot,  dot,    0, 0 // S
  .long  dash,    0,    0,    0, 0 // T
  .long  dot,   dot, dash,    0, 0 // U
  .long  dot,   dot,  dot, dash, 0 // V
  .long  dot,  dash, dash,    0, 0 // W
  .long  dash,  dot,  dot, dash, 0 // X
  .long  dash,  dot, dash, dash, 0 // Y
  .long  dash, dash,  dot,  dot, 0 // Z

  .align 2
  .global Lab2Grader
  .global Lab2
  .global Debug_Init // Lab3 programs
  .global Dump       // Lab3 programs

// Switch input: PB2 PB1 or PB0, depending on EID
// LED output:   PB18 PB17 or PB16, depending on EID
// logic analyzer pins PB18 PB17 PB16 PB2 PB1 PB0
// analog scope pin PB20
Lab2:
// Initially the main program will
//   set bus clock at 80 MHz,
//   reset and power enable both Port A and Port B
// Lab2Grader will
//   configure interrupts  on TIMERG0 for grader or TIMERG7 for TExaS
//   initialize ADC0 PB20 for scope,
//   initialize UART0 for grader or TExaS
    //MOVS R0,#3  // 0 for info, 1 debug with logic analyzer, 2 debug with scope, 10 for grade
   // BL   Lab2Grader
    BL   Lab2Init   // you initialize input pin and output pin
    BL   Debug_Init // your Lab3
loop:
  // 1 second wait
     // BL SwitchOff 
     // BL SwitchOn
      MOVS R0,#10  // 0 for info, 1 debug with logic analyzer, 2 debug with scope, 10 for grade
      BL Lab2Grader // R0 = random 'A'–'Z'
      MOVS R0, #'H'
      BL SendLetter // uses R0 to find the letter
      B loop 
SwitchOff: 
    PUSH {R4-R7, LR} 
OffLoop: 
    LDR R0, =GPIOB_DIN31_0 
    LDR R1, [R0] //loads contents of GPIOB
    LDR R2, =(1<<3) // loads into R2 a mask of putting a 1 into the third bit
    ANDS R1, R2 //Ands the mask value of R2 and R1 
    CMP R1, #0 //checks if R1 is NOT zero (toggling the 3rd bit will make it nonzero)
    BNE OffLoop //loop until it becomes zero in R1
    POP {R4-R7, PC} 
    
SwitchOn: 
    PUSH {R4-R7, LR} 
OnLoop: 
    LDR R0, =GPIOB_DIN31_0 
    LDR R1, [R0] 
    LDR R2, =(1<<3) 
    ANDS R1, R2 
    CMP R1, #0 
    BEQ OnLoop //checks if R1 is NOT zero (toggling it will turn off the third bit)
    POP {R4-R7, PC}

SendLetter:
    PUSH {R4-R7, LR}
    MOVS R1, #'A'//takes the ascii value of letter A      
    SUBS R2, R0, R1  //subtracts the value of R0 (letter provided by grader) with ascii value of A
    MOVS R3, R2 
    MOVS R4, #20 //R4 is 20
    MULS R3, R4 //Multiply the index value by 20 
    LDR R5, =Morse //loads the address of the morse code table
    ADDS R6, R5, R3 //adds the index*20 to the address of morse code to get the line of the letter
    MOVS R7, #5         
LetterLoop:
    LDR R4, [R6] //R6 has the new address of just the letter line, so R4 has the contents of the first thing in the line       
    CMP R4, #0 //compares if the line starts with a zero
    BEQ DoneLetter //ends the letter output
    MOVS R5, #dot //checks if the first thing is a dot       
    CMP R4, R5
    BEQ DoDot //if equal go to dot
    BL Dash //if not equal do dash
    B NextSymbol 
DoDot:
    BL Dot
NextSymbol:
    ADDS R6, R6, #4 //next .long
    SUBS R7, R7, #1 //max 5 things in each line 
    BNE LetterLoop
DoneLetter:
    POP {R4-R7, PC}
    
Dot: PUSH {R4-R7, LR}
    MOVS R0, #1 //Indicates that the LED is on
    BL LEDOut
    BL DelayDot
    BL LEDOff
    BL DelayDot
      POP {R4-R7, PC}
      
Dash: PUSH {R4-R7, LR}
    MOVS R0, #1 //indicates that the LED is on
    BL LEDOut
    BL DelayDash
    BL LEDOff
    BL DelayDot
      POP {R4-R7, PC}

Lab2Init:
    PUSH {R4-R7, LR}
    //Initializing Input Pin PB3
    LDR R0, =IOMUXPB3 //Loads address off the GPIO pin
    MOVS R1, #0x81 //Mask that turns on bit 7 and sets bits 5-0 to 00001
    LDR R2, =(1<<18)
    ORRS R1, R1, R2 //Sets bit 18 to a 1 because it is the input enable bit
    STR R1, [R0] //Stores this back into IOMUX to tell the microcontroller that the pin is software enabled
    //Initializing Output Pin PB17
    LDR R0, =IOMUXPB17
    MOVS R1, #0x81 
    STR R1, [R0]
    LDR R0, =GPIOB_DOE31_0
    LDR R1, [R0]
    LDR R2, =(1<<17) //only want to manipulate bit 17 bc GPIOB_DOE31_0 bit 17 is PB17
    ORRS R1, R1, R2
    STR R1, [R0]
    POP {R4-R7, PC}

DelayDot:
    PUSH {R4-R7, LR}
    LDR R4, =N100ms
dLoopDot:
    SUBS R4, #1
    NOP
    BNE dLoopDot
    POP{R4-R7, PC}

DelayDash:
    PUSH {R4-R7, LR}
    LDR R4, =N300ms //loads 6000000 into R4 and loops this for that many times to waste that many clock cycles
dLoopDash:
    SUBS R4, #1
    NOP
    BNE dLoopDash
    POP{R4-R7, PC}
    
LEDOut:
    PUSH {R4-R7, LR}
    LDR R4, =GPIOB_DOUT31_0
    LDR R5, [R4]
    LDR R6, =(1<<17) // R6 is mask for PB17 (LED pin) (1<<17) moves a 1 into the 17th bit 
    BICS R5, R5, R6 // Clear PB17 (force LED OFF, preserve other pins)
    LSLS R0, R0, #17 //shifts R0 into the 17th position (R0 has the input)
    ORRS R5, R5, R0 //ORRS the mask with the original value to preserve the other bits
    STR R5, [R4] //stores back into GPIOB_DOUT31_0
    BL Dump
    POP{R4-R7, PC}

LEDOff: 
    PUSH {R4-R7, LR} 
    LDR R4, =GPIOB_DOUT31_0
    LDR R5, [R4]
    MOVS R6, #0
    ANDS R5, R6, R5 //clears GPIOB_DOUT31_0 to turn off the LED
    STR R5, [R4]
    BL Dump
    POP{R4-R7, PC}
    

// make switch an input, LED an output
// PortB is already reset and powered
// Set IOMUX for your input and output
// Set GPIOB_DOE31_0 for your output (be friendly)


   .end
