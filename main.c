#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <assert.h>
#include <stdio.h>

cyhal_i2c_t mI2C;

// Print the probe table
void probe()
{
    cy_rslt_t rval;

    // Setup the screen and print the header
    printf("\n\n   ");
    for (unsigned int i = 0; i < 0x10; i++)
    {
        printf("%02X ", i);
    }

    // Iterate through the address starting at 0x00
    for (uint32_t i2caddress = 0; i2caddress < 0x80; i2caddress++)
    {
        if (i2caddress % 0x10 == 0)
            printf("\n%02X ", (unsigned int)i2caddress);

        uint8_t buffer[1] = {0}; // You can change this to your specific data to send
        rval = cyhal_i2c_master_write(&mI2C, i2caddress, buffer, 1, 0, false);
        if (rval == CY_RSLT_SUCCESS) // If you get ACK, then print the address
        {
            printf("%02X ", (unsigned int)i2caddress);
        }
        else //  Otherwise print a --
        {
            printf("-- ");
        }
    }
    printf("\n");
}

int main(void)
{
    cy_rslt_t result;
    cyhal_i2c_cfg_t mI2C_cfg;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    assert(result == CY_RSLT_SUCCESS);

    /* Initialize the retarget-io */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    assert(result == CY_RSLT_SUCCESS);

    /* Clear the screen */
    printf("\x1b[2J\x1b[;H");

    printf("I2C Detect\n");
    printf("Press p for probe, ? for help\n");

    /* I2C Master configuration settings */
    printf(">> Configuring I2C Master..... ");
    mI2C_cfg.is_slave = false;
    mI2C_cfg.frequencyhal_hz = 100000;  // Replace with your desired frequency (e.g., 100 kHz)

    result = cyhal_i2c_init(&mI2C, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    assert(result == CY_RSLT_SUCCESS);

    result = cyhal_i2c_configure(&mI2C, &mI2C_cfg);
    assert(result == CY_RSLT_SUCCESS);

    printf("Done\r\n\n");

    /* Enable interrupts */
    __enable_irq();

    probe(); // Do an initial probe

    while (1)
    {
        char c = getchar();
        switch (c)
        {
        case 'p':
            probe();
            break;
        case '?':
            printf("------------------\n");
            printf("Command\n");
            printf("p\tProbe\n");
            printf("?\tHelp\n");
            break;
        }
    }
}
