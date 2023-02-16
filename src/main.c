/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
// Screen includes
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"


/* Standard Includes */
#include <stdint.h>
#include<stdio.h>
#include <stdbool.h>

//LCD
Graphics_Context g_sContext;

//UART
uint8_t TXData = 1;
uint8_t RXData = 0;
uint8_t data_received[10];
int counter = 0;
char str[4];

//ADC conversion
static uint16_t joystickBuffer[2];
static uint16_t accelerometer_z_axis;

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const eUSCI_UART_ConfigV1 uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        26,//13,                                      // BRDIV = 13
        0,                                       // UCxBRF = 0
        111,//37,                                      // UCxBRS = 37
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // MSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
        EUSCI_A_UART_8_BIT_LEN                  // 8 bit data length
};


//LCD display
void _graphicsInit()
{
    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
}

//DriverLib

void setUpButtons(){
    //clean voltage in buttons
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN5);

    /* P1.1 as input for button */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);


    /* P1.4 as input for button */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);

    //buttons boosterpack
    // J4.33 -> p5.1
    // J4.32 -> p3.5
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5,GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P5,GPIO_PIN1);

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3,GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P3,GPIO_PIN5);
    //registering to NVIC
    Interrupt_enableInterrupt(INT_PORT3);
    Interrupt_enableInterrupt(INT_PORT5);
    Interrupt_enableInterrupt(INT_PORT1);

    //clear interrupt flags
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN5);

    /* activate interrupt notification */
    Interrupt_enableMaster();
}

void _adcInit(){
    /* Configures Pin 6.0 and 4.4 as ADC input for the jotstick*/
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);
        /*6.1 as ADC input in order to read z axis from accelerometer*/
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN1,GPIO_TERTIARY_MODULE_FUNCTION);

        /* Initializing ADC (ADCOSC/64/8) */
        ADC14_enableModule();
        ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

        /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM1 (A15, A9)  with repeat)
             * with internal 2.5v reference */
        ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);
        ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, true);
        ADC14_configureConversionMemory(ADC_MEM0,
                ADC_VREFPOS_AVCC_VREFNEG_VSS,
                ADC_INPUT_A15, ADC_NONDIFFERENTIAL_INPUTS);

        ADC14_configureConversionMemory(ADC_MEM1,
                ADC_VREFPOS_AVCC_VREFNEG_VSS,
                ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);

        ADC14_configureConversionMemory(ADC_MEM2,ADC_VREFPOS_AVCC_VREFNEG_VSS,
                ADC_INPUT_A11, ADC_NONDIFFERENTIAL_INPUTS);

        /* Enabling the interrupt when a conversion on channel 1 (end of sequence)
         *  is complete and enabling conversions */
        ADC14_enableInterrupt(ADC_INT1);
        //enable interrupt for accelerometer
        ADC14_enableInterrupt(ADC_INT2);

        /* Enabling Interrupts */
        Interrupt_enableInterrupt(INT_ADC14);
        Interrupt_enableMaster();

        /* Setting up the sample timer to automatically step through the sequence
         * convert.
         */
        ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

        /* Triggering the start of the sample */
        ADC14_enableConversion();
        ADC14_toggleConversionTrigger();
}

void sendString(char* str) {
    while(*str != '\0') {
        UART_transmitData(EUSCI_A2_BASE, *str);
        str++;
    }
    while(UART_receiveData(EUSCI_A2_BASE) != 37){};
}

void setUpUART(){
    /* Selecting P1.2 and P1.3 in UART mode and P1.0 as output (LED) */
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
        GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        /* Setting DCO to 24MHz (upping Vcore) -> CPU operates at 24 MHz!*/
        FlashCtl_setWaitState(FLASH_BANK0, 2);
        FlashCtl_setWaitState(FLASH_BANK1, 2);
        PCM_setCoreVoltageLevel(PCM_VCORE1);
        CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);

        /* Configuring UART Module */
        UART_initModule(EUSCI_A2_BASE, &uartConfig);

        /* Enable UART module */
        UART_enableModule(EUSCI_A2_BASE);

        /* Enabling interrupts */
        UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
        Interrupt_enableInterrupt(INT_EUSCIA2);
        Interrupt_enableSleepOnIsrExit();
}

void _hwinit(){
    WDT_A_holdTimer();
    Interrupt_disableMaster();

    /* Set the core voltage level to VCORE1 */
    PCM_setCoreVoltageLevel(PCM_VCORE1);

    /* Set 2 flash wait states for Flash bank 0 and 1*/
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);

    /* Initializes Clock System */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);


    //Flush stdout -- remove this!!!!
    fflush(stdout);
    fflush(stdout);
}




int main(void){
    _hwinit();
    setUpButtons();
    _adcInit();
    //_graphicsInit();
    setUpUART();
    //Graphics_drawStringCentered(&g_sContext, (int8_t *) "Hello", AUTO_STRING_LENGTH,60, 60, OPAQUE_TEXT);

    while(1)
    {
        Interrupt_enableSleepOnIsrExit();
        PCM_gotoLPM0InterruptSafe();
    }
}


/* EUSCI A0 UART ISR - Echos data back to PC host */
void EUSCIA2_IRQHandler(void)
{
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        RXData = UART_receiveData(EUSCI_A2_BASE);
        printf("%c\n", RXData);
        UART_transmitData(EUSCI_A2_BASE,'%');

        Interrupt_disableSleepOnIsrExit();
    }

}


//Interrupts
void PORT1_IRQHandler(){

    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    if (status & GPIO_PIN1){
        printf("ho premuto 1.1\n");
    }
    if (status & GPIO_PIN4){
        printf("ho premuto 1.4\n"); //check why tf this keeps launching an interrupt
    }
}

bool playing = false;

void PORT5_IRQHandler(){
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5,status);
    if (status & GPIO_PIN1){
        if (playing){
            printf("stop\n");
            playing = false;
        }
        else{
            printf("play\n");
            playing = true;
        }
    }
}


void PORT3_IRQHandler(){
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    GPIO_clearInterruptFlag(GPIO_PORT_P3,status);
    if (status & GPIO_PIN5 ){
        printf("ormai è periodica sta foto\n"); //same as P1.4
    }
}

//Joystick values reading

//check if Joystick is not tilted at all
bool isInIdleState(int x){
    return ((x>7000) && (x<9000));
}
bool tilted = false;

//ADC for joystick and accelerometer
void ADC14_IRQHandler(void){
    uint64_t status;
    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    //Joystick reading finished
    if(status & ADC_INT1){ //Conversion is over
        //x goes from 500 to 16k because it is broken
        //y goes from 0 to 16k
        joystickBuffer[0] = ADC14_getResult(ADC_MEM0);
        joystickBuffer[1] = ADC14_getResult(ADC_MEM1);
        //printf("x %d y %d\n",joystickBuffer[0],joystickBuffer[1]);
        //I need 2 bools to check if the joystick is tilted in a certain direction
        if(joystickBuffer[0] > 13000 && !tilted){
            printf("next\n");
            char str[4] = {'n', 'e', 'x', 't','\0'};
            sendString(str);
            tilted = true;
        }
        if(joystickBuffer[0] < 3500 && !tilted){
            printf("prev\n");
            char str[4] = {'p', 'r', 'e', 'v','\0'};
            sendString(str);
            tilted = true;
        }
        if(joystickBuffer[1] > 13000 && !tilted){
            printf("upup\n");
            char str[4] = {'u', 'p', 'u', 'p','\0'};
            sendString(str);
            tilted = true;
        }
        if(joystickBuffer[1] < 500 && !tilted){
            printf("down \n");
            char str[4] = {'d', 'o', 'w', 'n','\0'};
            sendString(str);
            tilted = true;
        }
        if(isInIdleState(joystickBuffer[0]) && isInIdleState(joystickBuffer[1])){
            tilted = false;
        }
    }
    //acceleometer reading
    if (status & ADC_INT2){
        accelerometer_z_axis = ADC14_getResult(ADC_MEM2);
        bool up_condition = accelerometer_z_axis> 14000;
        bool down_condition = accelerometer_z_axis < 9000;
        if (up_condition){
            printf("upup\n");
            _delay_cycles(5000000);
        }
        if (down_condition){
            printf("down\n");
            _delay_cycles(5000000);
        }
    }
}
