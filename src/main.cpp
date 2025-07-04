#include <map>
#include "Configuration.hpp"
#include "timeout.h"
#include "handle-sockets.h"
#include "poll-loop.h"
#include "signal-handler.h"

volatile sig_atomic_t g_signalCaught = false;

std::string getConfigFileString(int argc, char **argv)
{
    std::string configFile;

    if (argc < 2) {
        std::cout << "No config file found. Using default config file.\n";
        (void)argv;
        configFile = "./conf/default.conf";
    }
    else if (argc > 2)
        throw std::runtime_error("Expected 1 config file only");
    else
        configFile = argv[1];

    return configFile;
}

int main(int argc, char **argv)
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    try {
        std::string configFile = getConfigFileString(argc, argv);
        std::map< std::pair<std::string, std::string> , std::vector<Server> > servers = parseAllServers(configFile);
        std::vector<struct pollfd> pfds;
        std::vector<int> listeners;
        std::vector<Client> clients;
        
        setupAllListenerSockets(servers, pfds, listeners);
        pollLoop(servers, pfds, listeners, clients);
    }
    catch (const std::exception &e)
    {
        std::cerr << RED << "Error: " << RESET << e.what() << "\n";
        return 1;
    }

    return 0;
}
