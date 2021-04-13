#include "cg_local.h"

extern int decrypted_timer;

void CG_SavePos_f(void)
{
    char saveposname[MAX_TOKEN_CHARS];
    int saveposname_len;
    char set_cmd_str[MAX_TOKEN_CHARS];
    int len;
    playerState_t *ps;
    int powerups[MAX_POWERUPS];
    int time_stuff[2];
    int misc[7];
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
#define DUMP_AND_SET(prefix, str, spec, array) \
do\
{\
    len = sprintf(set_cmd_str, "set %s_%s", (prefix), (str));\
    for (i = 0; i < sizeof((array))/sizeof((array)[0]); i++) {\
        len += sprintf(set_cmd_str + len, " " spec, (array)[i]);\
    }\
    sprintf(set_cmd_str + len, "\n");\
    trap_SendConsoleCommand(set_cmd_str);\
} while(0);\
//#enddef
    DUMP_AND_SET(saveposname, "pos", "%f", ps->origin);
    DUMP_AND_SET(saveposname, "angles", "%f", ps->viewangles);
    DUMP_AND_SET(saveposname, "vel", "%f", ps->velocity);
    time_stuff[0] = decrypted_timer;
    time_stuff[1] = !!(ps->stats[12] & 2);
    DUMP_AND_SET(saveposname, "time", "%d", time_stuff);
    DUMP_AND_SET(saveposname, "weapon", "%d", &ps->weapon);
    DUMP_AND_SET(saveposname, "ammo", "%d", ps->ammo);
    for (i = 0; i < MAX_POWERUPS; i++) {
        if ((i == PW_REDFLAG || i == PW_BLUEFLAG) && ps->powerups[i])
            powerups[i] = 1;
        else
            powerups[i] = (ps->powerups[i] < cg.time) ? 0 : ps->powerups[i] - cg.time;
    }
    DUMP_AND_SET(saveposname, "items", "%d", powerups);
    misc[0] = ps->stats[STAT_HEALTH];
    misc[1] = ps->stats[STAT_ARMOR];
    misc[2] = ps->pm_flags;
    misc[3] = ps->pm_time;
    misc[4] = ps->weaponstate;
    misc[5] = ps->weaponTime;
    misc[6] = ps->persistant[PERS_SCORE];
    DUMP_AND_SET(saveposname, "misc", "%d", misc);
#undef DUMP_AND_SET

    sprintf(set_cmd_str, "set %s varCommand placeplayer $%s_pos $%s_angles $%s_vel $%s_time $%s_weapon $%s_ammo $%s_items $%s_misc\n",
            saveposname, saveposname, saveposname, saveposname, saveposname, saveposname, saveposname, saveposname, saveposname);
    trap_SendConsoleCommand(set_cmd_str);
    CG_Printf("^2Saved\n");
}
