#!/usr/bin/env python3
"""Read-only build metadata audit; Python standard library, native macOS arm64."""

import argparse
from dataclasses import dataclass, field
from datetime import datetime, timezone
import fnmatch
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
            yield prefix, raw, literal


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
    if not blobs:
        return revision, references, literal_paths
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
        for prefix, reference, literal in extract_references(text):
            if prefix:
                target = absolute(os.path.join(repo, prefix, "build"))
                targets = [target] if target in references else []
            else:
                targets = [root for root in roots if str(Path(root).parent) == repo]
            for target in targets:
                citing_file = path if str(Path(target).parent) == repo else str(Path(repo) / path)
                references[target].setdefault(reference, set()).add(citing_file)
                if literal:
                    literal_paths[target].add(reference)
    return revision, references, literal_paths


def collect_citations(roots):
    collected = {
        root: {"revision": None, "references": {}, "literal_paths": set(), "errors": []}
        for root in roots
    }
    for repo in dict.fromkeys(str(Path(root).parent) for root in roots):
        try:
            revision, references, literal_paths = read_citations(repo, roots)
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
               citation_data=None):
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
            for item in repo["dangling"]:
                print("DANGLING", safe(item["path"]), "cited:",
                      ", ".join(map(safe, item["citing_files"])))
            for item in repo["unresolved_references"]:
                print("UNRESOLVED", safe(item["path"]), "cited:",
                      ", ".join(map(safe, item["citing_files"])))
        for error in repo["errors"]:
            print("INCOMPLETE", safe(error["path"]), error["operation"],
                  error.get("errno", ""))
    print("\nALL REPOS")
    print_totals(report["totals"])


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
    parser.add_argument("--json", metavar="FILE", help="create a NEW JSON report; '-' for stdout")
    args = parser.parse_args(argv)
    roots = list(dict.fromkeys(absolute(p) for p in (
        (list(DEFAULT_ROOTS) if args.include_defaults or not args.root else []) + args.root)))
    if any(a != b and os.path.commonpath((a, b)) == a for a in roots for b in roots):
        parser.error("build roots must not overlap")
    now_ns = time.time_ns()
    protect = [pattern for group in args.protect for pattern in group]
    citations = collect_citations(roots)
    repos = [audit_root(root, args.split_gb * 1024**3, args.recent_hours,
                        protect, now_ns, args.dangling, citations[root]) for root in roots]
    totals = empty_totals()
    for repo in repos:
        for name in CLASSES:
            for key in totals[name]:
                totals[name][key] += repo["totals"][name][key]
    report = {"schema_version": 1, "generated_at": utc(now_ns),
              "split_gb": args.split_gb, "recent_hours": args.recent_hours,
              "dangling_requested": args.dangling, "repos": repos, "totals": totals}
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
    return 0 if all(repo["complete"] for repo in repos) else 1


if __name__ == "__main__":
    sys.exit(main())
