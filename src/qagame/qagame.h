#include "g_local.h"

typedef enum
{
    TIMER_NO_EVENT,
    TIMER_START_EVENT,
    TIMER_STOP_EVENT,
    TIMER_CHECKPOINT_EVENT
} timerEvent_t;

typedef struct
{
    int time;
    int one_ms_time;
    float total_timer_offset;
    timerEvent_t event;
    float timer_offset;
    qboolean timer_running;
    int checkpoint_bitmap;
    int num_checkpoints;
} timerInfo_t;

extern int levelTime; // should be level.time (too lazy to verify struct layout hasn't changed)
extern timerInfo_t timers[MAX_CLIENTS];
extern void ClientSpawnEntSetStuff(gentity_t *);
extern int get_cheats_enabled(void);
extern void placeplayer_teleport(gentity_t *, vec3_t, vec3_t, vec3_t);
