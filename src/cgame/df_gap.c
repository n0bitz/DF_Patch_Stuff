#include "cgame.h"

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
