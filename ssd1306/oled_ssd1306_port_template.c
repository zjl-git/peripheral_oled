#include "oled_ssd1306_port.h"
#include "oled_ssd1306_config.h"
#include "oled_ssd1306.h"


static void oled_ssd1306_port_dc_set(void)
{

}

static void oled_ssd1306_port_dc_clr(void)
{

}

static void oled_ssd1306_port_cs_set(void)
{

}

static void oled_ssd1306_port_cs_clr(void)
{

}


static void oled_ssd1306_port_rest_set(void)
{

}

static void oled_ssd1306_port_rest_clr(void)
{

}

static void oled_ssd1306_port_delay(unsigned int ms)
{

}

void oled_ssd1306_port_init(void)
{
    oled_ssd1306_port_delay(10);

    oled_ssd1306_port_rest_clr();
    oled_ssd1306_port_delay(10);
    oled_ssd1306_port_rest_set();

    oled_ssd1306_port_delay(200);
}

void oled_ssd1306_port_write_spi_data(unsigned char data, unsigned char mode)
{
    unsigned char buf = data;
    oled_ssd1306_port_cs_clr();

    if (mode == OLED_WRITE_CMD) {
        oled_ssd1306_port_dc_clr();
    } else if (mode == OLED_WRITE_DAT) {
        oled_ssd1306_port_dc_set();
    } else {
        return;
    }

    //send data;

    oled_ssd1306_port_cs_set();
    oled_ssd1306_port_dc_set();
}