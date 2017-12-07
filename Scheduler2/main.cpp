#include <ti/devices/msp432p4xx/inc/msp.h>
#include "main.hpp"
#include "Scheduler.hpp"
#include "Task.hpp"
#include "LED.hpp"
#include "LCD.hpp"

// ##########################
// Global/Static declarations
// ##########################

/* Graphic library context */
Graphics_Context g_sContext;
Graphics_Rectangle g_sTierra;
Graphics_Rectangle g_sCielo;
Graphics_Rectangle test;

/* ADC results buffer */
static uint16_t resultsBuffer[3];

uint8_t Task::m_u8NextTaskID = 0; // - Init task ID
volatile static uint64_t g_SystemTicks = 0; // - The system counter.
Scheduler g_MainScheduler; // - Instantiate a Scheduler

bool timeCorrect = true;
// #########################
//          MAIN
// #########################
void main(void)
{
    //int counter = 0;
    // - Instantiate two new Tasks
    LED BlueLED(BIT2,2);
    LED GreenLED(BIT1,1);
    LCD Screen(4);
    // - Run the overall setup function for the system
    Setup();
    // - Attach the Tasks to the Scheduler;
    g_MainScheduler.attach(&Screen);
    g_MainScheduler.attach(&BlueLED, 500);
    //g_MainScheduler.attach(&GreenLED);
    // - Run the Setup for the scheduler and all tasks
    g_MainScheduler.setup();
    // - Main Loop
    while(1)
    {
        __wfe();// Wait for Event
        if(g_SystemTicks != g_MainScheduler.m_u64ticks)
        {
            if(timeCorrect){
                if (g_SystemTicks+1==g_MainScheduler.m_u64ticks){
                    timeCorrect = true;
                }else{
                    timeCorrect = false;
                }
            }
            //- Only execute the tasks if one tick has passed.
            g_MainScheduler.m_u64ticks = g_SystemTicks;
//            counter++;
//            if (counter==2000){
//                SMS S1;
//                S1.u16Data = 1;
//                S1.u8Receiver = 1;
//                S1.u8Sender = 0;
//                S1.u8Type = 1;
//                g_MainScheduler.AttachMessage(S1);
//                counter=0;
//            }
            g_MainScheduler.GetMessage();
            g_MainScheduler.CalculateNextSchedule();
            g_MainScheduler.run();
        }
    }
}

// **********************************
// Setup function for the application
// @input - none
// @output - none
// **********************************
void Setup(void)
{
	// ****************************
	//         DEVICE CONFIG
	// ****************************
	// - Disable WDT
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

	// ****************************
	//         PORT CONFIG
	// ****************************
	// - P1.0 is connected to the Red LED
	// - This is the heart beat indicator.
	P1->DIR |= BIT0; //Red LED

	// ****************************
	//       TIMER CONFIG
	// ****************************
	// - Configure Timer32_1  with MCLK (3Mhz), Division by 1, Enable the interrupt, Periodic Mode
	// - Enable the interrupt in the NVIC
	// - Start the timer in UP mode.
	// - Re-enable interrupts
	TIMER32_1->LOAD = 0x000493E0; //~100ms ---> a 3Mhz
	TIMER32_1->LOAD = 0x00000BB8; //~1ms ---> a 3Mhz
	TIMER32_1->CONTROL = TIMER32_CONTROL_SIZE | TIMER32_CONTROL_PRESCALE_0 | TIMER32_CONTROL_MODE | TIMER32_CONTROL_IE | TIMER32_CONTROL_ENABLE;
	NVIC_SetPriority(T32_INT1_IRQn,1);
	NVIC_EnableIRQ(T32_INT1_IRQn);

    /* Halting WDT and disabling master interrupts */
    MAP_WDT_A_holdTimer();
    MAP_Interrupt_disableMaster();

    /* Set the core voltage level to VCORE1 */
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);

    /* Set 2 flash wait states for Flash bank 0 and 1*/
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

    /* Initializes Clock System */
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    /*Initializes rectangles*/
    g_sTierra.xMax = 128;
    g_sTierra.xMin =  0;
    g_sTierra.yMax = 128;
    g_sTierra.yMin = 64;

    g_sCielo.xMin = 0;
    g_sCielo.xMax = 128;
    g_sCielo.yMax = 63;
    g_sCielo.yMin =0 ;
    Graphics_clearDisplay(&g_sContext);//cleaning display
    Graphics_fillRectangleOnDisplay(g_sContext.display,&g_sTierra, 0xa347);// display land and sky in one half of the screen each
    Graphics_fillRectangleOnDisplay(g_sContext.display,&g_sCielo, 0x0080ff);
//    g_sTierra.xMax = 128;
//    g_sTierra.xMin =  0;
//    g_sTierra.yMax = 128;
//    g_sTierra.yMin = 90;
//
//    g_sCielo.xMin = 0;
//    g_sCielo.xMax = 128;
//    g_sCielo.yMax = 89;
//    g_sCielo.yMin =0 ;
//    Graphics_fillRectangleOnDisplay(g_sContext.display,&g_sTierra, 0xa347);// display land and sky in one half of the screen each
//    Graphics_fillRectangleOnDisplay(g_sContext.display,&g_sCielo, 0x0080ff);
//    test.xMin = 0;
//    test.xMax = 128;
//    test.yMax = 89;
//    test.yMin =50 ;
//    Graphics_fillRectangleOnDisplay(g_sContext.display,&test, 0x0080ff);

    /* Configures Pin 4.0, 4.2, and 6.1 as ADC input */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN2, GPIO_TERTIARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN1, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Initializing ADC (ADCOSC/64/8) */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_SMCLK, ADC_PREDIVIDER_32, ADC_DIVIDER_1,
            0);

    /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM2 (A11, A13, A14)  with no repeat)
         * with internal 2.5v reference */
    MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0,
            ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A14, ADC_NONDIFFERENTIAL_INPUTS);

    MAP_ADC14_configureConversionMemory(ADC_MEM1,
            ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A13, ADC_NONDIFFERENTIAL_INPUTS);

    MAP_ADC14_configureConversionMemory(ADC_MEM2,
            ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A11, ADC_NONDIFFERENTIAL_INPUTS);

    /* Enabling the interrupt when a conversion on channel 2 (end of sequence)
     *  is complete and enabling conversions */
    MAP_ADC14_enableInterrupt(ADC_INT2);

    /* Enabling Interrupts */
    MAP_Interrupt_enableInterrupt(INT_ADC14);
    MAP_Interrupt_enableMaster();

    /* Setting up the sample timer to automatically step through the sequence
     * convert.
     */
    MAP_ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

    /* Triggering the start of the sample */
    MAP_ADC14_enableConversion();
    MAP_ADC14_toggleConversionTrigger();
	__enable_irq();

	return;
}

extern "C"
{
    // - Handle the Timer32 Interrupt
	void T32_INT1_IRQHandler(void)
	{

		TIMER32_1->INTCLR = 0U;
		P1->OUT ^= BIT0; // - Toggle the heart beat indicator (1ms)
		g_SystemTicks++;
		return;

	}

	void ADC14_IRQHandler(void)
	{
	    uint64_t status;

	    status = MAP_ADC14_getEnabledInterruptStatus();
	    MAP_ADC14_clearInterruptFlag(status);

	    /* ADC_MEM2 conversion completed */
	    if(status & ADC_INT2)
	    {
	        /* Store ADC14 conversion results */
	        resultsBuffer[0] = ADC14_getResult(ADC_MEM0); //X
	        resultsBuffer[1] = ADC14_getResult(ADC_MEM1); //Y
	        resultsBuffer[2] = ADC14_getResult(ADC_MEM2); //Z
	        SMS S1;
	        SMS S2;
	        SMS S3;

	        S1.u16Data  =   resultsBuffer[0];
	        S1.u8Receiver=  4;
	        S1.u8Sender =   5;
	        S1.u8Type   =   2;

            S2.u16Data  =   resultsBuffer[1];
            S2.u8Receiver=  4;
            S2.u8Sender =   5;
            S2.u8Type   =   3;

            S3.u16Data  =   resultsBuffer[2];
            S3.u8Receiver=  4;
            S3.u8Sender =   5;
            S3.u8Type   =   4;
            g_MainScheduler.AttachMessage(S1);
            g_MainScheduler.AttachMessage(S2);
            g_MainScheduler.AttachMessage(S3);
	    }
	}
}
