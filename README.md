# 2025 OOPL Final Report

## 組別資訊

組別：35
組員：112590011 林維信 112590044 邱翊豪
復刻遊戲：I Wanna Be The Boshy

## 專案簡介
本專案為 2025 年 NTUT《物件導向程式設計》期末報告，目標為復刻經典遊戲《I Wanna Be The Boshy》的部分功能，包括：

- 玩家控制（移動、跳躍、死亡重生）
- 基本平台與陷阱機制
- 儲存點設置與回溯機制
- Debug Mode：顯示遊戲內物件座標,作弊模式等等
- 記憶體管理與資源釋放（避免 Memory Leak）
### 遊戲簡介
《I Wanna Be The Boshy》是一款高難度平台跳躍遊戲，由玩家操控角色「Boshy」挑戰重重關卡與陷阱。遊戲特色包括大量突發陷阱、精準跳躍操作、多樣化 Boss 戰與懷舊遊戲致敬元素，是經典的自虐型遊戲代表作之一。
### 組別分工
| 112590011 | 林維信 |50%|負責素材製作 主functions編寫
| 112590044 | 邱翊豪 |50%|debug主functions 副functions編寫

## 遊戲介紹
本專案為復刻高難度平台跳躍遊戲《I Wanna Be The Boshy》，玩家需操控主角在充滿陷阱與障礙的關卡中存活並前進。遊戲講求操作精準度與反應速度，強調挑戰性與過關成就感。關卡中設有儲存點，玩家失敗後可即時重生，反覆嘗試直到成功。遊戲亦融入經典遊戲角色與元素，提升趣味性與懷舊感。

### 遊戲規則
玩家使用鍵盤wasd,z,x,r操作主角進行移動與跳躍。

關卡中充滿即死陷阱（如尖刺、落石、隱藏機關），碰到即死亡並回到最近的儲存點。

玩家可透過射擊儲存點來保存進度。

遊戲目標為避開陷阱並通過所有平台與障礙物，成功到達終點。

無限制重試次數，鼓勵玩家反覆挑戰直到成功。
### 遊戲畫面
![image](gamescen.png)

## 程式設計

### 程式架構
#### 🧩 關鍵類別架構與系統說明

| 🗂️ 系統分類       | 🔧 主要類別                                                                 | 📄 標頭檔案位置                                   |
|------------------|----------------------------------------------------------------------------|--------------------------------------------------|
| **核心系統 (App Core)**   | `App`, `App::GamePhase`, `App::State`                                 | `App.hpp`                                         |
| **玩家系統 (Player System)** | `Character`, `AnimatedCharacter`                                       | `Character.hpp`, `AnimatedCharacter.hpp`         |
| **敵人系統 (Enemy System)** | `bear`, `spider`, `Bird`                                                | `bear.hpp`, `MapObject/spider.hpp`, `MapObject/bird.hpp` |
| **Boss 系統 (Boss System)** | `Boss1`, `BulletTypeA/B/C`, `LightAttack`                              | `Boss1/*.hpp`                                     |
| **地圖系統 (Map System)**   | `MapInfoLoader`, `World`                                               | `MapInfoLoader.hpp`, `World.hpp`                 |
| **地圖物件 (Map Objects)**  | `CheckPoint`, `Platform`, `FallingGround`, `JumpBoost`                | `MapObject/*.hpp`                                 |
| **資源管理系統 (Resource System)** | `ResourceManager`, `BgmManager`                                 | `ResourceManager.hpp`, `BgmManager.hpp`          |

#### 項目文件總覽 📋

#### 📁 項目文件組織

| 🔍 類別 | 📄 檔案位置 | 🛠️ 功能說明 |
|:--------:|:------------|:------------|
| 🚀 **主入口** | `src/main.cpp` | 應用程式入口點和主遊戲迴圈 |
| 🧠 **核心遊戲邏輯** | `src/App.cpp`<br>`src/AppStart.cpp`<br>`src/AppUpdate.cpp` | 主遊戲應用流程控制 |
| 💾 **資源管理** | `src/ResourceManager.cpp`<br>`include/ResourceManager.hpp` | 加載和管理遊戲資源 |
| 👤 **角色系統** | `src/Character.cpp`<br>`src/AnimatedCharacter.cpp` | 玩家角色實現 |
| 👾 **敵人系統** | `src/enemy.cpp`<br>`src/Bird.cpp` | 敵人行為和互動 |
| 🗺️ **地圖系統** | `src/MapInfoLoader.cpp`<br>`include/MapObject/*.hpp` | 地圖加載和與地圖物件的互動 |
| 🎮 **遊戲物件** | `include/Bullet.hpp`<br>`src/bullet.cpp`, etc. | 各種遊戲物件實現 |
| 📱 **UI系統** | `src/Menu.cpp`<br>`include/Menu.hpp` | 菜單和用戶界面元素 |
| ⚙️ **建置配置** | `CMakeLists.txt`<br>`files.cmake` | 項目建置設置和文件組織 |
### 程式技術

## 結語

### 問題與解決方法
### 自評

| 項次 | 項目                   | 完成 |
|------|------------------------|-------|
| 1    | 這是範例 |  V  |
| 2    | 完成專案權限改為 public |  V  |
| 3    | 具有 debug mode 的功能  |  V  |
| 4    | 解決專案上所有 Memory Leak 的問題  |  V  |
| 5    | 報告中沒有任何錯字，以及沒有任何一項遺漏  |  V  |
| 6    | 報告至少保持基本的美感，人類可讀  |  V  |

### 心得
### 貢獻比例
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/andy0716NTUT/I-wanna-be-the-boshy)