/*
 * Part2.c
 *
 *  Created on: Feb 11, 2023
 *      Author: Russell Trafford
 *
 *  Modified on: Feb 18, 2023
 *      Author: Isaiah Roepke
 *
 *      This code will need to change the speed of an LED between 3 different speeds by pressing a button.
 */

#include <msp430.h>

void gpioInit();
void timerInit();

char LED_Speed = 0x00;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

gpioInit();

timerInit();
    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;


    __bis_SR_register(GIE);         // Enter LPM3 w/ interrupt

    while (1){

        if (LED_Speed == 0x00){
             TB1CTL = TBSSEL_1 | MC_2  | ID_2;
        }
        else if (LED_Speed == 0x01){

            TB1CTL = TBSSEL_1 | MC_2  | ID_1;
        }
        else if (LED_Speed == 0x02){
             TB1CTL = TBSSEL_1 | MC_2  | ID_0;
        }



    }

}

void gpioInit(){
    // @TODO Initialize the Red OR Green LED
    P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
    P1DIR |= BIT0;                          // Set P1.0 to output direction
    // @TODO Initialize Button 2.3
    P2OUT |= BIT3;                          // Configure P2.3 as pulled-up
    P2REN |= BIT3;                          // P2.3 pull-up register enable
    P2IES |= BIT3;                         // P2.3 High to low edge/active low
    P2IE |= BIT3;                           // P2.3 interrupt enabled


}

void timerInit(){
    // @TODO Initialize Timer B1 in Continuous Mode using ACLK as the source CLK with Interrupts turned on
    // Timer1_B3 setup
    TB1CCTL0 = CCIE;                          // TBCCR0 interrupt enabled
    TB1CCR0 = 32768;
    TB1CTL = TBSSEL_1 | MC_2 | TBCLR | ID_2 | TBIE ;                 // ACLK, continuous mode

}


/*
 * INTERRUPT ROUTINES
 */

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    // @TODO Remember that when you service the GPIO Interrupt, you need to set the interrupt flag to 0.
    P2IFG &= ~BIT3;
    // @TODO When the button is pressed, you can change what the CCR0 Register is for the Timer. You will need to track what speed you should be flashing at.
    if ((P2IES &= BIT3) == BIT3){
        switch (LED_Speed){
            case 0x00:
                LED_Speed = 0x01;

                break;
            case 0x01:
                LED_Speed = 0x02;

                break;
            case 0x02:
                LED_Speed = 0x00;

                break;
        }
    }

}

/*
//TIMER0_B1_VECTOR

#pragma vector=TIMER0_B1_VECTOR
__interrupt void TB1_ISR(void)
{
    TB1CTL &= TBIFG ;
    P1OUT ^= BIT0;
    TB1CCR0 += 32768;                         // Add Offset to TB1CCR0


}
*/
// Timer B1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_B0_VECTOR))) Timer1_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    P1OUT ^= BIT0;
    TB1CCR0 += 32768;                         // Add Offset to TB1CCR0
}

