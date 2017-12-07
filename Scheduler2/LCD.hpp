/*
 * LCD.hpp
 *
 *  Created on: Oct 18, 2017
 *      Author: pablo
 */

#ifndef LCD_HPP_
#define LCD_HPP_
#define __NOP __nop
#include <ti/devices/msp432p4xx/inc/msp.h>
#include "Task.hpp"
extern "C"{
    #include <ti/devices/msp432p4xx/driverlib/driverlib.h>
    #include <ti/grlib/grlib.h>
    #include "LcdDriver/Crystalfontz128x128_ST7735.h"
}


extern uint16_t g_resultsBuffer[3];
class LCD : public Task
{
    public:
        LCD(uint8_t i_u8TaskID);
        virtual uint8_t run();
        virtual uint8_t setup(void);
        virtual void ProcessSMS(SMS i_NewSms);

        virtual void CalcLines();
        Graphics_Rectangle g_sTierra;
        Graphics_Rectangle g_sCielo;
        Graphics_Rectangle g_sDelta;
        int l_iLines;
        int l_iX;
        int l_iY;
        int l_iZ;



        //int PaintCalc();
        //PRINTING SCREEN METHODS
    protected:

    private:

};




#endif /* LCD_HPP_ */
