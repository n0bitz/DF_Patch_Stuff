#include "g_local.h"

gentity_t *fire_grapple_H00K(gentity_t *self, vec3_t start, vec3_t dir)
{
    gentity_t *hook = fire_grapple(self, start, dir);
    hook->s.clientNum = self->s.number;
    return hook;
}
