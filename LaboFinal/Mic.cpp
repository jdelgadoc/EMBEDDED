#include "Mic.hpp"

#define TEST_LENGTH_SAMPLES 512
#define SAMPLE_LENGTH 512
#define SMCLK_FREQUENCY     48000000
#define SAMPLE_FREQUENCY    8000
#define FREQ_SLOPE          6000000
#define LUX_DIVISOR         5000
#define NO_SOUND            20


/* Variables globales para la FFT */

uint32_t g_u32FftSize = SAMPLE_LENGTH;
uint32_t g_u32IfftFlag = 0;
uint32_t g_u32DoBitReverse = 1;
volatile arm_status g_asStatus;

/* Tabla de control del DMA */
#ifdef ewarm
#pragma data_alignment=256
#else
#pragma DATA_ALIGN(g_u8ControlTable, 256)
#endif

uint8_t g_u8ControlTable[256];

/* Buffers de procesamiento de la fft*/
float g_fHann[SAMPLE_LENGTH];
int16_t g_i16DataArray1[SAMPLE_LENGTH];
int16_t g_i16DataArray2[SAMPLE_LENGTH];
int16_t g_i16DataInput[SAMPLE_LENGTH*2];
int16_t g_i16DataOutput[SAMPLE_LENGTH];


Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        (SMCLK_FREQUENCY/SAMPLE_FREQUENCY),
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_SET_RESET,
        (SMCLK_FREQUENCY/SAMPLE_FREQUENCY)/2
};

Mic::Mic()
{

}

void Mic::Setup(){
    int n;
        for (n = 0; n < SAMPLE_LENGTH; n++)
        {
            g_fHann[n] = 0.5 - 0.5 * cosf((2*PI*n)/(SAMPLE_LENGTH-1));
        }

        /* Configuracion del Timer_A1   */
        Timer_A_generatePWM(TIMER_A1_BASE, &pwmConfig);

        /* Inicializacion del ADC14 */
        ADC14_enableModule();
        ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, 0);

        ADC14_setSampleHoldTrigger(ADC_TRIGGER_SOURCE3, false); // gatillo por timer A1

        /*Configuracion del TimerA0 para el buzzer*/
        TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_ID__8 ;
        TIMER_A0->CTL |=  TIMER_A_CTL_MC__UP;
        TIMER_A0->CCR[0] =NO_SOUND ;
        TIMER_A0->CCR[4] =NO_SOUND ;// Inicialmente no debe zonar
        TIMER_A0->CCTL[4] |= TIMER_A_CCTLN_OUTMOD_3; //Set/Reset (cuando CCR[0] se pone en 0, cuando CCR[4] se pone en 1)
        P2->DIR |= BIT7; // La salida para el buzzer es el bit 7 del puerto 2
        P2->SEL0 =BIT7;
        P2->SEL1 =0;
        // Mapeo del Timer_A0.4 al pin 2.7 (PWM) output
        PMAP->KEYID = PMAP_KEYID_VAL;
        P2MAP ->PMAP_REGISTER7 |= PMAP_TA0CCR4A;
        /* GPIO del microfono (4.3 A10) */
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN3,
        GPIO_TERTIARY_MODULE_FUNCTION);

        /* Memoria del ADc */
        ADC14_configureSingleSampleMode(ADC_MEM0, true);
        ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,
        ADC_INPUT_A10, false);

        /* Resultado del ADC en binario con signo*/
        ADC14_setResultFormat(ADC_SIGNED_BINARY);

        /* Configuracion del DMA */
        DMA_enableModule();
        DMA_setControlBase(g_u8ControlTable);


        DMA_disableChannelAttribute(DMA_CH7_ADC14,
                                     UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                     UDMA_ATTR_HIGH_PRIORITY |
                                     UDMA_ATTR_REQMASK);
        MAP_DMA_setChannelControl(UDMA_PRI_SELECT | DMA_CH7_ADC14,
                UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_1);
        MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT | DMA_CH7_ADC14,
            UDMA_MODE_PINGPONG, (void*) &ADC14->MEM[0],
            g_i16DataArray1, SAMPLE_LENGTH);

        MAP_DMA_setChannelControl(UDMA_ALT_SELECT | DMA_CH7_ADC14,
            UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_1);
        MAP_DMA_setChannelTransfer(UDMA_ALT_SELECT | DMA_CH7_ADC14,
            UDMA_MODE_PINGPONG, (void*) &ADC14->MEM[0],
            g_i16DataArray2, SAMPLE_LENGTH);
        /* Se habilitan las interrupciones */
        MAP_DMA_assignInterrupt(DMA_INT1, 7);
        MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
        MAP_DMA_assignChannel(DMA_CH7_ADC14);
        MAP_DMA_clearInterruptFlag(7);
        MAP_Interrupt_enableMaster();
        /*Se habilitan las conversiones */
        MAP_DMA_enableChannel(7);
        MAP_ADC14_enableConversion();
}

void Mic::Stop(){
    TIMER_A0->CCR[0] =NO_SOUND ;
    TIMER_A0->CCR[4] =NO_SOUND ;
};
void Mic::Reset(){
    l_fMaxFreq = 0;
    l_u32MaxIndex = 0;
};
void Mic::Record(){
    for (i=0; i<512; i++)
    {
        g_i16DataArray1[i] = (int16_t)(g_fHann[i]*g_i16DataArray1[i]);
    }
    arm_rfft_instance_q15 instance;
    g_asStatus = arm_rfft_init_q15(&instance, g_u32FftSize, g_u32IfftFlag, g_u32DoBitReverse);

    arm_rfft_q15(&instance, g_i16DataArray1, g_i16DataInput);
    for (i = 0; i < 1024; i+=2)
    {
        g_i16DataOutput[i/2] = (int32_t)(sqrtf((g_i16DataInput[i] * g_i16DataInput[i]) + (g_i16DataInput[i+1] * g_i16DataInput[i+1])));
    }
    arm_max_q15(g_i16DataOutput, g_u32FftSize, &l_q15MaxValue, &l_u32MaxIndex);

};
void Mic::Mod1(){
    l_fMaxFreq=l_u32MaxIndex*15.79-10.58;
    l_fSampledFreq=l_fMaxFreq;

    l_fSampledFreq=l_fMaxFreq; // si es la primera vez en este estado

    TIMER_A0->CCR[0] =FREQ_SLOPE/(l_fSampledFreq);  //Periodo
    TIMER_A0->CCR[4] =FREQ_SLOPE/(2*l_fSampledFreq); //Semiperiodo

};
void Mic::Mod2(){
    l_fMaxFreq=l_u32MaxIndex*15.79-50.58;
    l_fSampledFreq=l_fMaxFreq;

    l_fSampledFreq=l_fMaxFreq; // si es la primera vez en este estado

    TIMER_A0->CCR[0] =FREQ_SLOPE/(l_u32Lux*l_fSampledFreq);  //Periodo
    TIMER_A0->CCR[4] =FREQ_SLOPE/(2*l_u32Lux*l_fSampledFreq); //Semiperiodo
};




