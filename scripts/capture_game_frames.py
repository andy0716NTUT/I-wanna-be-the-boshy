"""Export actual App + ImGui backbuffers without desktop automation; always silent."""
import hashlib,json,os,subprocess
from pathlib import Path
from PIL import Image
ROOT=Path(__file__).absolute().parents[1]
out=ROOT/"build/visual-review"
out.mkdir(parents=True,exist_ok=True)
report={"method":"actual OpenGL backbuffer before swap; test-only export", "cases":{}}
for name,version,scenario,frame,width,height in [
    ("original-native","Original","spikes",10,1280,960),
    ("current-native","Current","spikes",10,1280,960),
    ("current-fullhd","Current","spikes",10,1920,1080),
    ("current-4k","Current","spikes",10,3840,2160),
    ("current-player","Current","full-jump",25,1920,1080)]:
    env=os.environ.copy()
    env.update(SDL_AUDIODRIVER="dummy",BOSHY_VISUAL_WIDTH=str(width),BOSHY_VISUAL_HEIGHT=str(height),BOSHY_CAPTURE_PATH=str(out/(name+".ppm")))
    env["PATH"]="C:/Windows/System32;C:/Windows"
    startup=subprocess.STARTUPINFO();startup.dwFlags|=subprocess.STARTF_USESHOWWINDOW;startup.wShowWindow=0
    target=ROOT/f"build/windows/bin/Debug/Boshy{version}Regression.exe"
    with (out/(name+".log")).open("wb") as log:
        run=subprocess.run([str(target),str(out/(name+".csv")),scenario,str(frame)],cwd="C:/Windows",env=env,startupinfo=startup,stdout=log,stderr=subprocess.STDOUT,timeout=45)
    if run.returncode: raise SystemExit(f"Capture failed {name}: {run.returncode}")
    png=out/(name+".png")
    with Image.open(out/(name+".ppm")) as image: image.save(png)
    report["cases"][name]={"size":[width,height],"frame":frame,"scenario":scenario,"png_sha256":hashlib.sha256(png.read_bytes()).hexdigest()}
    print(f"Captured {name}: {png}",flush=True)
(out/"report.json").write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
