#ifndef _OLED_SSD1306__
#define _OLED_SSD1306__

#ifdef __cplusplus
extern "C"
{
#endif

#define OLED_WRITE_CMD          0
#define OLED_WRITE_DAT          1

typedef enum {
    ASC_6X8,
    ASC_6X12,
    ASC_8X16,
    ASC_12X24,
} acs2_type;

void oled_ssd1306_refresh(void);

void oled_ssd1306_fill(unsigned char data);

void oled_ssd1306_init(void);

void oled_ssd1306_show_char(unsigned char x, unsigned char y, unsigned char chr, acs2_type type);

void oled_ssd1306_show_string(unsigned char x, unsigned char y, char *chr, acs2_type type);

void oled_ssd1306_show_hex(unsigned char x, unsigned char y, unsigned char *buf, unsigned char len, acs2_type type);

void oled_ssd1306_show_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);

void oled_ssd1306_show_circle(unsigned char x, unsigned char y, unsigned char r);

#ifdef __cplusplus
}
#endif

#endif
