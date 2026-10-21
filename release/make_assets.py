#!/usr/bin/env python3
"""Makes a release's assets zip: what the game downloads leave out.

    python release/make_assets.py v0.1.0-alpha <folder of dub packs> [--sums SHA256SUMS.txt]

The zip, RingRacersWorldwide-<version>-assets.zip, is laid out as the game's
folder, for Windows and the Linux tarball alike:

    data/worldwide.pk3    WORLDWIDE's title screen (assets/worldwide.pk3)
    dubs/<the packs>      each .wad and .pk3 of the folder given
    dubs/CREDITS.txt      release/DUBS-CREDITS.txt

The packs are never committed: they are given here, at the release. Each must
hold at least one DUBDEF lump. The credits must be finished (no TODO- left),
unless --allow-todo, for a trial. With --sums, the zip's line is added to a
sha256sum file (the draft's SHA256SUMS.txt, downloaded first).

Then add the zip (and SHA256SUMS.txt) to the draft release: on GitHub, its
Edit page; or `gh release upload <version> <zip> SHA256SUMS.txt --clobber`.
Python 3 alone, no package needed.
"""

import argparse
import hashlib
import os
import struct
import sys
import zipfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TITLE_PK3 = os.path.join(ROOT, "assets", "worldwide.pk3")
CREDITS = os.path.join(ROOT, "release", "DUBS-CREDITS.txt")


def wad_has_dubdef(path):
    with open(path, "rb") as f:
        head = f.read(12)
        if len(head) < 12 or head[:4] not in (b"IWAD", b"PWAD"):
            return None
        count, offset = struct.unpack("<ii", head[4:])
        f.seek(offset)
        table = f.read(16 * count)
    names = [table[i + 8:i + 16].rstrip(b"\0").upper() for i in range(0, len(table), 16)]
    return b"DUBDEF" in names


def pk3_has_dubdef(path):
    try:
        with zipfile.ZipFile(path) as z:
            return any(os.path.basename(n).upper().split(".")[0] == "DUBDEF" for n in z.namelist())
    except zipfile.BadZipFile:
        return None


def main():
    p = argparse.ArgumentParser(description="Makes a release's assets zip (worldwide.pk3 and the dub packs).")
    p.add_argument("version", help="the release's tag, v0.1.0-alpha say")
    p.add_argument("dubs", help="the folder holding the dub packs (.wad, .pk3)")
    p.add_argument("--out", default=".", help="where to write the zip (default: here)")
    p.add_argument("--sums", help="a sha256sum file to add the zip's line to")
    p.add_argument("--allow-todo", action="store_true", help="credits not finished: for a trial only")
    a = p.parse_args()

    if not a.version.startswith("v"):
        sys.exit("The version is the release's tag, starting with v: v0.1.0-alpha.")
    if not os.path.isfile(TITLE_PK3):
        sys.exit("No assets/worldwide.pk3 in this checkout: " + TITLE_PK3)
    if not os.path.isfile(CREDITS):
        sys.exit("No release/DUBS-CREDITS.txt in this checkout.")
    with open(CREDITS, encoding="utf-8") as f:
        credits = f.read()
    if "TODO-" in credits and not a.allow_todo:
        sys.exit("release/DUBS-CREDITS.txt still holds TODO- lines: finish it first (or --allow-todo for a trial).")

    if not os.path.isdir(a.dubs):
        sys.exit("Not a folder: " + a.dubs)
    packs = sorted(n for n in os.listdir(a.dubs)
                   if os.path.isfile(os.path.join(a.dubs, n)) and n.lower().endswith((".wad", ".pk3")))
    if not packs:
        sys.exit("No .wad or .pk3 in " + a.dubs)

    for n in packs:
        path = os.path.join(a.dubs, n)
        has = wad_has_dubdef(path) if n.lower().endswith(".wad") else pk3_has_dubdef(path)
        if has is None:
            sys.exit(n + ": not a WAD or PK3 the game can read.")
        if not has:
            sys.exit(n + ": no DUBDEF lump in it -- not a dub pack.")
        print("  pack  %-40s %8.1f MB" % (n, os.path.getsize(path) / 1e6))

    name = "RingRacersWorldwide-%s-assets.zip" % a.version
    out = os.path.join(a.out, name)
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as z:
        z.write(TITLE_PK3, "data/worldwide.pk3")
        for n in packs:
            z.write(os.path.join(a.dubs, n), "dubs/" + n)
        # Windows line ends: the same text reads in Notepad and elsewhere.
        z.writestr("dubs/CREDITS.txt", credits.replace("\r\n", "\n").replace("\n", "\r\n"))

    with open(out, "rb") as f:
        digest = hashlib.sha256(f.read()).hexdigest()
    line = "%s  %s\n" % (digest, name)
    print("Written: %s (%.1f MB)" % (out, os.path.getsize(out) / 1e6))
    print(line, end="")
    if a.sums:
        with open(a.sums, "a", encoding="utf-8", newline="\n") as f:
            f.write(line)
        print("Added to " + a.sums)


if __name__ == "__main__":
    main()
