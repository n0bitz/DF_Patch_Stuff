#include "g_local.h"

void target_laser_think_H00K(gentity_t *self)
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
    self->nextthink = level.time + FRAMETIME;
}
