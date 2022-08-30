#include "g_local.h"

void DF_ResetClient_H00K(gentity_t *ent)
{
    int interferenceOff;

    // When fixing this in DeFRaG, it should be straightforward.
    // Just set ent->clipmask to the right thing in ClientSpawn or
    // one of the things it calls maybe, like the following.
    DF_ResetClient(ent);
    interferenceOff = trap_Cvar_VariableIntegerValue("df_mp_interferenceOff");
    if (interferenceOff & 1)
        ent->clipmask &= ~CONTENTS_BODY;
}
