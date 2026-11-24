"""Audit original assets and static call paths; never claims gameplay parity."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
BASE = "eeb1f72c6c3a9a9e33a49c31946e4b723323d49f"


def git(*args):
    return subprocess.check_output(["git", *args], cwd=ROOT)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write-manifest", action="store_true")
    args = parser.parse_args()
    files = git("ls-tree", "-r", "--name-only", BASE, "Resources/").decode().splitlines()
    manifest = {"original_sha": BASE, "resources": []}
    failures, conversions = [], []
    for relative in files:
        original = git("show", f"{BASE}:{relative}")
        actual = (ROOT / relative).read_bytes()
        entry = {"path": relative, "sha256": hashlib.sha256(original).hexdigest(),
                 "windows_checkout_sha256": hashlib.sha256(actual).hexdigest()}
        manifest["resources"].append(entry)
        if original != actual:
            if relative.endswith(".txt") and original.replace(b"\r\n", b"\n") == actual.replace(b"\r\n", b"\n"):
                conversions.append(relative)
            else:
                failures.append(relative)
    target = ROOT / "docs/evidence/original-resources-sha256.json"
    if args.write_manifest:
        if target.exists():
            raise SystemExit("Manifest already exists; refusing to overwrite baseline")
        target.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    else:
        saved = json.loads(target.read_text(encoding="utf-8"))
        expected = [(e["path"], e["sha256"]) for e in saved["resources"]]
        current = [(e["path"], e["sha256"]) for e in manifest["resources"]]
        if expected != current:
            failures.append("Original manifest does not match original Git commit")
    source = (ROOT / "src/AppStart.cpp").read_text(encoding="utf-8")
    death_paths = re.findall(r'RESOURCE_DIR"(/Image/Character/death/[^"\n]+)"', source)
    missing = ["Resources" + p for p in death_paths if not (ROOT / ("Resources" + p)).is_file()]
    gameplay = ["src/App.cpp", "src/AppStart.cpp", "src/AppUpdate.cpp", "src/PlayerManager.cpp",
                "src/Character.cpp", "src/AnimatedCharacter.cpp", "include/App.hpp",
                "include/AppMembers.hpp", "include/World.hpp"]
    drift = [p for p in gameplay if git("show", f"{BASE}:{p}").replace(b"\r\n", b"\n") !=
             (ROOT / p).read_bytes().replace(b"\r\n", b"\n")]
    print(json.dumps({"original_sha": BASE, "resource_count": len(files),
                      "changed_resources": failures, "checkout_crlf_conversions": len(conversions),
                      "missing_death_frames": missing, "gameplay_source_drift": drift,
                      "scope": "Static asset/source audit only; gameplay not executed"}, indent=2))
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())
