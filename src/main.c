/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include<stdio.h>
#include <stdbool.h>

//UART
uint8_t TXData = 1;
uint8_t RXData = 0;
uint8_t data_received[10];
int counter = 0;
char str[4];

//ADC conversion
static uint16_t resultsBuffer[2];

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const eUSCI_UART_ConfigV1 uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        13,                                      // BRDIV = 13
        0,                                       // UCxBRF = 0
        37,                                      // UCxBRS = 37
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // MSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
        EUSCI_A_UART_8_BIT_LEN                  // 8 bit data length
};


//DriverLib

void setUpButtons(){

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

    /* activate interrupt notification */
    Interrupt_enableMaster();
}

void _adcInit(){
    /* Configures Pin 6.0 and 4.4 as ADC input */
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

        /* Initializing ADC (ADCOSC/64/8) */
        ADC14_enableModule();
        ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

        /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM1 (A15, A9)  with repeat)
             * with internal 2.5v reference */
        ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);
        ADC14_configureConversionMemory(ADC_MEM0,
                ADC_VREFPOS_AVCC_VREFNEG_VSS,
                ADC_INPUT_A15, ADC_NONDIFFERENTIAL_INPUTS);

        ADC14_configureConversionMemory(ADC_MEM1,
                ADC_VREFPOS_AVCC_VREFNEG_VSS,
                ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);

        /* Enabling the interrupt when a conversion on channel 1 (end of sequence)
         *  is complete and enabling conversions */
        ADC14_enableInterrupt(ADC_INT1);

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
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
                 GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
        GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        /* Setting DCO to 24MHz (upping Vcore) -> CPU operates at 24 MHz!*/
        FlashCtl_setWaitState(FLASH_BANK0, 1);
        FlashCtl_setWaitState(FLASH_BANK1, 1);
        PCM_setCoreVoltageLevel(PCM_VCORE1);
        CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);

        /* Configuring UART Module */
        UART_initModule(EUSCI_A2_BASE, &uartConfig);

        /* Enable UART module */
        UART_enableModule(EUSCI_A2_BASE);

        /* Enabling interrupts */
        UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
        Interrupt_enableInterrupt(INT_EUSCIA2);
        Interrupt_enableSleepOnIsrExit();
}

//check if Joystick is not tilted at all
bool isInIdleState(int x){
    return ((x>7000) && (x<9000));
}



int main(void){
    _adcInit();
    //setUpButtons();
    //setUpUART(); not using it atm
    /* Halting WDT  */
    WDT_A_holdTimer();

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
//        printf("data received\n");
//        data_received[0]= UART_receiveData(EUSCI_A2_BASE);
//        data_received[1]= UART_receiveData(EUSCI_A2_BASE);
//        data_received[2]= UART_receiveData(EUSCI_A2_BASE);
//        data_received[3]= UART_receiveData(EUSCI_A2_BASE);
//        data_received[4] = '\0';
//            int i=0;
//            for(;i<4;i++){
//                printf("%c", data_received[i]);
//            }
//            printf("\n");
        RXData = UART_receiveData(EUSCI_A2_BASE);
        printf("%c\n", RXData);
        UART_transmitData(EUSCI_A2_BASE,'%');

        Interrupt_disableSleepOnIsrExit();
    }

}


void PORT1_IRQHandler(){

    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    if (status & GPIO_PIN1){
        printf("ho premuto 1.1\n");
    }
    if (status & GPIO_PIN4){
        printf("ho premuto 1.4\n"); //check why tf this keeps launching an interrupt
    }
//    if(P1->IFG & BIT4){
//        printf("ho spedito brother\n");
//        char str[4] = {'n', 'e', 'x', 't','\0'};
//        sendString(str);
//        char str2[4] = {'p','l','a','y','\0'};
//        sendString(str2);
//        char str3[4] = {'s','t','o','p','\0'};
//        sendString(str3);
//    }
}

void PORT5_IRQHandler(){
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5,status);
    if (status & GPIO_PIN1){
        printf("Stai premendo S1\n");
    }
}

void PORT3_IRQHandler(){
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    GPIO_clearInterruptFlag(GPIO_PORT_P3,status);
    if (status & GPIO_PIN5){
        printf("Stai premendo S2\n"); //same as P1.4
    }
}

bool x_tilted = false;
bool y_tilted = false;

void ADC14_IRQHandler(void){
    uint64_t status;
    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    if(status & ADC_INT1){ //Conversion is over
        //x goes from 500 to 16k because it is broken
        //y goes from 0 to 16k
        resultsBuffer[0] = ADC14_getResult(ADC_MEM0);
        resultsBuffer[1] = ADC14_getResult(ADC_MEM1);
        //printf("x %d y %d\n",resultsBuffer[0],resultsBuffer[1]);
        //I need 2 bools to check if the joystick is tilted in a certain direction
        if(resultsBuffer[0] > 13000 && !x_tilted){
            printf("dx \n");
            x_tilted = true;
        }
        if(resultsBuffer[0] < 3500 && !x_tilted){
            printf("sx \n");
            x_tilted = true;
        }
        if(resultsBuffer[1] > 13000 && !y_tilted){
            printf("up \n");
            y_tilted = true;
        }
        if(resultsBuffer[1] < 500 && !y_tilted){
            printf("down \n");
            y_tilted = true;
        }
        if(isInIdleState(resultsBuffer[0]) && isInIdleState(resultsBuffer[1])){
            x_tilted = false;
            y_tilted = false;
        }
    }
}
