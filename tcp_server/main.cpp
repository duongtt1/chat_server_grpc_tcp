#include "Server.h"
// #include "db/groupchatdb.h"
#include <hiredis/hiredis.h>


const std::string iface = "lo";
const std::string ip	= "127.0.0.1";
const int		  port	= 12345;

int main(int, char **) {
	Server server(ip, port, iface);
	server.start_cache();
	server.run();
	
	return 0;
}
