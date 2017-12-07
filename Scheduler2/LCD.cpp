/*
 * LCD.cpp
 *
 *  Created on: Oct 18, 2017
 *      Author: pablo
 */
#define __NOP __nop
#include "LCD.hpp"
#include "Task.hpp"

#define BLUE            0x1D83F7
#define BROWN           0x8B4513
#define m_u8FirstP      0
#define m_u8LastP       128
#define g_iNull         0

Graphics_Context g_sContext;



LCD::LCD(uint8_t i_u8TaskID)
{
    g_sTierra.xMax = 128;
    g_sTierra.xMin =  0;
    g_sTierra.yMax = 128;
    g_sTierra.yMin = 64;

    g_sCielo.xMin = 0;
    g_sCielo.xMax = 128;
    g_sCielo.yMax = 63;
    g_sCielo.yMin =0 ;

    g_sDelta.xMin = 0;
    g_sDelta.xMax = 128;
    g_sDelta.yMax = 0;
    g_sDelta.yMin =0 ;
    l_iX = 0;
    l_iY = 0;
    l_iZ = 0;
    this->SetTaskID(i_u8TaskID);
//    l_ioldDataZ = 0;

}
uint8_t LCD::setup()
{
        return 0;
}
uint8_t LCD::run()
{
    CalcLines();
    return 0;
}
void LCD::ProcessSMS(SMS i_NewSms)
{
    uint16_t l_pData = i_NewSms.u16Data;
    if(i_NewSms.u8Sender==5){  // 5 es el ID del ADC
        switch (i_NewSms.u8Type) {
            case 2:
                this->l_iX = l_pData;
                break;
            case 3:
                this->l_iY = l_pData;
                break;
            case 4:
                this->l_iZ = l_pData;
                //this->Waiting = true;
                this->SetToExec(true);
                break;
            default:
                break;
        }
    }
}


void LCD::CalcLines()
{
    int l_iNewGroundMin= -0.0229*l_iZ+264.724;

    if(l_iNewGroundMin<=g_sTierra.yMin){
        g_sDelta.xMin = 0;
        g_sDelta.xMax = 128;
        g_sDelta.yMax = g_sTierra.yMin;
        g_sDelta.yMin = l_iNewGroundMin;
        Graphics_fillRectangleOnDisplay(g_sContext.display,&g_sDelta, 0xa347);
    }else{
        g_sDelta.xMin = 0;
        g_sDelta.xMax = 128;
        g_sDelta.yMax = l_iNewGroundMin;
        g_sDelta.yMin = g_sTierra.yMin;
        Graphics_fillRectangleOnDisplay(g_sContext.display,&g_sDelta, 0x0080ff);

        //Graphics_drawLine(&g_sContext, 4, 4, 8, 8);
    };
   // Graphics_setForegroundColor(&g_sContext, 0xffffff);
    //Graphics_drawLine(&g_sContext, 4, 4, 8, 8);
    //int degx1 = -0.0305*l_iX+246.08;
    //if (degx1<0)



    g_sTierra.xMax = 128;
    g_sTierra.xMin =  0;
    g_sTierra.yMax = 128;
    g_sTierra.yMin = l_iNewGroundMin;

    g_sCielo.xMax = 128;
    g_sCielo.xMin = 0;
    g_sCielo.yMax = l_iNewGroundMin-1;
    g_sCielo.yMin = 0 ;


}

