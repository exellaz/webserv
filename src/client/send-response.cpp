#include "client.h"
#include <sys/socket.h>

void Client::sendResponseToClient()
{
    if (send(_fd, response.toString().c_str(), response.toString().size(), 0) == -1) {
        std::cout << RED << "Error sending response to Socket " << _fd << ", closed connection." RESET << "\n";
		_connState = DISCONNECTED;
        return;
    }
    std::cout << infoTime() << "Response sent to client.\n";
    std::cout << response;
}
