#include <stdio.h>
#include "i2c.h"
#include "screen_cmds.h"

void screen_init(void)
{
	I2Csendcmd(SCREEN_ADDR, COMMAND_CLEAR_DISPLAY);
	
	I2Csendcmd(SCREEN_ADDR, COMMAND_8BIT_4LINES_NORMAL_RE1_IS0);
	I2Csendcmd(SCREEN_ADDR, COMMAND_NW);
	I2Csendcmd(SCREEN_ADDR, COMMAND_SEGMENT_BOTTOM_VIEW);
	I2Csendcmd(SCREEN_ADDR, COMMAND_BS1_1);
	I2Csendcmd(SCREEN_ADDR, COMMAND_8BIT_4LINES_RE0_IS1);
	I2Csendcmd(SCREEN_ADDR, COMMAND_BS0_1);
	I2Csendcmd(SCREEN_ADDR, COMMAND_FOLLOWER_CONTROL);
	I2Csendcmd(SCREEN_ADDR, COMMAND_POWER_BOOSTER_CONTRAST);
	I2Csendcmd(SCREEN_ADDR, COMMAND_SET_CONTRAST_1010);
	I2Csendcmd(SCREEN_ADDR, COMMAND_8BIT_4LINES_RE0_IS0);
	I2Csendcmd(SCREEN_ADDR, COMMAND_DISPLAY_ON_CURSOR_ON_BLINK_ON);
	
	I2Csendcmd(SCREEN_ADDR, COMMAND_CLEAR_DISPLAY);
}

void screen_write_string(char string_to_write[])
{
	int letter=0;
	
	I2Csendcmd(SCREEN_ADDR, COMMAND_CLEAR_DISPLAY);
	I2Csendcmd(SCREEN_ADDR, COMMAND_SET_CURSOR_LINE_1);
	int current_line = COMMAND_SET_CURSOR_LINE_1;
	
	while(string_to_write[letter]!='\0')
	{
		if ((letter != 0) && (letter % LINE_LENGTH == 0))
		{
			if (current_line == COMMAND_SET_CURSOR_LINE_4){
				current_line = COMMAND_SET_CURSOR_LINE_1;// We've gone past the end of the line, go to the next one
				I2Csendcmd(SCREEN_ADDR, COMMAND_CLEAR_DISPLAY);
			}
			else
			current_line = current_line+0x20;
			I2Csendcmd(SCREEN_ADDR, current_line); // We've gone past the end of the line, go to the next one
		}
		
		I2Csenddatum(SCREEN_ADDR, string_to_write[letter]);
		letter++;
	}
}