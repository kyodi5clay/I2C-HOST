#include <msp430.h>
#include "driverlib.h"
#include "eusci_b_i2c.h"

void delay_seconds(unsigned int seconds)
{
    // Assumes 1 MHz clock => 1,000,000 cycles/sec
    // Approx 1M cycles = 1 sec, so delay N seconds:
    int i = 0;
    for (i; i < seconds; i++)
        __delay_cycles(1000000);
}

void send_byte(uint8_t txData){

    UCB0CTLW0 |= UCTR + UCTXSTT;

    UCB0TXBUF = txData;

    UCB0CTLW0 |= UCTXSTP;


}


int main(void)
{

    __delay_cycles(5);

    //PMMCTL0 |= BIT3;
    WDTCTL = WDTPW | WDTHOLD; //stop watchdog timer

    P1SEL0 |= BIT6 | BIT7;            // Select primary I2C function
    P1SEL1 &= ~(BIT6 | BIT7);         // Clear secondary function

    P3DIR |= BIT4;
    P3SEL0 |= BIT4;                           // Output SMCLK
    P3SEL1 |= BIT4;

    P3OUT &= ~BIT4;     // Set output register low (for pulldown)
    P3REN |= BIT4;      // Enable pullup/pulldown resistor


    PM5CTL0 &= ~LOCKLPM5; // Unlock GPIO pins


    // Disable FLL control loop
    __bis_SR_register(SCG0);

    // Set DCO to 16 MHz
    CSCTL0_H = 0xA5;           // Unlock CS registers
    CSCTL1 = DCOFSEL_4 | DCORSEL;    // Set DCO = 16MHz
    CSCTL2 = SELS__DCOCLK;           // SMCLK source = DCO
    CSCTL3 = DIVS__1;                // SMCLK divider = 1
    CSCTL0_H = 0;                    // Lock CS registers

    __bic_SR_register(SCG0);         // Re-enable FLL control loop



    // Select SMCLK = DCO
    //CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8);

    EUSCI_B_I2C_initMasterParam param = {
        .selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK,
        .i2cClk = 16000000,
        .dataRate = EUSCI_B_I2C_SET_DATA_RATE_100KBPS,
        .byteCounterThreshold = 0,
        .autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP}; //Initialize basic settings

    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &param); // write settings to proper registers
    
    EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE, 0x0f); // Set slave address
    UCB0I2CSA = 0x0f;

    EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE); // Set to transit mode

    EUSCI_B_I2C_enable(EUSCI_B0_BASE); // Enable i2c



    EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    UCB0IE |= (UCTXIE0); //Should set interrupt pin

    _BIS_SR (GIE); // enable interrupts

    send_byte(0x0f);


    while(1){
        EUSCI_B_I2C_masterSendSingleByte(EUSCI_B0_BASE, 0x0f);
        __delay_cycles(20);
        
        
        
        };
    //EUSCI_B_I2C_masterSendSingleByte(EUSCI_B0_BASE, 0x0f);

    /*while(1)
    {
        //send Byte
        EUSCI_B_I2C_masterSendSingleByte(EUSCI_B0_BASE, 0x0010);

        delay_seconds(2);
    };*/
}

