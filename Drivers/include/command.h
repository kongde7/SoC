#ifndef __COMMAND_H
#define __COMMAND_H

#define CMD_BUF_SIZE 80
extern char Command_Buf[ ];

uint8_t cmd_line_parser(uint8_t *pCmd, uint8_t** options);
void led_process();
void date_process();
void time_process();
void Command_Process(uint8_t *pCommand);

#endif
