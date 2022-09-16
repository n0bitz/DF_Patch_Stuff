#include "cgame.h"

extern qboolean df_promode;
extern qboolean defrag_obs;

void DF_1e1f_H00K(snapshot_t *next, snapshot_t *curr)
{
    if (!(next->snapFlags & SNAPFLAG_NOT_ACTIVE)) {
        df_promode = !!(next->ps.pm_flags & PMF_PROMODE);
        defrag_obs = !(next->ps.stats[STAT_MISC] & MISC_NOOB);
    }
    DF_1e1f(next, curr);
}

int DecryptTimerWithoutAlteringTheGlobal(snapshot_t *snap)
{
    int tmp;
    int time;

    tmp = decrypted_timer;
    time = DecryptTimer(snap, NULL);
    decrypted_timer = tmp;
    return time;
}

qboolean SomeNoDrawRadiusChecks_H00K(centity_t *cent)
{
    // The real fix for hook coloring is to set clientNum in fire_grapple in qagame code.
    // This can be removed once DeFRaG fixes qagame and server runners update.
    if (cent->currentState.weapon == WP_GRAPPLING_HOOK)
        cent->currentState.clientNum = cent->currentState.otherEntityNum;
    return SomeNoDrawRadiusChecks(cent);
}
