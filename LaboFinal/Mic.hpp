extern "C"{
    #include <ti/devices/msp432p4xx/driverlib/driverlib.h>
    #include "LcdDriver/Crystalfontz128x128_ST7735.h"
    #include <ti/grlib/grlib.h>
    //#include <arm_math.h>
    //#include "arm_const_structs.h"
}
#include <arm_math.h>
#include "arm_const_structs.h"

#define TEST_LENGTH_SAMPLES 512
#define SAMPLE_LENGTH 512
#define SMCLK_FREQUENCY     48000000
#define SAMPLE_FREQUENCY    8000
#define FREQ_SLOPE          6000000
#define LUX_DIVISOR         5000
#define NO_SOUND            20

/* Configuracion PWM del Timer A */
Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        (SMCLK_FREQUENCY/SAMPLE_FREQUENCY),
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_SET_RESET,
        (SMCLK_FREQUENCY/SAMPLE_FREQUENCY)/2
};

#ifdef ewarm
#pragma data_alignment=256
#else
#pragma DATA_ALIGN(g_u8ControlTable, 256)
#endif
uint8_t g_u8ControlTable[256];

class Mic{
    public:
        q15_t l_q15MaxValue;
        uint32_t l_u32MaxIndex = 0;
        float l_fMaxFreq ;
        char l_cStringFreq [20];
        float l_fSampledFreq;
        uint32_t l_u32Lux;
        uint32_t g_u32FftSize = SAMPLE_LENGTH;
        uint32_t g_u32IfftFlag = 0;
        uint32_t g_u32DoBitReverse = 1;
        volatile arm_status g_asStatus;
        float g_fHann[SAMPLE_LENGTH];
        int16_t g_i16DataArray1[SAMPLE_LENGTH];
        int16_t g_i16DataArray2[SAMPLE_LENGTH];
        int16_t g_i16DataInput[SAMPLE_LENGTH*2];
        int16_t g_i16DataOutput[SAMPLE_LENGTH];
        bool g_bIsPlaying;
        void Setup();
        void Stop();
        void Reset();
        void Record();
        void Mod1();
        void Mod2();

};
