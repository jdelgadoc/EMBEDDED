#include "Screen.hpp"

//Screen::Screen()

Graphics_Context g_sContext;

Screen::Screen()
{
    g_iOption = 0;
    g_bIsRecSaved = false;
}
void Screen::setup()
{
    MAP_WDT_A_holdTimer();
    MAP_Interrupt_disableMaster();
    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);

    //Dimensions of rectangles to be drawn in the screen.
    g_sRect1.xMin = 0;
    g_sRect1.yMin = 22;
    g_sRect1.xMax = 127;
    g_sRect1.yMax = 26;

    g_sRect2.xMin = 0;
    g_sRect2.yMin = 46;
    g_sRect2.xMax = 127;
    g_sRect2.yMax = 50;

    g_sRect3.xMin = 0;
    g_sRect3.yMin = 70;
    g_sRect3.xMax = 127;
    g_sRect3.yMax = 74;

    g_sRect4.xMin = 0;
    g_sRect4.yMin = 94;
    g_sRect4.xMax = 127;
    g_sRect4.yMax = 98;

    g_sRect5.xMin = 14;
    g_sRect5.yMin = 22;
    g_sRect5.xMax = 18;
    g_sRect5.yMax = 127;

    g_sRect6.xMin = 0;
    g_sRect6.yMin = 27;
    g_sRect6.xMax = 13;
    g_sRect6.yMax = 45;

    g_sRect7.xMin = 0;
    g_sRect7.yMin = 51;
    g_sRect7.xMax = 13;
    g_sRect7.yMax = 69;

    g_sRect8.xMin = 0;
    g_sRect8.yMin = 75;
    g_sRect8.xMax = 13;
    g_sRect8.yMax = 93;

    g_sRect9.xMin = 0;
    g_sRect9.yMin = 99;
    g_sRect9.xMax = 13;
    g_sRect9.yMax = 127;

    g_sRect10.xMin = 0;
    g_sRect10.yMin = 0;
    g_sRect10.xMax = 18;
    g_sRect10.yMax = 21;

    drawMainScreen();
    paintRec(false); // paint red rectangle to represent no recording
    __enable_irq();

}

void Screen::drawMainScreen()
{
    Graphics_clearDisplay(&g_sContext);
    Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect1, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect2, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect3, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect4, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect5, GRAPHICS_COLOR_BLACK);
    Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect6, GRAPHICS_COLOR_SEASHELL);

    Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)"IN MEM",
                                        8,
                                        64,
                                        12,
                                        OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)"NEW REC",
                                        8,
                                        64,
                                        36,
                                        OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)"MOD 1",
                                        8,
                                        64,
                                        60,
                                        OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)"MOD 2",
                                        8,
                                        64,
                                        84,
                                        OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)"RESET",
                                        8,
                                        64,
                                        110,
                                        OPAQUE_TEXT);
    Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect10, 0xF800);

}

void Screen::changeSel(bool i_bUpDown)
{


    if(i_bUpDown){
        switch (g_iOption) {
            case 0:
                Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect6, GRAPHICS_COLOR_WHITE);
                Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect7, GRAPHICS_COLOR_SEASHELL);
                //Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect8, GRAPHICS_COLOR_WHITE);
                //Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect9, GRAPHICS_COLOR_WHITE);
                this->g_iOption = 1;

                break;
            case 1:
                Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect7, GRAPHICS_COLOR_WHITE);
                Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect8, GRAPHICS_COLOR_SEASHELL);
                this->g_iOption = 2;
                break;
            case 2:
                Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect8, GRAPHICS_COLOR_WHITE);
                Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect9, GRAPHICS_COLOR_SEASHELL);
                this->g_iOption = 3;
                break;
            case 3:
                this->g_iOption = 3;
                break;
            default:
                break;
        }

    }else
    {
        switch (g_iOption) {
                    case 0:
                        this->g_iOption = 0;

                        break;
                    case 1:
                        Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect7, GRAPHICS_COLOR_WHITE);
                        Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect6, GRAPHICS_COLOR_SEASHELL);
                        this->g_iOption = 0;
                        break;
                    case 2:
                        Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect8, GRAPHICS_COLOR_WHITE);
                        Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect7, GRAPHICS_COLOR_SEASHELL);
                        this->g_iOption = 1;
                        break;
                    case 3:
                        Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect9, GRAPHICS_COLOR_WHITE);
                        Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect8, GRAPHICS_COLOR_SEASHELL);
                        this->g_iOption = 2;
                        break;
                    default:
                        break;
                }
    }


}

void Screen::paintRec(bool i_bIsPainted)
{
    if(i_bIsPainted){
        Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect10, 0x07E0);
        this->g_bIsRecSaved = true;
    }else{
        Graphics_fillRectangleOnDisplay(g_sContext.display, &g_sRect10, 0xF800);
        this->g_bIsRecSaved = false;

    }
}
