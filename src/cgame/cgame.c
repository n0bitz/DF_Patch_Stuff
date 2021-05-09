#include "cg_local.h"
#define STAT_JUMPTIME 10
#define STAT_DJING 11

extern int decrypted_timer;
extern void CG_Grapple(centity_t *);
extern qboolean SomeNoDrawRadiusChecks(centity_t *);
int sprintf(char *, const char *, ...);
int DecryptTimerWithoutAlteringTheGlobal(snapshot_t *);

void CG_SaveState_f(void) {
    char savestatename[MAX_TOKEN_CHARS];
    int savestatename_len;
    char set_cmd_str[MAX_TOKEN_CHARS];
    int len;
    playerState_t *ps;
    int powerups[MAX_POWERUPS];
    int time_stuff[2];
    int misc[10];
    int i;

    trap_Cvar_VariableStringBuffer("savestatename", savestatename, sizeof(savestatename));
    savestatename_len = strlen(savestatename);
    if (!savestatename_len) {
        CG_Printf("%s is empty. use: set %s <mystatename>\n", "savestatename",
                  "savestatename");
        return;
    }
    if (savestatename_len > 32) {
        CG_Printf("Believe in a shorter name please (ie. 32 characters or less).");
        return;
    }
    ps = &cg.snap->ps;

// HELL AWAITS ME
#define DUMP_AND_SET(array, type, prefix, str)                    \
do {                                                              \
    len = sprintf(set_cmd_str, "set %s_%s", (prefix), (str));     \
    for (i = 0; i < sizeof((array))/sizeof((array)[0]); i++)      \
        len += sprintf(set_cmd_str + len, " %" #type, (array)[i]);\
    sprintf(set_cmd_str + len, "\n");                             \
    trap_SendConsoleCommand(set_cmd_str);                         \
} while(0);
//#enddef
    DUMP_AND_SET(ps->origin, f, savestatename, "pos");
    DUMP_AND_SET(ps->viewangles, f, savestatename, "angles");
    DUMP_AND_SET(ps->velocity, f, savestatename, "vel");
    time_stuff[0] = DecryptTimerWithoutAlteringTheGlobal(cg.snap);
    time_stuff[1] = !!(ps->stats[12] & 2);
    DUMP_AND_SET(time_stuff, i, savestatename, "time");
    DUMP_AND_SET(&ps->weapon, i, savestatename, "weapon");
    DUMP_AND_SET(ps->ammo, i, savestatename, "ammo");
    for (i = 0; i < MAX_POWERUPS; i++) {
        powerups[i] = ps->powerups[i];
        if (!powerups[i]) powerups[i] = -1;
        else if (i == PW_REDFLAG || i == PW_BLUEFLAG) powerups[i] = 1;
        else powerups[i] -= cg.time;
    }
    DUMP_AND_SET(powerups, i, savestatename, "items");
    misc[0] = ps->stats[STAT_HEALTH];
    misc[1] = ps->stats[STAT_ARMOR];
    misc[2] = ps->pm_flags;
    misc[3] = ps->pm_time;
    misc[4] = ps->weaponstate;
    misc[5] = ps->weaponTime;
    misc[6] = ps->stats[STAT_WEAPONS];
    misc[7] = ps->persistant[PERS_SCORE];
    misc[8] = ps->stats[STAT_JUMPTIME];
    misc[9] = ps->stats[STAT_DJING];
    DUMP_AND_SET(misc, i, savestatename, "misc");
#undef DUMP_AND_SET

    sprintf(set_cmd_str, "set %s silent varCommand restorestate $%s_pos $%s_angles $%s_vel $%s_time $%s_weapon $%s_ammo $%s_items $%s_misc\n",
            savestatename, savestatename, savestatename, savestatename, savestatename, savestatename, savestatename, savestatename, savestatename);
    trap_SendConsoleCommand(set_cmd_str);
    CG_Printf("^2Saved\n");
}

void CG_RestoreState_f(void) {
    char buf[MAX_TOKEN_CHARS];

    trap_Argv(12, buf, sizeof(buf));
    cg.weaponSelect = atoi(buf);
    cg.weaponSelectTime = cg.time; // probably isn't needed
    // send the weapon we want to restore not the old one
    trap_SetUserCmdValue(cg.weaponSelect, cg.zoomSensitivity);
}

void CG_InitConsoleCommands_Hook(void) {
    CG_InitConsoleCommands();
    trap_AddCommand("savestate");
    trap_AddCommand("restorestate");
}

qboolean CG_ConsoleCommand_Hook(void) {
    char cmd[MAX_TOKEN_CHARS];

    if (CG_ConsoleCommand()) return qtrue;
    trap_Argv(0, cmd, sizeof(cmd));
    if (!Q_stricmp(cmd, "savestate")) {
        CG_SaveState_f();
        return qtrue;
    }
    // intercept but let it pass to the server after we're done
    if (!Q_stricmp(cmd, "restorestate")) CG_RestoreState_f();
    return qfalse;
}

int DecryptTimerWithoutAlteringTheGlobal(snapshot_t *snap) {
    int tmp;
    int time;

    tmp = decrypted_timer;
    time = DecryptTimer(snap, NULL);
    decrypted_timer = tmp;
    return time;
}

qboolean SomeNoDrawRadiusChecks_Hook(centity_t *cent) {
    // The real fix for hook coloring is to set clientNum in fire_grapple in qagame code.
    // This can be removed once DeFRaG fixes qagame and server runners update.
    if (cent->currentState.weapon == WP_GRAPPLING_HOOK)
        cent->currentState.clientNum = cent->currentState.otherEntityNum;
    return SomeNoDrawRadiusChecks(cent);
}

void CG_Grapple_Hook(centity_t *cent) {
    qboolean draw;

    // When hooked on to something, the eType is ET_GRAPPLE. DeFRaG doesn't do
    // multiplayer no draw radius checks for ET_GRAPPLE though, and you can see
    // other hooks that are hooked. We don't want that, so I'm doing the checks
    // here in CG_Grapple (for convenience). When fixing this in DeFRaG,
    // it's probably a good idea to instead:
    //     1. In CG_AddCEntity, for ET_GRAPPLE, call the same no draw radius checking
    //     function used for ET_MISSILE.
    //     2. Update that no draw radius checking function to check for ET_GRAPPLE or
    //     ET_MISSILE instead of just the latter.
    cent->currentState.eType = ET_MISSILE;
    draw = !SomeNoDrawRadiusChecks_Hook(cent);
    cent->currentState.eType = ET_GRAPPLE;
    if (draw) CG_Grapple(cent);
}

int sprintf(char *buf, const char *fmt, ...) {
    va_list argptr;

    va_start(argptr, fmt);
    return vsprintf(buf, fmt, argptr);
}