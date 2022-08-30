#include "cgame.h"

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

void DF_InitRecallCommands(void) {
    trap_AddCommand("recall");
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
    out->dj_time = ps->stats[STAT_JUMPTIME];
    out->djing = ps->stats[STAT_DJING];
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
    len = DF_sprintf(buf, "restorestate");
// HELL AWAITS ME
#define xx(array, type, prefix)                                    \
    do                                                                 \
    {                                                                   \
        int i;                                                  \
        for (i = 0; i < sizeof((array)) / sizeof((array)[0]); i++)     \
            len += DF_sprintf(buf + len, " %" #type, (array)[i]); \
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
    DF_sprintf(buf + len, "\n");
    trap_SendConsoleCommand(buf);
    return qtrue;

}
