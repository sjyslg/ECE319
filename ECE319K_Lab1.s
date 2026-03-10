// ****************** ECE319K_Lab1.s ***************
// Your solution to Lab 1 in assembly code
// Author: Selina Song
// Last Modified: 1/24/26
// Spring 2026
        .data
        .align 2
        .global EID
//EID:    .string "SJS5984"
  //      .global Phase
    //    Phase: .long 0
// Declare global variables here if needed
// with the .space assembly directive

        .text
        .thumb
        .align 2
        .global EID
EID:    .string "SJS5984" // replace ZZZ123 with your EID here

       .global Phase
     .align 2
Phase:  .long 10
// Phase= 0 will display your objective and some of the test cases, 
// Phase= 1 to 5 will run one test case (the ones you have been given)
// Phase= 6 to 7 will run one test case (the inputs you have not been given)
// Phase=10 will run the grader (all cases 1 to 7)
        .global Lab1
// Input: R0 points to the list
// Return: R0 as specified in Lab 1 assignment and terminal window
// According to AAPCS, you must save/restore R4-R7
// If your function calls another function, you must save/restore LR
Lab1: PUSH {R4-R7,LR}
        // your solution goes here
        MOVS R1, R0 //has the address of the array pointer
        MOVS R0, #0 //sets R0 to zero for the later part
        LDR R2, =EID //R2 has the address of my EID string
        MOVS R5, #0 //sets the array index to 0
        LDRB R3, [R2] //gets value of first char in my EID
        CMP R3, #0 // checks if the EID is empty
        BEQ NOTFOUND //if EID is empty not found
ArrayLoop:
        LDR R4, [R1] //gets value of pointer to first string of array
        CMP R4, #0   // Checks if array is empty
        BEQ EndArray           
        BL CharacterCompare //compares string subroutine
        CMP R0, #0 //if subroutine returns 0 in R0 it is not found 
        BGT EndArray
        LDR R2, =EID //Resets the string pointer to be at beginning every time 
        ADDS R5, #1 // Adds 1 to the array index counter
        ADDS R1, #8 // increments the array pointer by 2 to skip past the grade 
        B ArrayLoop 
EndArray: 
        CMP R0, #1
        BEQ FOUND
NOTFOUND: 
        MOVS R0, #0
        SUBS R0, #1
        POP {R4-R7,PC}
FOUND: 
        MOVS R0, R5 //Moves the array pointer to R0
        POP {R4-R7,PC}
CharacterCompare:
        Push{R4-R7,LR}
CharLoop: 
        LDRB R3, [R2]
        LDRB R6, [R4]
        CMP R6, R3
        BNE GoBack
        CMP R6, #0
        BEQ Same
        ADDS R2, #1
        ADDS R4, #1
        B CharLoop
Same: 
        MOVS R0, #1
        POP {R4-R7, PC}
        BX LR
GoBack:
        MOVS R0, #0
        POP {R4-R7, PC}
        BX LR
        .align 2
        .global myClass
myClass: .long 0  // pointer to EID
         //.long 95      // Score
         //.long pXYZ1   // pointer to EID
         //.long 96      // Score
         //.long pAB5549 // pointer to EID
         //.long 94      // Score
         //.long pAB1235  // pointer to EID
         //.long 95      // Score
         //.long pXYZ17   // pointer to EID
         //.long 96      // Score
         //.long pAB5548 // pointer to EID
         //.long 94      // Score
         //.long pXYZ173   // pointer to EID
         //.long 96      // Score
         //.long 0       // null pointer means end of list
         //.long 0
pAB123:  .string "ZCI40"
pXYZ1:   .string "AE7604"
pAB5549: .string "LGX524"
pAB1235: .string "SJS2984"
pXYZ17:  .string "FKQ80"
pAB5548: .string "SJS4984"
pXYZ173: .string "SJS5984"
        .end
