#!/usr/bin/env python3
"""record_original_play.py - record a person playing the ORIGINAL game.

A take is played in a visible debugging-fork window (../pcsx2-fork), keyboard
or controller, at normal speed with sound. The fork logs the pad it applied at
every vsync. When the take ends, the same pad is replayed hidden from the same
state and the machine is compared at the take's last vsync: if EE RAM,
scratchpad, VU data and IOP RAM hash equal, the input file alone reproduces
the take, so it can be re-driven for traces, states and comparisons.

Takes chain: a take can start from the state an earlier take ended on, so a
long playthrough can be played over several sittings.

Usage (native arm64 macOS; the fork app runs x86_64 under Rosetta; use the
decomp's .venv python):

  .venv/bin/python tools/record_original_play.py play run1
  .venv/bin/python tools/record_original_play.py play run2 --after run1
  .venv/bin/python tools/record_original_play.py verify run1
  .venv/bin/python tools/record_original_play.py list

Press Enter in the terminal to end a take; do not close the game window, as
that loses the end state. The take is verified straight after, at about 3.5
times normal speed; Ctrl+C skips that, and verify runs it later.

The window is the one exception to the fork's hidden-only launch rule: the
user asked for it (2026-11-01). Everything else stays on the launcher's
scratch data: the user's slots, memory cards and settings are never touched.
A game saved inside a take goes to a scratch card that is deleted with the
session.

Output: build/human_play/<take>/ (derived from the user's disc: never
committed): input.log (the fork's 32-byte input records), rows.tbl (the pad
block and player position per game tick, for finding a first difference),
start.json, end.json, end.p2s, verify.json.
"""
import argparse
import json
import shutil
import struct
import subprocess
import sys
import threading
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import pcsx2_session as session  # noqa: E402

sys.path.insert(0, str(session.FORK_PY))
from pcsx2dbg import launcher  # noqa: E402
from pcsx2dbg.launcher import LaunchConfig, Session  # noqa: E402
from pcsx2dbg.tables import Table  # noqa: E402

OUT = session.ROOT / 'build/human_play'
TITLE = session.FORK_STATES / 'slot01_title/state.p2s'
RTC = '2026-01-01 00:00:00'
REGIONS = ['ee_ram', 'ee_scratch', 'vu0_data', 'vu1_data', 'iop_ram']
SPANS = [dict(name='pad', src='ee', addr='0x00810E70', len=32),
         dict(name='pos', src='ee', addr='0x00810350', len=12)]
SDL = {'Up': 'DPadUp', 'Right': 'DPadRight', 'Down': 'DPadDown', 'Left': 'DPadLeft', 'Triangle': 'FaceNorth',
       'Circle': 'FaceEast', 'Cross': 'FaceSouth', 'Square': 'FaceWest', 'Select': 'Back', 'Start': 'Start',
       'L1': 'LeftShoulder', 'R1': 'RightShoulder', 'L2': '+LeftTrigger', 'R2': '+RightTrigger', 'L3': 'LeftStick',
       'R3': 'RightStick', 'LUp': '-LeftY', 'LDown': '+LeftY', 'LLeft': '-LeftX', 'LRight': '+LeftX',
       'RUp': '-RightY', 'RDown': '+RightY', 'RLeft': '-RightX', 'RRight': '+RightX'}


def play_ini(take):
    """The launcher's source ini with the first controller bound beside the
    keyboard (a key may repeat in the section; the launcher's dict can't)."""
    text = LaunchConfig().ini.read_text()
    lines = ''.join(f'{pad} = SDL-0/{name}\n' for pad, name in SDL.items())
    assert '[Pad1]\n' in text, 'no [Pad1] section in the source ini'
    path = take / 'PCSX2.ini'
    path.write_text(text.replace('[Pad1]\n', '[Pad1]\n' + lines, 1))
    return path


def show(pid):
    subprocess.run(['osascript', '-e', 'tell application "System Events" to tell (first process whose unix id is '
                    f'{pid}) to set {{visible, frontmost}} to {{true, true}}'], capture_output=True, timeout=20)


def live_config(take, state, hidden):
    return LaunchConfig(iso=session.DEFAULT_ISO, statefile=state, rtc=RTC, renderer=17, present=True, unlimited=False,
                        audio_backend='Cubeb', lease_s=86400, ini=play_ini(take),
                        ini_overrides={'InputSources': {'Keyboard': 'true', 'SDL': 'true'},
                                       'SPU2/Output': {'OutputMuted': 'true' if hidden else 'false'}})


def replay_config(state):
    return LaunchConfig(iso=session.DEFAULT_ISO, statefile=state, rtc=RTC, renderer=13, unlimited=True, lease_s=86400)


def records(take):
    """The input log as (vsync, tick, buttons, lx, ly, rx, ry)."""
    data = (take / 'input.log').read_bytes()
    return [struct.unpack_from('<IIH4B', data, i) for i in range(0, len(data) - 31, 32)]


def timeline(rows):
    entries, last = [], None
    for row in rows:
        if row[2:] != last:
            entries.append([row[0] - rows[0][0], *row[2:]])
            last = row[2:]
    return entries


def arm(c):
    c.call('halt')
    c.call('set_tick_pc', pc=session.LOOP_TOP)
    c.call('input_log', on=True, path='input.log')
    c.call('table_add', name='rows', path='rows.tbl', trigger='tick', spans=SPANS)


def disarm(c, root, take, prefix=''):
    """Collect the run's logs from the instance root into the take."""
    c.call('input_log', on=False)
    c.call('table_remove', name='rows')
    for name in ('input.log', 'rows.tbl', 'rows.tbl.json'):
        shutil.copyfile(root / name, take / f'{prefix}{name}')


def machine(c):
    state = c.call('state')
    hashes = c.call('hash_regions', regions=REGIONS)
    return dict(vsync=state['vsync'], tick=state['tick'], ee_cycle=state['ee_cycle'], pc=state['pc'],
                hashes=hashes['regions'])


def play(args):
    take = OUT / args.take
    if take.exists():
        sys.exit(f'{take} already exists: choose a new take name')
    state = OUT / args.after / 'end.p2s' if args.after else TITLE
    if not state.exists():
        sys.exit(f'{state} is missing')
    take.mkdir(parents=True)
    if args.hidden:
        print('hidden check: no window, no input')
    else:
        launcher._hide = lambda pid: None
    with Session() as s:
        inst = s.launch('play', live_config(take, state, args.hidden))
        c = inst.client()
        c.wait_vm(120)
        arm(c)
        start = machine(c)
        (take / 'start.json').write_text(json.dumps(dict(start, state=str(state), after=args.after), indent=1) + '\n')
        c.call('run', **{'until': {'vsyncs': 1 << 30}, 'timeout_s': 7 * 86400, 'async': True})
        if not args.hidden:
            show(inst.pid)
            print('Playing. The game window should be in front; click it if keys do nothing.')
            print('Press Enter here to end the take.', flush=True)
        finished = threading.Event()
        if args.seconds:
            threading.Timer(args.seconds, finished.set).start()
        else:
            threading.Thread(target=lambda: (sys.stdin.readline(), finished.set()), daemon=True).start()
        t0 = time.time()
        root = Path(inst.info().get('root') or inst.scratch / 'root')
        while not finished.wait(5):
            try:
                c.call('state')         # the lease; also shows an emulator that died
            except OSError:
                sys.exit(f'The emulator closed before Enter: the take has no end state. '
                         f'The pad so far is in {take}/input.partial.log.')
            shutil.copyfile(root / 'input.log', take / 'input.partial.log')
        c.call('halt')
        end = machine(c)
        disarm(c, root, take)
        c.call('state_save', path='end.p2s', exact=True)
        shutil.copyfile(root / 'end.p2s', take / 'end.p2s')
    end['seconds'] = round(time.time() - t0, 1)
    (take / 'end.json').write_text(json.dumps(end, indent=1) + '\n')
    n = end['vsync'] - start['vsync']
    print(f'Take {args.take}: {n} vsyncs, {n / 59.94 / 60:.1f} minutes of game time.', flush=True)
    print('Checking that the pad alone reproduces it; Ctrl+C skips this.', flush=True)
    return verify(args)


def verify(args):
    take = OUT / args.take
    start, end = (json.loads((take / f).read_text()) for f in ('start.json', 'end.json'))
    rows = records(take)
    with Session() as s:
        inst = s.launch('replay', replay_config(Path(start['state'])))
        c = inst.client()
        c.wait_vm(120)
        arm(c)
        got = machine(c)
        if {k: got[k] for k in ('vsync', 'hashes')} != {k: start[k] for k in ('vsync', 'hashes')}:
            print('the replay does not start from the take\'s machine', got, start)
        c.call('input_timeline', entries=timeline(rows), index='vsync', mode='replace')
        t0 = time.time()
        c.call('run', timeout=86400 + 60, until={'vsyncs': end['vsync'] - start['vsync']}, timeout_s=86400)
        seconds = time.time() - t0
        root = Path(inst.info().get('root') or inst.scratch / 'root')
        got = machine(c)
        disarm(c, root, take, 'replay_')
    same = got['hashes'] == end['hashes'] and got['ee_cycle'] == end['ee_cycle']
    first = first_difference(take)
    report = dict(exact=same, seconds=round(seconds, 1), take=end, replay=got, first_difference=first)
    (take / 'verify.json').write_text(json.dumps(report, indent=1) + '\n')
    print(f'{args.take}: replay {"EXACT" if same else "DIFFERS"} at vsync {got["vsync"]} (replay took {seconds:.0f} s)')
    if first:
        print(f'  first differing row: {first}')
    return 0 if same else 1


def first_difference(take):
    """The first game tick whose pad block or position differs, or None."""
    a, b = Table(take / 'rows.tbl').numpy(), Table(take / 'replay_rows.tbl').numpy()
    for i in range(min(len(a), len(b))):
        for name in ('pad', 'pos'):
            if a[name][i].tobytes() != b[name][i].tobytes():
                return dict(row=i, tick=int(a['tick'][i]), vsync=int(a['vsync'][i]), field=name,
                            take=a[name][i].tobytes().hex(), replay=b[name][i].tobytes().hex())
    return None if len(a) == len(b) else dict(row=min(len(a), len(b)), field='row count', take=len(a), replay=len(b))


def listing(_args):
    for take in sorted(OUT.glob('*/end.json')):
        end = json.loads(take.read_text())
        ver = take.with_name('verify.json')
        state = json.loads(ver.read_text())['exact'] if ver.exists() else 'not verified'
        print(f'{take.parent.name}: {end["seconds"] / 60:.1f} min, replay exact: {state}')
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest='command', required=True)
    p = sub.add_parser('play')
    p.add_argument('take')
    p.add_argument('--after', metavar='TAKE', help='start from the state this take ended on')
    p.add_argument('--hidden', action='store_true', help='a check of the tool: no window, muted')
    p.add_argument('--seconds', type=float, help='end after this long instead of on Enter')
    p.set_defaults(run=play)
    p = sub.add_parser('verify')
    p.add_argument('take')
    p.set_defaults(run=verify)
    sub.add_parser('list').set_defaults(run=listing)
    args = parser.parse_args()
    return args.run(args)


if __name__ == '__main__':
    sys.exit(main())
