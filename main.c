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

    

    //PMMCTL0 |= BIT3;
    WDTCTL = WDTPW | WDTHOLD; //stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5; // Unlock GPIO pins

    P1SEL0 |= BIT6 | BIT7;            // Select primary I2C function
    P1SEL1 &= ~(BIT6 | BIT7);         // Clear secondary function

    P3DIR |= BIT4;       // Set P3.4 as output
    P3SEL0 |= BIT4;      // Select primary peripheral function
    P3SEL1 &= ~BIT4;     // Ensure second function is disabled

    P3OUT &= ~BIT4;     // Set output register low (for pulldown)
    P3REN |= BIT4;      // Enable pullup/pulldown resistor


    // Set DCO to 16MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_0); // 16 MHz from table

    // Select SMCLK = DCO
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8);

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


    while(1){;};
    //EUSCI_B_I2C_masterSendSingleByte(EUSCI_B0_BASE, 0x0f);

    /*while(1)
    {
        //send Byte
        EUSCI_B_I2C_masterSendSingleByte(EUSCI_B0_BASE, 0x0010);

        delay_seconds(2);
    };*/
}

