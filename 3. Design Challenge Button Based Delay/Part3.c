/*
 * Part3.c
 *
 *  Created on: Feb 18, 2023
 *      Author: Isaiah Roepke
 *
 *      YOU NEED TO FILL IN THIS AUTHOR BLOCK
 */

#include <msp430.h>

char timerstate = 0x00; //a state is saved for changing between the default 4Hz state to the user defined state.
int timeheld = 0; //the amount of time the user holds down the button is saved to this variable. The timer compare module is incremented by this to control when the interrupt happens.

void gpioInit(); //declaration for GPIO initialization
void timerInit(); //declaration for timer initialization

int main(void){

    WDTCTL = WDTPW | WDTHOLD; //stops the watchdog timer to prevent the processor from resetting

    gpioInit(); //calls the GPIO initialization function
    timerInit(); //calls the timer initialization function

    PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings


    __bis_SR_register(GIE); // Enter LPM3 w/interrupt

    while(1){
        if (timerstate == 0x00){ //if the timer state is in the initial state
                    TB1CTL = TBSSEL_1 | MC_2  | ID_1; //timer B1 control module is set to Aclk and a clk divider of /2
               }
        if (timerstate == 0x01){ //if the timer state is in the button-timed mode
            TB1CTL = TBSSEL_1 | MC_2  | ID_3; //timer B1 control module is set to Aclk with a clk divider of /8
        }


    }


}


void gpioInit(){
    P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
    P1DIR |= BIT0;                          // Set P1.0 to output direction

    P2OUT |= BIT3;                          // Configure P2.3 as pulled-up
    P2REN |= BIT3;                          // P2.3 pull-up register enable
    P2IES |= BIT3;                         // P2.3 High to low edge/active low
    P2IE |= BIT3;                           // P2.3 interrupt enabled

    P4OUT |= BIT1;                          // Configure P2.3 as pulled-up
    P4REN |= BIT1;                          // P2.3 pull-up register enable
    P4IES |= BIT1;                         // P2.3 High to low edge/active low
    P4IE |= BIT1;                           // P2.3 interrupt enabled



}

void timerInit(){
    // @TODO Initialize Timer B1 in Continuous Mode using ACLK as the source CLK with Interrupts turned on
    // Timer1_B1 setup
    TB1CCTL0 = CCIE;                          // TB1CCR0 interrupt enabled
    TB1CCR0 = 4096; //4096 is used to start for the Capture Compare Register
    TB1CTL = TBSSEL_1 | MC_2 | TBCLR | ID_1 | TBIE ;                 // ACLK, continuous mode, interrupts enabled, clk divider = /2

}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_B0_VECTOR //timer B0 interrupt vector
__interrupt void Timer1_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_B0_VECTOR))) Timer1_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (timerstate == 0x00){ //when the timer count register reaches the value inside the Capture compare register in the initialized state
    P1OUT ^= BIT0; //the red LED is toggled
    TB1CCR0 += 4096;                         // Add Offset to TB1CCR0
    }

    else if(timerstate == 0x01){ //when the timer count register reaches the value inside the CCR in the button defined state
        P1OUT ^= BIT0; //the red LED is toggled
        TB1CCR0 += timeheld; //The timer B0 is incremented by the time the button is held down by the user
    }

}


#pragma vector=PORT2_VECTOR //port 2 interrupt vector
__interrupt void Port_2(void)
{

    P2IFG &= ~BIT3; //the port 2 interupt flag is cleared


        TB1CTL = TBSSEL_1 | MC_2 | TBCLR | ID_3 | TBIE ;                 // ACLK, continuous mode, count is cleared, interrupt enabled, clk divider = /8
        timerstate = 0x01; //the timer state is set to button defined mode for the while loop

    while (~P2IN & BIT3){ //while button P2.3 is held
        timeheld = TB1R; //the count of the Timer b1 is saved into the variable timeheld

    }

}

#pragma vector=PORT4_VECTOR //port 4 interrupt vector
__interrupt void Port_4(void)
{
     P4IFG &= ~BIT1; //port 4 interrupt flag is cleared
     TB1CCR0 = 4096; //capture compare registered is reverted to its original state

        timerstate = 0x00; //timerstate is reverted back to the 4Hz state

}
