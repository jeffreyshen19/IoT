#include <mraa/i2c.h>
#include <mraa.h>
#include <stdio.h>
#include <unistd.h>
#include <upm/jhd1313m1.h>

int main()
{
  // 0x62 RGB_ADDRESS, 0x3E LCD_ADDRESS
    upm::Jhd1313m1 lcd(0, 0x3E, 0x62);
    int ndx = 0;
    uint8_t rgb[7][3] = {
        {0xd1, 0x00, 0x00},
        {0xff, 0x66, 0x22},
        {0xff, 0xda, 0x21},
        {0x33, 0xdd, 0x00},
        {0x11, 0x33, 0xcc},
        {0x22, 0x00, 0x66},
        {0x33, 0x00, 0x44}};
    while (1)
    {
        // Alternate rows on the LCD
        lcd.setCursor(ndx%2,0);
        // Change the color
        uint8_t r = rgb[ndx%7][0];
        uint8_t g = rgb[ndx%7][1];
        uint8_t b = rgb[ndx%7][2];
        lcd.setColor(r, g, b);
        lcd.write("Hello World " + std::to_string(ndx));
        // Echo via printf
        printf("Hello World %d rgb: 0x%02x%02x%02x\n", ndx++, r, g, b);
        upm_delay(1);
    }

}
