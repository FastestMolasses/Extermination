// Tail-call thunk: raises the calling thread to priority 5.
extern void RotateThreadReadyQueue(int prio);

void func_00203980(void) {
    RotateThreadReadyQueue(5);
}
