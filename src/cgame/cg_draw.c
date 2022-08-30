#include "cgame.h"

void CG_DrawActive_H00K(stereoFrame_t stereoView) {
    CG_DrawActive(stereoView);
    DF_RecallUpdateKeys();
    DF_RenderRecallState();
}
