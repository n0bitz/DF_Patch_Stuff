#include "cgame.h"

void CG_Init_H00K(int serverMessageNum, int serverCommandSequence, int clientNum) {
    CG_Init(serverMessageNum, serverCommandSequence, clientNum);
    DF_InitRecall();
}

void CG_Shutdown_H00K(void) {
    CG_Shutdown();
    DF_ShutdownRecall();
}
