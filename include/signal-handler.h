#ifndef SIGNAL_HANDLER_H
# define SIGNAL_HANDLER_H

#include <signal.h>

extern volatile sig_atomic_t g_signalCaught;

void signalHandler(int sigNum);

#endif