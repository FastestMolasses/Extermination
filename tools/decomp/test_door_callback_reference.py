#!/usr/bin/env python3
"""Check readable BC350 state changes and arguments against the original ELF.

The caller executes its original instructions. Each callee is an explicit
boundary with the same deterministic result on both sides. This test needs
the adjacent native project's instruction oracle and the user's original ELF;
it contains no game instruction bytes or disassembly.
"""
import ctypes as C
import hashlib
import itertools
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'extermination-port/tools'))
from test_door_original_reference import DoorOracle, ACTOR  # noqa: E402
from test_interaction_scan_reference import ELF_SHA  # noqa: E402


BRIDGE = r'''
#include <stdint.h>
#include <string.h>

unsigned char D_00810700;
unsigned char D_00810841[256];
int D_0024DBC0[1];
static unsigned char *actor;
static int worker_result, initialize_result;
static int events[32], event_count, bad_argument;

static void record(int event, unsigned char *self) {
    if (self != actor) bad_argument = 1;
    events[event_count++] = event;
}
static void block(unsigned char *value) {
    if (value != actor + 0x1f0) bad_argument = 1;
}
void func_001BBDA0(unsigned char *self) {
    record(1, self);
    if (self == actor) self[4] = initialize_result ? 1 : 3;
}
int func_001BBE40(unsigned char *self, unsigned char *script, int locked) {
    record(2 + locked, self); block(script); return worker_result;
}
int func_001BC0E0(unsigned char *self, unsigned char *script) {
    record(4, self); block(script); return worker_result;
}
void func_001BA1A0(unsigned char *script, int *entry) {
    block(script);
    if (entry != D_0024DBC0) bad_argument = 1;
    events[event_count++] = 5;
}
void func_001BC240(unsigned char *self, unsigned char *script) {
    record(6, self); block(script);
}
int func_001BC290(unsigned char *self, unsigned char *script) {
    record(7, self); block(script); return worker_result;
}
void func_001BC300(unsigned char *self) { record(8, self); }
void func_001AFC10(unsigned char *self) { record(9, self); }
void func_001BC350(unsigned char *self);

int run(unsigned char *self, int done, int initialized, int unlocked,
        int *output) {
    actor = self; worker_result = done; initialize_result = initialized;
    event_count = bad_argument = 0;
    D_00810700 = 11;
    memset(D_00810841, 0, sizeof(D_00810841));
    D_00810841[11] = unlocked ? 1 << *(short *)(self + 0x34) : 0;
    func_001BC350(self);
    memcpy(output, events, event_count * sizeof(int));
    return bad_argument ? -1 : event_count;
}
'''


def compare(elf, library, lifecycle, phase, subtype, unlocked, done,
            initialized, bit_index):
    original = DoorOracle(elf)
    actor = (C.c_uint8 * 0x220)()
    actor[0], actor[3], actor[4], actor[5], actor[11] = 2, subtype, lifecycle, phase, 4
    actor[0x34] = bit_index
    for index, value in enumerate(actor):
        original.save(ACTOR + index, value, 1)
    original.save(0x810700, 11, 1)
    original.save(0x81084c, (1 << bit_index) if unlocked else 0, 1)
    expected = []

    def event(identifier, script=False, result=False):
        def call(o):
            assert o.r[4] == ACTOR
            if script:
                assert o.r[5] == ACTOR + 0x1f0
            expected.append(identifier)
            if result:
                o.r[2] = done
        return call

    def initialize(o):
        event(1)(o)
        o.save(ACTOR + 4, 1 if initialized else 3, 1)

    def kickoff(o):
        event(2 + o.r[6], script=True, result=True)(o)

    def start(o):
        assert o.r[4:6] == [ACTOR + 0x1f0, 0x24dbc0]
        expected.append(5)

    original.calls.update({
        0x1bbda0: initialize, 0x1bbe40: kickoff,
        0x1bc0e0: event(4, script=True, result=True), 0x1ba1a0: start,
        0x1bc240: event(6, script=True),
        0x1bc290: event(7, script=True, result=True),
        0x1bc300: event(8), 0x1afc10: event(9),
    })
    original.run(0x1bc350, (ACTOR,))
    output = (C.c_int * 32)()
    count = library.run(actor, done, initialized, unlocked, output)
    assert count >= 0, 'readable C forwarded the wrong worker argument'
    assert list(output[:count]) == expected, (lifecycle, phase, list(output[:count]), expected)
    assert bytes(actor) == bytes(original.load(ACTOR + i, 1) for i in range(len(actor)))


def main():
    elf = (ROOT / 'config/SCUS_971.12').read_bytes()
    assert hashlib.sha256(elf).hexdigest() == ELF_SHA
    build = ROOT / 'build/door_match'
    build.mkdir(parents=True, exist_ok=True)
    bridge = build / 'callback_bridge.c'
    bridge.write_text(BRIDGE)
    output = build / 'callback.dylib'
    subprocess.run(['cc', '-std=c11', '-O2', '-shared', '-fPIC', '-Wall',
                    '-Wextra', '-Werror', str(bridge),
                    str(ROOT / 'src/func_001BC350.c'), '-o', str(output)], check=True)
    library = C.CDLL(str(output))
    library.run.argtypes = [C.POINTER(C.c_uint8), C.c_int, C.c_int,
                            C.c_int, C.POINTER(C.c_int)]
    count = 0
    for case in itertools.product((0, 1, 2, 3, 4, 255),
                                  (*range(7), 255), (3, 0x15),
                                  (0, 1), (0, 1), (0, 1), (0, 7)):
        compare(elf, library, *case)
        count += 1
    report = dict(status='PASS', cases=count,
                  source_sha256=hashlib.sha256((ROOT / 'src/func_001BC350.c').read_bytes()).hexdigest(),
                  limits='Original caller instructions; callees are explicit deterministic boundaries.')
    (build / 'callback_result.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'Readable door callback: PASS {count} original state/argument cases')


if __name__ == '__main__':
    main()
