# I Wanna Be The Boshy


## 🧑‍💻 組員
- 112590011 
- 112590044

## 🕹️ 控制方式
- `Z`：跳躍（支援地面連跳 / 空中一段跳）
- `X`：射擊
- 方向鍵 ← → ↑ ↓：移動角色
- `R`：重生回到最近的 checkpoint


## 📆 開發日誌 

| 日期         | 內容概要                                                    |
|------------|---------------------------------------------------------|
| 2025/05/01 | ✅ 修正角色空中二段跳問題<br/>✅ 修正子彈擊中紀錄點會全部清除問題<br/>✅優化jumpBoost功能 |
| 2025/05/01 | ✅ 選單系統                                                  |
| 2025/05/02 | ✅ phase2trap碰撞<br/>✅ Boss1出生->移動邏輯                      |

# 備註
** 修改檔案：** `PTSD/include/config.hpp`
```c++
constexpr unsigned int WINDOW_WIDTH = 1280;
constexpr unsigned int WINDOW_HEIGHT = 960;

```