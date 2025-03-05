#ifndef OLED_H
#define OLED_H

#include "spi.h"
#include "../oled/Adafruit_GFX.h"
#include "../oled/Adafruit_OLED.h"
#include "../oled/Adafruit_SSD1351.h"
#include "../oled/glcdfont.h"
#include "buffers.h"

//*****************************************************************************
//                 OLED SYSTEMS
//*****************************************************************************

#define SPI_IF_BIT_RATE  100000
#define TR_BUFF_SIZE     100

#define BLACK           0x0000
#define BLUE            0x001F
#define GREEN           0x07E0
#define CYAN            0x07FF
#define RED             0xF800
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

#define OLED_MIN_PX 0
#define OLED_MAX_PX 100

unsigned long ucTxBuffNdx, ucRxBuffNdx;

void InitOLED() {
    ucTxBuffNdx = 0;
    ucRxBuffNdx = 0;

    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralReset(PRCM_GSPI);

    MAP_SPIReset(GSPI_BASE);
    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                     SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                     (SPI_SW_CTRL_CS |
                     SPI_4PIN_MODE |
                     SPI_TURBO_OFF |
                     SPI_CS_ACTIVEHIGH |
                     SPI_WL_8));
    MAP_SPIEnable(GSPI_BASE);

    Adafruit_Init();

    fillScreen(WHITE);
    fillScreen(BLACK);
}

void DrawText(int x, int y, char* text, int color, int bg, int size){
    int i = 0;
    while (text[i] != '\0'){
        drawChar(x, y, text[i], color, bg, size);
        x += 8;
        i++;
        if (x >= 120) {
            y += 8;
            x = 0;
        }
    }
}

void DrawMessage(Buffer_t *buff, int x_loc, int y_loc) {
    DrawText(x_loc,y_loc, buff->buf, BLACK, BLACK, 1);
    DrawText(x_loc,y_loc, buff->buf, WHITE, BLACK, 1);
}

void ClearMessage(Buffer_t *buff, int x_loc, int y_loc) {
    DrawText(x_loc,y_loc, buff->buf, BLACK, BLACK, 1);
}

#endif
