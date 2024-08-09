#ifndef GRAPHIC_H
#define GRAPHIC_H

void InitGraphic ();

typedef enum
{
  INPUT_MODE_MESSAGE,
  INPUT_MODE_LOGIN
} InputMode;

typedef enum
{
  WND_LEFT_MSGS_OF_CLIENTS,
  WND_RIGHT_CLIENTS_ID,
  WND_DOWN_INPUT
} WndsNames;

char *InputHandler (int __return_login_or_msg_mode);

#include <ncurses.h>
void OutputStringArrayToWindowNum (char *__this_client_id_str,
                                   char *__str_array_1, char *__str_array_2,
                                   int __str_count, int __wnd_num);

void SignalGraphicExit ();

#endif // GRAPHIC_H