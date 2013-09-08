#include "net.h"
#include "database.h"
#include "services.h"

int main ()
{
	connect_database ();
	init_listen_socket();

	start_services ();

	return 0;
}
