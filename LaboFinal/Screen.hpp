#ifndef SCREEN_HPP_
#define SCREEN_HPP_

extern "C"{
    #include <ti/devices/msp432p4xx/driverlib/driverlib.h>
    #include "LcdDriver/Crystalfontz128x128_ST7735.h"
    #include <ti/grlib/grlib.h>
}

class Screen
{
    public:
        /* Graphic library context */
        //Graphics_Context g_sContext;

        /* ADC results buffer */
        int g_iOption;
        bool g_bIsRecSaved;
        Screen();
        void setup(void);
        void drawMainScreen(void);
        void changeSel(bool i_bUpDown); // true for up
        void paintRec(bool i_bIsPainted);

        Graphics_Rectangle g_sRect1;
        Graphics_Rectangle g_sRect2;
        Graphics_Rectangle g_sRect3;
        Graphics_Rectangle g_sRect4;
        Graphics_Rectangle g_sRect5;
        Graphics_Rectangle g_sRect6;
        Graphics_Rectangle g_sRect7;
        Graphics_Rectangle g_sRect8;
        Graphics_Rectangle g_sRect9;
        Graphics_Rectangle g_sRect10;
};

#endif /* SCREEN_HPP_ */
