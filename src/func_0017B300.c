// CFLAGS: -O4,p -sdatathreshold 0
// SPR4 magazine reload (called from the player fire state machines
// func_00170A60/func_001703E0 family and the weapon-mode tops).
//
// The reserve s16 D_00810CB4 counts the player's TOTAL rifle rounds
// (including those in the magazine); the magazine byte D_00810C62 is just
// "rounds until next reload", so a reload sets mag = min(30, reserve)
// WITHOUT subtracting from the reserve (each shot decrements both).
//
// mode 0: reload only if the mag is empty (auto-reload when dry).
// mode 1: unconditional reload.
// else  : top-up — only when the mag is short AND the reserve holds more
//         rounds than the mag.
// Returns 0 on success, 1 if no reload happened.
//
// Matching notes: globals via idiom-8 raw volatile casts (CW $at absolute
// addressing; inject_relocs.py re-attaches HI16/LO16). The `(short)` cast
// on the cached reserve reproduces CW's redundant dsll32/dsra32 re-sign-
// extension before the slti; assigning the comparison back into the same
// variable lands it in CW's register. In the top-up arm the reserve is
// parked in the dead first-parameter register ($a0, `self`) and `thirty`
// is declared first / materialized between the guards so mwcc allocates
// $v0/$v1/$a0 exactly as CW did (li 30 sinks into the second guard's
// delay slot).
#define MAG (*(volatile unsigned char *)0x00810C62)        // D_00810C62
#define RESERVE_S (*(volatile short *)0x00810CB4)          // D_00810CB4
#define RESERVE_LO (*(volatile unsigned char *)0x00810CB4) // D_00810CB4 low byte

int func_0017B300(int self, int mode) {
    if (mode == 0) {
        int reserve = RESERVE_S;
        if (reserve != 0) {
            if (MAG == 0) {
                int low = (short)reserve;
                low = low < 30;
                if (!low) {
                    MAG = 30;
                } else {
                    MAG = RESERVE_LO;
                }
                return 0;
            }
        }
        return 1;
    }
    if (mode == 1) {
        int reserve = RESERVE_S;
        if (reserve != 0) {
            int low = (short)reserve;
            low = low < 30;
            if (!low) {
                MAG = 30;
            } else {
                MAG = RESERVE_LO;
            }
            return 0;
        }
        return 1;
    }
    {
        int thirty;
        int mag = MAG;
        if (mag < 30) {
            self = RESERVE_S;
            thirty = 30;
            if (mag < self) {
                mag = thirty - mag;
                mag = self < mag;
                if (!mag) {
                    MAG = thirty;
                } else {
                    MAG = RESERVE_LO;
                }
                return 0;
            }
        }
        return 1;
    }
}
