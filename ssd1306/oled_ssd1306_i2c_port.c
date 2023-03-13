#include "oled_ssd1306_i2c_port.h"
#include "oled_ssd1306.h"

#include "hl_simulate_i2c.h"

#define I2C_ADDR                    0x78

void oled_ssd1306_i2c_port_init(void)
{
    hl_simulate_i2c_init();
}

void oled_ssd1306_i2c_port_write_data(unsigned char data, unsigned char mode)
{
    hl_simulate_i2c_start();
    hl_simulate_write_data(I2C_ADDR);
    hl_simulate_i2c_wait_ack();

    if (mode == OLED_WRITE_CMD) {
        hl_simulate_write_data(0x00);
    } else if (mode == OLED_WRITE_DAT) {
        hl_simulate_write_data(0x40);
    } else {
        return ;
    }
    hl_simulate_i2c_wait_ack();

    hl_simulate_write_data(data);
    hl_simulate_i2c_wait_ack();
    hl_simulate_i2c_stop();
}