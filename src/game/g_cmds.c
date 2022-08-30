#include "g_local.h"

void G_Say_H00K(gentity_t *ent, gentity_t *target, int mode, const char *chatText)
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

    if (!DF_GetCheatsValue())
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
            ps->powerups[i] += level.time;
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
    PARSE_ARG(ps->stats[STAT_JUMPTIME], i);
    PARSE_ARG(ps->stats[STAT_DJING], i);
#undef PARSE_ARG

    DF_PlacePlayerTeleport(ent, origin, angles, velocity);
    trap_SendServerCommand(ent - g_entities, "print \"^3Restored\n\"");
}

void ClientCommand_H00K(int clientNum)
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
