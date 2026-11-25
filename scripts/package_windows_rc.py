"""Package a verified local Windows RC. Never uploads, merges, or publishes."""
import argparse,hashlib,json,os,re,shutil,subprocess,time,zipfile
from datetime import datetime,timezone,timedelta
from pathlib import Path
ROOT=Path(__file__).absolute().parents[1]
p=argparse.ArgumentParser();p.add_argument("--build-dir",default="build/windows-rc2");p.add_argument("--runtime-licenses",type=Path,required=True);p.add_argument("--objdump",type=Path,required=True);p.add_argument("--name",default="RC1");a=p.parse_args()
if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9._-]{0,32}",a.name): raise SystemExit("--name must be a short filename label")
build=ROOT/a.build_dir
for config in ("Debug","Release"):
    log=build/f"ctest-{config.lower()}.log"
    if not log.is_file() or "100% tests passed" not in log.read_text(encoding="utf-8",errors="replace"):
        raise SystemExit(f"Require passing fresh {config} CTest: {log}")
revision=subprocess.check_output(["git","rev-parse","HEAD"],cwd=ROOT).decode().strip()
name=f"BoshyRemaster-Windows-x64-{a.name}-{revision[:8]}"
output=ROOT/"build/release-candidate";output.mkdir(parents=True,exist_ok=True)
candidate=output/name
if candidate.exists(): raise SystemExit(f"Preserve existing candidate; choose a new --name: {candidate}")
candidate.mkdir()
source=build/"bin/Release"
exe=source/"IWannaBeTheBoshy.exe"
if not exe.is_file(): raise SystemExit(f"Missing Release executable: {exe}")
shutil.copy2(exe,candidate/exe.name)
for directory in ("Resources","PTSDAssets"): shutil.copytree(source/directory,candidate/directory)
shutil.copy2(source/"config.json",candidate/"config.json")
shutil.copy2(ROOT/"LICENSE",candidate/"LICENSE")
# Include only actual recursive PE imports found beside the game, not GLEW tools
# or other build outputs that happen to share the binary directory.
queue=[exe];seen=set();dlls=[]
while queue:
    binary=queue.pop()
    text=subprocess.check_output([str(a.objdump),"-p",str(binary)],stderr=subprocess.STDOUT).decode(errors="replace")
    for dependency in re.findall(r"DLL Name:\s*([^\r\n]+)",text):
        key=dependency.strip().lower()
        if key in seen: continue
        seen.add(key)
        local=source/dependency.strip()
        if local.is_file():
            shutil.copy2(local,candidate/local.name);dlls.append(local.name);queue.append(local)
        elif not (Path(os.environ.get("SystemRoot","C:/Windows"))/"System32"/dependency.strip()).is_file() and not key.startswith(("api-ms-","ext-ms-")):
            raise SystemExit(f"Unresolved package dependency: {dependency}")
# Preserve supplied license bytes and directory context; no license edits.
licenses=candidate/"Licenses"
for entry in (ROOT/"PTSD").rglob("*"):
    if entry.is_file() and entry.name.upper().startswith(("LICENSE","COPYING","COPYRIGHT","FTL.","GPL.","LGPL.")):
        destination=licenses/"PTSD"/entry.relative_to(ROOT/"PTSD")
        destination.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(entry,destination)
for component in ("gcc","winpthreads","crt","headers"):
    directory=a.runtime_licenses/component
    if not directory.is_dir(): raise SystemExit(f"Missing runtime license directory: {directory}")
    shutil.copytree(directory,licenses/"MinGW"/component)
(candidate/"README.txt").write_text("""I Wanna Be The Boshy Remaster - INTERNAL Windows x64 Release Candidate
Run IWannaBeTheBoshy.exe; keep Resources, PTSDAssets, config.json and DLLs together.
Controls remain arrows / Z jump / X shoot / R checkpoint respawn.
Window dimensions are physical; logical game canvas stays 1280x960, nearest scaling.
Optional config boolean fullscreen=true; Debug Fullscreen/Collision overlay controls.
Normal music volume/default behavior preserved. Agent validation uses dummy audio.
Known original gaps: eight missing death frames and initial unused phase1 PNG;
Game Over UI remains functional. Physical keyboard feel, audible devices, other
GPU/DPI/multi-monitor combinations and every level route need additional playtest.
The simulation retains the original frame-coupled clock and unusual collisions.
Source/license provenance is retained; this is an internal validation artifact,
not a public game release. No main merge or public publishing was performed.
""",encoding="utf-8")
resources=[x for x in (ROOT/"Resources").rglob("*") if x.is_file()]
for original in resources:
    relative=original.relative_to(ROOT)
    if original.read_bytes()!=(candidate/relative).read_bytes(): raise SystemExit(f"Resource differs: {relative}")
for original in (ROOT/"PTSD/assets/shaders").glob("*"):
    if original.is_file() and original.read_bytes()!=(candidate/"PTSDAssets/shaders"/original.name).read_bytes(): raise SystemExit("Shader differs")
manifest={"internal_candidate":True,"runtime_revision":revision,"created_at":datetime.now(timezone(timedelta(hours=8))).isoformat(timespec="seconds"),
          "resource_count":len(resources),"runtime_dlls":sorted(dlls),"files":{}}
for file in sorted(candidate.rglob("*")):
    if file.is_file(): manifest["files"][file.relative_to(candidate).as_posix()]=hashlib.sha256(file.read_bytes()).hexdigest()
(candidate/"manifest.json").write_text(json.dumps(manifest,indent=2)+"\n",encoding="utf-8")
archive=output/(name+".zip")
with zipfile.ZipFile(archive,"w",zipfile.ZIP_DEFLATED) as zip:
    for file in sorted(candidate.rglob("*")):
        if file.is_file(): zip.write(file,file.relative_to(candidate))
with zipfile.ZipFile(archive) as zip:
    if zip.testzip(): raise SystemExit("ZIP integrity check failed")
    portable=ROOT/"build"/"可攜驗證"/name
    if portable.exists(): raise SystemExit("Preserve existing Unicode validation directory")
    for member in zip.namelist():
        if Path(member).is_absolute() or ".." in Path(member).parts: raise SystemExit("Unsafe ZIP member")
    zip.extractall(portable)
# Startup verification of actual shipped executables, not the instrumented probes.
# Runner terminates only its own process after observing a healthy startup.
def smoke(target,label):
    env=os.environ.copy();env.update(SDL_AUDIODRIVER="dummy",PATH="C:/Windows/System32;C:/Windows")
    startup=subprocess.STARTUPINFO();startup.dwFlags|=subprocess.STARTF_USESHOWWINDOW;startup.wShowWindow=0
    log=output/(name+"-"+label+".log")
    with log.open("wb") as stream:
        child=subprocess.Popen([str(target)],cwd="C:/Windows",env=env,startupinfo=startup,stdout=stream,stderr=subprocess.STDOUT)
        try:
            for _ in range(60):
                if child.poll() is not None: raise RuntimeError(f"{label} exited early: {child.returncode}")
                time.sleep(.05)
        finally:
            if child.poll() is None: child.terminate()
            child.wait(timeout=10)
    text=log.read_text(encoding="utf-8",errors="replace")
    if "OpenGL Info" not in text: raise RuntimeError(f"No OpenGL startup evidence: {log}")
    missing=re.findall(r"Failed to load image: '([^']+)'",text)
    known=lambda path: "/Character/death/death" in path or path.endswith("Background/phase1.png")
    unexpected=[line for line in text.splitlines() if ("[error]" in line or "Boshy failed:" in line or "Boshy startup failed:" in line) and not ("Failed to load image:" in line or "Couldn't open" in line or "Unable to open" in line)]
    if any(not known(path) for path in missing) or unexpected: raise RuntimeError(f"Unexpected startup errors: {log}: {unexpected}")
    return {"result":"PASS","cwd":"C:/Windows","path":"System32/Windows only","audio_driver":"dummy","known_missing_images":sorted(set(missing)),
            "termination":"runner stopped its owned startup-only process; natural shutdown covered by regression/GL tests","log_sha256":hashlib.sha256(log.read_bytes()).hexdigest()}
report={"runtime_revision":revision,"archive":str(archive),"sha256":hashlib.sha256(archive.read_bytes()).hexdigest(),"resource_count":len(resources),"dlls":sorted(dlls),"startup":{}}
report["startup"]["fresh-debug"]=smoke(build/"bin/Debug/IWannaBeTheBoshy.exe","debug")
report["startup"]["packaged-release"]=smoke(candidate/"IWannaBeTheBoshy.exe","release")
report["startup"]["extracted-unicode-release"]=smoke(portable/"IWannaBeTheBoshy.exe","unicode")
for relative,expected in manifest["files"].items():
    if hashlib.sha256((portable/relative).read_bytes()).hexdigest()!=expected: raise SystemExit(f"Extracted file differs: {relative}")
report["extracted_manifest"]="PASS"
(output/(name+".sha256")).write_text(report["sha256"]+"  "+archive.name+"\n",encoding="utf-8")
(output/(name+"-verification.json")).write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
print(json.dumps(report,indent=2),flush=True)
