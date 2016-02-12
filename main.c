
/*************************************************************************************************
 * 					ITS-PLC Startup Project
 *
 * 					(c) Polytech Montpellier - MEA
 *
 * 					20/09/2015 - Laurent Latorre
 *
 * 					FreeRTOS getting started project to work with ITS-PLC Simulator
 *
 * 					Creates 3 asynchronous tasks :
 *
 * 					- Graphical User Interface (TouchScreen and I/O extender interface)
 * 					- Task1 : Actuator A00 toggle
 * 					- Task2 : Actuator A01 copies the state of Sensor S00
 *
 *************************************************************************************************/


// Local defines

#define	PAD_WIDTH	56
#define PAD_HEIGHT	36
#define PAD_MARGIN	3

#define DBG_OFFLINE	0
#define	DBG_ONLINE	1


// Include main headers

#include "stm32f4xx.h"

// Include BSP headers

#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_sdram.h"
#include "stm32f429i_discovery_ioe.h"

// Include FreeRTOS headers

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"

// Declare Tasks

void vTask1   (void *pvParameters);
void vTask2   (void *pvParameters);

// Trace Labels

traceLabel user_event_channel;


// Global variables

static	uint16_t	   its_sensors;			// 16 bits representing sensors states from ITS-PLC
static	uint16_t	   forced_sensors;		// 16 bits representing sensors states from User Interface
static	uint16_t	   sensors;				// Bitwise OR between the two above
static	uint16_t	   actuators;			// 16 bits representing actuators states

static  uint8_t		   dbg_mode;			// ONLINE/OFFLINE upon detection of the I/O Extender on boot


// Main program

int main(void)
{
	// Init board LEDS and Blue Push Button

	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);

    // Start IO Extenders (Touchpad & PCF IO Extenders)

    IOE_Config();

    // Test if ITS-PLC I/O Extender is connected

    if (I2C_WritePCFRegister(PCF_B_WRITE_ADDR, (int8_t)0x00))
    {
    	// I/O Extender found, ONLINE mode -> Green LED

    	dbg_mode = DBG_ONLINE;
    	STM_EVAL_LEDOn(LED3);
    	STM_EVAL_LEDOff(LED4);
    }
    else
    {
    	// I/O Extender not found, OFFLINE mode -> Red LED

    	dbg_mode = DBG_OFFLINE;
    	STM_EVAL_LEDOff(LED3);
    	STM_EVAL_LEDOn(LED4);
	}

    // Init sensors & actuators variables

    actuators = 0x0000;
    forced_sensors = 0x000;

	// Launch FreeRTOS Trace recording

	vTraceInitTraceData();
	uiTraceStart();

	// Create Tasks

	xTaskCreate(vTask1,   "Task_1",   128, NULL, 1, NULL);		// 128 bytes stack, priority 1
	xTaskCreate(vTask2,   "Task_2",   128, NULL, 2, NULL);		// 128 bytes stack, priority 2

	user_event_channel = xTraceOpenLabel("UEV");

	// Start the Scheduler

	vTaskStartScheduler();

    while(1)
    {
    	// The program should never be here...
    }
}


/*
 * 	TASK 1
 *
 * 	- Actuator A00 blinking
 * 	- Green LED3 blinking
 *
 * 	This task is executed every 800ms
 *
 */

void vTask1 (void *pvParameters)
{
	static uint16_t	n;

	while(1)
	{
		actuators = actuators ^ 0x0001;

		vTracePrintF(user_event_channel, "Hello from task 1 : %d", n);

		n++;

		// Wait for 800ms
		vTaskDelay(800);
	}
}

/*
 * 	TASK 2
 *
 * 	- Actuator A01 copies the state of Sensor S00
 * 	- Red LED4 blinking
 *
 * 	This task is executed every 100ms
 *
 */

void vTask2 (void *pvParameters)
{
	while(1)
	{
    	if ((sensors & 0x0001) == 0x0001)
    	{
    		actuators = actuators | 0x0002;
    	}
    	else
    	{
    		actuators = actuators & 0xFFFD;
    	}

		// Wait for 100ms
		vTaskDelay(100);
	}
}
