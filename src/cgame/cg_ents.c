#include "cgame.h"

void CG_Grapple_H00K(centity_t *cent)
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
    draw = !SomeNoDrawRadiusChecks_H00K(cent);
    cent->currentState.eType = ET_GRAPPLE;
    if (draw)
        CG_Grapple(cent);
}

void CG_Beam_H00K(centity_t *cent)
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
