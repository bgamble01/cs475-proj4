// TODO - add your deadlock function prototypes

void rag_request(int pid, int lockid);
void rag_alloc(int pid, int lockid);
void rag_dealloc(int pid, int lockid);
void rag_print();
void deadlock_detect();
int isCycle(int *visited, int start, int arrSize);
void deadlock_recover(lid32 culprit);

// TODO - add an "extern" declaration for the RAG

extern int rag[NLOCK + NPROC][NLOCK + NPROC];
extern lid32 culprit;
