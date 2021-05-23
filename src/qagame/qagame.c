#include "qagame.h"

void Cmd_RestoreState_f(gentity_t *ent)
{
    const char *s;
    int i;
    int len;
    char buf[MAX_TOKEN_CHARS];
    float angles[3];
    float origin[3];
    float velocity[3];
    playerState_t *ps;

    if (!get_cheats_enabled())
    {
        trap_SendServerCommand(ent - g_entities, "print \"Cheats are not enabled on this server.\n\"");
        return;
    }
    if (trap_Argc() != 55)
    {
        trap_SendServerCommand(ent - g_entities, "print \"Usage: Just use savestate please.\n\"");
        return;
    }
    ps = &ent->client->ps;
    len = 1;

// HELL AWAITS ME
#define PARSE_ARG(dest, type)               \
    do                                      \
    {                                       \
        trap_Argv(len++, buf, sizeof(buf)); \
        (dest) = ato##type(buf);            \
    } while (0);

    for (i = 0; i < 3; i++)
        PARSE_ARG(origin[i], f);
    for (i = 0; i < 3; i++)
        PARSE_ARG(angles[i], f);
    for (i = 0; i < 3; i++)
        PARSE_ARG(velocity[i], f);
    // DF technically does ent->client - level.clients in its get_timer function
    // but the following is equivalent
    PARSE_ARG(timers[ent - g_entities].time, i);
    PARSE_ARG(timers[ent - g_entities].timer_running, i);
    PARSE_ARG(ps->weapon, i);
    for (i = 0; i < MAX_WEAPONS; i++)
        PARSE_ARG(ps->ammo[i], i);
    for (i = 0; i < MAX_POWERUPS; i++)
    {
        PARSE_ARG(ps->powerups[i], i);
        if (ps->powerups[i] == -1)
            ps->powerups[i] = 0;
        else if (i == PW_REDFLAG || i == PW_BLUEFLAG)
            ps->powerups[i] = INT_MAX;
        else
            ps->powerups[i] += levelTime;
    }
    PARSE_ARG(ent->health, i);
    ps->stats[STAT_HEALTH] = ent->health;
    PARSE_ARG(ps->stats[STAT_ARMOR], i);
    PARSE_ARG(ps->pm_flags, i);
    PARSE_ARG(ps->pm_time, i);
    PARSE_ARG(ps->weaponstate, i);
    PARSE_ARG(ps->weaponTime, i);
    PARSE_ARG(ps->stats[STAT_WEAPONS], i);
    PARSE_ARG(ps->persistant[PERS_SCORE], i);
    PARSE_ARG(ps->stats[STAT_JUMP_TIME], i);
    PARSE_ARG(ps->stats[STAT_DOUBLE_JUMPING], i);
#undef PARSE_ARG

    placeplayer_teleport(ent, origin, angles, velocity);
    trap_SendServerCommand(ent - g_entities, "print \"^3Restored\n\"");
}

void ClientCommand_Hook(int clientNum)
{
    gentity_t *ent;
    char cmd[MAX_TOKEN_CHARS];

    ent = g_entities + clientNum;
    if (!ent->client)
        return; // not fully in game yet
    trap_Argv(0, cmd, sizeof(cmd));
    if (!Q_stricmp(cmd, "restorestate"))
    {
        Cmd_RestoreState_f(ent);
        return;
    }
    ClientCommand(clientNum);
}

gentity_t *fire_grapple_Hook(gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t *hook = fire_grapple(self, start, dir);
    hook->s.clientNum = self->s.number;
    return hook;
}

void G_Say_Hook(gentity_t *ent, gentity_t *target, int mode, const char *chatText)
{
    int clientNum;

    // DeFRaG's ignore checking function is incorrectly called in G_Say with
    // `ent->client->ps.clientNum`. That is not always the client num of the client.
    // For example when the client is follow spectating another player,
    // it's that of the client being follow spectated.
    // I temporarily set `ent->client->ps.clientNum` to the right thing,
    // so the ignore logic works. This is not the right solution, it's just
    // convenient when patching. When fixing this in DeFRaG, it's probably a
    // good idea to instead call the ignore checking function with the right
    // client number (ie. `ent - g_entities` or `ent->client - level.clients`).
    clientNum = ent->client->ps.clientNum;
    ent->client->ps.clientNum = ent - g_entities;
    G_Say(ent, target, mode, chatText);
    ent->client->ps.clientNum = clientNum;
}

void target_laser_think_Hook(gentity_t *self)
{
    vec3_t end;
    trace_t tr;
    vec3_t point;

    // if pointed at another entity, set movedir to point at it
    if (self->enemy)
    {
        VectorMA(self->enemy->s.origin, 0.5, self->enemy->r.mins, point);
        VectorMA(point, 0.5, self->enemy->r.maxs, point);
        VectorSubtract(point, self->s.origin, self->movedir);
        VectorNormalize(self->movedir);
    }

    // fire forward and see what we hit
    VectorMA(self->s.origin, 2048, self->movedir, end);

    trap_Trace(&tr, self->s.origin, NULL, NULL, end, self->s.number, CONTENTS_SOLID | CONTENTS_BODY | CONTENTS_CORPSE);

    // Quake III Arena and DeFRaG incorrectly check `tr.entityNum != 0`.
    // Causing client 0 to not hurt from laser. Should actually be what is below.
    if (tr.entityNum != ENTITYNUM_NONE)
    {
        // hurt it if we can
        // breadsticks changed attacker from self->activator to self
        // I have no idea why he did that, and neither does he.
        // Shooters use self too, so whatever, breadsticks is never wrong.
        G_Damage(g_entities + tr.entityNum, self, self, self->movedir,
                 tr.endpos, self->damage, DAMAGE_NO_KNOCKBACK, MOD_TARGET_LASER);
    }

    VectorCopy(tr.endpos, self->s.origin2);

    trap_LinkEntity(self);
    self->nextthink = levelTime + FRAMETIME;
}

void ClientSpawnEntSetStuff_Hook(gentity_t *ent)
{
    int interferenceOff;

    // When fixing this in DeFRaG, it should be straightforward.
    // Just set ent->clipmask to the right thing in ClientSpawn or
    // one of the things it calls maybe, like the following.
    ClientSpawnEntSetStuff(ent);
    interferenceOff = trap_Cvar_VariableIntegerValue("df_mp_interferenceOff");
    if (interferenceOff & 1)
        ent->clipmask &= ~CONTENTS_BODY;
}
