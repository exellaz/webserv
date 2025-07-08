#include "Client.h"
#include <sys/socket.h>

void Client::sendResponseToClient()
{
    if (send(_fd, response.toString().c_str(), response.toString().size(), 0) == -1) {
        std::cout << "server: error sending response to Socket " << _fd << ", closed connection.\n";
		_connState = DISCONNECTED;
        return;
    }
    std::cout << "server: Response sent to client.\n";
}
