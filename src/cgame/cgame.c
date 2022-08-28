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
    misc[8] = ps->stats[STAT_JUMP_TIME];
    misc[9] = ps->stats[STAT_DOUBLE_JUMPING];
    DUMP_AND_SET(misc, i, savestatename, "misc");
#undef DUMP_AND_SET

    sprintf(set_cmd_str, "set %s silent varCommand restorestate $%s_pos $%s_angles $%s_vel $%s_time $%s_weapon $%s_ammo $%s_items $%s_misc\n",
            savestatename, savestatename, savestatename, savestatename, savestatename, savestatename, savestatename, savestatename, savestatename);
    trap_SendConsoleCommand(set_cmd_str);
    CG_Printf("^2Saved\n");
}

void CG_RestoreState_f(void)
{
    char buf[MAX_TOKEN_CHARS];

    trap_Argv(12, buf, sizeof(buf));
    cg.weaponSelect = atoi(buf);
    cg.weaponSelectTime = cg.time; // probably isn't needed
    // send the weapon we want to restore not the old one
    trap_SetUserCmdValue(cg.weaponSelect, cg.zoomSensitivity);
}
typedef struct {
    vec3_t origin;
    vec3_t viewangles;
    vec3_t velocity;
    int timer_time;
    qboolean timer_running;
    int weapon;
    int weaponstate;
    int weaponTime;
    int weapons;
	int	ammo[MAX_WEAPONS];
    int	powerups[MAX_POWERUPS];
    int health;
    int armor;
    int frags;
    int pm_flags;
    int pm_time;
    int dj_time;
    qboolean djing;
} recallState_t;

typedef struct {
    int idx;
    recallState_t buf[7777]; // TODO: idk
} recallRingBuf_t;

recallRingBuf_t recall_states;
static qboolean is_recalling;
static int recall_frame_idx = 0;

void DF_InitRecallBuffer(void) { // TODO: maybe call this when we worry about maps and files and stuff?
    memset(&recall_states, 0, sizeof(recall_states));
}

void DF_MakeRecallState(recallState_t *out) {
    int i;
    playerState_t *ps = &cg.snap->ps; // TODO: should we use pps?

    VectorCopy(ps->origin, out->origin);
    VectorCopy(ps->viewangles, out->viewangles);
    VectorCopy(ps->velocity, out->velocity);
    out->timer_time = DecryptTimerWithoutAlteringTheGlobal(cg.snap);
    out->timer_running = !!(ps->stats[12] & 2);
    out->weapon = ps->weapon;
    out->weaponstate = ps->weaponstate;
    out->weaponTime = ps->weaponTime;
    out->weapons = ps->stats[STAT_WEAPONS];
    memcpy(out->ammo, ps->ammo, sizeof(ps->ammo));
    for (i = 0; i < MAX_POWERUPS; i++)
    {
        out->powerups[i] = ps->powerups[i];
        if (!out->powerups[i])
            out->powerups[i] = -1;
        else if (i == PW_REDFLAG || i == PW_BLUEFLAG)
            out->powerups[i] = 1;
        else
            out->powerups[i] -= cg.time;
    }
    out->health = ps->stats[STAT_HEALTH];
    out->armor = ps->stats[STAT_ARMOR];
    out->frags = ps->persistant[PERS_SCORE];
    out->pm_flags = ps->pm_flags;
    out->pm_time = ps->pm_time;
    out->dj_time = ps->stats[STAT_JUMP_TIME];
    out->djing = ps->stats[STAT_DOUBLE_JUMPING];
}

void DF_AddRecallState(void)
{
    if (is_recalling) return;
    DF_MakeRecallState(&recall_states.buf[recall_states.idx]);
    recall_states.idx = (recall_states.idx + 1) % (sizeof(recall_states.buf) / sizeof(recall_states.buf[0]));
}

void DF_RenderRecallState(void) {
    refdef_t refdef;

    if (!is_recalling) return;
    memset(&refdef, 0, sizeof(refdef));
	AxisClear( refdef.viewaxis );
    AnglesToAxis(recall_states.buf[recall_frame_idx].viewangles, refdef.viewaxis);
	refdef.fov_x = cg.refdef.fov_x; // TODO: fix since player can be in water or whatever when recalling a position not in water or something
	refdef.fov_y = cg.refdef.fov_y; // TODO: same as above
	refdef.x = 0;
	refdef.y = 0;
	refdef.width = 300;
	refdef.height = 300;
	refdef.time = cg.time;
    VectorCopy(recall_states.buf[recall_frame_idx].origin, refdef.vieworg);
    trap_R_RenderScene(&refdef);
}

#define BUTTON_SEEK_BWD (1 << 14)
#define BUTTON_SEEK_FWD (1 << 15)
void DF_RecallUpdateKeys(void) {
    static int commandTime = -1;
    usercmd_t cmd;
    
    trap_GetUserCmd(trap_GetCurrentCmdNumber(), &cmd);
    if (cmd.serverTime == commandTime) return; // NOTE: is this even possible?
    commandTime = cmd.serverTime;
    if (cmd.buttons & BUTTON_SEEK_FWD) recall_frame_idx = (recall_frame_idx + 1) % (sizeof(recall_states.buf) / sizeof(recall_states.buf[0]));
    else if (cmd.buttons & BUTTON_SEEK_BWD) recall_frame_idx = (recall_frame_idx - 1) % (sizeof(recall_states.buf) / sizeof(recall_states.buf[0]));
}

void CG_Recall_f(void) {
    is_recalling = !is_recalling;
    recall_frame_idx = recall_states.idx;
    CG_Printf("Recall Mode: %s\n", (is_recalling) ? "ON" : "OFF");
}

qboolean DF_RestoreRecall(void) {
    char buf[MAX_TOKEN_CHARS];
    playerState_t *ps = &cg.snap->ps;
    team_t team = ps->persistant[PERS_TEAM];
    recallState_t *recall_state = &recall_states.buf[recall_frame_idx];
    int len;

    if (!is_recalling || team == TEAM_SPECTATOR) return qfalse;
    len = sprintf(buf, "restorestate");
// HELL AWAITS ME
#define xx(array, type, prefix)                                    \
    do                                                                 \
    {                                                                   \
        int i;                                                  \
        for (i = 0; i < sizeof((array)) / sizeof((array)[0]); i++)     \
            len += sprintf(buf + len, " %" #type, (array)[i]); \
    } while (0);

    xx(recall_state->origin, f, buf);
    xx(recall_state->viewangles, f, buf);
    xx(recall_state->velocity, f, buf);
    xx(&recall_state->timer_time, i, buf);
    xx(&recall_state->timer_running, i, buf);
    xx(&recall_state->weapon, i, buf);
    xx(recall_state->ammo, i, buf);
    xx(recall_state->powerups, i, buf);
    xx(&recall_state->health, i, buf);
    xx(&recall_state->armor, i, buf);
    xx(&recall_state->pm_flags, i, buf);
    xx(&recall_state->pm_time, i, buf);
    xx(&recall_state->weaponstate, i, buf);
    xx(&recall_state->weaponTime, i, buf);
    xx(&recall_state->weapons, i, buf);
    xx(&recall_state->frags, i, buf);
    xx(&recall_state->dj_time, i, buf);
    xx(&recall_state->djing, i, buf);
#undef xx
    sprintf(buf + len, "\n");
    trap_SendConsoleCommand(buf);
    return qtrue;

}

void CG_ProcessSnapshots_Hook(void) {
    static int serverTime = -1;
    
    CG_ProcessSnapshots();
    if (cg.snap && cg.snap->serverTime > serverTime) {
        DF_AddRecallState();
        serverTime = cg.snap->serverTime;
    }
}

void CG_DrawActive_Hook(stereoFrame_t stereoView) {
    CG_DrawActive(stereoView);
    DF_RecallUpdateKeys();
    DF_RenderRecallState();
}

void CG_InitConsoleCommands_Hook(void)
{
    CG_InitConsoleCommands();
    trap_AddCommand("savestate");
    trap_AddCommand("restorestate");
    trap_AddCommand("recall"); // TODO: name me properly
}

qboolean CG_ConsoleCommand_Hook(void)
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
    // intercept but let it pass to the server after we're done
    if (!Q_stricmp(cmd, "restorestate"))
        CG_RestoreState_f();
    else if (!Q_stricmp(cmd, "kill"))
        return DF_RestoreRecall();
    return qfalse;
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

qboolean SomeNoDrawRadiusChecks_Hook(centity_t *cent)
{
    // The real fix for hook coloring is to set clientNum in fire_grapple in qagame code.
    // This can be removed once DeFRaG fixes qagame and server runners update.
    if (cent->currentState.weapon == WP_GRAPPLING_HOOK)
        cent->currentState.clientNum = cent->currentState.otherEntityNum;
    return SomeNoDrawRadiusChecks(cent);
}

void CG_Grapple_Hook(centity_t *cent)
{
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
    if (draw)
        CG_Grapple(cent);
}

void CG_Beam_Hook(centity_t *cent)
{
    refEntity_t ent;
    entityState_t *s1;
    static qhandle_t shader = 0;

    s1 = &cent->currentState;

    // create the render entity
    memset(&ent, 0, sizeof(ent));
    VectorCopy(s1->pos.trBase, ent.origin);
    VectorCopy(s1->origin2, ent.oldorigin);
    AxisClear(ent.axis);

    // RB_SurfaceBeam in most if not all engines are broken, causing beams to only be
    // visible in very specific circumstances. Breadsticks worked around this in mod
    // code by using the rail core for beams instead. When fixing this in DeFRaG,
    // believe in setting cgs.media.railCoreShader in CG_Register and using it instead.
    ent.reType = RT_RAIL_CORE;
    if (!shader)
        shader = trap_R_RegisterShader("railCore");
    ent.customShader = shader;
    ent.shaderRGBA[0] = 255;
    ent.shaderRGBA[1] = 0;
    ent.shaderRGBA[2] = 0;
    ent.shaderRGBA[3] = 255;
    ent.renderfx = RF_NOSHADOW;

    // add to refresh list
    trap_R_AddRefEntityToScene(&ent);
}

int sprintf(char *buf, const char *fmt, ...)
{
    va_list argptr;

    va_start(argptr, fmt);
    return vsprintf(buf, fmt, argptr);
}
