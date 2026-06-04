# DiagonalChess — 可扩展棋类游戏框架 / Extensible Chess Game Framework

基于 SFML 2.6.2 + C++17。内置**对角象棋**（Diagonal Chess）和**五子棋**（Gomoku），通过抽象接口 + 工厂模式支持扩展新棋种。

Built with SFML 2.6.2 + C++17. Includes **Diagonal Chess** and **Gomoku**. Abstract interfaces + factory pattern support adding new games.

---

## 架构总览 / Architecture

```
                         ┌──────────────────────────┐
                         │ MainMenuScene  主菜单     │ 游戏选择界面
                         └──────────┬───────────────┘
                                    │ 点击游戏卡片
                                    ▼
                         ┌──────────────────────────┐
                         │   ChessPlatform  平台入口 │ 主循环 / 注册棋类
                         └──────────┬───────────────┘
                                    │
                         ┌──────────▼───────────────┐
                         │   GameManager   游戏管理器 │ 创建 / 切换游戏实例
                         └──────────┬───────────────┘
                                    │
                          ┌─────────▼─────────┐
                          │  Game  抽象接口    │ 仅 5 个纯虚方法: Update / Render / Reset / GetName / IsRunning
                          └─────────┬─────────┘
                                    │
                     ┌──────────────┼──────────────┐
                     │              │              │
             ┌──────▼─────┐ ┌──────▼─────┐ ┌──────▼─────┐
             │ 对角象棋    │ │  五子棋    │ │ 任意新棋种 │ 各自独立，互不依赖
             │ DiagonalChessGame     │ │ GomokuGame        │ │ Future ... │
             └──────┬─────┘ └────────────┘ └────────────┘
                    │
     ┌──────────────┼──────────────┬──────────────┬──────────────┬──────────────┐
     │              │              │              │              │              │
┌────▼───┐  ┌──────▼─────┐ ┌──────▼──┐  ┌──────▼─────┐ ┌─────▼───┐  ┌─────▼──────┐
│ Board  │  │    Rule    │ │   AI    │  │  Network   │ │   UI    │  │   Audio    │
│  棋盘  │  │    规则    │ │  决策   │  │    联机    │ │  界面   │  │   音效     │
│ (接口) │  │  (接口)    │ │         │  │            │ │         │  │            │
│        │  │            │ │         │  │            │ │         │  │            │
│GetPiece│  │IsValidMove │ │Think    │  │SendMove    │          │  │PlayMove    │
│SetPiece│  │IsGameOver  │          │  │PollNetwork │          │  │PlayClick   │
│TakePiece│ │HasLegalMoves│         │  │Callbacks   │          │  │            │
└───┬────┘  └────────────┘ └────────┘  └────────────┘ └────────┘  └────────────┘
    │                                                                      │
    │                                                                ┌─────▼──────┐
    │                                                                │  Particle  │
┌───▼──────┐                                                        │   System   │
│  Piece   │ 抽象基类, 各棋种子类化                                    │   粒子特效  │
│  (棋子)  │ DiagonalChessPiece / GomokuPiece / ...                  └────────────┘
│          │
│GetSymbol │
│GetType   │
│GetValue  │
│Clone     │
└──────────┘
```

---

## 目录结构 / Directory Structure

```
DiagonalChess/
│
├── include/Engine/
│   │
│   ├── Common.h                                 共享类型定义
│   │   ├── Side         阵营枚举（红方/黑方）
│   │   ├── AIDifficulty AI 难度枚举
│   │   ├── NetState     联机状态枚举
│   │   ├── MoveRecord   走棋记录结构体
│   │   ├── AIMove       AI 走法结构体
│   │   ├── UIButton     按钮结构体
│   │   ├── Particle     粒子结构体
│   │   ├── Move         走法结构体（轻量级四元组）
│   │   └── 棋盘常量     (DIAG, ORIGIN_X, WIN_W, PIECE_R, DRAW_LIMIT ...)
│   │
│   ├── Piece.h                                  Piece 棋子抽象基类
│   │   ├── GetSide() / IsAlive() / IsSameSide()
│   │   ├── GetSymbol() → wstring     棋子符号（虚函数）
│   │   ├── GetType()   → int         棋子类型（虚函数）
│   │   ├── GetValue()  → int         子力价值（虚函数）
│   │   └── Clone() → unique_ptr      深拷贝（虚函数）
│   │
│   ├── Piece/                                  棋子子类
│   │   ├── DiagonalChessPiece.h                对角象棋棋子（7 种兵种）
│   │   └── GomokuPiece.h                       五子棋棋子（黑白子）
│   │
│   ├── Board.h                                  Board 棋盘抽象接口
│   │   ├── GetRows() / GetCols()
│   │   ├── GetPiece(r,c) → Piece*              获取棋子
│   │   ├── SetPiece(r,c, unique_ptr<Piece>)    放置棋子（所有权转移）
│   │   ├── TakePiece(r,c) → unique_ptr<Piece>  取出棋子（所有权转移）
│   │   ├── Clear() / Reset() / IsInside()
│   │   └── ForEachPiece(fn)                    遍历所有活子
│   │
│   ├── Rule.h                                   Rule 规则抽象基类
│   │   └── IsValidMove() / IsGameOver() / IsInCheck() / HasLegalMoves()
│   │
│   ├── Core/                                   核心框架层
│   │   ├── Scene.h                  场景抽象基类（所有"画面"的基类）
│   │   │   └── Update(dt) / Render()           仅 2 个纯虚方法
│   │   ├── Game.h                   游戏抽象接口（继承 Scene）
│   │   │   └── Reset() / GetName() / IsRunning()额外 3 个方法
│   │   ├── IGameFactory.h           工厂接口
│   │   │   └── GetName() / Create()             Create() 返回 unique_ptr<Game>
│   │   ├── GameManager.h            游戏管理器
│   │   │   ├── RegisterGame(factory)           注册游戏工厂
│   │   │   ├── StartGame(name)                 按名称创建游戏实例
│   │   │   ├── ExitGame()                      退出当前游戏
│   │   │   ├── CurrentGame() → Game*
│   │   │   └── GetFactories() → vector
│   │   └── ChessPlatform.h          平台入口
│   │       ├── Run()                            驱动主循环
│   │       └── RegisterGame(factory)            注册棋类工厂
│   │
│   ├── Board/                                  棋盘实现
│   │   ├── DiagonalChessBoard.h                对角象棋棋盘（9×9，45°菱形投影）
│   │   │   ├── DiagonalChessPiece cells[9][9]  值类型存储
│   │   │   ├── bool occupied[9][9]             占位标记
│   │   │   ├── MovePieceInternal()             内部走子
│   │   │   ├── CopyTo(other)                   全盘深拷贝（AI 搜索用）
│   │   │   └── GridToScreen() / ScreenToGrid()  坐标转换
│   │   └── GomokuBoard.h                       五子棋棋盘（15×15，正交网格）
│   │       ├── int cells[15][15]               值存储
│   │       ├── PlaceStone() / RemoveStone()    落子/悔棋
│   │       └── GridToScreen() / ScreenToGrid()  坐标转换
│   │
│   ├── Rule/                                   规则引擎
│   │   ├── DiagonalChessRule.h                 对角象棋规则
│   │   │   ├── GetValidMoves(b, r, c)          获取某棋子全部合法走法
│   │   │   ├── IsValidMove()                   走法是否合法（7 兵种分发）
│   │   │   ├── IsInCheck()                     是否被将军
│   │   │   ├── IsCheckmate()                   是否被绝杀
│   │   │   ├── HasLegalMoves()                 是否存在合法走法
│   │   │   ├── WouldBeInCheck()                模拟走子后是否自将
│   │   │   ├── HasInsufficientMaterial()       子力不足和棋
│   │   │   ├── SideHasNoAttack()               某方是否无攻击子力
│   │   │   └── CanXXXMove()                    各兵种走法判定（7种）
│   │   └── GomokuRule.h                        五子棋规则
│   │       ├── IsValidPlace()                  落子合法性
│   │       ├── CheckWin(b, r, c, side)         五连判定（4方向扫描）
│   │       ├── CanWin(side)                    是否存在五连可能
│   │       ├── IsBoardFull()                   盘满检测
│   │       └── CountDir()                      单方向连续子计数
│   │
│   ├── AI/                                     AI 人工智能
│   │   ├── AIPlayer.h                 AI 抽象基类（仅 SetDifficulty 接口）
│   │   └── DiagonalChessAI.h          对角象棋 AI
│   │       ├── Think(b, s, r)         主决策入口
│   │       ├── SetDifficulty(d)       难度设置（影响搜索深度）
│   │       ├── UpdateDepth()          根据难度更新搜索深度
│   │       └── Minimax / GenerateAllMoves / Evaluate /
│   │           GetPieceValue / GetPositionBonus
│   │
│   ├── Network/NetworkManager.h           联机管理器
│   │   ├── StartHost()               创建房间（端口 55001）
│   │   ├── StartClient(ip)           加入房间
│   │   ├── Disconnect()              断开连接
│   │   ├── SendMove() / SendUndo... / SendRestart... /
│   │   │   SendSurrender... / SendDraw...      请求与应答
│   │   ├── PollNetwork()             轮询收包（每帧调用）
│   │   ├── GetState() / GetNetSide() / IsNetMode() / GetLocalIP()
│   │   └── 回调机制（std::function）:
│   │       onMoveReceived / onUndoRequest / onRestartRequest /
│   │       onSurrenderRequest / onDrawRequest /
│   │       onUndoAccepted / onRestartAccepted /
│   │       onSurrenderAccepted / onDrawAccepted /
│   │       onRejected / onDisconnected
│   │
│   ├── UI/UIManager.h                     UI 管理器
│   │   ├── Init(window, font, fontOk) 初始化
│   │   ├── DrawBoard()               绘制棋盘背景/网格线/九宫斜线/星位点
│   │   ├── DrawPieces()              绘制棋子（阴影/底色/高光/选中框/可行走标记）
│   │   ├── DrawUI()                  绘制标题栏/回合指示/将军警告/AI思考提示
│   │   ├── DrawButtons() / DrawRequestPopups()
│   │   ├── DrawMoveLog() / DrawGameOverEffect()
│   │   ├── DrawNetUI() / DrawTutorialPanel() / DrawSurrenderPopup()
│   │   ├── DrawNotification()        通用通知系统
│   │   ├── DrawText() / DrawTextWithShadow()
│   │   └── UpdateHover() / SetNotification()
│   │
│   ├── UI/MainMenuScene.h                  主菜单场景
│   │   ├── 列出所有已注册的游戏工厂
│   │   ├── 点击卡片 → HasPickedGame() → GetPickedGame()
│   │   └── 800×600 独立窗口，游戏卡片带悬停高亮
│   │
│   ├── Audio/AudioManager.h               音效管理器
│   │   ├── PlayMoveSound()           走子音效 (800Hz)
│   │   ├── PlayCaptureSound()        吃子音效 (600+900Hz)
│   │   ├── PlayWinSound()            胜利音效 (523→785Hz 滑音)
│   │   ├── PlayLoseSound()           失败音效 (440→330Hz 滑音)
│   │   ├── PlayDrawSound()           和棋音效 (440+554Hz)
│   │   ├── PlayClickSound()          点击音效 (1200Hz)
│   │   └── GenerateSounds()          程序化合成（无需外部文件）
│   │
│   ├── Effects/ParticleSystem.h           粒子特效系统
│   │   ├── Update(dt)                更新粒子
│   │   ├── Draw(window)              渲染粒子
│   │   ├── CreateWinParticles(winner)胜利粒子（×100）
│   │   ├── CreateDrawParticles()     和棋粒子（×80）
│   │   └── Clear()                   清空
│   │
│   └── Games/                                 具体游戏实现
│       ├── DiagonalChess/                     对角象棋
│       │   ├── DiagonalChessGame.h
│       │   │   ├── 组合: Board / Rule / AI / Network / UI / Audio / Particles
│       │   │   ├── Update(dt) / Render() / Reset()
│       │   │   ├── ProcessEvents() / HandleBoardClick()
│       │   │   ├── ExecuteMove() / UndoMove()
│       │   │   ├── RestartGame() / DoSurrender() / DoDraw()
│       │   │   ├── CheckGameEnd() / DoAITurn()
│       │   │   └── 状态: currentTurn / gameOver / winner / aiMode / moveHistory ...
│       │   └── DiagonalChessFactory.h
│       └── Gomoku/                             五子棋
│           ├── GomokuGame.h
│           │   ├── 自包含渲染: DrawBoard / DrawStones / DrawUI
│           │   ├── 15×15 棋盘，天元+四角星位
│           │   ├── 鼠标悬停预览落子
│           │   ├── ProcessEvents() / HandleBoardClick() / PlaceStone()
│           │   ├── CheckGameEnd() / UndoMove() / RestartGame()
│           │   └── 本地双人对战
│           └── GomokuFactory.h
│
├── src/Engine/                                 源代码（.cpp 实现）
│   ├── Piece.cpp
│   ├── Board/DiagonalChessBoard.cpp
│   ├── Board/GomokuBoard.cpp
│   ├── Rule/DiagonalChessRule.cpp
│   ├── Rule/GomokuRule.cpp
│   ├── AI/DiagonalChessAI.cpp
│   ├── Network/NetworkManager.cpp
│   ├── UI/UIManager.cpp
│   ├── UI/MainMenuScene.cpp
│   ├── Audio/AudioManager.cpp
│   ├── Effects/ParticleSystem.cpp
│   ├── Core/ChessPlatform.cpp
│   ├── Core/GameManager.cpp
│   ├── Games/DiagonalChess/DiagonalChessGame.cpp
│   └── Games/Gomoku/GomokuGame.cpp
│
├── src/main.cpp                                程序入口
├── CMakeLists.txt                              CMake 构建配置
├── build_local.bat                             MSYS2 本地编译脚本
└── README.md
```

---

## 核心模块 / Core Modules

### 1. Board 棋盘

抽象接口 `Board` 定义 `GetPiece` / `SetPiece` / `TakePiece`（所有权转移模式，`unique_ptr`），便于 AI 搜索时安全模拟走子。

| 实现 | 规格 | 存储 | 坐标系 |
|------|------|------|--------|
| `DiagonalChessBoard` | 9×9 | `DiagonalChessPiece cells[9][9]` 值类型 | 45°菱形投影 |
| `GomokuBoard` | 15×15 | `int cells[15][15]` 值类型 | 正交网格 |

坐标转换公式：`x = ORIGIN_X + (r+c)×DIAG`, `y = ORIGIN_Y + (c-r)×DIAG`

### 2. Rule 规则引擎

**DiagonalChessRule** — 完全无状态，纯函数式：
- 7 种兵种走法：车/马/象/士/将/炮/兵
- 蹩马腿、塞象眼、炮架计数
- 将军判断 + 自将过滤（`WouldBeInCheck`）
- 终局条件：绝杀 / 困毙 / 子力不足 / 自然限着(120步) / 认输 / 同意和棋

**GomokuRule** — 四方向五连扫描，盘满和棋检测，`CanWin()` 提前终局优化。

### 3. AI 人工智能

`DiagonalChessAI`：Minimax + Alpha-Beta 剪枝，三档难度（深度 2/3/4），评估 = 棋子价值 + 位置奖励。

| 棋子 | 将/帅 | 车 | 炮 | 马 | 象/士 | 兵 |
|------|-------|----|----|----|-------|----|
| 基础价值 | 10000 | 900 | 450 | 400 | 200 | 100 |

五子棋当前无 AI（本地双人对战）。

### 4. Network 联机

TCP Socket（端口 55001），15 种消息协议：

| 编号 | 消息 | 说明 |
|------|------|------|
| 0 | move | 走棋同步 |
| 1-3 | undo_req/acc/rej | 悔棋请求/同意/拒绝 |
| 4-6 | restart_req/acc/rej | 重开请求/同意/拒绝 |
| 7-8 | undo_ack / restart_ack | 步数确认 |
| 9-11 | surrender_req/acc/rej | 认输请求/同意/拒绝 |
| 12-14 | draw_req/acc/rej | 和棋请求/同意/拒绝 |

Host 执红先手，Client 执黑后手。12 个 `std::function` 回调通知游戏层。仅对角象棋使用。

### 5. UI 界面

**DiagonalChess（UIManager）**：菱形棋盘渲染、双层棋子/阴影/高光、选中高亮框、绿色可行走标记/红色攻击圈、回合指示器、将军警告、4态按钮、可拖拽走棋记录面板、IP输入框联机面板、结算遮罩（根据终局原因差异化显示）、25条教程面板、认输弹窗、通知系统、`sf::View` letterboxing 窗口缩放。

**Gomoku（自包含渲染）**：正交棋盘、棋子渐变高光、鼠标悬停半透明预览、简洁按钮系统、600×600 独立窗口。

### 6. Audio 音效

全部程序化合成（`sf::SoundBuffer::loadFromSamples`），无需外部音频文件。6 种音效（走子/吃子/胜利/失败/和棋/点击），正弦波+包络线。仅对角象棋使用。

### 7. ParticleSystem 粒子特效

胜利粒子：红蓝双色 ×100，喷发+重力，3秒。和棋粒子：金色 ×80，缓慢上升，2.5秒。Alpha = life/maxLife × 255。仅对角象棋使用。

---

## 平台层 / Platform Layer

```
MainMenuScene → ChessPlatform → GameManager → IGameFactory → Game
```

- **MainMenuScene**：列出已注册游戏工厂，卡片式布局，点击启动
- **ChessPlatform**：程序入口，驱动 `菜单 → 游戏 → 菜单` 循环
- **GameManager**：管理工厂列表，按名创建游戏实例
- **IGameFactory**：每个棋种实现 `GetName()` 和 `Create()` 即可接入

当前已注册游戏：

| 名称 | 描述 |
|------|------|
| DiagonalChess | 对角象棋：AI / 联机 / 音效 / 粒子特效 |
| Gomoku | 五子棋：本地双人对战 |

---

## 启动流程 / Launch Flow

```
main() → ChessPlatform::Run()
    ├→ 创建 MainMenuScene（列出所有注册的游戏）
    ├→ 用户点击游戏卡片
    ├→ MainMenuScene 关闭 → ChessPlatform 读取 launchTarget
    ├→ GameManager::StartGame(name) → 创建具体 Game 实例
    ├→ RunScene(game) → 驱动 Game 的 Update / Render 循环
    └→ Game 退出 → 回到 MainMenuScene（循环）
```

---

## 数据流 / Data Flow

```
User clicks board
    ↓
DiagonalChessGame::ProcessEvents（含 letterboxing 缩放）
    ↓
DiagonalChessGame::HandleBoardClick(r, c)
    ↓
DiagonalChessRule::GetValidMoves(board, r, c)
    ├── IsValidMoveRaw() → CanChariotMove ... 兵种分发
    └── WouldBeInCheck() → IsInCheckRaw()     防自将过滤
    ↓
User clicks target
    ↓
DiagonalChessGame::ExecuteMove(from, to)
    ├── MoveRecord 入栈 (moveHistory)
    ├── AudioManager::PlayMoveSound / PlayCaptureSound
    ├── NetworkManager::SendMove (联机模式)
    ├── ParticleSystem::CreateWinParticles (局终)
    ├── currentTurn 切换
    └── DiagonalChessRule::CheckGameEnd
        ├── HasInsufficientMaterial
        ├── DRAW_LIMIT 120步自然限着
        └── HasLegalMoves() + IsInCheck()
    ↓
If AI mode and AI's turn:
    DiagonalChessAI::Think(board, aiSide, rule)
        ├── GenerateAllMoves
        └── Minimax (depth 2-4, alpha-beta pruning)
    → ExecuteMove(best) after 1.5s delay
    ↓
Every frame:
UIManager::DrawBoard + DrawPieces + DrawUI + DrawButtons + ...
ParticleSystem::Update + Draw
NetworkManager::PollNetwork
```

---

## 编译 & 运行 / Build & Run

### 依赖 / Dependencies

- CMake 3.16+
- SFML 2.6.2
- MinGW-w64 (MSYS2 UCRT64) 或 Visual Studio 2019+
- Windows 系统字体：SimHei / Microsoft YaHei / SimSun / KaiTi

### 方式一：MSYS2 UCRT64 本地编译

```batch
build_local.bat
```

使用 g++ 直接编译所有 cpp 并链接 SFML（路径为 MSYS2 UCRT64 安装目录）。

### 方式二：CMake 构建

```powershell
cmake -S . -B build
cmake --build build --config Debug
# EXE 输出在项目根目录: DiagonalChess.exe
```

### SFML 路径配置

CMakeLists.txt 中 SFML 路径默认为 MSYS2 UCRT64 安装路径。若路径不同，修改 `CMakeLists.txt`：

```cmake
set(SFML_DIR "你的SFML路径 / your SFML path")
```

---

## 扩展新棋种 / Adding a New Game

以添加**中国象棋**为例：

### 1. 实现子类

```cpp
// include/Engine/Piece/ChineseChessPiece.h
class ChineseChessPiece : public Piece {
    ChessPieceType type;
    std::wstring GetSymbol() const override;
    int GetType() const override;
    int GetValue() const override;
    std::unique_ptr<Piece> Clone() const override;
};

// include/Engine/Board/ChineseChessBoard.h
class ChineseChessBoard : public Board {
    // 10×9 布局
};

// include/Engine/Rule/ChineseChessRule.h
class ChineseChessRule : public Rule {
    // 走法 / 将军 / 绝杀
};
```

### 2. 创建 Game 类

两种模式：
- **完整模式**（DiagonalChessGame 风格）：组合 Board + Rule + AI + Network + UIManager + Audio + Particles
- **简洁模式**（GomokuGame 风格）：自包含渲染，不依赖 UIManager/Audio/Particles

```cpp
// include/Engine/Games/ChineseChess/ChineseChessGame.h
class ChineseChessGame : public Game {
public:
    void Update(float dt) override;
    void Render() override;
    void Reset() override;
    std::string GetName() const override { return "ChineseChess"; }
    bool IsRunning() const override;
private:
    ChineseChessBoard board;
    ChineseChessRule rule;
    sf::RenderWindow window;
};
```

### 3. 创建工厂

```cpp
// include/Engine/Games/ChineseChess/ChineseChessFactory.h
class ChineseChessFactory : public IGameFactory {
public:
    std::string GetName() const override { return "ChineseChess"; }
    std::unique_ptr<Game> Create() override {
        return std::make_unique<ChineseChessGame>();
    }
};
```

### 4. 注册

```cpp
// src/main.cpp
int main() {
    ChessPlatform platform;
    platform.RegisterGame(std::make_unique<DiagonalChessFactory>());
    platform.RegisterGame(std::make_unique<GomokuFactory>());
    platform.RegisterGame(std::make_unique<ChineseChessFactory>()); // 新增
    platform.Run();
    return 0;
}
```

主菜单自动显示新注册的游戏。

---

## 五子棋 / Gomoku

五子棋作为平台中第二个棋种，与对角象棋共享抽象接口（`Game`、`Board`、`Rule`、`Piece`），但采取完全独立的实现路径：

- **独立渲染**：自行实现 `DrawBoard` / `DrawStones` / `DrawUI` / `DrawGameOver`，有独立 `RectButton` 结构，不使用 UIManager
- **独立棋子**：`GomokuPiece` 无类型枚举，仅表示黑白子
- **独立规则**：`GomokuRule` 四方向五连扫描，盘满和棋，`CanWin()` 提前终局优化
- **独立棋盘**：15×15，`GRID_SIZE = 40`，星位：天元(7,7) + 四角(3,3)/(3,11)/(11,3)/(11,11)
- **只依赖抽象接口**：不使用 AI / Network / Audio / Particles，仅依赖 `Board` 和 `Rule` 两个抽象接口

---

## 设计模式 / Design Patterns

| 模式 | 应用位置 |
|------|----------|
| 工厂模式 | `IGameFactory` → `DiagonalChessFactory` / `GomokuFactory` |
| 策略模式 | `Rule` / `Board` 抽象基类，各棋种不同实现 |
| 组合模式 | `DiagonalChessGame` 组合 Board + Rule + AI + Network + UI + Audio + Effects |
| 观察者模式 | `NetworkManager` 12 个 `std::function` 回调 |
| 模板方法 | `Game::Update / Render` 定义接口，子类实现 |

---

## 已知问题 / Known Issues

| # | 问题 | 说明 |
|---|------|------|
| 1 | 五子棋无 AI/联机 | 本地双人对战，无 AI、无联机、无音效、无粒子特效 |
| 2 | 窗口缩放部分支持 | DiagonalChessGame 支持 letterboxing；GomokuGame 不支持 |
| 3 | AI 评估可优化 | 可加入更精确的位置评估表提升棋力 |
| 4 | 联机仅 IP 直连 | 无房间列表 / 匹配功能 |
| 5 | 缺少单元测试 | 各模块未编写独立测试用例 |
