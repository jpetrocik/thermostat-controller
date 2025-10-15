#include "configuration.h"
#ifdef LCD_ENABLED

#ifndef LCD_H
#define LCD_H

#define READ_UPDATE 1

void lcd_setup();
void lcd_loop(bool refresh);
#endif
#endif