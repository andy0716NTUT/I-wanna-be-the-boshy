"""Extract immutable original code for the optional graphical comparison probe."""
from pathlib import Path
import subprocess

ROOT = Path(__file__).absolute().parents[1]
BASE = "eeb1f72c6c3a9a9e33a49c31946e4b723323d49f"
DESTINATION = ROOT / "build/original-probe-source"


def main():
    paths = subprocess.check_output(
        ["git", "ls-tree", "-r", "--name-only", BASE, "src/", "include/"], cwd=ROOT
    ).decode().splitlines()
    for relative in paths:
        target = DESTINATION / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        original = subprocess.check_output(["git", "show", f"{BASE}:{relative}"], cwd=ROOT)
        if target.exists():
            if target.read_bytes() != original:
                raise SystemExit(f"Refusing to overwrite changed original probe source: {target}")
        else:
            target.write_bytes(original)
    print(f"Verified/extracted {len(paths)} original code files at {DESTINATION}")
    print("Assets are not duplicated or modified; the probe uses the audited deployment.")


if __name__ == "__main__":
    main()
