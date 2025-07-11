#include "signal-handler.h"

void signalHandler(int sigNum)
{
    if (sigNum == SIGINT || sigNum == SIGTERM)
        g_signalCaught = true;
}
