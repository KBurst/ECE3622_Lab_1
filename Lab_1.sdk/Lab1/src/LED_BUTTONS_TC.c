/********************************************************************************************
* VERSION HISTORY
********************************************************************************************
* 	v1.2 - 13 February 2015
* 		Modified for Zybo Development Board ~ DN
*
* 	v1.1 - 27 January 2014
* 		GPIO_DEVICE_ID definition updated to reflect new naming conventions in Vivado 2013.3
*		onwards.
*
*	v1.0 - 13 June 2013
*		First version created.
*******************************************************************************************/

/********************************************************************************************
 * This file contains an example of using the GPIO driver to provide communication between
 * the Zynq Processing System (PS) and the AXI GPIO block implemented in the Zynq Programmable
 * Logic (PL). The AXI GPIO is connected to the LEDs on the Zybo.
 *
 * The provided code demonstrates how to use the GPIO driver to write to the memory mapped AXI
 * GPIO block, which in turn controls the LEDs.
 ********************************************************************************************/

/* Include Files */
#include "xparameters.h"
#include "xgpio.h"
#include "xstatus.h"
#include "xil_printf.h"
#include <stdio.h>

/* Definitions */
#define LED 0x0										/* Initial LED value - X00X */
#define LED_DELAY 10000000							/* Software delay length */
#define LED_CHANNEL 1								/* GPIO port for LEDs */
#define printf xil_printf							/* smaller, optimised printf */
#define BTNS_DEVICE_ID  XPAR_AXI_GPIO_0_DEVICE_ID
#define LEDS_DEVICE_ID  XPAR_AXI_GPIO_1_DEVICE_ID

XGpio LEDInst, BTNInst;

int LEDOutputExample(void)
{
	/* Variable Declaration */
	volatile int Delay;
	int Status;
	int led = LED; /* Hold current LED value. Initialise to LED definition */
	u32 Buttons;

	/* LED Initialization */
	Status = XGpio_Initialize(&LEDInst, LEDS_DEVICE_ID);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	/* Button Initialization */
	Status = XGpio_Initialize(&BTNInst, BTNS_DEVICE_ID);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	/* LED Data direction */
	XGpio_SetDataDirection(&LEDInst, 1, 0x00);

	/* Button Data direction */
	XGpio_SetDataDirection(&BTNInst, 1, 0xFF);

	/* Loop forever blinking the LED. */
	while (1)
	{
		// Reads buttons
		Buttons = XGpio_DiscreteRead(&BTNInst, 0x1);

		// Print result
		xil_printf("%d\n", Buttons);

		// For Buttons == 0b0011
		if(Buttons == 3)
		{
			// Set LEDs to 0.
			led = 0;

			// Display LEDs
			XGpio_DiscreteWrite(&LEDInst, LED_CHANNEL, led);

			while(XGpio_DiscreteRead(&BTNInst, 0x1) == 0)
			{
				// Display LEDs
				XGpio_DiscreteWrite(&LEDInst, LED_CHANNEL, led);

				// Increment LEDs by two, which results in only even numbers
				led += 2;

				// Delay
				for (Delay = 0; Delay < LED_DELAY; Delay++);
			}
		}
		// For Buttons == 0b1010
		else if(Buttons == 10)
		{
			// Set LEDs to zero
			led = 0;

			// Display
			XGpio_DiscreteWrite(&LEDInst, LED_CHANNEL, led);

			// Set LEDs to 15
			led = 15;

			while(XGpio_DiscreteRead(&BTNInst, 0x1) == 0)
			{
				// Display LEDs
				XGpio_DiscreteWrite(&LEDInst, LED_CHANNEL, led);

				// Decrement LEDs which will result in only odd numbers.
				led -= 2;

				// Delay
				for (Delay = 0; Delay < LED_DELAY; Delay++);
			}
		}
		// For other options
		else
		{
			XGpio_DiscreteWrite(&LEDInst, LED_CHANNEL, 15);
			for (Delay = 0; Delay < LED_DELAY; Delay++);

			XGpio_DiscreteWrite(&LEDInst, LED_CHANNEL, 0);
			for (Delay = 0; Delay < LED_DELAY; Delay++);
		}

	}

	return XST_SUCCESS; /* Should be unreachable */
}

/* Main function. */
int main(void){

	int Status;

	/* Execute the LED output. */
	Status = LEDOutputExample();
	if (Status != XST_SUCCESS) {
		xil_printf("GPIO output to the LEDs failed!\r\n");
	}

	return 0;
}

