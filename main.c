#include <msp430.h>

#define SLAVE_ADDR   0x00
#define BYTE_TO_SEND 0x00

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog
    PM5CTL0 &= ~LOCKLPM5;        // Unlock GPIO from high-impedance mode

    // === GPIO Setup ===
    P1SEL0 |= BIT6 | BIT7;       // Set P1.6, P1.7 to USCI_B0 I2C function
    P1SEL1 &= ~(BIT6 | BIT7);    // Primary module function

    P1DIR &= ~(BIT6 | BIT7);     // Set as inputs (I2C open-drain)
    P1OUT |= BIT6 | BIT7;        // Enable pull-up (OUT=1)
    P1REN |= ~(BIT6 | BIT7);        // Enable resistor

    // === Clock: Default DCO ~1MHz used for SMCLK ===

    // === USCI_B0 I2C Master Setup ===
    UCB0CTLW0 = UCSWRST;                     // Hold eUSCI_B0 in reset
    UCB0CTLW0 |= UCSSEL_2 | UCMST | UCMODE_3 | UCSYNC; // SMCLK, master, I2C, sync
    UCB0BRW = 1000;                            // Divide SMCLK (1MHz) by 10 = 100kHz
    UCB0I2CSA = SLAVE_ADDR;                 // Set slave address
    UCB0CTLW0 &= ~UCSWRST;                   // Release from reset
    UCB0IE = 0;                              // Disable USCI interrupts (polling)

    while (1)
    {
           // Wait for bus to be free

        // === Transmit 1 byte ===
        UCB0CTLW0 |= UCTR | UCTXSTT;         // I2C TX, start condition
        while (!(UCB0IFG & UCTXIFG0));       // Wait for TX buffer ready
        UCB0TXBUF = BYTE_TO_SEND;            // Load byte to transmit
        while (!(UCB0IFG & UCSTPIFG))        // Wait for stop condition (sent automatically)
            ;
        UCB0IFG &= ~UCSTPIFG;                // Clear stop flag

        __delay_cycles(5000000);                // 5 ms delay @ 1 MHz
    }
}
