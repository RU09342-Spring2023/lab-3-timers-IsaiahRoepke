# Button-Based Delay System

 
## Prerequisites 
One must have an MSP430FR2533 available for use. 
 
## Modules 
* Code Composer Studio - 12.2.0  
 
## Function of the Program 
The code begins by blinking the red LED of the MSP430 at about 4Hz using the timer module. Using interrupts, the user is able to hold 
down a button for a certain amount of time where that time is then used to blink the red LED. The second button is used to 
reset the timer module, reverting back to the original 4Hz blink. 
 
## Pre-main.c code 
The character `timerstate` is initialized to 0 as this is the initial 4Hz state of the timer and red LED.
The integer `timeheld` is initialized to 0 to be later used for the user button defined state.
The function `gpioInit()` is declared.
The funciton `timerInit()` is declared.
 
## main.c 
The code `WDTCTL = WDTPW | WDTHOLD` is used to stop the watchdog timer, preventing the processor from reseting.
Both functions `gpioInit()` and `timerInit()` are called to initial the perpherials and timer of the MSP430.
`PM5CTL0 &= ~LOCKLPM5` is necessary for the interrupts of the code to work correctly.
The MSP430 is set to LPM3 with interrupts enabled using `__bis_SR_register(GIE)`.
 
## while(1) 
The purpose of the while loop is hold the code that defines the timerstates.

### if timerstate == 0x00
If the timer state is in the initial state, the timer B1 control register is set to the Auxillary clock with a clock divider of /2
 
### if timerstate == 0x01
If the timer state is in the user button defined state, the timer B1 control register is set to the Auxillary clock with a clock divider of /8. The clock divider is increased so that the user has a large time frame to keep the button held.
 
## gpioInit()
The void function `gpioInit()` is used to initialize all the peripherals of the MSP430.
P1.0, the red LED, is initialized as an output with a defined power-on state of off. 
P2.3, the right side button, is initialized as an input. This input is configured as a pull-up resistor with active low with interrupts enabled. 
P4.1, the left side button, is initialized as an input. This input is configured the same way as P2.3. 

## timerInit()
The timer B1 capture compare register interrupt is enabled. The capture compare register is assigned 4096 since this allows the 4Hz blink.
The timer B1 control registered is set to the auxillary clock, interrupts are enabled, and the count is clear so the timer must start at 0.

## TimerB1 interrupt vector
Every time the timer B1 count reaches the value stored inside the capture compare registered, this interrupt is triggered.
The interrupt first checks which state the timer is in. If `timerstate` is 0, then the red LED is toggled and the capture compare register is incremented by 4096. This ensures even intervals of the red LED being off and on. 
Else if `timerstate` is 1, the red LED is toggled and the capture compare register of timer B1 is incremented by the value found in the variable `timeheld`.

## Port 2 interrupt vector
This interrupt is triggered on the high to low edge trigger of P2.3. 
The interrupt flag of P2 is cleared. 
The timer B1 control register is assigned so that the timer count is cleared, the clock divider is assigned /8, and timer interrupts are enabled. 
`timerstate` is assigned 1 so that the while loop will run the button mode instead of the initial mode. 
A while loop is declared that runs so long as the button P2.3 is held. This prevent the program from exiting the interrupt vector before the user unpresses the button. 
While the loop is looping, the variable time held is assigned the value stored in timer B1. As the count stored in timer B1 increments, timeheld also increments. This is the amount of time the user is holding down the button. 

## Port 4 interrupt vector
The interrupt is triggered on the high to low edge trigger of P4.1. 
The interrupt flag of P4 is cleared. 
The capture compare register of timer B1 is set back to 4096, matching the initial state.
The character `timerstate` is set back to 0 to match the intital state. 

## How is timer rollover handled?
Timer rollover is handled through the timer control register. The macro `ID_x` controls the clock divider of the timer. The higher the value x is,
the longer the interval granted to the user to hold down the button. Using a rollover variable was avoided to keep the code simple. 

## How should the user use my code?
The most customizable part of the code is the capture compare module of the timer, which peripheral are to be used and how, and the clock dividers of the timer. 
The capture compare module of the original state can be extended or shortened to control how fast the red LED blinks. The clock divider can also be shortened or extended as is starts as /2. 
The clock divider of the user button controlled state can be shortened or extended to customize the interval provided to the user. 
By default, the peripherals used are the two buttons and the red LED on board. The user can initialize different ports to allow sensors to interface with the board and therefore timer. The output of the timer can be used to oscilate a sensor or some kind of output at a rate customizable in the real world. 
 
