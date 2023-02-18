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

#define SCREEN_MAXWIDTH 128
#define MAX_VOLUME 100
#define MIN_VOLUME 0
#define TIMER_PERIOD 0x2400 // 9216 / 32700 = 0.33s
#define MAX_TIME_PERIOD_SHOW_BAR 6


//LCD
#include <image.c> //DON'T OPEN THIS FILE IN CCS, IT WILL CRASH 
Graphics_Context g_sContext;

//UART
uint8_t TXData = 1;
uint8_t RXData = 0;
uint8_t data_received[10];
int counter = 0;
char str[4];
//String sending
#define MAX_SIZE_READ 64
uint8_t readBuffer[MAX_SIZE_READ];
int count = 0;
int ack = 1;
uint8_t AuthorName[MAX_SIZE_READ];
uint8_t SongName[MAX_SIZE_READ];//HAVEN't USED YET

//ADC conversion
static uint16_t joystickBuffer[2];
static uint16_t accelerometer_z_axis;
//Joystick reading
bool tilted = false;
//Button reading
bool token = false;

//Idk where to put this one lol
bool stringChanged = false;

//All images in image.c careful don't open it
Graphics_Image spotify_logos[12];
int32_t rotation = 0;
int32_t slide_value =SCREEN_MAXWIDTH;
//syncronization
volatile bool playing = false; //tell if the image has to be rotated or not
volatile int32_t volume = 50;
volatile bool changed = false;
//draw image
const int32_t VOLUME_BAR_POSITION_X = 14;
int32_t show_bar_counter = MAX_TIME_PERIOD_SHOW_BAR;



/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
//mmhhh... could use 2 uart
const eUSCI_UART_ConfigV1 uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        26,//13,                                      // BRDIV = 13
        0,                                       // UCxBRF = 0
        111,//37,                                      // UCxBRS = 37
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
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
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
}

//DriverLib

void setUpButtons(){
    //clean voltage in buttons
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN5);


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

    //clear interrupt flags
    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN5);

    /* activate interrupt notification */
    Interrupt_enableMaster();
}


bool consumetoken(){
    if (token){
        token = false;
        return true;
    }
    return false;
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

//UART 
void sendString(char* str) {
    while(*str != '\0') {
        UART_transmitData(EUSCI_A2_BASE, *str);
        str++;
    }
}

void setUpUART(){
    /* Selecting P1.2 and P1.3 in UART mode and P1.0 as output (LED) */
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
        GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        //MODIFIED FROM 24MHz to 48MHz DONT TOUCH IT
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


//Timer
const Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_ACLK,              // ACLK = 32768 Hz
        TIMER_A_CLOCKSOURCE_DIVIDER_1,         // ACLK/1
        TIMER_PERIOD,                           // every second
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
}; //TIMER IN UPMODE


void set_timer(){
    CS_setReferenceOscillatorFrequency(CS_REFO_32KHZ); //either 32 or 128
    CS_initClockSignal(CS_ACLK,CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);
    Interrupt_enableInterrupt(INT_TA1_0);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
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
    setUpButtons();
    _adcInit();
    _graphicsInit();
    setUpUART();
    set_timer();
}


//GRAPHICS


void logosinit(){
    spotify_logos[0] = spotify_image0;
    spotify_logos[1] = spotify_image1;
    spotify_logos[2] = spotify_image2;
    spotify_logos[3] = spotify_image3;
    spotify_logos[4] = spotify_image4;
    spotify_logos[5] = spotify_image5;
    spotify_logos[6] = spotify_image6;
    spotify_logos[7] = spotify_image7;
    spotify_logos[8] = spotify_image8;
    spotify_logos[9] = spotify_image9;
    spotify_logos[10] = spotify_image10;
    spotify_logos[11] = spotify_image11;
}

void drawscreen(int32_t slide_value,int32_t rotation){
    //making text slide
    if (stringChanged){
        //Delete old stirng
        GrContextForegroundSet(&g_sContext, ClrBlack);
        tRectangle delete_rect = {0,80,128,50};
        GrRectFill(&g_sContext, &delete_rect);
        GrContextForegroundSet(&g_sContext, ClrWhite);
        //place new one
        Graphics_drawStringCentered(&g_sContext, (int8_t *) AuthorName, AUTO_STRING_LENGTH,slide_value, 60, OPAQUE_TEXT);
        stringChanged = false;
    }
    Graphics_drawStringCentered(&g_sContext, (int8_t *) AuthorName, AUTO_STRING_LENGTH,slide_value, 60, OPAQUE_TEXT);
    Graphics_drawImage(&g_sContext,&spotify_logos[rotation],0,0);
    //Volume bar
    tRectangle rect_volume = {VOLUME_BAR_POSITION_X,102,VOLUME_BAR_POSITION_X + volume,114};
    tRectangle delete_rect = {VOLUME_BAR_POSITION_X,102,128,114};
    //Delete old one only if it is not the same
    if (changed){
        GrContextForegroundSet(&g_sContext, ClrBlack);
        GrRectFill(&g_sContext, &delete_rect);
        GrFlush(&g_sContext);
        GrContextForegroundSet(&g_sContext, 0x00ff00);
        GrRectFill(&g_sContext, &rect_volume);
        GrContextForegroundSet(&g_sContext, ClrWhite);
        changed = false;
        show_bar_counter = MAX_TIME_PERIOD_SHOW_BAR;
    }
    if (show_bar_counter == 0){
        GrContextForegroundSet(&g_sContext, ClrBlack);
        GrRectFill(&g_sContext, &delete_rect);
        GrContextForegroundSet(&g_sContext, ClrWhite);
    }
    GrFlush(&g_sContext);
}

void setmetadata(){
    //Set authorname to "Hello" and Song name to "World"
    AuthorName[1] = 'H';
    AuthorName[2] = 'e';
    AuthorName[3] = 'l';
    AuthorName[4] = 'l';
    AuthorName[5] = 'o';
    AuthorName[6] = '\0';
    SongName[1] = 'W';
    SongName[2] = 'o';
    SongName[3] = 'r';
    SongName[4] = 'l';
    SongName[5] = 'd';
    SongName[6] = '\0';
}

int main(void){
    _hwinit();
    logosinit();
    setmetadata();

    while(1)
    {
        Interrupt_enableSleepOnIsrExit();
        PCM_gotoLPM0InterruptSafe();
    }
}



void TA1_0_IRQHandler(void){
        Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
        //if button was pressed update the state
        if (consumetoken()){
            if (playing){
                char str[5] = "stop";
                sendString(str);
                printf("stop\n");
                playing = false;
            }
            else{
                char str[5] = "play";
                sendString(str);
                printf("play\n");
                playing = true;
            }
        }

        drawscreen(slide_value,rotation);
        
        slide_value--;
        if (slide_value == 0){
            printf("Resizing\n");
            GrContextForegroundSet(&g_sContext, ClrBlack);
            tRectangle rect = {0,55,128,80}; //x0,y0,x1,y1
            GrRectFill(&g_sContext, &rect);
            GrFlush(&g_sContext);
            GrContextForegroundSet(&g_sContext, ClrWhite);
            slide_value = SCREEN_MAXWIDTH;
        }
        
        //if the device is playing, rotate the logo
        if (playing){
            rotation++;
            if (rotation == 11){
                rotation = 0;
            }
            //rotation = (rotation + 1) % 11; from mp
        }
        if (show_bar_counter > 0){
            show_bar_counter--;
        }
}





void EUSCIA2_IRQHandler(void)
{
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A2_BASE);                               

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        RXData = UART_receiveData(EUSCI_A2_BASE);
        if (RXData != '#'){
            readBuffer[count] = RXData;
            count++;
            ack--;
            if (ack ==0){
                UART_transmitData(EUSCI_A2_BASE, '%');
                ack = 1;
            }
        }
        else{
            readBuffer[count] = '\0'; //close buffer
            //copy contents from readBuffer to AuthorName
            int i=0;
            while (readBuffer[i] != '\0'){
                AuthorName[i] = readBuffer[i];
                i++;
            }
            AuthorName[i] = '\0'; //close buffer
            stringChanged = true;
            fflush(stdout);
            count = 0;
        }
        printf("received from uart %c\n", RXData);
        fflush(stdout);    
        Interrupt_disableSleepOnIsrExit();
    }
    
}



//Button handlers


void PORT5_IRQHandler(){
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5,status);
    if (status & GPIO_PIN1){
        token = true;
    }
}


void PORT3_IRQHandler(){
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    GPIO_clearInterruptFlag(GPIO_PORT_P3,status);
    if (status & GPIO_PIN5 ){
        printf("ormai è periodica sta foto\n");
    }
}

//Joystick values reading

//check if Joystick is not tilted at all
bool isInIdleState(int x){
    return ((x>7000) && (x<9000));
}



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
            printf("next\n");;
            char str[5] = "next";
            sendString(str);
            tilted = true;
        }
        if(joystickBuffer[0] < 3500 && !tilted){
            printf("prev\n");
            char str[5] = "prev";
            sendString(str);
            tilted = true;
        }
        if(joystickBuffer[1] > 13000 && !tilted){
            printf("upup\n");
            char str[5] = "upup";
            sendString(str);
            if (volume < MAX_VOLUME){
                volume+=10;
                changed = true;
            }
            tilted = true;
        }
        if(joystickBuffer[1] < 500 && !tilted){
            printf("down \n");
            char str[5] = "down";
            sendString(str);
            if (volume > MIN_VOLUME){
                volume -= 10;
                changed = true;
            }
            tilted = true;
        }
        if(isInIdleState(joystickBuffer[0]) && isInIdleState(joystickBuffer[1])){
            tilted = false;
        }
    }
    /*
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
    */
}
