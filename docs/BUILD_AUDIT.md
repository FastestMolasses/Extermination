# Build audit

`tools/build_audit.py` runs headlessly on native arm64 macOS with Python 3.9+
and Git (standard library only). It reads build metadata without opening build
files, following symlinks, or deleting anything.

```sh
python3 -B tools/build_audit.py
python3 -B tools/build_audit.py --dangling --json /absolute/new-report.json
python3 -B tools/build_audit.py --root /absolute/repo/build --split-gb 0.5 \
  --recent-hours 48 --protect 'build/my-receipts' 'build/keep-*'
python3 -B tools/build_audit.py --include-defaults --root /absolute/other-repo/build
python3 -B tools/test_build_audit.py
```

The default roots are
`/Users/abe/Documents/Extermination.nosync/Extermination/build` and
`/Users/abe/Documents/Extermination.nosync/extermination-port/build`.
Repeating `--root` replaces those defaults; `--include-defaults` adds to them.
Each root's parent must be its Git checkout. Roots and report parents with
symlink components are refused (use `/private/tmp`, rather than macOS's `/tmp`).

Every top-level entry appears, largest first; directories above `--split-gb`
(default 1 GiB) expose children recursively. `D` is the detail depth, starting
at 0. Totals count **only top-level rows**, without overlapping detail.
Sizes sum regular-file `st_size`, counting hard links separately: logical
bytes, not reclaimable APFS disk space. Newest timestamps include directories,
files, and symlinks themselves. Symlinks contribute zero bytes and zero files.

Classes, in priority order:

- **PROTECTED:** startup-reference, fork_refs, s87, .pcsx2.lock, and paths matched
  by `--protect`. Patterns may include or omit the `build/` prefix, are
  case-sensitive shell patterns, and `*` can span `/`. Protection covers
  descendants and any parent that contains a protected child. Incomplete
  metadata or unavailable citation history also makes entries PROTECTED.
- **IN-USE:** any contained entry was modified within `--recent-hours` (default
  24), including future timestamps.
- **CITED:** a committed citation names this entry, a descendant, or a parent.
- **CANDIDATE:** none of those conditions applies. This is a review list for the
  lead; the tool never deletes anything.

`git ls-files` selects `CLAUDE.md`, immediate `docs/*.md`, and immediate
`tools/*.py`. Text comes from pinned `HEAD` blobs in one Git batch; working
edits, staged additions, untracked/nested files, binary files, and symlinks are
excluded. Bare `build/` references belong to the citing repo; absolute and
repo-relative paths such as `../Extermination/build/…` target the corresponding
audited root. Cross-repo citing filenames are absolute. Only filenames appear,
never source text. Extraction recognizes literal path tokens, not constructed
paths or names containing whitespace. Globs/templates cite their literal parent.

`--dangling` lists missing literal paths and their citing files. Templates,
globs, and ellipses are not reported as missing literal paths. References
below symlinks or incomplete scans are UNRESOLVED. A broken symlink itself
exists for this purpose. Other processes can change files during the scan.
Read failures give an INCOMPLETE notice and exit status 1; complete audits exit 0.

`--json FILE` is the only write: it creates a new metadata report outside all
audited roots, refuses to overwrite any existing path, and does not create
parent directories. `--json -` emits JSON alone. Reports include exact bytes,
counts, nanosecond mtimes, classes, citations, totals, and requested dangling
paths. There is one `os.scandir` visit per directory and no per-file subprocesses.
Reads may update filesystem access times; inputs are never edited.

Example excerpt (invented paths, timestamps, and numbers):

```text
CLASS              BYTES      FILES  NEWEST (UTC)               LINK  D  PATH
PROTECTED      2000000000        120  2026-01-01T00:00:00+00:00  no    0  "build/reference-example"
CANDIDATE       700000000         40  2026-01-01T00:00:00+00:00  no    0  "build/old-example"
CITED           100000000          5  2026-01-01T00:00:00+00:00  no    0  "build/receipt-example"
  cited: "docs/EXAMPLE.md"
TOTALS (top-level entries only; D > 0 rows are overlapping detail)
PROTECTED      2000000000 bytes        120 files      1 entries
IN-USE                 0 bytes          0 files      0 entries
CITED          100000000 bytes          5 files      1 entries
CANDIDATE       700000000 bytes         40 files      1 entries
REPO TOTAL 2800000000 bytes; 165 files
```

Tests cover metadata, classes, citations, dangling paths, one-pass traversal,
unchanged inputs, and symlink/report safety. The deliberate-mistake control
substitutes a scanner that follows symlinks and requires the size test to fail.
