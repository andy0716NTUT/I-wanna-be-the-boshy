"""Generate test-only friends/observers; never edit the real/original game sources."""
import hashlib
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).absolute().parents[1]
BASE = "eeb1f72c6c3a9a9e33a49c31946e4b723323d49f"


def insert_once(text, anchor, replacement, label):
    if text.count(anchor) != 1:
        raise SystemExit(f"Instrumentation anchor changed: {label}; inspect the source, do not skip it")
    return text.replace(anchor, replacement, 1)


def main():
    cmake_source = subprocess.check_output(["git", "show", f"{BASE}:CMakeLists.txt"], cwd=ROOT).decode()
    file_list = subprocess.check_output(["git", "show", f"{BASE}:files.cmake"], cwd=ROOT).decode()
    expected = set(re.findall(r"src/[A-Za-z0-9_/]+\.cpp", cmake_source))
    expected.update("src/" + p for p in re.findall(r"[A-Za-z0-9_/]+\.cpp", file_list))
    expected.discard("src/main.cpp")
    frozen = set(re.findall(r"src/[A-Za-z0-9_/]+\.cpp", (ROOT / "cmake/OriginalGameSources.cmake").read_text()))
    if expected != frozen:
        raise SystemExit(f"Frozen reference target drift: missing={expected-frozen}, extra={frozen-expected}")
    original_paths = subprocess.check_output(
        ["git", "ls-tree", "-r", "--name-only", BASE, "src/", "include/"], cwd=ROOT
    ).decode().splitlines()
    friend_headers = {"include/App.hpp", "include/Bullet.hpp", "include/Boss1/Boss1.hpp",
                      "include/MapObject/CheckPoint.hpp", "include/GameOverUI.hpp",
                      "include/AnimatedCharacter.hpp"}
    for version in ("Original", "Current"):
        destination = ROOT / "build/regression-source" / version
        paths = original_paths if version == "Original" else [
            p.relative_to(ROOT).as_posix() for directory in ("src", "include")
            for p in (ROOT / directory).rglob("*") if p.suffix in (".cpp", ".hpp")]
        manifest = []
        for relative in paths:
            raw = subprocess.check_output(["git", "show", f"{BASE}:{relative}"], cwd=ROOT) \
                if version == "Original" else (ROOT / relative).read_bytes()
            text = raw.decode("utf-8").replace("\r\n", "\n")
            if relative in friend_headers:
                text = insert_once(text, "private:", "private:\n    friend struct BoshyRegressionAccess;", relative)
            if relative == "src/AppUpdate.cpp":
                text = '#include "RegressionAccess.hpp"\n' + text
                anchor = "    static bool isJumping = false;"
                # Later refactors must explicitly retain/update this observation point.
                if anchor not in text:
                    anchor = "    // Regression frame observation point."
                text = insert_once(text, anchor, anchor + "\n    const BoshyRegressionScope traceScope([&] {\n"
                                   "        BoshyRegressionAccess::Physics(velocityY, jumpCount, shootCooldown, isJumping);\n"
                                   "    });", "AppUpdate physics observer")
                events = {
                    "head": "if ((aboveTile == 2) && velocityY > 0){",
                    "floor": "if ((belowTile == 1 || belowTile == 8)  && velocityY < 0){",
                    "jump": "if (jumpCount < 2 || GodMode) {",
                    "shorten_jump": "else if (Util::Input::IsKeyUp(Util::Keycode::Z) && isJumping && velocityY > 0) {",
                }
                for event, anchor in events.items():
                    if version == "Current" and event in ("head", "floor") and "m_Player.ResolveTiles(" in text:
                        continue  # Observed in the extracted PlayerManager implementation below.
                    text = insert_once(text, anchor, anchor + f'\n BoshyRegressionAccess::Event("{event}");', event)
                for event, anchor in {
                    "shot": "m_Bullets.push_back(bullet);",
                    "checkpoint_saved": "currentCheckPoint = savePosition;",
                    "spike": "// 不再使用needsRespawn和立即return，让死亡处理代码有机会执行",
                    "room_transition": 'std::cout << "Current Phase : "',
                }.items():
                    text = insert_once(text, anchor, f'BoshyRegressionAccess::Event("{event}");\n ' + anchor, event)
            if relative == "src/App.cpp":
                text = '#include "RegressionAccess.hpp"\n' + text
                text = insert_once(text, "void App::Respawn() {", 'void App::Respawn() {\n BoshyRegressionAccess::Event("respawn");', "respawn")
            if version == "Current" and relative == "src/PlayerManager.cpp" and "PlayerManager::ResolveTiles" in text:
                text = '#include "RegressionAccess.hpp"\n' + text
                for event, anchor in {
                    "head": "if ((aboveTile == 2) && velocityY > 0){",
                    "floor": "if ((belowTile == 1 || belowTile == 8)  && velocityY < 0){",
                }.items():
                    text = insert_once(text, anchor, anchor + f'\n BoshyRegressionAccess::Event("{event}");', event)
            target = destination / relative
            target.parent.mkdir(parents=True, exist_ok=True)
            result = text.encode("utf-8")
            if not target.exists() or target.read_bytes() != result:
                target.write_bytes(result)
            manifest.append({"path": relative, "source_sha256": hashlib.sha256(raw).hexdigest(),
                             "instrumented_sha256": hashlib.sha256(result).hexdigest()})
        (destination / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
        print(f"{version}: {len(manifest)} source files; friends/observations confined to {destination}")


if __name__ == "__main__":
    main()
