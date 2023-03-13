#include "oled_ssd1306.h"
#include "oled_ssd1306_utils.h"
#include "oled_ssd1306_spi_port.h"
#include "oled_ssd1306_i2c_port.h"

#include <stdio.h>

// #define OLED_COMM_MODE_SPI

#ifdef OLED_COMM_MODE_SPI
#define X_WIDTH         128
#define Y_WIDTH         64
#else
#define X_WIDTH         64
#define Y_WIDTH         48
#endif

#define PAGE_NUM        Y_WIDTH / 8
#define PAGE_BATE_NUM   X_WIDTH

unsigned char g_oled_gram[PAGE_BATE_NUM][PAGE_NUM];

static void oled_ssd1306_write(unsigned char data, unsigned char mode)
{
#ifdef OLED_COMM_MODE_SPI
    oled_ssd1306_spi_port_write_data(data, mode);
#else
    oled_ssd1306_i2c_port_write_data(data, mode);
#endif
}

static void oled_ssd1306_draw_point(unsigned char x, unsigned char y, unsigned char mode)
{
    unsigned char i = y / 8, m = y % 8, n;
	n = 1 << m;

    if (mode) { 
        g_oled_gram[x][i] |= n;
    } else {
        g_oled_gram[x][i] = ~g_oled_gram[x][i];
        g_oled_gram[x][i] |= n;
        g_oled_gram[x][i] = ~g_oled_gram[x][i];
    }
}

void oled_ssd1306_fill(unsigned char data)
{
    unsigned char x, y;
    for (y = 0; y < PAGE_NUM; y++) {
        for (x = 0; x < PAGE_BATE_NUM; x++) {
            g_oled_gram[x][y] = data;
        }
    }
    oled_ssd1306_refresh();
}

void oled_ssd1306_show_char(unsigned char x, unsigned char y, unsigned char chr, acs2_type type)
{
    unsigned char i, m, size, chr1, temp;
    unsigned char x0 = x, y0 = y;

    chr1=chr-' ';
    if (type == ASC_6X8) {
        size = 6;
    } else if (type == ASC_6X12) {
        size = 12;
    } else if (type == ASC_8X16) {
        size = 16;
    } else if (type == ASC_12X24) {
        size = 36;
    }else {
        return ;
    }

    for (i = 0; i < size; i++) {
        if (type == ASC_6X8) {
            temp = asc2_0806[chr1][i];
        } else if (type == ASC_6X12) {
            temp = asc2_1206[chr1][i];
        } else if (type == ASC_8X16) {
            temp = asc2_1608[chr1][i];
        } else if (type == ASC_12X24) {
            temp = asc2_2412[chr1][i];
        } else {
            return;
        }

        for (m = 0; m < 8; m++) {
            if (temp & 0x01) {
                oled_ssd1306_draw_point(x, y, 1);
            } else {
                oled_ssd1306_draw_point(x, y, 0);
            }

            temp >>= 1;
            y += 1;
        }

        x += 1;
        if ((size != 6) && ((x - x0) == size / 2)) {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

void oled_ssd1306_show_string(unsigned char x, unsigned char y, char *chr, acs2_type type)
{
    while((*chr >= ' ') && (*chr <= '~')) {
        oled_ssd1306_show_char(x, y, *chr, type);
        if (type == ASC_6X8 || type == ASC_6X12) {
            x += 6;
        } else if (type == ASC_8X16){
            x += 8;
        } else if (type == ASC_12X24){
            x += 12;
        } else {
            return ;
        }
        chr++;
    }
}

void oled_ssd1306_show_hex(unsigned char x, unsigned char y, unsigned char *buf, unsigned char len, acs2_type type)
{
    unsigned char x1;
    char str[10];
    for (unsigned char i = 0; i < len; i++) {
        sprintf(str, "%02X", buf[i]);
        oled_ssd1306_show_string(x, y, str, type);

        if (type == ASC_6X8 || type == ASC_6X12) {
            x += 6 * 3;
            x1 = x + 6 *2;
        } else if (type == ASC_8X16){
            x += 8 * 3;
            x1 = x + 8 *2;
        } else if (type == ASC_12X24){
            x += 12 * 3;
            x1 = x + 12 *2;
        } else {
            return ;
        }

        if (i + 1 < len) {
            oled_ssd1306_show_char(x1, y, ' ', type);
        } 
    }
}

void oled_ssd1306_show_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{
    unsigned short t;
    int incx = 0, incy = 0, urow = 0, ucol = 0;
    int delta_x = 0, delta_y = 0, xerr = 0, yerr = 0, distance = 0;

    delta_x = x2 - x1;
    delta_y = y2 - y1;

    urow = x1;
    ucol = y1;

    if (delta_x > 0) {
        incx = 1;
    } else if (delta_x == 0) {
        incx = 0;
    } else {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0) {
        incy = 1;
    } else if (delta_y == 0) {
        incy = 0;
    } else {
        incy = -1;
        delta_y = - delta_y;
    }

    if (delta_x > delta_y) {
        distance = delta_x;
    } else {
        distance = delta_y;
    }

    for (t = 0; t < distance + 1; t++) {
        oled_ssd1306_draw_point(urow, ucol, 1);
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance) {
            xerr -= distance;
            urow += incx;
        }
        if (yerr > distance) {
            yerr -= distance;
            ucol += incy;
        }
    }
}

void oled_ssd1306_show_circle(unsigned char x, unsigned char y, unsigned char r)
{
    int a = 0, b = r, num;
    while(2 *b * b >= r *r) {
        oled_ssd1306_draw_point(x + a, y - b, 1);
        oled_ssd1306_draw_point(x - a, y - b, 1);
        oled_ssd1306_draw_point(x - a, y + b, 1);
        oled_ssd1306_draw_point(x + a, y + b, 1);

        oled_ssd1306_draw_point(x + b, y + a, 1);
        oled_ssd1306_draw_point(x + b, y - a, 1);
        oled_ssd1306_draw_point(x - b, y - a, 1);
        oled_ssd1306_draw_point(x - b, y + a, 1);

        a++;
        num = (a * a + b * b) - r * r;
        if (num > 0) {
            b--;
            a--;
        }
    }
}


void oled_ssd1306_refresh(void)
{
    unsigned char i, n;
    for (i = 0; i < PAGE_NUM; i++) {
        oled_ssd1306_write(0xb0+i, OLED_WRITE_CMD);
#ifdef OLED_COMM_MODE_SPI
        oled_ssd1306_write(0x02, OLED_WRITE_CMD);
        oled_ssd1306_write(0x10, OLED_WRITE_CMD);
#else
        oled_ssd1306_write(0x00, OLED_WRITE_CMD);
        oled_ssd1306_write(0x12, OLED_WRITE_CMD);
#endif
        for (n = 0; n < X_WIDTH; n++) {
            oled_ssd1306_write(g_oled_gram[n][i], OLED_WRITE_DAT);
        }
    }
}

void oled_ssd1306_init(void)
{
#ifdef OLED_COMM_MODE_SPI
    oled_ssd1306_spi_port_init();

    oled_ssd1306_write(0xae,OLED_WRITE_CMD);
    oled_ssd1306_write(0x00,OLED_WRITE_CMD);
    oled_ssd1306_write(0x10,OLED_WRITE_CMD);
    oled_ssd1306_write(0x40,OLED_WRITE_CMD);
    oled_ssd1306_write(0x81,OLED_WRITE_CMD);
    oled_ssd1306_write(0xcf,OLED_WRITE_CMD);
	
    oled_ssd1306_write(0xa1,OLED_WRITE_CMD);
    oled_ssd1306_write(0xc8,OLED_WRITE_CMD);
    oled_ssd1306_write(0xa6,OLED_WRITE_CMD);
    oled_ssd1306_write(0xa8,OLED_WRITE_CMD);
    oled_ssd1306_write(0x3f,OLED_WRITE_CMD);
	
    oled_ssd1306_write(0xd3,OLED_WRITE_CMD);
    oled_ssd1306_write(0x00,OLED_WRITE_CMD);
		
    oled_ssd1306_write(0xd5,OLED_WRITE_CMD);
    oled_ssd1306_write(0x80,OLED_WRITE_CMD);
    oled_ssd1306_write(0xd9,OLED_WRITE_CMD);
		
    oled_ssd1306_write(0xf1,OLED_WRITE_CMD);
	//oled_ssd1306_write(0x22,OLED_WRITE_CMD);
    oled_ssd1306_write(0xda,OLED_WRITE_CMD);
    oled_ssd1306_write(0x12,OLED_WRITE_CMD);
    oled_ssd1306_write(0xdb,OLED_WRITE_CMD);
		
    oled_ssd1306_write(0x40,OLED_WRITE_CMD);
		
    oled_ssd1306_write(0x20,OLED_WRITE_CMD);
    oled_ssd1306_write(0x02,OLED_WRITE_CMD);
    oled_ssd1306_write(0x8d,OLED_WRITE_CMD);
    oled_ssd1306_write(0x14,OLED_WRITE_CMD);
    oled_ssd1306_write(0xa4,OLED_WRITE_CMD);
    oled_ssd1306_write(0xa6,OLED_WRITE_CMD);
    oled_ssd1306_write(0xaf,OLED_WRITE_CMD);
    oled_ssd1306_fill(0x00);
#else
    oled_ssd1306_i2c_port_init();
    oled_ssd1306_write(0xAE,OLED_WRITE_CMD);
    oled_ssd1306_write(0x00,OLED_WRITE_CMD);
    oled_ssd1306_write(0x12,OLED_WRITE_CMD);
    oled_ssd1306_write(0x40,OLED_WRITE_CMD);
    oled_ssd1306_write(0xB0,OLED_WRITE_CMD);
    oled_ssd1306_write(0x81,OLED_WRITE_CMD);
    oled_ssd1306_write(0xff,OLED_WRITE_CMD);
    oled_ssd1306_write(0xA1,OLED_WRITE_CMD);
    oled_ssd1306_write(0xA6,OLED_WRITE_CMD);
    oled_ssd1306_write(0xA8,OLED_WRITE_CMD);
    oled_ssd1306_write(0x2F,OLED_WRITE_CMD);
    oled_ssd1306_write(0xC8,OLED_WRITE_CMD);
	oled_ssd1306_write(0xD3,OLED_WRITE_CMD);
	oled_ssd1306_write(0x00,OLED_WRITE_CMD);
	oled_ssd1306_write(0xD5,OLED_WRITE_CMD);
	oled_ssd1306_write(0x80,OLED_WRITE_CMD);
    oled_ssd1306_write(0xD9,OLED_WRITE_CMD);
	oled_ssd1306_write(0x21,OLED_WRITE_CMD);
	oled_ssd1306_write(0xDA,OLED_WRITE_CMD);
	oled_ssd1306_write(0x12,OLED_WRITE_CMD);
	oled_ssd1306_write(0xdb,OLED_WRITE_CMD);
	oled_ssd1306_write(0x40,OLED_WRITE_CMD);
	oled_ssd1306_write(0x8d,OLED_WRITE_CMD);
	oled_ssd1306_write(0x14,OLED_WRITE_CMD);
	oled_ssd1306_fill(0x00);
    oled_ssd1306_write(0xAF,OLED_WRITE_CMD);
#endif
    
}
