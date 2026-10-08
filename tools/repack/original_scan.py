"""Exact 65-byte original-run scan, with bounded memory and native C99 loops.

Payloads are concatenated deliberately: splitting a copied run across operations
or ZIP members does not evade this conservative distribution check. Hash matches
are always confirmed byte for byte. This detects copied runs, not authorship.
"""
from __future__ import annotations

import ctypes
import functools
import hashlib
import os
from pathlib import Path
import platform
import shutil
import subprocess
import tempfile

from .archive import OUTPUT_ROOT, safe_output

WINDOW = 65
DEFAULT_BATCH = 4 * 1024 * 1024
DEFAULT_READ = 4 * 1024 * 1024


@functools.lru_cache(maxsize=1)
def _native():
    source = Path(__file__).with_suffix('.c')
    compiler = shutil.which('cc')
    if compiler is None:
        raise RuntimeError('delta encoding and original-byte scans require a local C99 compiler (cc)')
    digest = hashlib.sha256(source.read_bytes() + platform.machine().encode() + platform.system().encode()).hexdigest()[:24]
    directory = safe_output(OUTPUT_ROOT / 'native-delta')
    directory.mkdir(parents=True, exist_ok=True)
    library = directory / ('scan-' + digest + '.so')
    if library.is_symlink():
        raise ValueError('native helper cache must not be a symbolic link')
    if not library.is_file():
        descriptor, temporary = tempfile.mkstemp(prefix='scan-', suffix='.so', dir=directory)
        os.close(descriptor)
        try:
            result = subprocess.run([compiler, '-std=c99', '-O3', '-fPIC', '-shared', str(source), '-o', temporary],
                                    capture_output=True, text=True)
            if result.returncode:
                raise RuntimeError('native delta helper compilation failed: ' + result.stderr[-2000:])
            os.replace(temporary, library)
        finally:
            if os.path.exists(temporary):
                os.unlink(temporary)
    lib = ctypes.CDLL(str(library))
    pointer, number = ctypes.c_void_p, ctypes.c_uint64
    for name in ('em_scan_make', 'em_match_make'):
        fn = getattr(lib, name)
        fn.argtypes, fn.restype = [ctypes.c_char_p, number], pointer
    lib.em_index_free.argtypes, lib.em_index_free.restype = [pointer], None
    for name in ('em_scan_chunk', 'em_match_next'):
        fn = getattr(lib, name)
        fn.argtypes, fn.restype = [pointer, ctypes.c_char_p, number, number, ctypes.POINTER(number)], ctypes.c_int
    lib.em_xor.argtypes, lib.em_xor.restype = [ctypes.c_char_p, ctypes.c_char_p, pointer, number], None
    return lib


def _payload_batches(payloads, batch_size):
    pending, start = bytearray(), 0
    for payload in payloads:
        if not isinstance(payload, (bytes, bytearray, memoryview)):
            raise TypeError('scan payloads must contain byte strings')
        view = memoryview(payload).cast('B')
        for offset in range(0, len(view), batch_size):
            pending.extend(view[offset:offset + batch_size])
            while len(pending) >= batch_size + WINDOW - 1:
                yield start, bytes(pending[:batch_size + WINDOW - 1])
                del pending[:batch_size]
                start += batch_size
    if len(pending) >= WINDOW:
        yield start, bytes(pending)


def scan_original_runs(original: Path, payloads, *, batch_size=DEFAULT_BATCH, read_size=DEFAULT_READ):
    """Return the first exact 65-byte match as offsets only, or None.

    Every uncompressed payload window is checked against every possible source
    offset across the complete original file. Memory is bounded by payload batch
    size; 64-byte overlap covers payload batches, source reads and member joins.
    """
    if (type(batch_size) is not int or not 65 <= batch_size <= DEFAULT_BATCH
            or type(read_size) is not int or not 65 <= read_size <= 64 * 1024 * 1024):
        raise ValueError('scan batches must be 65..4194304 bytes and reads 65..67108864 bytes')
    original = Path(original)
    size = original.stat().st_size
    if not original.is_file():
        raise ValueError('original source must be a regular file')
    for payload_start, payload in _payload_batches(payloads, batch_size):
        lib = _native()
        index = lib.em_scan_make(payload, len(payload))
        if not index:
            raise MemoryError('could not allocate bounded original-run index')
        try:
            found = (ctypes.c_uint64 * 2)()
            with original.open('rb') as source:
                for offset in range(0, size, read_size):
                    source.seek(offset)
                    block = source.read(min(read_size + WINDOW - 1, size - offset))
                    expected = min(read_size + WINDOW - 1, size - offset)
                    if len(block) != expected:
                        raise ValueError('original source changed or became truncated during scan')
                    if lib.em_scan_chunk(index, block, len(block), read_size, found):
                        return dict(original_offset=offset + found[0], payload_offset=payload_start + found[1], length=WINDOW)
                if os.fstat(source.fileno()).st_size != size:
                    raise ValueError('original source size changed during scan')
        finally:
            lib.em_index_free(index)
    return None
