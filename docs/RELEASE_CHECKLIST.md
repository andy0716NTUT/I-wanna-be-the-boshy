# Internal Windows RC checklist

| Gate | Status | Evidence |
| --- | --- | --- |
| Phase 1/1.5/2/3/4/5 gates | PASS | REMASTER_PROGRESS.md and stage commits |
| Fresh Debug/Release + all CTest | PASS | windows-rc2, 20/20 each |
| Original/current gameplay | PASS | 19/19 each configuration; exact final runtime source identity |
| Pixel display and graphical review | PASS | real GL matrices and inspected GPU backbuffer artifacts |
| Assets/map format/coordinates | PASS | 153 originals; maps/masks cell-exact; no image/audio edits |
| Resources/shaders/config/DLL deployment | PASS | CTest + package manifest/recursive PE imports |
| ZIP extraction, Unicode path, unrelated cwd/minimal PATH | PASS | phase6-package.json |
| Invalid deployment fails with exact path | PASS | missing-resource negative test |
| Internal RC + checksum/known issues/licenses | PASS | RELEASE_REPORT.md and local build/release-candidate |
| Natural test-runner shutdown | PASS | regression + GL runners |
| Physical keyboard feel and audible devices | SUPPLEMENTAL MANUAL NOT_RUN | scripted input/dummy-device checks do not claim these |
| Other GPU/DPI/monitors and every route/seed | SUPPLEMENTAL MANUAL NOT_RUN | stated coverage bounds |
| Merge main / public binary release | NOT_PERFORMED | intentionally outside authorization |

All technical acceptance gates are complete within the stated measured scope.
Manual supplemental limits are not represented as passed. Follow RELEASE_REPORT.md
for exact commands, checksum and source revision. Never publish the internal ZIP
or merge main automatically. Original asset rights still require review before
any public distribution.
