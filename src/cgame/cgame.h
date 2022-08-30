#include "cg_local.h"

// df_gap.c
extern int decrypted_timer;
extern void CG_Grapple(centity_t *);
extern qboolean SomeNoDrawRadiusChecks(centity_t *);
int DF_sprintf(char *, const char *, ...);
int DecryptTimerWithoutAlteringTheGlobal(snapshot_t *);

// cg_recall.c
qboolean DF_RestoreRecall(void);
void DF_RecallUpdateKeys(void);
void DF_RenderRecallState(void);
void DF_AddRecallState(void);
void DF_InitRecallCommands(void);
