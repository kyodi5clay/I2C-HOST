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

int main(void)
{
    /*WDTCTL = WDTPW | WDTHOLD;     // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;         // Unlock GPIOs
    SFRRPCR |= SYSRSTRE | SYSRSTUP;


    
    P1SEL0 |= BIT6 | BIT7;            // Select primary I2C function
    P1SEL1 &= ~(BIT6 | BIT7);         // Clear secondary function

*/

    EUSCI_B_I2C_initMasterParam param = {
        .selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK,
        .i2cClk = 1000000,
        .dataRate = EUSCI_B_I2C_SET_DATA_RATE_100KBPS,
        .byteCounterThreshold = 0,
        .autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP};

    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &param);
    
    EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE, 0x0f);
    EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    HWREG16(0x0064E) &= 0b111111111;
    HWREG16(EUSCI_B0_BASE + OFS_UCBxIFG) &= ~(UCTXIFG);
    EUSCI_B_I2C_enable(EUSCI_B0_BASE);

  

    EUSCI_B_I2C_masterSendSingleByte(EUSCI_B0_BASE, 0x0010);

    /*while(1)
    {
        //send Byte
        EUSCI_B_I2C_masterSendSingleByte(EUSCI_B0_BASE, 0x0010);

        delay_seconds(2);
    };*/
}
