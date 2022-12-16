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
 * MSP432 UART - Loopback with 24MHz DCO BRCLK
 *
 * Description: This demo connects TX to RX of the MSP432 UART
 * The example code shows proper initialization of registers
 * and interrupts to receive and transmit data. If data is incorrect P1.0 LED
 * is turned ON.
 *
 *  MCLK = HSMCLK = SMCLK = DCO of 24MHz
 *
 *               MSP432P401
 *             -----------------
 *            |                 |
 *       RST -|     P3.3/UCA0TXD|----|
 *            |                 |    |
 *           -|                 |    |
 *            |     P3.2/UCA0RXD|----|
 *            |                 |
 *            |             P1.0|---> LED
 *            |                 |
 *
 *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include<stdio.h>
#include <stdbool.h>

uint8_t TXData = 1;
uint8_t RXData = 0;

uint8_t data_received[10];
int counter = 0;
char str[4];

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


//boring

void add_buttons(){
    //Adding S1
    P1->DIR &= ~BIT1; //P1.1 as input
    P1->OUT |=BIT1; //Select pullup mode
    P1->REN |= BIT1; //Resistor Enable pullup mode
    //Adding S2
    P1->DIR &= ~BIT4; //P1.4 as input
    P1->OUT |= BIT4;
    P1->REN |= BIT4;
}

void set_interrupts(){
    P1->IES |= (BIT1 | BIT4); //P1.1, P1.4 set as high to low transition interrupt
    P1->IE |= (BIT1 |BIT4); //Enable interrupts
    P1->IFG = 0; //clear Interrupt flag bit
    NVIC->ISER[1] |= 1 << (PORT1_IRQn & 31);
    //è un casino, faccio shift aritmetico sx di (35 & 31 =) 3
    //lo inserisco nel Nested Vector Interrupt Controller
    //ISER[1] copre i valori da 32 a 63, noi abbiamo 35
    //perchè è la Interrupt Request di P1
}

void sendString(char* str) {
    while(*str != '\0') {
        UART_transmitData(EUSCI_A2_BASE, *str);
        str++;
    }
    while(UART_receiveData(EUSCI_A2_BASE) != 37){};
}


int main(void)
{
    add_buttons();
    set_interrupts();
    /* Halting WDT  */
    WDT_A_holdTimer();

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

    /* check if there is a pending interrupt on P1.1 */
    if(P1->IFG & BIT1){
        //Do nothing
    }

    if(P1->IFG & BIT4){
        printf("ho spedito brother\n");
        char str[4] = {'n', 'e', 'x', 't','\0'};
        sendString(str);
        char str2[4] = {'p','l','a','y','\0'};
        sendString(str2);
        char str3[4] = {'s','t','o','p','\0'};
        sendString(str3);
    }

    /* clear the interrupt flags */
    P1->IFG &= ~(BIT1 | BIT4);
}

