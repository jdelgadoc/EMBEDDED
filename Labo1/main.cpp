/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
*******************************************************************************/
/* Includes */
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "HAL_I2C.h"
#include "HAL_OPT3001.h"
#include "HAL_I2C.c"
#include "HAL_OPT3001.c"
#include <stdint.h>     // Variables sintaxis
#include <stdbool.h>    // Boolean sintaxis

#include <vector>

#include "labo1.h"

using namespace std;
// Global Variables

uint8_t lights = 3;  // 1 for 1 light , 2 for 2 lights, 3 for 3 lights
bool blink = true;
bool debug = false;
bool sysinitial_ready = false;
bool syscounter_ready = false;
int blink_counter = 5; // ON and OFF times 3
bool L_State = false; // LAMP STATE- True = ON , False = OFF
volatile _Bool g_fState = false;                                // Global State Flag, to know if light is on or off
float lux;
bool day_night = true; // true = day, false = night
uint16_t lux_min = 40;
uint16_t ADC14Result = 8192;
bool newdata = false;
int data_counter = 0;
float samples [20];
float average5sec;
float averagelastsec;
float Noise_Level = 1.4;





int main(void)
{
    /* Stop Watchdog  */
    WDT_A_holdTimer();

    setup();
    BLINK_STARTHandler();
    TurnOFF_LIGHTHandler();


    while(true){
//        if(newdata){
            DATA_ADCHandler();
//        }
    }


}




void setup(void)
{
    //LIGHTS
    /* Configuring RGB in expansion pack as outputs, controlling 1, 2 or 3 lights */
    switch (lights) {
        case 1:
            GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN6);
        break;
        case 2:
            GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN6);
            GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN4);
        break;
        case 3:
            GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN6);   // RED
            GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN4);   // GREEN
            GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN6);   // BLUE
        break;

        default:
            GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN6);
            break;
    }

    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);   // RED
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);   // GREEN
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);   // BLUE


    //BUTTON
    /* Configuring P3.5 (button) as an input and enabling interrupts */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN5);
    Interrupt_enableInterrupt(INT_PORT3);
    __disable_irq();

    /* Enabling SRAM Bank Retention */
    SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK1);

    //LIGHT SENSOR

    /* Initialize I2C communication */
    Init_I2C_GPIO();
    I2C_init();

    /* Initialize OPT3001 digital ambient light sensor */
    OPT3001_init();
    //__delay_cycles(100000);


    // ADC14 setup
    ADC14->CTL0 = ADC14_CTL0_PDIV_0 | ADC14_CTL0_SHS_0 | ADC14_CTL0_DIV_7 |
                  ADC14_CTL0_SSEL__MCLK | ADC14_CTL0_SHT0_1 | ADC14_CTL0_ON
                  | ADC14_CTL0_SHP;
    ADC14->MCTL[0] = ADC14_MCTLN_INCH_10 | ADC14_MCTLN_VRSEL_0;
    ADC14->CTL0 = ADC14->CTL0 | ADC14_CTL0_ENC;
    ADC14->IER0 = ADC14_IER0_IE0;
    NVIC_SetPriority(ADC14_IRQn,1);
    NVIC_EnableIRQ(ADC14_IRQn);

    // PORT MIC setup

    P4->SEL0 = BIT3;
    P4->SEL1 = BIT3;
    P4->DIR &= ~BIT3;


    // First day_night test

    CHECK_LIGHTHandler();



    /* Enabling MASTER interrupts */
    Interrupt_enableMaster();


}

void TurnON_LIGHTHandler(void){

    CHECK_LIGHTHandler();
    if (!day_night){
        switch (lights) {
            case 1:
                GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
                L_State=true;
            break;
            case 2:
                GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
                GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);
                L_State=true;
            break;
            case 3:
                GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);   // RED
                GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);   // GREEN
                GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);   // BLUE
                L_State=true;
            break;

            default:
                L_State=true;
                break;
         }

    }
    WAIT_TIMEHandler();
}

void TurnOFF_LIGHTHandler(void){

    switch (lights) {
        case 1:
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
            L_State=false;
        break;
        case 2:
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);
            L_State=false;
        break;
        case 3:
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);   // RED
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);   // GREEN
            GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);   // BLUE
            L_State=false;
        break;

        default:
            L_State=false;
            break;
    }

}

void CHECK_LIGHTHandler(void){
    lux = OPT3001_getLux();
    if (lux <= lux_min) {
        day_night = false;
    }else {
        day_night = true;
    }
}
void BLINK_STARTHandler(void){
    /* Timer 32_1 setup */ //BLINKING
    TIMER32_1->CONTROL = 0;
    TIMER32_1->LOAD = 0x001FE360; // 0.5 segundos
    TIMER32_1->CONTROL = TIMER32_CONTROL_SIZE | TIMER32_CONTROL_PRESCALE_0 | TIMER32_CONTROL_MODE | TIMER32_CONTROL_IE | TIMER32_CONTROL_ENABLE;
    NVIC_SetPriority(T32_INT1_IRQn,1);
    NVIC_EnableIRQ(T32_INT1_IRQn);
}

void WAIT_TIMEHandler(void){
    /* Timer 32_2 setup*/
    TIMER32_2->CONTROL = 0; // restarts the count
    TIMER32_2->LOAD = 0x0036E360; // 4 segundos
    TIMER32_2->CONTROL = TIMER32_CONTROL_SIZE | TIMER32_CONTROL_PRESCALE_0 | TIMER32_CONTROL_MODE | TIMER32_CONTROL_IE | TIMER32_CONTROL_ENABLE;
    NVIC_SetPriority(T32_INT2_IRQn, 1);
    NVIC_EnableIRQ(T32_INT2_IRQn);
}

void CHECK_READY(void){
    int checkready;

    for (int i = 30; i < 36 ; i++) {
            checkready += samples[i];
        }
    if (checkready ==0){
        sysinitial_ready = false;       // COntador para ver que el array este lleno antes de empezar
    }else{                              // se espera que se llenen los primeros 5 segundos
        sysinitial_ready = true;
    }
}


void CHECK_COUNTER(void){

    if (data_counter<4) {
        syscounter_ready=false ;         // Contador para activar condicion de luces solo una vez por segundo
    }else{
        syscounter_ready=true;
        data_counter=0;
    }
}
void DATA_ADCHandler(void){
    debug = false;

    for (int i = 0; i < 19; i++) {
        samples[i+1] = samples[i];
    }
    if (ADC14Result > 8192) {
        samples[0] = ADC14Result - 8192; // Valor absoluto
    } else {
        samples[0] = 8192 -ADC14Result ;
    }
    CHECK_READY();
    CHECK_COUNTER();

    float sum_samples=0;
    for (int i = 0; i < 20 ; i++) {
        sum_samples += samples[i];
    }

    average5sec = sum_samples/20;
    average5sec = average5sec*Noise_Level; //MAX accepted noise
    sum_samples = 0;
    for (int i = 0; i < 4 ; i++) {
        sum_samples += samples[i];
    }

    averagelastsec = sum_samples/4;
    if((averagelastsec>average5sec)&(sysinitial_ready)&(syscounter_ready)){
        TurnON_LIGHTHandler();
    }
    newdata=false;
}



extern "C"{

    void ADC14_IRQHandler(void)
    {
        __disable_irq();
        ADC14Result = ADC14->MEM[0];
        ADC14->CLRIFGR0 = ADC14_CLRIFGR0_CLRIFG0;
        newdata=true;
        data_counter++;
        __enable_irq();
        return;
    }


    void PORT3_IRQHandler(void)
    {
        uint32_t status;

        status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
        GPIO_clearInterruptFlag(GPIO_PORT_P3, status);

        /* Toggling the output on the LED */
        if(L_State == false)
        {
            switch (lights) {
                        case 1:
                            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
                            L_State=true;
                        break;
                        case 2:
                            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
                            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);
                            L_State=true;
                        break;
                        case 3:
                            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);   // RED
                            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);   // GREEN
                            GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);   // BLUE
                            L_State=true;
                        break;

                        default:
                            L_State=true;
                            break;
                     }
            TurnON_LIGHTHandler();
//                int x =0;
//
//                while(x<0x000F1B00){
//                    x++;
//                };
//                TurnOFF_LIGHTHandler();
        }else {
            TurnOFF_LIGHTHandler();
        }
    }


    void T32_INT1_IRQHandler(void)
    {
        __disable_irq();
        TIMER32_1->INTCLR = 0U;
        if (blink){
        if (blink_counter>0){
            blink = true;
        }else {
            blink = false;
            TIMER32_1->CONTROL = 0;
            TIMER32_1->LOAD = 0x000B71B0; // 0.25 segundos
//            TIMER32_1->LOAD = 0x000493E0; // 0.1 segundo
            TIMER32_1->CONTROL = TIMER32_CONTROL_SIZE | TIMER32_CONTROL_PRESCALE_0 | TIMER32_CONTROL_MODE | TIMER32_CONTROL_IE | TIMER32_CONTROL_ENABLE;
            NVIC_SetPriority(T32_INT1_IRQn,1);
            NVIC_EnableIRQ(T32_INT1_IRQn);
        }


            switch (lights) {
                case 0:
                    blink_counter = 0;
                break;

                case 1:
                    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN6);
                    L_State= !L_State;
                    blink_counter --;
                break;

                case 2:
                    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN6);
                    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN4);
                    L_State= !L_State;
                    blink_counter --;
                break;

                case 3:
                    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN6);   // RED
                    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN4);   // GREEN
                    GPIO_toggleOutputOnPin(GPIO_PORT_P5, GPIO_PIN6);   // BLUE
                    L_State= !L_State;
                    blink_counter --;
                break;

                default:
                    blink_counter --;
                    break;
            }
        }else {
            ADC14->CTL0 = ADC14->CTL0 | ADC14_CTL0_SC; // Start //init adc 6 samples per sec
        }
        __enable_irq();
        return;
    }

    void T32_INT2_IRQHandler(void) {
        __disable_irq();
        TIMER32_2->INTCLR = 0U;
        debug =true;
        switch (lights) {
            case 1:
                GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
                L_State=false;
            break;
            case 2:
                GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
                GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);
                L_State=false;
            break;
            case 3:
                GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);   // RED
                GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);   // GREEN
                GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);   // BLUE
                L_State=false;
            break;

            default:
                L_State=false;
                break;
        }
        __enable_irq();
        return;
    }

}
