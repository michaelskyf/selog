#include "../include/selog.h"

int main(void)
{
	selog_setup_default();

	log_trace("test");
	log_debug("test");
	log_info("test");
	log_warning("test");
	log_error("test");
	log_fatal("test");

	return 0;
}
