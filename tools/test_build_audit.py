#!/usr/bin/env python3
"""Headless synthetic tests; no disc inputs. Run: python3 -B tools/test_build_audit.py."""

import contextlib
import hashlib
import io
import json
import os
from pathlib import Path
import stat
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

sys.dont_write_bytecode = True
import build_audit as audit


NOW = 2_000_000_000_000_000_000
OLD = NOW - 7 * 24 * 3600 * 1_000_000_000


def snapshot(root):
    """Include bytes only for our own synthetic fixtures; omit access times."""
    result = {}
    pending = [root]
    while pending:
        path = pending.pop()
        info = path.lstat()
        value = (info.st_mode, info.st_size, info.st_mtime_ns, info.st_ctime_ns,
                 info.st_ino)
        if path.is_symlink():
            value += (os.readlink(path),)
        elif stat.S_ISREG(info.st_mode):
            value += (hashlib.sha256(path.read_bytes()).digest(),)
        elif stat.S_ISDIR(info.st_mode):
            pending.extend(path.iterdir())
        result[str(path.relative_to(root))] = value
    return result


class BuildAuditTests(unittest.TestCase):
    def setUp(self):
        # Resolve the OS temp-dir alias before constructing roots; the audit itself
        # deliberately refuses symlink components such as macOS /tmp.
        self.temp = tempfile.TemporaryDirectory(
            prefix="build-audit-test-", dir=os.path.realpath(tempfile.gettempdir()))
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.repo = self.base / "fixture repo"
        self.repo.mkdir()
        self.root = self.repo / "build"
        self.root.mkdir()
        self.make("build/cited/nested/receipt.dat", b"x" * 11)
        self.make("build/uncited/nested/payload.dat", b"x" * 13)
        self.make("build/cited-other/payload.dat", b"x" * 3)
        self.make("build/recent/payload.dat", b"x" * 7)
        self.make("build/startup-reference/payload.dat", b"x" * 19)
        self.make("build/keep/receipt.dat", b"x" * 9)
        self.make("build/top.dat", b"x" * 5)
        for name in ("fork_refs", "s87", ".pcsx2.lock", "empty"):
            (self.root / name).mkdir()
        (self.base / "outside").mkdir()
        (self.base / "outside" / "secret.dat").write_bytes(b"x" * 4096)
        (self.root / "links").mkdir()
        (self.root / "links" / "file").symlink_to(self.base / "outside" / "secret.dat")
        (self.root / "links" / "dir").symlink_to(self.base / "outside",
                                              target_is_directory=True)
        (self.root / "links" / "broken").symlink_to(self.base / "missing")
        (self.root / "links" / "loop").symlink_to(self.root, target_is_directory=True)
        os.mkfifo(self.root / "pipe")
        self.make("docs/README.md", (
            "`build/cited/nested/receipt.dat`, build/gone/report.dat.\n"
            "`build/recent/payload.dat` `build/startup-reference/payload.dat`\n"
            "`build/links/file` `build/links/broken` `build/links/dir/secret.dat`\n"
            "`build/template/<task>` `build/glob/*.dat` `build/unknown/...`\n"
        ).encode())
        self.make("CLAUDE.md", b"Receipt: build/cited/nested/receipt.dat\n")
        self.make("tools/helper.py", b"# build/cited/nested/receipt.dat\n")
        self.make("docs/nested/ignored.md", b"build/uncited\n")
        self.make("docs/binary.md", b"\0build/uncited")
        self.make("README.md", b"build/uncited\n")
        (self.repo / "docs" / "symlink.md").symlink_to(self.base / "outside" / "secret.dat")
        self.git("init", "-q")
        paths = ["CLAUDE.md", "docs/README.md", "tools/helper.py",
                 "docs/nested/ignored.md", "docs/binary.md", "README.md", "docs/symlink.md"]
        self.git("add", "--", *paths)
        self.git("-c", "user.name=Audit Fixture", "-c", "user.email=audit@example.invalid",
                 "-c", "commit.gpgsign=false", "-c", "core.hooksPath=/dev/null",
                 "commit", "-q", "--only", "-m", "Synthetic audit fixture", "--", *paths)
        # Tracked working edits and an untracked doc must not become citations.
        self.make("docs/README.md", b"build/uncited\n")
        self.make("docs/untracked.md", b"build/uncited\n")
        self.make("docs/staged.md", b"build/uncited\n")
        self.git("add", "--", "docs/staged.md")
        for path in self.build_paths():
            os.utime(path, ns=(OLD, OLD), follow_symlinks=False)
        os.utime(self.root / "recent" / "payload.dat", ns=(NOW, NOW))

    def make(self, relative, contents):
        path = self.repo / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(contents)

    def git(self, *args):
        subprocess.run(["git", "-C", str(self.repo), *args], check=True,
                       stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    def build_paths(self):
        pending = [self.root]
        while pending:
            path = pending.pop()
            yield path
            if path.is_dir() and not path.is_symlink():
                pending.extend(path.iterdir())

    def run_audit(self, split=10, protect=("keep/*",)):
        return audit.audit_root(str(self.root), split, 24, protect, NOW, dangling=True)

    def assert_sizes(self, result):
        self.assertEqual(result["size_bytes"], 67)
        self.assertEqual(result["file_count"], 7)
        entries = {item["path"]: item for item in result["entries"]}
        self.assertEqual(entries["build/links"]["size_bytes"], 0)
        self.assertEqual(entries["build/links"]["file_count"], 0)
        self.assertTrue(entries["build/links"]["contains_symlinks"])

    def test_sizes_classes_citations_and_dangling(self):
        result = self.run_audit()
        self.assertTrue(result["complete"])
        self.assert_sizes(result)
        entries = {item["path"]: item for item in result["entries"]}
        expected = {"cited": "CITED", "uncited": "CANDIDATE", "recent": "IN-USE",
                    "startup-reference": "PROTECTED", "keep": "PROTECTED",
                    "fork_refs": "PROTECTED", "s87": "PROTECTED", ".pcsx2.lock": "PROTECTED",
                    "cited-other": "CANDIDATE", "empty": "CANDIDATE", "pipe": "CANDIDATE"}
        for name, classification in expected.items():
            self.assertEqual(entries["build/" + name]["class"], classification, name)
        self.assertEqual(entries["build/cited"]["citing_files"],
                         ["CLAUDE.md", "docs/README.md", "tools/helper.py"])
        self.assertEqual(entries["build/cited/nested/receipt.dat"]["class"], "CITED")
        self.assertEqual(entries["build/uncited"]["newest_mtime_ns"], OLD)
        self.assertEqual(entries["build/recent"]["newest_mtime_ns"], NOW)
        self.assertEqual([r["path"] for r in result["dangling"]], ["build/gone/report.dat"])
        self.assertEqual([r["path"] for r in result["unresolved_references"]],
                         ["build/links/dir/secret.dat"])
        self.assertEqual(sum(t["size_bytes"] for t in result["totals"].values()), 67)
        self.assertEqual(sum(t["file_count"] for t in result["totals"].values()), 7)
        self.assertEqual(result["totals"]["PROTECTED"]["size_bytes"], 28)
        self.assertEqual(result["totals"]["IN-USE"]["size_bytes"], 7)
        self.assertEqual(result["totals"]["CITED"]["size_bytes"], 11)
        self.assertEqual(result["totals"]["CANDIDATE"]["size_bytes"], 21)

    def test_nothing_changed_and_no_file_data_opened(self):
        before = snapshot(self.base)
        real_open = os.open

        def directories_only(path, flags, *args, **kwargs):
            self.assertTrue(flags & os.O_DIRECTORY)
            self.assertTrue(flags & os.O_NOFOLLOW)
            return real_open(path, flags, *args, **kwargs)

        with mock.patch.object(audit.os, "open", side_effect=directories_only):
            with mock.patch("builtins.open", side_effect=AssertionError("file data opened")):
                result = self.run_audit()
        self.assert_sizes(result)
        self.assertEqual(before, snapshot(self.base))

    def test_one_scandir_per_directory_and_recursive_split(self):
        real_scandir = os.scandir
        visited = set()

        def once(fd):
            identity = (os.fstat(fd).st_dev, os.fstat(fd).st_ino)
            self.assertNotIn(identity, visited)
            visited.add(identity)
            return real_scandir(fd)

        with mock.patch.object(audit.os, "scandir", side_effect=once):
            result = self.run_audit()
        self.assertEqual(len(visited), sum(
            path.is_dir() and not path.is_symlink() for path in self.build_paths()))
        paths = {item["path"] for item in result["entries"]}
        self.assertIn("build/uncited/nested/payload.dat", paths)
        self.assertNotIn("build/recent/payload.dat", paths)
        self.assertEqual([r["size_bytes"] for r in result["entries"]],
                         sorted((r["size_bytes"] for r in result["entries"]), reverse=True))

    def test_deliberate_follow_symlinks_mistake_is_rejected(self):
        """Mutation control: the exact size test MUST reject following file links."""
        real_scandir = os.scandir

        class BadEntry:
            def __init__(self, entry):
                self.entry, self.name = entry, entry.name

            def stat(self, follow_symlinks=False):
                return self.entry.stat(follow_symlinks=True)  # deliberate defect

        class BadIterator:
            def __init__(self, fd):
                self.iterator = real_scandir(fd)

            def __next__(self):
                return BadEntry(next(self.iterator))

            def close(self):
                self.iterator.close()

        with mock.patch.object(audit.os, "scandir", side_effect=BadIterator):
            bad_result = self.run_audit()
        with self.assertRaises(AssertionError):
            self.assert_sizes(bad_result)

    def test_symlink_roots_and_parent_components_are_refused(self):
        alias = self.base / "alias"
        alias.symlink_to(self.repo, target_is_directory=True)
        tree, nodes, errors = audit.scan(str(alias / "build"))
        self.assertFalse(tree.complete)
        self.assertFalse(nodes)
        self.assertTrue(errors)
        linked_root = self.repo / "linked-build"
        linked_root.symlink_to(self.root, target_is_directory=True)
        self.assertFalse(audit.scan(str(linked_root))[0].complete)

    def test_directory_replaced_by_symlink_is_never_entered(self):
        real_open = os.open
        swapped = False

        def race(path, flags, *args, **kwargs):
            nonlocal swapped
            if path == "uncited" and not swapped:
                swapped = True
                (self.root / "uncited").rename(self.base / "saved-uncited")
                (self.root / "uncited").symlink_to(self.base / "outside",
                                                 target_is_directory=True)
            return real_open(path, flags, *args, **kwargs)

        with mock.patch.object(audit.os, "open", side_effect=race):
            result = self.run_audit()
        self.assertTrue(swapped)
        self.assertFalse(result["complete"])
        self.assertEqual(result["size_bytes"], 54)
        self.assertNotIn("CANDIDATE", {item["class"] for item in result["entries"]})

    def test_unreadable_directory_and_unavailable_git_are_conservative(self):
        real_open = os.open

        def denied(path, flags, *args, **kwargs):
            if path == "uncited":
                raise PermissionError(13, "synthetic permission error")
            return real_open(path, flags, *args, **kwargs)

        with mock.patch.object(audit.os, "open", side_effect=denied):
            result = self.run_audit()
        self.assertFalse(result["complete"])
        self.assertFalse(result["dangling"])
        with mock.patch.object(audit, "read_citations", side_effect=ValueError("unavailable")):
            result = self.run_audit()
        self.assertEqual({item["class"] for item in result["entries"]}, {"PROTECTED"})

    def test_ancestor_citation_and_protection_cover_descendants(self):
        tree, nodes, _ = audit.scan(str(self.root))
        audit.classify(tree, nodes, {"build/uncited": {"docs/receipt.md"}},
                       ["build/cited"], NOW - 24 * 3600 * 1_000_000_000)
        self.assertEqual(nodes["build/uncited/nested/payload.dat"].classification, "CITED")
        self.assertEqual(nodes["build/cited/nested/receipt.dat"].classification, "PROTECTED")

    def test_json_cli_no_clobber_no_audit_writes_and_no_contents_printed(self):
        output = self.base / "report.json"
        before = snapshot(self.repo)
        stream = io.StringIO()
        with contextlib.redirect_stdout(stream):
            self.assertEqual(audit.main(["--root", str(self.root), "--split-gb", "0",
                                         "--dangling", "--json", str(output)]), 0)
        result = json.loads(output.read_text())
        self.assertEqual(result["repos"][0]["size_bytes"], 67)
        self.assertEqual(before, snapshot(self.repo))
        self.assertNotIn("x" * 11, stream.getvalue())
        original = output.read_bytes()
        with self.assertRaises(FileExistsError):
            audit.write_json(str(output), result, [str(self.root)])
        self.assertEqual(original, output.read_bytes())
        with self.assertRaises(ValueError):
            audit.write_json(str(self.root / "report.json"), result, [str(self.root)])
        self.assertFalse((self.root / "report.json").exists())
        alias = self.base / "output-alias"
        alias.symlink_to(self.base / "outside", target_is_directory=True)
        with self.assertRaises(OSError):
            audit.write_json(str(alias / "report.json"), result, [str(self.root)])

    def test_cli_root_override_addition_and_invalid_numbers(self):
        roots = []
        fixture_result = self.run_audit()

        def record(root, *args):
            roots.append(root)
            return fixture_result

        with mock.patch.object(audit, "audit_root", side_effect=record):
            with mock.patch.object(audit, "collect_citations",
                                   return_value={p: {} for p in [*audit.DEFAULT_ROOTS,
                                                                 str(self.root)]}):
                with contextlib.redirect_stdout(io.StringIO()):
                    self.assertEqual(audit.main(["--root", str(self.root), "--include-defaults",
                                                 "--json", "-"]), 0)
        self.assertEqual(roots, [*audit.DEFAULT_ROOTS, str(self.root)])
        for value in ("-1", "nan", "inf"):
            with self.subTest(value=value), contextlib.redirect_stderr(io.StringIO()):
                with self.assertRaises(SystemExit):
                    audit.main(["--split-gb", value])

    def test_cross_repo_citations_use_the_target_root(self):
        other = self.base / "other-repo"
        other.mkdir()
        other_root = other / "build"
        other_root.mkdir()
        (other_root / "cross.dat").write_bytes(b"x" * 23)
        (other / "CLAUDE.md").write_text("No build references.\n")
        subprocess.run(["git", "-C", str(other), "init", "-q"], check=True)
        subprocess.run(["git", "-C", str(other), "add", "--", "CLAUDE.md"], check=True)
        subprocess.run(
            ["git", "-C", str(other), "-c", "user.name=Audit Fixture",
             "-c", "user.email=audit@example.invalid", "-c", "commit.gpgsign=false",
             "-c", "core.hooksPath=/dev/null", "commit", "-q", "--only",
             "-m", "Synthetic cross-repo fixture", "--", "CLAUDE.md"], check=True)
        os.utime(other_root / "cross.dat", ns=(OLD, OLD))
        os.utime(other_root, ns=(OLD, OLD))
        self.make("docs/CROSS.md", (
            "`../other-repo/build/cross.dat` "
            f"`{other_root}/absent.dat`\n"
        ).encode())
        self.git("add", "--", "docs/CROSS.md")
        self.git("-c", "user.name=Audit Fixture", "-c", "user.email=audit@example.invalid",
                 "-c", "commit.gpgsign=false", "-c", "core.hooksPath=/dev/null",
                 "commit", "-q", "--only", "-m", "Synthetic cross-repo citation",
                 "--", "docs/CROSS.md")
        citations = audit.collect_citations([str(self.root), str(other_root)])
        result = audit.audit_root(str(other_root), 1, 24, [], NOW, True,
                                  citations[str(other_root)])
        self.assertEqual(result["entries"][0]["class"], "CITED")
        self.assertEqual(result["entries"][0]["citing_files"],
                         [str(self.repo / "docs/CROSS.md")])
        self.assertEqual([r["path"] for r in result["dangling"]], ["build/absent.dat"])
        self.assertNotIn("build/cross.dat", citations[str(self.root)]["references"])


if __name__ == "__main__":
    unittest.main(verbosity=2)
