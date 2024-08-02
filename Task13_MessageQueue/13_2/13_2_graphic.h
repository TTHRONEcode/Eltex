#ifndef GRAPHIC_H
#define GRAPHIC_H

void InitGraphic();

void PutLoginText ();

void EnterStdinString (int mode, char *entered_str);

void SignalExit (int signo);

void PrintThisId (char *id);

void RenderInputMsgs (int is_this);

void RenderIds ();

void DelWins ();

#endif