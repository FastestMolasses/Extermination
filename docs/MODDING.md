# Modding Extermination — From Zero to "I Changed Something"

A practical walkthrough for programmers who are new to PS2 internals and
matching decomp. After one sitting you will have:

- the toolchain running,
- the dev loop working (edit C → see it in PCSX2), and
- made your first actual change to the game.

No PS2 experience required. Basic C and command-line comfort assumed.

---

## 1. The Big Picture

Before touching a file, it's worth understanding what this project produces
and why that matters for modding.

### What "matching decompilation" means

The Extermination boot executable (`SCUS_971.12`) is a stripped binary with
no debug symbols — just raw machine code. Every function in the game is
currently a string of bytes at some VRAM address.

Matching decomp means: for each function, we write C source that the
*original compiler* (Metrowerks CodeWarrior `mwccmips` 2.3.1.01) compiles
into **byte-for-byte identical machine code**. Not "similar" — identical.
That's what "100% match" means in the objdiff tool.

When a function is matched and lives in `src/`, you own its logic. You can
change the C and the game's behavior changes.

### What the pipeline produces

```
src/func_XXXXXXXX.c      ← you edit this
         │
         ▼
tools/decomp/build.py build   ← mwccmips compiles it inside the container
         │
         ▼
elf/SCUS_971.12.elf      ← mwldmips links all 3014 functions into one ELF
         │
         ▼
tools/decomp/repack_iso.py    ← strips and patches it into the ISO
         │
         ▼
Extermination-rebuilt.iso     ← load this in PCSX2
```

Functions that aren't yet decompiled still run from their original bytes
(assembled by splat into `build/filler/`). So the game is always runnable —
it's a mix of your decompiled code and the original binary, with more
shifting to your code over time.

### The current state

As of 2026-05-23: **1008 of 3014 functions** (~33%) are in `src/` and
matched at 100%. The linked ELF is byte-identical to the original and boots
in PCSX2. The dev loop is open — your edits land in the running game.

---

## 2. Setup Checklist

You need these things set up once. If you already followed
`textbook/12-how-to-contribute.md`, most of this is done.

### 2a. Apple `container` CLI and the toolchain image

The Metrowerks compiler is a 32-bit Windows PE that can't run under Rosetta
on Apple Silicon. It runs inside a Linux container under `qemu-i386 + wibo`.

```bash
# macOS-arm64 — check if container CLI is available
container --version
```

If it's missing, install it from [Apple's open-source release](https://github.com/apple/container).

Build the image (takes ~2 minutes the first time, cached after):

```bash
# macOS-arm64
container build -t exterm-toolchain -f docker/Dockerfile docker/
```

### 2b. The mwccmips compiler

The compiler is proprietary and never committed. Obtain `mwccmips.exe` from
`Adubbz/DCDecomp` (that project uses the identical binary — CodeWarrior
2.3.1.01). Put it here:

```
tools/mwccps2/mwccmips.exe
```

### 2c. The 32-bit wibo

wibo lets Linux run 32-bit Windows EXEs. A cross-built arm64 version is
produced by the setup script:

```bash
# macOS-arm64
container run --rm -v "$PWD":/work -w /work exterm-toolchain \
    sh docker/build-wibo.sh
# Output: tools/bin/wibo32
```

### 2d. Python environment and splat

```bash
# macOS-arm64
python3 -m venv .venv
.venv/bin/pip install splat==0.40.1 pycdlib
```

### 2e. Run splat (one-time setup)

splat reads the boot ELF and writes per-function assembly files to
`build/`. You need the boot ELF at `config/SCUS_971.12` (extract it from
your own disc; SHA1 `2cca045edce7db2af2c865bd80b46f79661608f0`).

```bash
# macOS-arm64
.venv/bin/python tools/decomp/build.py setup
```

This writes ~3014 `.s` files under `build/asm/` and generates `objdiff.json`.
These are disc-derived and git-ignored — you re-generate them from your own
copy.

### 2f. Build the base objects once

Before you edit anything, do a full build to confirm the container is
working:

```bash
# macOS-arm64 (runs inside the container automatically)
.venv/bin/python tools/decomp/build.py build
```

Expect this to take 30–60 seconds for 1000+ files. Subsequent incremental
builds are faster.

### 2g. Prepare the working ISO

The repack script patches the ELF into an ISO in place. Keep your original
disc image untouched; make a working copy:

```bash
# macOS-arm64
cp /path/to/Extermination.iso Extermination-rebuilt.iso
```

`repack_iso.py` defaults to `Extermination-rebuilt.iso` at the repo root.
Don't commit this file — it's git-ignored.

### 2h. PCSX2

Install PCSX2 (the native Apple Silicon build from [pcsx2.net](https://pcsx2.net)).
Point it at `Extermination-rebuilt.iso` in the game list or via File → Boot ISO.

---

## 3. The Dev Loop

Once setup is done, every change follows the same four steps. This is the
loop you'll run dozens of times.

```bash
# Step 1 — edit your function
$EDITOR src/func_XXXXXXXX.c

# Step 2 — compile (inside container automatically)
.venv/bin/python tools/decomp/build.py build

# Step 3 — link
.venv/bin/python tools/decomp/link.py

# Step 4 — repack the ISO
.venv/bin/python tools/decomp/repack_iso.py

# Step 5 — load in PCSX2
# (open PCSX2 and boot Extermination-rebuilt.iso)
```

**Timing:** steps 2–4 together take roughly 30–60 seconds with the current
function count. Step 2 is the slowest; it only recompiles files whose source
changed. Step 4 is nearly instant (it just patches a few sectors).

### One-liner for step 2+3+4

```bash
.venv/bin/python tools/decomp/build.py build && \
.venv/bin/python tools/decomp/link.py && \
.venv/bin/python tools/decomp/repack_iso.py
```

### Iterating faster with objdiff

If you're in a matching loop (trying to get a function to 100%), you don't
need to re-link and repack for every attempt. Use objdiff to iterate:

```bash
# Compile just one function (fast — one file)
.venv/bin/python tools/decomp/build.py --single-file build/obj/func_XXXXXXXX.o

# Or open the objdiff GUI — it calls --single-file automatically when you save
open tools/bin/objdiff  # if installed there; or wherever you placed it
```

Only run link + repack when you're happy with the match and want to test
in-game behavior.

---

## 4. Your First Mod — A Worked Example

Let's make a real change. We'll use `func_001D1C10`, a clean 100%-matched
function that calls a chain of other functions. It's a good first target
because it's readable C with no assembly tricks.

### 4a. Read the current source

```c
// src/func_001D1C10.c
extern void func_001D1AE0(void);
extern int D_007635C0;
extern void func_001CB5C0(int *);
extern void func_001F0310(void);
extern void func_001D2830(int, int);

void func_001D1C10(void) {
    func_001D1AE0();
    func_001CB5C0(&D_007635C0);
    func_001F0310();
    func_001D2830(4, 1);
}
```

The last call is `func_001D2830(4, 1)` — two integer arguments. We don't
yet know what this function does (it's unnamed), but the integer arguments
are constants we can change.

> **Note:** changing constants in an unidentified function is a low-risk
> way to explore. The worst realistic outcome is the game crashes or
> glitches — PCSX2 makes that safe to recover from.

### 4b. Make the change

Open the file and change the arguments:

```c
void func_001D1C10(void) {
    func_001D1AE0();
    func_001CB5C0(&D_007635C0);
    func_001F0310();
    func_001D2830(0, 0);  // changed from (4, 1)
}
```

### 4c. Build, link, and repack

```bash
.venv/bin/python tools/decomp/build.py build && \
.venv/bin/python tools/decomp/link.py && \
.venv/bin/python tools/decomp/repack_iso.py
```

### 4d. Boot in PCSX2

Open `Extermination-rebuilt.iso` in PCSX2 and play until the relevant game
state triggers. Watch for any behavioral difference.

> **Speculation:** `func_001D2830` takes two integers and is called from
> what looks like an initialization sequence. Changing (4, 1) to (0, 0)
> may suppress some initialization behavior, produce a visual glitch, or do
> nothing observable — we don't know without running it. That's the nature
> of modding an unidentified function. If nothing changes visibly, try a
> different function with a more identifiable role (see section 6 below).

### 4e. A safer first example — a trivial getter

If you want a completely safe change with an immediately visible effect
(once you identify the right data), try one of the constant-return functions.
Here is `func_00100268`, which returns a pointer to a global array:

```c
// src/func_00100268.c
extern int D_00241010[];

int * func_00100268(void) {
    return D_00241010;
}
```

And here's `func_001DB240`, a simple setter that writes a byte to a global:

```c
// src/func_001DB240.c
extern char D_0081C011;

void func_001DB240(int a0) {
    D_0081C011 = a0;
}
```

Or the zeroing function `func_001DB800`, which clears four bytes in a row:

```c
// src/func_001DB800.c
extern char D_0081C040;
extern char D_0081C041;
extern char D_0081C042;
extern char D_0081C043;

void func_001DB800(void) {
    D_0081C043 = 0;
    D_0081C042 = 0;
    D_0081C041 = 0;
    D_0081C040 = 0;
}
```

You could change any of those `0` values to `1` (or anything else) and
observe what changes. Tracking down which game system lives at `D_0081C040`
is the detective work — see section 6.

---

## 5. Reading the Matched Functions

You'll quickly notice that many functions in `src/` look like this:

```c
// src/func_001008C0.c
asm void func_001008C0(void) {
    .word 0x00073c00
    .word 0x00094c00
    .word 0x00073c03
    // ... 60+ more .word lines ...
    .word 0x03e00008
    .word 0x24020006
}
```

That's **raw machine code as hex literals**, wrapped in an `asm void` block.
It's byte-correct — this compiles to exactly the right bytes — but it tells
you nothing about what the function does.

This happens when:
- The function uses EE-specific instructions (`lqc2`, `sqc2`, VU0 ops) that
  the inline assembler handles at the raw-encoding level.
- The function has dead code, complex branch delay slots, or register
  allocation that can't be reproduced from C or labeled asm, so the
  `.word` form is the only way to get a 100% match.

### What you can do with a `.word` function

**Option A — read the MIPS to understand it.** Each `.word` decodes to a
MIPS instruction. PCSX2's debugger will show you the live disassembly at the
function's VRAM address. `textbook/03-mips-assembly-primer.md` is the
starting point if you're new to MIPS.

**Option B — decompile it to C first.** Paste the `.word` values into a
MIPS disassembler (or let splat show you — the original `.s` file is in
`build/asm/matchings/main/code/func_001008C0.s`) and reverse-engineer the
logic to C. Then compile your C and use objdiff to verify a 100% match.
Once it matches, the function is readable and moddable. This is the core
decomp workflow — see `textbook/12-how-to-contribute.md` for the full loop.

**Option C — skip it.** If you want to mod behavior and a function is
opaque `.word` soup, find a different function in the call chain that is
already readable C.

### Syscall stubs are different

Functions like `EndOfHeap`, `SetGsCrt`, and the 270+ others in `src/` that
look like:

```c
void EndOfHeap(void) {
    asm { addiu $v1, $zero, 64; syscall 0; };
}
```

...are **EE kernel syscall stubs**. They invoke the PS2 OS kernel. You
generally don't want to change these — the syscall number is fixed by Sony's
kernel ABI. They're in `src/` because they're matched, not because they're
modding targets.

---

## 6. Finding the Right Function to Mod

"I want to change player speed" or "I want to disable that effect" — the
challenge is mapping your intention to a VRAM address, and from there to a
`src/` file. Here's the workflow.

### Path 1 — You know the address

If you found the address in a cheat code database, an old FAQ, or a previous
session in PCSX2's debugger:

```
VRAM 0x001885B0  →  src/func_001885B0.c
```

Simple. The file is `src/func_<VRAM in hex, no 0x prefix>.c`. If that file
exists in `src/`, it's matched and you can edit it. If it doesn't exist
there, it's still running from original bytes in `build/filler/` — see
"Functions not yet in src/" below.

### Path 2 — Find it in Ghidra

Ghidra is your primary RE tool for unnamed functions.

1. Open the original ELF (`config/SCUS_971.12`) in Ghidra. The ELF is at
   vaddr `0x00100000`; Ghidra auto-detects it as MIPS LE.
2. Navigate to something you recognize — e.g. search memory for a known
   string, or set a breakpoint in PCSX2's debugger and note the PC.
3. Ghidra's "Functions" list shows auto-detected functions. Find the one at
   the address you care about.
4. Read the decompiled output (Window → Decompile). Even though Ghidra's
   decompiler won't match mwcc exactly, it gives you the logic.
5. The function's VRAM (e.g. `0x001885B0`) maps directly to `src/func_001885B0.c`.

### Path 3 — Use PCSX2's debugger live

1. Boot `Extermination-rebuilt.iso` in PCSX2.
2. Open Debug → Show Debugger (if your PCSX2 build includes it — the dev
   builds do; check the PCSX2 docs for your version).
3. Navigate to the R5900 view. Set a breakpoint on a function by address.
4. Trigger the in-game action; the debugger halts at that PC.
5. Read the VRAM from the PC register → `src/func_<VRAM>.c`.

### Path 4 — Search `config/symbol_addrs.txt`

Named functions (all 270+ EE syscall stubs, plus any you've named yourself)
are in `config/symbol_addrs.txt`:

```
SetGsCrt = 0x0010B420;  // type:func
Exit     = 0x0010B440;  // type:func
```

If you're looking for a known SDK function (memory allocation, pad reading,
etc.), search here first.

### Functions not yet in `src/`

If the function you want to mod lives in `build/filler/` (i.e. there's no
`src/func_XXXXXXXX.c`), it's running from its original bytes and you can't
easily change it without first decompiling it. The path is:

1. Look at `build/asm/matchings/main/code/func_XXXXXXXX.s` — splat's
   disassembly of the original.
2. Write `src/func_XXXXXXXX.c` — your C guess.
3. Run the matching loop until objdiff shows 100%.
4. Now the function is yours.

This is the main work of the decomp project. For a complex function it may
take hours; for a simple one, minutes. `textbook/12-how-to-contribute.md`
walks through the full loop.

---

## 7. Common Pitfalls

### "I edited C but the change doesn't appear in PCSX2"

**Most likely cause:** you forgot to repack the ISO, or PCSX2 is using a
cached game entry.

Checklist:
1. Did you run `build.py build`? Check for errors.
2. Did you run `link.py`? It must succeed (exit 0).
3. Did you run `repack_iso.py`? Confirm it prints something like
   "patched SCUS_971.12 into Extermination-rebuilt.iso."
4. In PCSX2: close the game, then re-open the ISO. PCSX2 doesn't hot-reload.
5. Confirm PCSX2 is loading `Extermination-rebuilt.iso`, not your original.

### "Build fails after my edit"

mwcc's error messages appear in the `container run` output. Common issues:

| Error | Fix |
|-------|-----|
| `illegal token: $t0` | Use numeric registers `$8`–`$15` instead of `$t0`–`$t7` in inline asm |
| `undeclared identifier` | Add an `extern` declaration at the top of the file |
| `function must return a value` | Change `void` to `int` and add `return ...` |
| `expression must be lvalue` | Don't assign to a cast — assign to a typed pointer |
| No error but wrong bytes | That's a match issue — use objdiff to compare |

If the container exits with "image not found," rebuild the image:
```bash
container build -t exterm-toolchain -f docker/Dockerfile docker/
```

### "Function is in `src/` but it's all `.word`"

You can still call that function from a different context, or read the PCSX2
debugger to observe its inputs/outputs. But to *change what it does*, you
need to replace the `.word` block with real C and bring it to a 100% match
first. Skip to a different function that's already readable C if you want a
quick win.

### "I want to mod something that's not in `src/`"

That function is in `build/filler/` — it's running from the original
assembled bytes. To make it moddable:
1. Find the `.s` file at `build/asm/matchings/main/code/func_XXXXXXXX.s`.
2. Write `src/func_XXXXXXXX.c`.
3. Iterate with objdiff until 100%.
4. Now you can change the behavior in C.

### "link.py fails"

Most link failures are caused by a missing filler object. Try:
```bash
.venv/bin/python tools/decomp/link.py --no-fill  # skips fill_unmatched
```

If that succeeds, run fill_unmatched explicitly:
```bash
# inside the container
container run --rm -v "$PWD":/work -w /work exterm-toolchain \
    python3 tools/decomp/fill_unmatched.py
```

Look for the failing object name in the linker error and check whether the
corresponding `.s` file exists in `build/asm/`.

### "The game crashes immediately after my change"

That's expected for some experiments — you're modifying a running game
binary. PCSX2 sandboxes it. Hard reset the emulator (or close and reopen),
revert your change, and try something less drastic. The safest experiments
are: changing integer constants, changing the order of function calls,
removing a call entirely (replace with a comment, recompile).

---

## 8. What's Next

### Adding more matched functions

The biggest leverage you have for expanding what's moddable is matching
more functions. Every function you decompile to C is one more thing you can
change in a readable way.

- Start with `textbook/12-how-to-contribute.md` for the full loop.
- `docs/PROGRESS.md` lists which matching techniques work and which don't
  (mwcc quirks, register allocation patterns, etc.).
- Look for small functions in `build/asm/matchings/main/code/` — sort by
  `.s` file size and pick the shortest ones that aren't already in `src/`.

### Decompiling a `.word`-asm match to readable C

For any function that's currently `.word`-only:

1. Open the splat disassembly: `build/asm/matchings/main/code/func_XXXXXXXX.s`
2. Read `textbook/03-mips-assembly-primer.md` to interpret the instructions.
3. Write C that expresses the same logic.
4. Run `build.py --single-file build/obj/func_XXXXXXXX.o` and check objdiff.
5. Iterate. Common hurdles: `textbook/11-case-study-the-hard-ones.md`.

### The ~2000 still in filler

About 2000 functions still run from their original bytes in `build/filler/`.
The overall progress arc is:

- ~33% in `src/` → 100% byte match (where we are now)
- Growing `src/` function by function, pattern by pattern
- At some threshold (~60–70%), the game's major systems (rendering, physics,
  input, audio) will be readable C — that's when mods become much more
  powerful because you'll understand what you're changing
- Eventually: full source, native port, real modding foundation

It's a multi-year effort. But every function matters, and the toolchain is
already working end-to-end.

### Asset modding (independent of code decomp)

Track B (asset tools) is largely complete. If you want to mod content
rather than code, see `docs/FINDINGS.md`:

- `tools/decode_sound.py` — replace SFX or music (VAG ADPCM)
- `tools/extract_textures.py` / `extract_subtextures.py` — read textures
- `tools/extract_models.py` — read geometry (OBJ export)
- Repacking assets back into `DATA.DAT`/`INDEX.IDX` is on the roadmap but
  not yet implemented (see `docs/PROGRESS.md` roadmap item 3)

### Pointing PCSX2 at a mod build automatically

For a tighter loop, PCSX2 supports launching from the command line:

```bash
# macOS — adjust path to your PCSX2.app
/Applications/PCSX2.app/Contents/MacOS/PCSX2 \
    --fullscreen --nogui \
    "$PWD/Extermination-rebuilt.iso"
```

You can wire this into your build chain so step 4 of the dev loop
auto-relaunches the game.

---

## Appendix A — Quick Reference

### Environments at a glance

| Task | Where it runs |
|------|--------------|
| Edit C source | macOS-arm64 (your editor) |
| `build.py setup` (splat) | macOS-arm64 |
| `build.py build` (mwccmips) | container (automatically) |
| `link.py` (mwldmips) | container (automatically) |
| `repack_iso.py` | macOS-arm64 |
| objdiff GUI | macOS-arm64 (native arm64 binary) |
| PCSX2 | macOS-arm64 (native Apple Silicon build) |

### Key paths

| Path | Contents |
|------|---------|
| `src/` | Decompiled C (edit these) |
| `build/asm/matchings/main/code/` | splat disassembly (read-only, gitignored) |
| `build/obj/` | Compiled objects from `src/` (gitignored) |
| `build/filler/` | Assembled originals for unmatched functions (gitignored) |
| `elf/SCUS_971.12.elf` | Rebuilt ELF (gitignored) |
| `config/symbol_addrs.txt` | Named symbols (committed) |
| `config/SCUS_971.12` | Boot ELF extracted from your disc (gitignored) |
| `Extermination-rebuilt.iso` | Working ISO (gitignored) |

### Key docs

| Doc | What it covers |
|-----|---------------|
| `docs/PROGRESS.md` | Current status, matched functions, mwcc patterns |
| `docs/FINDINGS.md` | Asset format reference |
| `docs/LINKER.md` | mwldmips pipeline detail |
| `textbook/03-mips-assembly-primer.md` | Reading MIPS assembly |
| `textbook/12-how-to-contribute.md` | Full matching workflow |
| `textbook/13-glossary.md` | PS2/decomp jargon |

### The full dev loop, copy-paste ready

```bash
# Edit your function
$EDITOR src/func_XXXXXXXX.c

# Build + link + repack
.venv/bin/python tools/decomp/build.py build && \
.venv/bin/python tools/decomp/link.py && \
.venv/bin/python tools/decomp/repack_iso.py

# (Re)open Extermination-rebuilt.iso in PCSX2
```

---

_Last updated: 2026-05-23. Reflects 1008 matched functions, 100% byte identity._
