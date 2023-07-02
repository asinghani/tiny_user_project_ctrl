#include <defs.h>
#include <stdint.h>

void putchar(char x);
bool haschar();
char getchar();

void configure_io();
void set_proj_inputs(uint8_t x);
uint8_t get_proj_outputs();

void delay_ms(int d) {
    reg_timer0_config = 0;
    reg_timer0_data = d;
    reg_timer0_config = 1;

    reg_timer0_update = 1;
    while (reg_timer0_value > 0) {
        reg_timer0_update = 1;
    }
}

uint8_t io_state;

void main() {
    // the GPIO pin is just a nice debug tool
    reg_gpio_mode1 = 1;
    reg_gpio_mode0 = 0;
    reg_gpio_ien = 1;
    reg_gpio_oeb = 0;

    reg_uart_enable = 1;

    configure_io();

    while (1) {
        while (haschar()) {
            uint8_t x = getchar();
            set_proj_inputs(x);
            putchar(get_proj_outputs());

            // for debugging, mirror bit 0 of inputs to caravel GPIO pin
            reg_gpio_out = x & 0x1;
        }
    }
}



// ===============================
// Project I/O config
// ===============================

// mgmt_en = 1, oe_ovr = 1, ie = 1, oe = 1
// all other cfg bits = 0
#define MPRJ_INPUT_CFG 0x00F

// mgmt_en = 0, oe_ovr = 1, ie = 1, oe = 1
// all other cfg bits = 0
#define MPRJ_OUTPUT_CFG 0x00E

void configure_io() {
    // User proj inputs (written by caravel core)
    reg_mprj_io_8  = MPRJ_INPUT_CFG;
    reg_mprj_io_9  = MPRJ_INPUT_CFG;
    reg_mprj_io_10 = MPRJ_INPUT_CFG;
    reg_mprj_io_11 = MPRJ_INPUT_CFG;
    reg_mprj_io_12 = MPRJ_INPUT_CFG;
    reg_mprj_io_13 = MPRJ_INPUT_CFG;
    reg_mprj_io_14 = MPRJ_INPUT_CFG;
    reg_mprj_io_15 = MPRJ_INPUT_CFG;

    // User proj outputs (read by caravel core)
    reg_mprj_io_16 = MPRJ_OUTPUT_CFG;
    reg_mprj_io_17 = MPRJ_OUTPUT_CFG;
    reg_mprj_io_18 = MPRJ_OUTPUT_CFG;
    reg_mprj_io_19 = MPRJ_OUTPUT_CFG;
    reg_mprj_io_20 = MPRJ_OUTPUT_CFG;
    reg_mprj_io_21 = MPRJ_OUTPUT_CFG;
    reg_mprj_io_22 = MPRJ_OUTPUT_CFG;
    reg_mprj_io_23 = MPRJ_OUTPUT_CFG;

    // Pins for UART
    reg_mprj_io_5 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;
    reg_mprj_io_6 = GPIO_MODE_MGMT_STD_OUTPUT;

    reg_mprj_xfer = 1;
    while (reg_mprj_xfer == 1);
}

void set_proj_inputs(uint8_t x) {
    reg_mprj_datal = (x << 8);
}

uint8_t get_proj_outputs() {
    return (reg_mprj_datal >> 16) & 0xFF;
}



// ===============================
// UART-related utilities
// ===============================
#define UART_EV_TX	0x1
#define UART_EV_RX	0x2
#define UART_RXTX    (*(volatile uint32_t*) 0xf0005800)
#define UART_TXFULL  (*(volatile uint32_t*) 0xf0005804)
#define UART_RXEMPTY (*(volatile uint32_t*) 0xf0005808)
#define UART_EV_PEND (*(volatile uint32_t*) 0xf0005810)

void putchar(char x) {
    while(UART_TXFULL);
    UART_RXTX = x;
}

bool haschar() {
    return !UART_RXEMPTY;
}

char getchar() {
    while (!haschar());
    uint8_t c = UART_RXTX;
    UART_EV_PEND = UART_EV_RX;
    return c;
}

