"""Profile owned Release probes at normal cap; dummy audio, no desktop control."""
import argparse,csv,hashlib,json,os,subprocess,time,statistics
from pathlib import Path
import psutil
ROOT=Path(__file__).absolute().parents[1]
p=argparse.ArgumentParser();p.add_argument("--label",required=True);p.add_argument("--repeats",type=int,default=2);a=p.parse_args()
out=ROOT/"build/profiles"/a.label;out.mkdir(parents=True,exist_ok=True)
target=ROOT/"build/windows/bin/Release/BoshyCurrentRegression.exe"
report={"label":a.label,"git_head":subprocess.check_output(["git","rev-parse","HEAD"]).decode().strip(),
        "binary_sha256":hashlib.sha256(target.read_bytes()).hexdigest(),"psutil":psutil.__version__,
        "config_sha256":hashlib.sha256((target.parent/"config.json").read_bytes()).hexdigest(),"samples":[]}
for scenario in ("enemy","reload-stress","boss-death"):
 for repeat in range(a.repeats):
    name=f"{scenario}-{repeat}";profile=out/(name+"-profile.csv");trace=out/(name+".csv");log=out/(name+".log")
    env=os.environ.copy();env.update(SDL_AUDIODRIVER="dummy",BOSHY_PROFILE_PATH=str(profile),PATH="C:/Windows/System32;C:/Windows")
    startup=subprocess.STARTUPINFO();startup.dwFlags|=subprocess.STARTF_USESHOWWINDOW;startup.wShowWindow=0
    started=time.perf_counter();memory=[];cpu=[]
    with log.open("wb") as stream:
      child=subprocess.Popen([str(target),str(trace),scenario],cwd="C:/Windows",env=env,startupinfo=startup,stdout=stream,stderr=subprocess.STDOUT)
      process=psutil.Process(child.pid)
      while child.poll() is None:
        if time.perf_counter()-started>150: child.kill();child.wait();raise SystemExit("Profile timed out")
        try:
          info=process.memory_info();memory.append(info.private);t=process.cpu_times();cpu.append(t.user+t.system)
        except psutil.NoSuchProcess: pass
        time.sleep(.05)
    if child.returncode: raise SystemExit(f"Profile {name} exited {child.returncode}")
    with profile.open(encoding="utf-8") as f: rows=list(csv.DictReader(f,delimiter="|"))
    def metrics(key):
      values=sorted(float(r[key]) for r in rows)
      return {"mean":statistics.mean(values),"p95":values[int(.95*(len(values)-1))],"max":max(values)}
    sample={"case":scenario,"repeat":repeat,"elapsed_s":round(time.perf_counter()-started,4),"cpu_s":cpu[-1] if cpu else None,
      "private_peak_bytes":max(memory) if memory else None,"private_final_bytes":memory[-1] if memory else None,
      "startup_ms":float(rows[0]["startup_ms"]),"update_ms":metrics("update_ms"),"fixture_ms":metrics("fixture_ms"),
      "frame_ms":metrics("frame_ms"),"tracked_creates":{k:sum(int(r[k]) for r in rows) for k in ("images","animations","actors")},
      "log_bytes":log.stat().st_size,"rows":len(rows)}
    report["samples"].append(sample);(out/"report.json").write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(name,sample["update_ms"],sample["tracked_creates"],flush=True)
print(out/"report.json",flush=True)
