#include "cgame.h"

void CG_ProcessSnapshots_H00K(void) {
    static int serverTime = -1;
    
    CG_ProcessSnapshots();
    if (cg.snap && cg.snap->serverTime > serverTime) {
        DF_AddRecallState();
        serverTime = cg.snap->serverTime;
    }
}
