#include "cgame.h"

void CG_SaveState_f(void)
{
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
    if (!savestatename_len)
    {
        CG_Printf("%s is empty. use: set %s <mystatename>\n", "savestatename",
                  "savestatename");
        return;
    }
    if (savestatename_len > 32)
    {
        CG_Printf("Believe in a shorter name please (ie. 32 characters or less).");
        return;
    }
    ps = &cg.snap->ps;

// HELL AWAITS ME
#define DUMP_AND_SET(array, type, prefix, str)                         \
    do                                                                 \
    {                                                                  \
        len = sprintf(set_cmd_str, "set %s_%s", (prefix), (str));      \
        for (i = 0; i < sizeof((array)) / sizeof((array)[0]); i++)     \
            len += sprintf(set_cmd_str + len, " %" #type, (array)[i]); \
        sprintf(set_cmd_str + len, "\n");                              \
        trap_SendConsoleCommand(set_cmd_str);                          \
    } while (0);

    DUMP_AND_SET(ps->origin, f, savestatename, "pos");
    DUMP_AND_SET(ps->viewangles, f, savestatename, "angles");
    DUMP_AND_SET(ps->velocity, f, savestatename, "vel");
    time_stuff[0] = DecryptTimerWithoutAlteringTheGlobal(cg.snap);
    time_stuff[1] = !!(ps->stats[12] & 2);
    DUMP_AND_SET(time_stuff, i, savestatename, "time");
    DUMP_AND_SET(&ps->weapon, i, savestatename, "weapon");
    DUMP_AND_SET(ps->ammo, i, savestatename, "ammo");
    for (i = 0; i < MAX_POWERUPS; i++)
    {
        powerups[i] = ps->powerups[i];
        if (!powerups[i])
            powerups[i] = -1;
        else if (i == PW_REDFLAG || i == PW_BLUEFLAG)
            powerups[i] = 1;
        else
            powerups[i] -= cg.time;
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

void CG_InitConsoleCommands_H00K(void)
{
    CG_InitConsoleCommands();
    trap_AddCommand("savestate");
    DF_InitRecallCommands();
}

qboolean CG_ConsoleCommand_H00K(void)
{
    char cmd[MAX_TOKEN_CHARS];

    if (CG_ConsoleCommand())
        return qtrue;
    trap_Argv(0, cmd, sizeof(cmd));
    if (!Q_stricmp(cmd, "recall")) {
        CG_Recall_f();
        return qtrue;
    }
    if (!Q_stricmp(cmd, "savestate"))
    {
        CG_SaveState_f();
        return qtrue;
    }
    if (!Q_stricmp(cmd, "kill"))
        return DF_RestoreRecall();
    return qfalse;
}
