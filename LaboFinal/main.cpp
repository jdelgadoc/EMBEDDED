#include "main.hpp"
#include "Mic.hpp"
#include <stdio.h>
#include <ti/devices/msp432p4xx/inc/msp.h>

extern "C"{
    #include <ti/devices/msp432p4xx/driverlib/driverlib.h>
    #include "LcdDriver/Crystalfontz128x128_ST7735.h"
    #include <ti/grlib/grlib.h>
}

Graphics_Context g_sContext;
int g_iOption = 1;
bool g_bIsRecSaved = 0;
//Defining rectangles
Screen Sc;




int main(){
    setup();
    Sc.setup();
  //  Mic
    while(1){

    }
}

void setup(){
    //// Esto es lo que tiene que ir en el setup principal, el del main.
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0); //Red
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1); //Green
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);// Blue
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN5); //down
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN1); //up
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P1, GPIO_PIN4); //select
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P4, GPIO_PIN1); //select from joystick

    //Button interreput rutine

    GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_clearInterruptFlag(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN1);
    Interrupt_enableInterrupt(INT_PORT3);
    Interrupt_enableInterrupt(INT_PORT5);
    Interrupt_enableInterrupt(INT_PORT1);
    Interrupt_enableInterrupt(INT_PORT4);
    //Disable all LED before starting
    P2->OUT = 0;
    __disable_irq();
}

extern "C"
{
void PORT3_IRQHandler(void) //down
{
    __disable_irq();
    for(int i = 0; i == 10000; i++)
    {
        __no_operation();
    }

    uint32_t status;

    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status);
    P2->OUT ^= BIT1;

    //Checking where is selection buffer
    Sc.changeSel(true); //It shows which option to execute down botton

    //Sc.paintRec(true);
    __enable_irq();

}

void PORT5_IRQHandler(void) //up
{
    __disable_irq();
    for(int i = 0; i == 10000; i++)
    {
        __no_operation();
    }

    uint32_t status;

    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
    P2->OUT ^= BIT0;
    //Checking where is selection buffer
    Sc.changeSel(false);  //It shows which option to execute Up botton
   // Sc.paintRec(false);
    __enable_irq();
}

//void PORT1_IRQHandler(void)
//{
//    //rebound fix
//    __disable_irq();
//    for(int i = 0; i == 10000; i++)
//    {
//
//    }
//
//    uint32_t status;
//
//
//
//    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
//    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
//    P2->OUT ^= BIT2;
//
////    //Checking where is selection buffer
////    if(g_iOption == 1)
////    {
////        //record();
////        GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN6);
////            for(int i = 0; i<5000; i++)
////            {
////            }
////        GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN6);
////    }else if(g_iOption == 2)
////    {
////        //mod1();
////        GPIO_toggleOutputOnPin(GPIO_PORT_P5, GPIO_PIN6);
////            for(int i = 0; i<5000; i++)
////            {
////            }
////        GPIO_toggleOutputOnPin(GPIO_PORT_P5, GPIO_PIN6);
////    }else if(g_iOption == 3)
////    {
////        //mod2();
////        GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN4);
////            for(int i = 0; i<5000; i++)
////            {
////            }
////        GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN4);
////    }else if(g_iOption == 4)
////    {
////        //reset();
////        GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN6);
////            for(int i = 0; i<5000; i++)
////            {
////            }
////        GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN6);
////    }
//    __enable_irq();
//}

void PORT4_IRQHandler(void) // Select button
{
    __disable_irq();

    uint32_t status;


    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P4);
    GPIO_clearInterruptFlag(GPIO_PORT_P4, status);
    for(int i = 0; i == 1000; i++)
    {
        __no_operation();
    }
    switch (Sc.g_iOption) {
        case 0: //Recording mode
            P2->OUT = BIT0; //red LED ON
            //execute record save highest frecuency value in a determined amount of time
            //Mic.Record();
            P2->OUT ^= BIT0;//Turn Off red LED
            //P2->OUT = BIT1; // Turn ON GREEN LED system ready to reproduce high or low pitch
            Sc.paintRec(true); //Paint green rectangle to make sure recording is on

            break;

        case 1:
            //modulation type one
            //Reproduce modulation 1 with current recording
            //P2->OUT = BIT1; //Green LED
            //Mic.Mod1();
            //P2->OUT ^= BIT1;

            if(Mic.g_bIsPlaying){
                Mic.Stop();
                P2->OUT ^= BIT1; // No sound
            }else{
                Mic.Mod1();
                P2->OUT = BIT1; //playing sound
            }

            break;
        case 2:
            if(Mic.g_bIsPlaying){
                Mic.Stop();
                P2->OUT ^= BIT1; // No sound
            }else{
                Mic.Mod2();
                P2->OUT = BIT1; //playing sound
            }

            break;
        case 3:
            //Reset case
            //Delete recording and print initial screen again
            Mic.Reset();

            break;

        default:
            break;
    }
    __enable_irq();
}
}
