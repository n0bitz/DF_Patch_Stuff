#include "g_local.h"

typedef struct new_gentity_s new_gentity_t;
struct new_gentity_s {
    entityState_t s;
    entityShared_t r;
    gclient_t *client;
    qboolean inuse;
    char *classname;
    int spawnflags;
    qboolean neverFree;
    int flags;
    char *model;
    char *model2;
    int freetime;
    int eventTime;
    qboolean freeAfterEvent;
    qboolean unlinkAfterEvent;
    qboolean physicsObject;
    float physicsBounce;
    int clipmask;
    moverState_t moverState;
    int soundPos1;
    int sound1to2;
    int sound2to1;
    int soundPos2;
    int soundLoop;
    new_gentity_t *parent;
    new_gentity_t *nextTrain;
    new_gentity_t *prevTrain;
    vec3_t pos1;
    vec3_t pos2;
    char *message;
    int timestamp;
    float angle;
    char *target;
    char *targetname;
    char *team;
    char *targetShaderName;
    char *targetShaderNewName;
    new_gentity_t *target_ent;
    float speed;
    vec3_t movedir;
    int nextthink;
    void (*think)(new_gentity_t *);
    void (*reached)(new_gentity_t *);
    void (*blocked)(new_gentity_t *, new_gentity_t *);
    void (*touch)(new_gentity_t *, new_gentity_t *, trace_t *);
    void (*use)(new_gentity_t *, new_gentity_t *, new_gentity_t *);
    void (*pain)(new_gentity_t *, new_gentity_t *, int);
    void (*die)(new_gentity_t *, new_gentity_t *, new_gentity_t *, int, int);
    int pain_debounce_time;
    int fly_sound_debounce_time;
    int last_move_time;
    int health;
    qboolean takedamage;
    int damage;
    int splashDamage;
    int splashRadius;
    int methodOfDeath;
    int splashMethodOfDeath;
    int count;
    new_gentity_t *chain;
    new_gentity_t *enemy;
    new_gentity_t *activator;
    new_gentity_t *teamchain;
    new_gentity_t *teammaster;
    int watertype;
    int waterlevel;
    int noise_index;
    float wait;
    float random;
    gitem_t *item;
    int unknown;
    int waittable[64];
};

typedef enum {
    TIMER_NO_EVENT,
    TIMER_START_EVENT,
    TIMER_STOP_EVENT,
    TIMER_CHECKPOINT_EVENT
} timerEvent_t;

typedef struct {
    int time;
    int one_ms_time;
    float total_timer_offset;
    timerEvent_t event;
    float timer_offset;
    qboolean timer_running;
    int checkpoint_bitmap;
    int num_checkpoints;
} timerInfo_t;

extern int get_cheats_enabled(void);
extern void placeplayer_teleport(new_gentity_t *, vec3_t, vec3_t, vec3_t);
extern timerInfo_t timers[MAX_CLIENTS];
extern int levelTime; // should be level.time (too lazy to verify struct layout hasn't changed)

void Cmd_PlacePlayer_f(new_gentity_t *ent)
{
    const char *s;
    int i;
    int len;
    char buf[MAX_TOKEN_CHARS];
    float angles[3];
    float origin[3];
    float velocity[3];
    playerState_t *ps;

    if (!get_cheats_enabled()) {
        trap_SendServerCommand(ent - (new_gentity_t *)g_entities, "print \"Cheats are not enabled on this server.\n\"");
        return;
    }
    if (trap_Argc() != 52) {
        trap_SendServerCommand(ent - (new_gentity_t *)g_entities, "print \"Usage: Just use savepos please.\n\"");
        return;
    }
    ps = &ent->client->ps;
    len = 1;

// HELL AWAITS ME
#define PARSE_ARG(dest, type) \
do\
{\
    trap_Argv(len++, buf, sizeof(buf));\
    (dest) = ato##type(buf);\
} while (0); \
//#enddef
    for (i = 0; i < 3; i++) PARSE_ARG(origin[i], f);
    for (i = 0; i < 3; i++) PARSE_ARG(angles[i], f);
    for (i = 0; i < 3; i++) PARSE_ARG(velocity[i], f);
    // DF technically does ent->client - level.clients,
    // ent - g_entities is equivalent
    PARSE_ARG(timers[ent - (new_gentity_t *)g_entities].time, i);
    PARSE_ARG(timers[ent - (new_gentity_t *)g_entities].timer_running, i);
    PARSE_ARG(ps->weapon, i);
    for (i = 0; i < MAX_WEAPONS; i++) PARSE_ARG(ps->ammo[i], i);
    for (i = 0; i < MAX_POWERUPS; i++) {
        PARSE_ARG(ps->powerups[i], i);
        if (ps->powerups[i] == -1) ps->powerups[i] = 0;
        else if (i == PW_REDFLAG || i == PW_BLUEFLAG) ps->powerups[i] = INT_MAX;
        else ps->powerups[i] += levelTime;
    }
    PARSE_ARG(ps->stats[STAT_HEALTH], i);
    PARSE_ARG(ps->stats[STAT_ARMOR], i);
    PARSE_ARG(ps->pm_flags, i);
    PARSE_ARG(ps->pm_time, i);
    PARSE_ARG(ps->weaponstate, i);
    PARSE_ARG(ps->weaponTime, i);
    PARSE_ARG(ps->persistant[PERS_SCORE], i);
#undef PARSE_ARG

    placeplayer_teleport(ent, origin, angles, velocity);
    trap_SendServerCommand(ent - (new_gentity_t *)g_entities, "print \"^3Restored\n\"");
}
