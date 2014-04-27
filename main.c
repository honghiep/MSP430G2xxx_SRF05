/******************************************************************************
 *
 * www.payitforward.edu.vn
 *
 ******************************************************************************/

/******************************************************************************
 *
 * CHICKEN BOARD - HAPPY CODING
 *
 ******************************************************************************/

/******************************************************************************
 *
 *    Module       : main.C
 *    Description  : This demonstration use MSP430G2xxx to communicate with SRF05
 *
 *  Tool           : CCS 5.4
 *  Chip           : MSP430G2xxx
 *  History        : 27-04-2014
 *  Version        : 1.1
 *
 *  Author         : PIFers, Pay It Forward Club
 *  Notes          :
 *
******************************************************************************/

 /****************************************************************************
 * IMPORT
******************************************************************************/
#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>

/****************************************************************************
* DEFINITIONS
******************************************************************************/
/****************************************************************************
* GLOBAL VARIABLES
******************************************************************************/
volatile uint32_t res = 0;

/****************************************************************************
* FUNCTIONS
******************************************************************************/

//*****************************************************************************
// Configure Timer1 to capture echo pulse
//*****************************************************************************

void Config_Capture(void)
{
	TA1CTL = TASSEL_2 + MC_2 + TAIE;
	/*
	 * Timer Clock Source: SMCLK
	 * Continuous Mode
	 */
	TA1CCTL2 = CM_3 + CCIS_0 + CAP + SCS + CCIE;
	/*
	 * Capture Mode: Both Edges
	 * Capture Input Signal: CCIxA
	 * Capture Mode
	 * Synchronous Capture
	 * Enable Compare/Capture Interrupt
	 */

	//Configure Capture Input Signal Pin: P2.4 (CCI2A)
	P2DIR &= ~BIT4;
	P2SEL |= BIT4;
	P2SEL2 &= ~BIT4;

	P2REN |= BIT4;
	P2OUT |= BIT4;
}

//*****************************************************************************
// Configure Timer0 to generate Trigger Pulse
//*****************************************************************************

void Config_Timer(void)
{
	TACTL = TASSEL_2 + MC_1 + ID_3 + TAIE;
	TACCR0 = 60000;		//60ms cycle
	TACCR1 = 15;		//15us Pulse
	TACCTL1 = CCIE;

	P2DIR |= BIT5;
	P2OUT &= ~BIT5;
}

//*****************************************************************************
// Main Routine
//*****************************************************************************
void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;	// Stop watchdog timer
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;
	Config_Timer();
	Config_Capture();

	/*
	P1DIR |= BIT3;
	P1REN |= BIT3;
	P1OUT |= BIT3;
	 */

	__enable_interrupt();	//Enable Interrupt

	while(1)
	{
	}
}

//*****************************************************************************
// Timer0 interrupt service routine
//*****************************************************************************
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_ISR (void)
{
	switch (TAIV)
	{
	case 0x02:	//TACCR1 CCIFG
		P2OUT &= ~BIT5;
		break;
	case 0x0a:	//Timer Overflow
		P2OUT |= BIT5;
		break;
	}
}

//*****************************************************************************
// Timer1 interrupt service routine
//*****************************************************************************
#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_ISR (void)
{
	static uint8_t count = 0;
	switch (TA1IV)
	{
	case 0x04:	//TACCR2 CCIFG
		if (P2IN & BIT4)
		{
			count = 0;
			TA1CTL |= TACLR;
		}
		else
		{
			res = TA1CCR1 + (count * 65536);
		}
		/*
		TA0CTL |= TACLR;
		//Resets Counter, the clock divider, and the count direction
		 */
		_delay_cycles(1);	//Set Breakpoint here to view result

		TA1CCTL1 &= ~CCIFG;		//Clear Interrupt Flag
		break;
	case 0x0a:	//Timer Overflow
		/*
		 *
		 */
		count++;
		break;
	}
}
