#!/usr/bin/env python3
"""Read-only build metadata audit; Python standard library, native macOS arm64."""

import argparse
from bisect import bisect_right
from collections import defaultdict
from dataclasses import dataclass, field
from datetime import datetime, timezone
import errno
import fnmatch
import hashlib
import json
import math
import os
from pathlib import Path
import re
import stat
import subprocess
import sys
import time


DEFAULT_ROOTS = (
    "/Users/abe/Documents/Extermination.nosync/Extermination/build",
    "/Users/abe/Documents/Extermination.nosync/extermination-port/build",
)
PROTECTED = ("build/startup-reference", "build/fork_refs", "build/s87",
             "build/.pcsx2.lock")
CLASSES = ("PROTECTED", "IN-USE", "CITED", "CANDIDATE")
DANGLING_CLASSES = ("removed and recorded", "moved", "another checkout", "missing")
# Stop at prose / code delimiters. Templates and globs cite their literal parent
# conservatively, but are not literal missing paths for --dangling.
REFERENCE = re.compile(r"(?<![\w])build/[^\s`'\"<>{}\[\](),;:|=#\\]+")
PATH_DELIMITERS = frozenset("`'\"<>{}[](),;:|=\\")
DIRECTORY_FLAGS = os.O_RDONLY | os.O_DIRECTORY | os.O_NOFOLLOW


@dataclass
class Node:
    path: str
    kind: str
    size_bytes: int = 0
    file_count: int = 0
    newest_mtime_ns: int = 0
    contains_symlinks: bool = False
    complete: bool = True
    children: list = field(default_factory=list)
    citations: set = field(default_factory=set)
    classification: str = ""
    protection_reasons: set = field(default_factory=set)
    identity: tuple = ()


def absolute(path):
    """Normalize lexically only: resolving symlinks would violate the audit."""
    return os.path.abspath(os.path.expanduser(os.fspath(path)))


def open_directory(path):
    """Open each absolute path component without following symlinks."""
    fd = os.open("/", DIRECTORY_FLAGS)
    try:
        for part in Path(absolute(path)).parts[1:]:
            next_fd = os.open(part, DIRECTORY_FLAGS, dir_fd=fd)
            os.close(fd)
            fd = next_fd
        return fd
    except BaseException:
        os.close(fd)
        raise


def identity(info):
    return (info.st_dev, info.st_ino, info.st_size, info.st_mtime_ns,
            info.st_ctime_ns, stat.S_IFMT(info.st_mode))


def scan(root):
    """One scandir per directory, metadata only; fd-relative descent is race safe."""
    errors = []
    nodes = {}

    def problem(node, path, error):
        node.complete = False
        errors.append({"path": path, "operation": "metadata", "errno": error.errno})

    def location(node):
        return str(Path(root).joinpath(*node.path.split("/")[1:]))

    root_node = Node("build", "directory")
    try:
        root_fd = open_directory(root)
        root_node.newest_mtime_ns = os.fstat(root_fd).st_mtime_ns
    except OSError as error:
        problem(root_node, root, error)
        return root_node, nodes, errors

    # Iterative depth-first walk: no Python recursion limit, no file data reads.
    stack = []

    def enter(node, fd):
        try:
            iterator = os.scandir(fd)
        except OSError as error:
            problem(node, location(node), error)
            os.close(fd)
            return False
        stack.append((node, fd, iterator))
        return True

    enter(root_node, root_fd)
    try:
        while stack:
            parent, fd, iterator = stack[-1]
            try:
                entry = next(iterator)
            except (StopIteration, OSError) as error:
                if isinstance(error, OSError):
                    problem(parent, location(parent), error)
                iterator.close()
                os.close(fd)
                stack.pop()
                if stack:
                    owner = stack[-1][0]
                    owner.size_bytes += parent.size_bytes
                    owner.file_count += parent.file_count
                    owner.newest_mtime_ns = max(owner.newest_mtime_ns,
                                               parent.newest_mtime_ns)
                    owner.contains_symlinks |= parent.contains_symlinks
                    owner.complete &= parent.complete
                continue
            node = Node(parent.path + "/" + entry.name, "unknown")
            parent.children.append(node)
            nodes[node.path] = node
            try:
                info = entry.stat(follow_symlinks=False)
                node.newest_mtime_ns = info.st_mtime_ns
                if stat.S_ISLNK(info.st_mode):
                    node.kind = "symlink"
                    node.contains_symlinks = True
                elif stat.S_ISREG(info.st_mode):
                    node.kind = "file"
                    node.size_bytes = info.st_size
                    node.file_count = 1
                    node.identity = identity(info)
                elif stat.S_ISDIR(info.st_mode):
                    node.kind = "directory"
                    child_fd = os.open(entry.name, DIRECTORY_FLAGS, dir_fd=fd)
                    current = os.fstat(child_fd)
                    if (info.st_dev, info.st_ino) != (current.st_dev, current.st_ino):
                        os.close(child_fd)
                        raise OSError(11, "directory changed")
                    if enter(node, child_fd):
                        continue
                else:
                    node.kind = "special"
            except OSError as error:
                problem(node, location(node), error)
            parent.size_bytes += node.size_bytes
            parent.file_count += node.file_count
            parent.newest_mtime_ns = max(parent.newest_mtime_ns, node.newest_mtime_ns)
            parent.contains_symlinks |= node.contains_symlinks
            parent.complete &= node.complete
    finally:
        for _, fd, iterator in reversed(stack):
            iterator.close()
            os.close(fd)
    return root_node, nodes, errors


def git(repo, *args, input_data=None):
    """Never surface Git's stdout/stderr on failure (it may contain source text)."""
    result = subprocess.run(
        ["git", "--no-optional-locks", "-C", repo, *args],
        input=input_data, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        env={**os.environ, "GIT_OPTIONAL_LOCKS": "0", "GIT_NO_LAZY_FETCH": "1"},
    )
    if result.returncode:
        raise ValueError("cannot read committed citation files")
    return result.stdout


def citation_file(path):
    parts = path.split("/")
    return path == "CLAUDE.md" or (
        len(parts) == 2 and
        ((parts[0] == "docs" and parts[1].endswith(".md")) or
         (parts[0] == "tools" and parts[1].endswith(".py"))))


def extract_references(text):
    for match in REFERENCE.finditer(text):
        raw = match.group().rstrip(".!/")
        start = match.start()
        while start and not (text[start - 1].isspace() or
                             text[start - 1] in PATH_DELIMITERS):
            start -= 1
        prefix = text[start:match.start()]
        if not prefix.endswith("/"):
            prefix = ""
        literal = True
        # "build/foo/..." and "build/foo/<task>" cite foo, not a literal ellipsis.
        if match.group().endswith("/..."):
            literal = False
        wildcard = re.search(r"[*?$]", raw)
        if wildcard:
            raw = raw[:wildcard.start()].rsplit("/", 1)[0]
            literal = False
        if match.end() < len(text) and text[match.end()] in "<{[":
            literal = False
        parts = raw.split("/")
        if len(parts) > 1 and all(p and p not in (".", "..") for p in parts):
            yield prefix, raw, literal, match.start()


def removal_context(text):
    """Return line offsets and lines covered by an affirmative removal note.

    A section is the nearest ATX Markdown heading, up to the next heading.
    Unheaded text uses paragraphs only. No source text leaves this function.
    """
    lines = text.splitlines(keepends=True)
    offsets, offset = [], 0
    for line in lines:
        offsets.append(offset)
        offset += len(line)
    recorded = set()

    def mark(start, end):
        prose = REFERENCE.sub("", "".join(lines[start:end]))
        for match in re.finditer(r"\b(?:removed|deleted|purged)\b", prose, re.I):
            before = re.split(r"[.!?;\n]", prose[:match.start()])[-1]
            if re.search(r"\b(?:not|never|no|cannot|can't|don't|didn't|wasn't|weren't|"
                         r"hasn't|haven't|isn't|aren't|will|would|should|could|can|may|"
                         r"must|if|unless)\b|\bto be\s*$", before, re.I):
                continue
            recorded.update(range(start + 1, end + 1))
            break

    paragraph, section = 0, None
    for index, line in enumerate(lines):
        if re.match(r"^ {0,3}#{1,6}\s+", line):
            mark(paragraph, index)
            paragraph = index
            if section is not None:
                mark(section, index)
            section = index
        elif not line.strip():
            mark(paragraph, index)
            paragraph = index + 1
    mark(paragraph, len(lines))
    if section is not None:
        mark(section, len(lines))
    return offsets, recorded


def read_citations(repo, roots):
    """ls-files selects tracked names; one cat-file batch reads pinned HEAD blobs."""
    # Do not let Git enter a checkout through a symlink component either.
    repo_fd = open_directory(repo)
    os.close(repo_fd)
    tracked = set(git(repo, "ls-files", "-z", "--cached").split(b"\0"))
    revision = git(repo, "rev-parse", "--verify", "HEAD").strip().decode("ascii")
    tree = git(repo, "ls-tree", "-rz", "--full-tree", revision,
               "--", "CLAUDE.md", "docs", "tools")
    blobs = []
    for record in tree.split(b"\0"):
        if not record:
            continue
        metadata, name = record.split(b"\t", 1)
        mode, kind, oid = metadata.split()
        path = os.fsdecode(name)
        if (name in tracked and citation_file(path) and kind == b"blob" and
                mode in (b"100644", b"100755")):
            blobs.append((path, oid))
    references = {root: {} for root in roots}
    literal_paths = {root: set() for root in roots}
    details = {root: [] for root in roots}
    if not blobs:
        return revision, references, literal_paths, details
    data = git(repo, "cat-file", "--batch",
               input_data=b"".join(oid + b"\n" for _, oid in blobs))
    offset = 0
    for path, expected_oid in blobs:
        end = data.index(b"\n", offset)
        oid, kind, length = data[offset:end].split()
        if oid != expected_oid or kind != b"blob":
            raise ValueError("cannot read committed citation files")
        size = int(length)
        content = data[end + 1:end + 1 + size]
        offset = end + size + 2
        if b"\0" in content:
            continue
        try:
            text = content.decode("utf-8")
        except UnicodeDecodeError:
            continue
        line_offsets, recorded = removal_context(text)
        for prefix, reference, literal, position in extract_references(text):
            if prefix:
                full_path = absolute(os.path.join(repo, prefix, reference))
                targets = [
                    (root, "build/" + os.path.relpath(full_path, root))
                    for root in roots
                    if full_path != root and os.path.commonpath((full_path, root)) == root
                ]
            else:
                targets = [(root, reference) for root in roots if str(Path(root).parent) == repo]
            for target, routed_reference in targets:
                citing_file = path if str(Path(target).parent) == repo else str(Path(repo) / path)
                references[target].setdefault(routed_reference, set()).add(citing_file)
                if literal:
                    literal_paths[target].add(routed_reference)
                    line = bisect_right(line_offsets, position)
                    details[target].append({
                        "path": routed_reference, "citing_file": citing_file, "line": line,
                        "removed_recorded": line in recorded,
                    })
    return revision, references, literal_paths, details


def collect_citations(roots):
    collected = {
        root: {"revision": None, "references": {}, "literal_paths": set(),
               "details": [], "errors": []}
        for root in roots
    }
    for repo in dict.fromkeys(str(Path(root).parent) for root in roots):
        try:
            revision, references, literal_paths, details = read_citations(repo, roots)
        except (OSError, ValueError):
            # An unavailable source might contain a cross-repo citation.
            for data in collected.values():
                data["errors"].append({"path": repo, "operation": "committed citations"})
            continue
        for root, data in collected.items():
            if str(Path(root).parent) == repo:
                data["revision"] = revision
            for path, files in references[root].items():
                data["references"].setdefault(path, set()).update(files)
            data["literal_paths"].update(literal_paths[root])
            data["details"].extend(details[root])
    return collected


def parents(path):
    while "/" in path:
        yield path
        path = path.rsplit("/", 1)[0]


def classify(tree, nodes, references, patterns, cutoff_ns, citations_complete=True):
    """Protection and citations propagate up; explicit ancestor citations go down."""
    for reference, files in references.items():
        for ancestor in parents(reference):
            if ancestor in nodes:
                nodes[ancestor].citations.update(files)
    pending = [(tree, set(), set())]
    ordered = []
    while pending:
        node, inherited_citations, inherited_protection = pending.pop()
        node.citations.update(inherited_citations)
        own_citations = inherited_citations | references.get(node.path, set())
        node.protection_reasons.update(inherited_protection)
        if any(fnmatch.fnmatchcase(node.path, pattern) for pattern in patterns):
            node.protection_reasons.add("protected path")
        if not node.complete:
            node.protection_reasons.add("incomplete metadata")
        if not citations_complete:
            node.protection_reasons.add("unavailable citations")
        ordered.append(node)
        pending.extend((child, own_citations, node.protection_reasons.copy())
                       for child in node.children)
    for node in reversed(ordered):
        for child in node.children:
            node.protection_reasons.update(child.protection_reasons)
        if node.protection_reasons:
            node.classification = "PROTECTED"
        elif node.newest_mtime_ns >= cutoff_ns:
            node.classification = "IN-USE"
        elif node.citations:
            node.classification = "CITED"
        else:
            node.classification = "CANDIDATE"


def utc(ns):
    return datetime.fromtimestamp(ns / 1e9, timezone.utc).isoformat(timespec="seconds")


def row(node, root):
    return {
        "path": node.path, "absolute_path": str(Path(root) / node.path[len("build/"):]),
        "kind": node.kind, "size_bytes": node.size_bytes, "file_count": node.file_count,
        "newest_mtime_ns": node.newest_mtime_ns, "newest_mtime": utc(node.newest_mtime_ns),
        "contains_symlinks": node.contains_symlinks, "complete": node.complete,
        "class": node.classification, "citing_files": sorted(node.citations),
        "protection_reasons": sorted(node.protection_reasons),
        "depth": node.path.count("/") - 1,
    }


def missing_references(nodes, references, literal_paths):
    missing, unresolved = [], []
    for path in sorted(literal_paths):
        if path in nodes:
            continue
        # A symlink is itself present, including a broken symlink. Looking below
        # it would follow it, so that reference cannot be declared dangling.
        blocked = any(nodes[p].kind == "symlink" or not nodes[p].complete
                      for p in parents(path) if p in nodes)
        item = {"path": path, "citing_files": sorted(references[path])}
        (unresolved if blocked else missing).append(item)
    return missing, unresolved


def empty_totals():
    return {name: {"size_bytes": 0, "file_count": 0, "entry_count": 0}
            for name in CLASSES}


def audit_root(root, split_bytes, recent_hours, protect, now_ns, dangling=False,
               citation_data=None, inventory=None):
    repo = str(Path(root).parent)
    tree, nodes, errors = scan(root)
    if citation_data is None:
        citation_data = collect_citations([root])[root]
    revision = citation_data["revision"]
    references = citation_data["references"]
    literal_paths = citation_data["literal_paths"]
    errors.extend(citation_data["errors"])
    citations_complete = not citation_data["errors"]
    patterns = list(PROTECTED)
    for pattern in protect:
        pattern = pattern.rstrip("/")
        patterns.append(pattern if pattern.startswith("build/") else "build/" + pattern)
    classify(tree, nodes, references, patterns, now_ns - int(recent_hours * 3600e9),
             citations_complete)
    if inventory is not None:
        inventory.update(nodes)
    entries = []
    pending = list(tree.children)
    while pending:
        node = pending.pop()
        entries.append(row(node, root))
        if node.size_bytes > split_bytes:
            pending.extend(node.children)
    entries.sort(key=lambda item: (-item["size_bytes"], item["path"]))
    totals = empty_totals()
    for node in tree.children:
        counts = totals[node.classification]
        counts["size_bytes"] += node.size_bytes
        counts["file_count"] += node.file_count
        counts["entry_count"] += 1
    missing, unresolved = ([], [])
    if dangling:
        missing, unresolved = missing_references(nodes, references, literal_paths)
        if not tree.complete:
            unresolved.extend(missing)
            missing = []
    return {"repo": repo, "root": root, "revision": revision, "entries": entries,
            "size_bytes": tree.size_bytes, "file_count": tree.file_count,
            "totals": totals, "dangling": missing, "unresolved_references": unresolved,
            "errors": errors, "complete": tree.complete and citations_complete}


def file_path(root, node):
    return str(Path(root).joinpath(*node.path.split("/")[1:]))


def exact_stat(path):
    """Metadata-only lookup, refusing symlink ancestors; no directory listings."""
    parent_fd = open_directory(str(Path(path).parent))
    try:
        return os.stat(Path(path).name, dir_fd=parent_fd, follow_symlinks=False)
    finally:
        os.close(parent_fd)


def hash_file(root, node):
    """Bounded streamed SHA-256; reject replaced or changed files before/after."""
    path = file_path(root, node)
    parent_fd = open_directory(str(Path(path).parent))
    try:
        # NONBLOCK prevents a concurrent replacement with a FIFO from hanging.
        fd = os.open(Path(path).name, os.O_RDONLY | os.O_NOFOLLOW | os.O_NONBLOCK,
                     dir_fd=parent_fd)
    finally:
        os.close(parent_fd)
    try:
        info = os.fstat(fd)
        if not stat.S_ISREG(info.st_mode) or identity(info) != node.identity:
            raise OSError(errno.EAGAIN, "file changed")
        digest = hashlib.sha256()
        remaining = node.size_bytes
        while remaining:
            chunk = os.read(fd, min(1024 * 1024, remaining))
            if not chunk:
                raise OSError(errno.EAGAIN, "file changed")
            digest.update(chunk)
            remaining -= len(chunk)
        if identity(os.fstat(fd)) != node.identity:
            raise OSError(errno.EAGAIN, "file changed")
        return digest.digest()
    finally:
        os.close(fd)


def find_duplicates(inventories, min_bytes):
    sizes = defaultdict(list)
    for root, nodes in inventories.items():
        for node in nodes.values():
            if node.kind == "file" and node.complete and node.size_bytes >= min_bytes:
                sizes[node.size_bytes].append((root, node))
    groups, errors = [], []
    hashed_files = hashed_bytes = 0
    for size, candidates in sorted(sizes.items(), reverse=True):
        if len(candidates) < 2:
            continue
        hashes = defaultdict(list)
        for root, node in candidates:
            try:
                digest = hash_file(root, node)
                hashed_files += 1
                hashed_bytes += size
                hashes[digest].append((root, node))
            except OSError as error:
                errors.append({"path": file_path(root, node), "operation": "hash",
                               "errno": error.errno})
        for copies in hashes.values():
            valid = []
            for root, node in copies:
                try:
                    if identity(exact_stat(file_path(root, node))) != node.identity:
                        raise OSError(errno.EAGAIN, "file changed")
                    valid.append(row(node, root))
                except OSError as error:
                    errors.append({"path": file_path(root, node),
                                   "operation": "hash validation", "errno": error.errno})
            if len(valid) < 2:
                continue
            anchored = any(copy["class"] in ("PROTECTED", "CITED") or copy["citing_files"]
                           for copy in valid)
            for copy in valid:
                copy["redundant"] = bool(
                    anchored and copy["class"] not in ("PROTECTED", "CITED") and
                    not copy["citing_files"])
            valid.sort(key=lambda copy: copy["absolute_path"])
            groups.append({"size_bytes": size, "copy_count": len(valid),
                           "bytes_wasted": size * (len(valid) - 1), "copies": valid})
    groups.sort(key=lambda group: (-group["bytes_wasted"],
                                  group["copies"][0]["absolute_path"]))
    return {"groups": groups, "total_bytes_wasted": sum(g["bytes_wasted"] for g in groups),
            "hashed_files": hashed_files, "hashed_bytes": hashed_bytes,
            "errors": errors, "complete": not errors}


def checkpoint_plan(repos, inventories, duplicates, now_ns, scratch_patterns=()):
    """Review only: disjoint old candidates plus old redundant duplicate copies.

    A path name cannot prove that bytes are disposable. Only caller-declared
    generated scratch can receive the safe label, and all ordinary protection,
    citation, age, completeness and symlink checks still apply.
    """
    cutoff = now_ns - 7 * 24 * 3600 * 1_000_000_000
    patterns = [p.rstrip("/") if p.startswith("build/") else "build/" + p.rstrip("/")
                for p in scratch_patterns]
    complete = all(repo["complete"] for repo in repos) and duplicates["complete"]
    choices, regular = {}, {}
    for root, nodes in inventories.items():
        for node in sorted(nodes.values(), key=lambda n: n.path.count("/"), reverse=True):
            regular[file_path(root, node)] = (
                node.complete and not node.contains_symlinks and
                node.kind in ("file", "directory") and
                all(regular[file_path(root, child)] for child in node.children))

    def add(entry, root, source):
        if (entry["newest_mtime_ns"] >= cutoff or entry["class"] in ("PROTECTED", "CITED")
                or entry["citing_files"]):
            return
        path = entry["absolute_path"]
        if path not in choices:
            choices[path] = {**entry, "root": root, "sources": [], "redundant_copies": []}
        if source not in choices[path]["sources"]:
            choices[path]["sources"].append(source)
        if source == "redundant duplicate":
            choices[path]["redundant_copies"].append(
                {"path": path, "size_bytes": entry["size_bytes"]})

    for repo in repos:
        for entry in repo["entries"]:
            if entry["class"] == "CANDIDATE":
                add(entry, repo["root"], "CANDIDATE")
    for group in duplicates["groups"]:
        for copy in group["copies"]:
            if copy["redundant"]:
                root = next(root for root in inventories
                            if os.path.commonpath((copy["absolute_path"], root)) == root)
                add(copy, root, "redundant duplicate")

    # Keep a parent once instead of also proposing its descendants. Record any
    # duplicate detail it covers without counting those bytes a second time.
    selected = {}
    for path, item in sorted(choices.items(), key=lambda pair: (len(Path(pair[0]).parts), pair[0])):
        owner = next((selected[str(p)] for p in Path(path).parents if str(p) in selected), None)
        if owner is not None:
            owner["redundant_copies"].extend(item["redundant_copies"])
            continue
        selected[path] = item

    totals = {label: {"entry_count": 0, "size_bytes": 0} for label in ("safe", "needs a look")}
    by_repo = {repo["root"]: {label: dict(counts) for label, counts in totals.items()} for repo in repos}
    entries = sorted(selected.values(), key=lambda e: (-e["size_bytes"], e["absolute_path"]))
    for item in entries:
        reasons = []
        if not complete:
            reasons.append("incomplete audit")
        if item["class"] != "CANDIDATE":
            reasons.append("still IN-USE under the requested recent-hours window")
        if not regular.get(item["absolute_path"], False):
            reasons.append("symlink, special file, or incomplete metadata in subtree")
        if not any(fnmatch.fnmatchcase(ancestor, pattern)
                   for ancestor in parents(item["path"]) for pattern in patterns):
            reasons.append("generated scratch provenance not established")
        item["review"] = "needs a look" if reasons else "safe"
        item["review_reasons"] = reasons or ["uncited generated scratch declared by --scratch"]
        for bucket in (totals, by_repo[item["root"]]):
            bucket[item["review"]]["entry_count"] += 1
            bucket[item["review"]]["size_bytes"] += item["size_bytes"]
    return {"minimum_age_days": 7, "older_than_ns": cutoff, "scratch_patterns": patterns,
            "entries": entries, "totals": totals, "totals_by_root": by_repo,
            "safe_paths": sorted(e["absolute_path"] for e in entries if e["review"] == "safe"),
            "complete": complete}


def registered_worktrees(repo):
    return [absolute(os.fsdecode(field[len(b"worktree "):]))
            for field in git(repo, "worktree", "list", "--porcelain", "-z").split(b"\0")
            if field.startswith(b"worktree ")]


def checkout_locations(repo, path, worktrees):
    """Probe only this exact path in registered worktrees, never enumerate them."""
    locations, errors = [], []
    for checkout in worktrees:
        if checkout == repo:
            continue
        candidate = str(Path(checkout) / path)
        try:
            exact_stat(candidate)
            locations.append({"worktree": checkout, "path": candidate})
        except FileNotFoundError:
            continue
        except OSError as error:
            errors.append({"path": candidate, "operation": "worktree metadata",
                           "errno": error.errno})
    return locations, errors


def moved_locations(path, by_name):
    wanted = path.split("/")[1:]
    best, matches = 0, []
    for candidate, parts in by_name.get(wanted[-1], []):
        length = 0
        for a, b in zip(reversed(wanted), reversed(parts)):
            if a != b:
                break
            length += 1
        if length > best:
            best, matches = length, []
        if length == best:
            matches.append(candidate)
    return sorted(matches)


def classify_dangling(repos, inventories, citations):
    by_name = defaultdict(list)
    for root, nodes in inventories.items():
        for node in nodes.values():
            if node.kind in ("file", "directory") and node.complete:
                parts = node.path.split("/")[1:]
                by_name[parts[-1]].append((file_path(root, node), parts))
    for repo in repos:
        missing = {item["path"] for item in repo["dangling"]}
        worktrees = None
        lookup_failed = False
        checkout_cache, moved_cache, grouped = {}, {}, {}
        seen = set()
        counts = {name: 0 for name in DANGLING_CLASSES}
        by_doc = {}
        for detail in citations[repo["root"]]["details"]:
            path, doc, line = detail["path"], detail["citing_file"], detail["line"]
            if path not in missing or (path, doc, line) in seen:
                continue
            seen.add((path, doc, line))
            alternatives, checkouts = [], []
            if detail["removed_recorded"]:
                classification = "removed and recorded"
            else:
                if path not in moved_cache:
                    moved_cache[path] = moved_locations(path, by_name)
                alternatives = moved_cache[path]
                if alternatives:
                    classification = "moved"
                else:
                    if worktrees is None:
                        try:
                            worktrees = registered_worktrees(repo["repo"])
                        except (OSError, ValueError):
                            worktrees, lookup_failed = [], True
                            repo["errors"].append({"path": repo["repo"],
                                                   "operation": "worktree list"})
                    if path not in checkout_cache:
                        checkout_cache[path] = checkout_locations(repo["repo"], path, worktrees)
                        repo["errors"].extend(checkout_cache[path][1])
                    checkouts, errors = checkout_cache[path]
                    classification = "another checkout" if checkouts else "missing"
                    if errors or lookup_failed:
                        repo["complete"] = False
            key = (doc, classification, path)
            item = grouped.setdefault(key, {
                "path": path, "citing_file": doc, "class": classification,
                "line_numbers": [], "alternatives": alternatives, "checkouts": checkouts,
            })
            item["line_numbers"].append(line)
            counts[classification] += 1
            by_doc.setdefault(doc, {name: 0 for name in DANGLING_CLASSES})[classification] += 1
        repo["dangling_citations"] = [grouped[key] for key in sorted(grouped)]
        for item in repo["dangling_citations"]:
            item["line_numbers"].sort()
        repo["dangling_counts"] = counts
        repo["dangling_by_doc"] = dict(sorted(by_doc.items()))


def print_table(report):
    # Escape paths to prevent control characters in filenames affecting terminals.
    def safe(value):
        return json.dumps(value, ensure_ascii=True)

    for repo in report["repos"]:
        print("\nROOT", safe(repo["root"]), "HEAD", repo["revision"] or "unavailable")
        print("CLASS              BYTES      FILES  NEWEST (UTC)               LINK  D  PATH")
        for entry in repo["entries"]:
            print(f'{entry["class"]:10} {entry["size_bytes"]:14d} '
                  f'{entry["file_count"]:10d}  {entry["newest_mtime"]:25}  '
                  f'{"yes" if entry["contains_symlinks"] else "no":4}  '
                  f'{entry["depth"]}  {safe(entry["path"])}')
            if entry["citing_files"]:
                print("  cited:", ", ".join(map(safe, entry["citing_files"])))
        print("TOTALS (top-level entries only; D > 0 rows are overlapping detail)")
        print_totals(repo["totals"])
        print(f'REPO TOTAL {repo["size_bytes"]} bytes; {repo["file_count"]} files')
        if report["dangling_requested"]:
            for doc, counts in repo["dangling_by_doc"].items():
                print("DANGLING DOC", safe(doc),
                      " ".join(f"{name}={count}" for name, count in counts.items()))
                for item in repo["dangling_citations"]:
                    if item["citing_file"] == doc:
                        print(" ", item["class"], safe(item["path"]), "lines",
                              ",".join(map(str, item["line_numbers"])))
                        for path in item["alternatives"]:
                            print("    MATCH", safe(path))
                        for checkout in item["checkouts"]:
                            print("    CHECKOUT", safe(checkout["worktree"]),
                                  safe(checkout["path"]))
            print("DANGLING TOTALS (citation lines)", repo["dangling_counts"])
            for item in repo["unresolved_references"]:
                print("UNRESOLVED", safe(item["path"]), "cited:",
                      ", ".join(map(safe, item["citing_files"])))
        for error in repo["errors"]:
            print("INCOMPLETE", safe(error["path"]), error["operation"],
                  error.get("errno", ""))
    print("\nALL REPOS")
    print_totals(report["totals"])
    if report["duplicates_requested"]:
        duplicates = report["duplicates"]
        for group in duplicates["groups"]:
            print("DUPLICATES", group["size_bytes"], "bytes each;",
                  group["bytes_wasted"], "bytes wasted")
            for copy in group["copies"]:
                print(" ", copy["class"], safe(copy["absolute_path"]),
                      "redundant" if copy["redundant"] else "")
                if copy["citing_files"]:
                    print("    cited:", ", ".join(map(safe, copy["citing_files"])))
        print("DUPLICATES TOTAL", duplicates["total_bytes_wasted"], "bytes wasted")
        for error in duplicates["errors"]:
            print("INCOMPLETE", safe(error["path"]), error["operation"], error["errno"])
    if report["checkpoint_requested"]:
        plan = report["checkpoint"]
        print("\nCHECKPOINT (older than 7 days; review only)")
        for entry in plan["entries"]:
            print(entry["review"], entry["size_bytes"], "bytes", safe(entry["absolute_path"]),
                  "; ".join(entry["review_reasons"]))
            for copy in entry["redundant_copies"]:
                print("  REDUNDANT", copy["size_bytes"], "bytes", safe(copy["path"]))
        for label, counts in plan["totals"].items():
            print("CHECKPOINT TOTAL", label, counts["entry_count"], "entries;", counts["size_bytes"], "bytes")
        print("CHECKPOINT SAFE PATHS (JSON; recheck before any removal)")
        print(json.dumps(plan["safe_paths"], ensure_ascii=True))


def print_totals(totals):
    for name, counts in totals.items():
        print(f'{name:10} {counts["size_bytes"]:14d} bytes '
              f'{counts["file_count"]:10d} files {counts["entry_count"]:6d} entries')


def write_json(path, report, roots):
    """Only explicit output is written: a new report, never inside audited roots."""
    path = absolute(path)
    if any(os.path.commonpath((path, root)) == root for root in roots):
        raise ValueError("JSON output must be outside audited roots")
    parent_fd = open_directory(str(Path(path).parent))
    try:
        fd = os.open(Path(path).name,
                     os.O_WRONLY | os.O_CREAT | os.O_EXCL | os.O_NOFOLLOW,
                     0o600, dir_fd=parent_fd)
    finally:
        os.close(parent_fd)
    with os.fdopen(fd, "w", encoding="utf-8") as output:
        json.dump(report, output, indent=2, ensure_ascii=True)
        output.write("\n")


def nonnegative(value):
    number = float(value)
    if not math.isfinite(number) or number < 0:
        raise argparse.ArgumentTypeError("must be a finite nonnegative number")
    return number


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", action="append", default=[], metavar="BUILD_DIR",
                        help="repeatable build root; replaces defaults")
    parser.add_argument("--include-defaults", action="store_true",
                        help="add --root paths to the two default build roots")
    parser.add_argument("--split-gb", type=nonnegative, default=1,
                        help="show children of directories above this many GiB (default 1)")
    parser.add_argument("--recent-hours", type=nonnegative, default=24)
    parser.add_argument("--protect", action="append", nargs="+", default=[],
                        metavar="PATTERN", help="quoted build-relative shell patterns")
    parser.add_argument("--dangling", action="store_true")
    parser.add_argument("--duplicates", action="store_true",
                        help="stream SHA-256 only for regular files with colliding sizes")
    parser.add_argument("--min-mb", type=nonnegative, default=50,
                        help="minimum duplicate file size in MiB (default 50)")
    parser.add_argument("--checkpoint", action="store_true",
                        help="review candidates and redundant copies older than 7 days; implies --duplicates")
    parser.add_argument("--scratch", action="append", nargs="+", default=[], metavar="PATTERN",
                        help="assert known generated scratch for checkpoint safe labels; default: all ambiguous")
    parser.add_argument("--checkpoint-paths", metavar="FILE",
                        help="create a NEW JSON array of safe absolute paths (requires --checkpoint)")
    parser.add_argument("--json", metavar="FILE", help="create a NEW JSON report; '-' for stdout")
    args = parser.parse_args(argv)
    if (args.checkpoint_paths or args.scratch) and not args.checkpoint:
        parser.error("--checkpoint-paths and --scratch require --checkpoint")
    roots = list(dict.fromkeys(absolute(p) for p in (
        (list(DEFAULT_ROOTS) if args.include_defaults or not args.root else []) + args.root)))
    if any(a != b and os.path.commonpath((a, b)) == a for a in roots for b in roots):
        parser.error("build roots must not overlap")
    now_ns = time.time_ns()
    protect = [pattern for group in args.protect for pattern in group]
    citations = collect_citations(roots)
    inventories = {root: {} for root in roots}
    repos = [audit_root(root, args.split_gb * 1024**3, args.recent_hours,
                        protect, now_ns, args.dangling, citations[root], inventories[root])
             for root in roots]
    if args.dangling:
        classify_dangling(repos, inventories, citations)
    duplicates_requested = args.duplicates or args.checkpoint
    duplicates = find_duplicates(inventories, args.min_mb * 1024**2) if duplicates_requested else None
    checkpoint = checkpoint_plan(repos, inventories, duplicates, now_ns,
                                 [p for group in args.scratch for p in group]) if args.checkpoint else None
    totals = empty_totals()
    for repo in repos:
        for name in CLASSES:
            for key in totals[name]:
                totals[name][key] += repo["totals"][name][key]
    report = {"schema_version": 3, "generated_at": utc(now_ns),
              "split_gb": args.split_gb, "recent_hours": args.recent_hours,
              "dangling_requested": args.dangling, "duplicates_requested": duplicates_requested,
              "checkpoint_requested": args.checkpoint, "checkpoint": checkpoint,
              "min_mb": args.min_mb, "duplicates": duplicates, "repos": repos, "totals": totals}
    if args.json == "-":
        print(json.dumps(report, indent=2, ensure_ascii=True))
    else:
        print_table(report)
        if args.json:
            try:
                write_json(args.json, report, roots)
            except (OSError, ValueError):
                print("Cannot create JSON report (requires a new file outside audited "
                      "roots, with no symlink components): " + json.dumps(args.json),
                      file=sys.stderr)
                return 1
    if args.checkpoint_paths:
        try:
            write_json(args.checkpoint_paths, checkpoint["safe_paths"], roots)
        except (OSError, ValueError):
            print("Cannot create checkpoint paths (requires a new file outside audited roots, "
                  "with no symlink components): " + json.dumps(args.checkpoint_paths), file=sys.stderr)
            return 1
    return 0 if (all(repo["complete"] for repo in repos) and
                 (duplicates is None or duplicates["complete"])) else 1


if __name__ == "__main__":
    sys.exit(main())
