#include "cg_local.h"

// df_gap.c
extern int decrypted_timer;
extern void CG_Grapple(centity_t *);
extern qboolean SomeNoDrawRadiusChecks(centity_t *);
int DecryptTimerWithoutAlteringTheGlobal(snapshot_t *);

// misc.c
int sprintf(char *, const char *, ...);

// cg_recall.c
void DF_InitRecall(void);
void DF_ShutdownRecall(void);
qboolean DF_RestoreRecall(void);
void DF_RecallUpdateKeys(void);
void DF_RenderRecallState(void);
void DF_AddRecallState(void);
void DF_InitRecallCommands(void);
