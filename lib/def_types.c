#include "def_types.h"

#define X(a, b) b,
static const string retStateBuff[] =
{
	RET_STATE_TABLE()
};
#undef X

string RetStateStr_Get(RET_STATE_t retState)
{
	return retStateBuff[retState];
}
