// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 73.65% NEARMISS).
// Fixes read from the original instructions:
// - The body is one do/while (result == 2) loop around a switch on the handler
//   result (case 3 aborts, cases 1/2 advance, case 0 stays), not a goto chain.
// - The two fast-forward scans differ: the stay-path scan clears the wait
//   word before returning on opcode 0x18 and before running a STOP record;
//   the advance-path scan does not.
// - Each scan's "STOP flag on the current record" exit is inlined (count = -1,
//   wait = 0, return 3); the scan loop tests the flag at its bottom.
// - The script block at actor+0x1F0 is a record (count, wait, current record
//   pointer, signed phase byte); records are (flags, next) at their head.
//
// Script interpreter: runs the actor's script block at actor+0x1F0 through
// the handler table ftab_0024D880 indexed by the record's low 12 bits.
// Handler result 0 = stay, 1 = advance, 2 = advance and run the next record
// this tick, 3 = abort. Both skip gates accept D_70003B91 == 2 and signed
// phase < 2 (phase is then set to 2) and fast-forward the script: from the
// current record, follow next (flag 0x40000000) or step 0x40 bytes, stop at
// opcode 0x18, run any record with the STOP flag (0x80000000) until its
// handler returns nonzero, and abort when the record being left has the STOP
// flag. Advancing past a STOP record ends the script (count = -1, return 1);
// flag 0x20000000 on a stepped record forces another pass this tick.
typedef struct ScriptRec {
    int flags;
    struct ScriptRec *next;
} ScriptRec;

typedef struct Script {
    int count;
    int wait;
    ScriptRec *rec;
    signed char phase;
} Script;

extern int (*ftab_0024D880[])(char *, Script *, ScriptRec *);
extern unsigned char D_70003B91;

int func_001BA1F0(char *actor) {
    Script *script = (Script *)(actor + 0x1F0);
    ScriptRec *record;
    int result;

    do {
        if (script->count <= 0) return 1;
        record = script->rec;
        result = ftab_0024D880[record->flags & 0xFFF](actor, script, record);
        switch (result) {
        case 0:
            if (D_70003B91 == 2 && script->phase < 2) {
                script->phase = 2;
                while (!(record->flags & 0x80000000)) {
                    if (record->flags & 0x40000000) {
                        script->rec = record->next;
                    } else {
                        script->rec = (ScriptRec *)((char *)script->rec + 0x40);
                    }
                    record = script->rec;
                    if ((record->flags & 0xFFF) == 0x18) {
                        script->wait = 0;
                        return 0;
                    }
                    if (record->flags & 0x80000000) {
                        script->wait = 0;
                        do {
                        } while (ftab_0024D880[record->flags & 0xFFF](actor, script, record) == 0);
                    }
                }
                script->count = -1;
                script->wait = 0;
                return 3;
            }
            break;
        case 1:
        case 2:
            if (record->flags & 0x80000000) {
                script->count = -1;
                script->wait = 0;
                return 1;
            }
            if (record->flags & 0x40000000) {
                script->rec = record->next;
            } else if (record->flags & 0x20000000) {
                result = 2;
                script->rec = (ScriptRec *)((char *)script->rec + 0x40);
            } else {
                script->rec = (ScriptRec *)((char *)script->rec + 0x40);
            }
            script->wait = 0;
            if (D_70003B91 == 2 && script->phase < 2) {
                script->phase = 2;
                while (!(record->flags & 0x80000000)) {
                    if (record->flags & 0x40000000) {
                        script->rec = record->next;
                    } else {
                        script->rec = (ScriptRec *)((char *)script->rec + 0x40);
                    }
                    record = script->rec;
                    if ((record->flags & 0xFFF) == 0x18) {
                        return 0;
                    }
                    if (record->flags & 0x80000000) {
                        do {
                        } while (ftab_0024D880[record->flags & 0xFFF](actor, script, record) == 0);
                    }
                }
                script->count = -1;
                script->wait = 0;
                return 3;
            }
            break;
        case 3:
            script->count = -1;
            script->wait = 0;
            return 3;
        }
    } while (result == 2);
    return 0;
}
