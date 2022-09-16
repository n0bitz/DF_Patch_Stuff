#include "cgame.h"

extern	int			cg_numSolidEntities;
extern	centity_t	*cg_solidEntities[MAX_ENTITIES_IN_SNAPSHOT];
extern	int			cg_numTriggerEntities;
extern	centity_t	*cg_triggerEntities[MAX_ENTITIES_IN_SNAPSHOT];

extern qboolean IsItemEntityAvailableToClient(entityState_t *state,int clientNum);

void CG_BuildSolidList_H00K( void ) {
	int			i;
	centity_t	*cent;
	snapshot_t	*snap;
	entityState_t	*ent;

	cg_numSolidEntities = 0;
	cg_numTriggerEntities = 0;

	if ( cg.nextSnap && !cg.nextFrameTeleport && !cg.thisFrameTeleport ) {
		snap = cg.nextSnap;
	} else {
		snap = cg.snap;
	}

	for ( i = 0 ; i < snap->numEntities ; i++ ) {
		cent = &cg_entities[ snap->entities[ i ].number ];
		ent = &cent->currentState;

        if ( ent->generic1 & ((cg.snap->ps.pm_flags & PMF_PROMODE) ? GEN1_NOTCPM : GEN1_NOTVQ3) ) {
            continue;
        }

        if (ent->eType == ET_ITEM && !IsItemEntityAvailableToClient(&snap->entities[i], snap->ps.clientNum)) {
            continue;
        }

		if ( ent->eType == ET_ITEM || ent->eType == ET_PUSH_TRIGGER || ent->eType == ET_TELEPORT_TRIGGER ) {
			cg_triggerEntities[cg_numTriggerEntities] = cent;
			cg_numTriggerEntities++;
			continue;
		}

		if ( cent->nextState.solid ) {
			cg_solidEntities[cg_numSolidEntities] = cent;
			cg_numSolidEntities++;
			continue;
		}
	}
}
