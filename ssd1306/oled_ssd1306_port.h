#ifndef _OLED_SSD1306_SPI_PORT__
#define _OLED_SSD1306_SPI_PORT__

#ifdef __cplusplus
extern "C"
{
#endif

void oled_ssd1306_port_init(void);

void oled_ssd1306_port_write_spi_data(unsigned char data, unsigned char mode);



#ifdef __cplusplus
}
#endif

#endif
