#include <stdlib.h>
#include "commands/cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"

int cmds_TERMINATE(void)
{
	net_SendEmptyCmd(CMDS_TERMINATE);

	exit(0);

	return 0;
}