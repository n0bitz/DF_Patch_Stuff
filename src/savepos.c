#include "cg_local.h"
#define STAT_JUMPTIME 10
#define STAT_DJING 11

extern int decrypted_timer;
int sprintf(char *, const char *, ...);
int DecryptTimerWithoutAlteringTheGlobal(snapshot_t *);

void CG_SavePos_f(void) {
    char saveposname[MAX_TOKEN_CHARS];
    int saveposname_len;
    char set_cmd_str[MAX_TOKEN_CHARS];
    int len;
    playerState_t *ps;
    int powerups[MAX_POWERUPS];
    int time_stuff[2];
    int misc[10];
    int i;

    trap_Cvar_VariableStringBuffer("saveposname", saveposname, sizeof(saveposname));
    saveposname_len = strlen(saveposname);
    if (!saveposname_len) {
        CG_Printf("%s is empty. use: set %s <myposname> <options>\n", "saveposname",
                  "saveposname");
        return;
    }
    if (saveposname_len > 32) {
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
} while(0);                                                       \
//#enddef
    DUMP_AND_SET(ps->origin, f, saveposname, "pos");
    DUMP_AND_SET(ps->viewangles, f, saveposname, "angles");
    DUMP_AND_SET(ps->velocity, f, saveposname, "vel");
    time_stuff[0] = DecryptTimerWithoutAlteringTheGlobal(cg.snap);
    time_stuff[1] = !!(ps->stats[12] & 2);
    DUMP_AND_SET(time_stuff, i, saveposname, "time");
    DUMP_AND_SET(&ps->weapon, i, saveposname, "weapon");
    DUMP_AND_SET(ps->ammo, i, saveposname, "ammo");
    for (i = 0; i < MAX_POWERUPS; i++) {
        powerups[i] = ps->powerups[i];
        if (!powerups[i]) powerups[i] = -1;
        else if (i == PW_REDFLAG || i == PW_BLUEFLAG) powerups[i] = 1;
        else powerups[i] -= cg.time;
    }
    DUMP_AND_SET(powerups, i, saveposname, "items");
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
    DUMP_AND_SET(misc, i, saveposname, "misc");
#undef DUMP_AND_SET

    sprintf(set_cmd_str, "set %s varCommand placeplayer $%s_pos $%s_angles $%s_vel $%s_time $%s_weapon $%s_ammo $%s_items $%s_misc\n",
            saveposname, saveposname, saveposname, saveposname, saveposname, saveposname, saveposname, saveposname, saveposname);
    trap_SendConsoleCommand(set_cmd_str);
    CG_Printf("^2Saved\n");
}

// We don't need trap_AddCommand for placeplayer as DF does it for autocomplete
void CG_PlacePlayer_f(void) {
    char buf[MAX_TOKEN_CHARS];

    trap_Argv(12, buf, sizeof(buf));
    cg.weaponSelect = atoi(buf);
    cg.weaponSelectTime = cg.time; // probably isn't needed
    // send the weapon we want to restore not the old one
    trap_SetUserCmdValue(cg.weaponSelect, cg.zoomSensitivity);
}

qboolean CG_ConsoleCommand_cust(void) {
    char cmd[MAX_TOKEN_CHARS];

    if (CG_ConsoleCommand()) return qtrue;
    trap_Argv(0, cmd, sizeof(cmd));
    // intercept placeplayer but let it pass to the server after we're done
    if (!Q_stricmp(cmd, "placeplayer")) CG_PlacePlayer_f();
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

int sprintf(char *buf, const char *fmt, ...) {
    va_list argptr;

    va_start(argptr, fmt);
    return vsprintf(buf, fmt, argptr);
}