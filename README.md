# 棋类游戏大全平台 / Chess Collection Platform

基于 SFML 2.6.2 + C++17 的可扩展棋类游戏平台。当前内置**对角象棋**（Diagonal Chess）和**五子棋**（Gomoku），并预留了插件式架构方便后续扩展中国象棋、国际象棋、围棋、军棋等棋种。

An extensible chess game platform built with SFML 2.6.2 + C++17. Currently features **Diagonal Chess** and **Gomoku**, with a plugin architecture ready for Chinese Chess, Chess, Go, Shogi, and more.

---

## 设计目标 / Design Goals

原项目存在典型的 **God Object** 反模式——单个 Game 类同时承担棋盘管理、棋子规则、胜负判定、AI、联机、UI、粒子特效、音效、历史记录、回合管理等十余项职责，导致：

- 类体积过大（Game.cpp 高达 2459 行）
- 耦合严重，无法独立测试
- 几乎无法扩展新棋种

重构后采用**组件化架构**，将原 Game 类拆分为 10 个独立模块，每个模块可通过接口替换。

The original project suffered from the classic **God Object** anti-pattern—a single `Game` class handling board management, piece rules, win/loss detection, AI, networking, UI, particle effects, audio, history, turn management, and more. This resulted in:

- Bloated class (Game.cpp: 2459 lines)
- Heavy coupling, impossible to unit test
- Nearly impossible to add new games

After refactoring, a **component architecture** splits the original Game class into 10 independent modules, each replaceable via interfaces.

---

## 架构总览 / Architecture

```
                         ┌──────────────────────────┐
                         │ MainMenuScene  主菜单     │ ← 游戏选择界面 / game launcher
                         └──────────┬───────────────┘
                                    │ 点击游戏卡片 / click tile
                                    ▼
                         ┌──────────────────────────┐
                         │   ChessPlatform  平台入口 │ ← 主循环 / 场景切换 / 注册棋类
                         └──────────┬───────────────┘
                                    │
                         ┌──────────▼───────────────┐
                         │   GameManager   游戏管理器 │ ← 创建 / 销毁 / 切换游戏实例
                         └──────────┬───────────────┘
                                    │
                         ┌──────────▼───────────────┐
                         │   Game  游戏（抽象基类）   │ ← Scene 接口: Update / Render / Reset / IsRunning
                         └──────────┬───────────────┘
                                    │
                    ┌───────────────┼───────────────┐
                    │               │               │
            ┌───────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
            │ 对角象棋游戏  │ │  五子棋游戏  │ │ 未来棋种... │ ← 具体棋类
            │ DiagonalChessGame       │ │  GomokuGame           │ │  Future ...│
            └───────┬──────┘ └─────────────┘ └─────────────┘
                    │
     ┌──────────────┼──────────────┬──────────────┬──────────────┬──────────────┐
     │              │              │              │              │              │
┌────▼────┐  ┌──────▼──────┐ ┌────▼───┐  ┌──────▼──────┐ ┌────▼────┐  ┌─────▼──────┐
│  Board  │  │    Rule     │ │   AI   │  │  Network    │ │   UI    │  │   Audio    │
│  棋盘   │  │    规则     │ │  决策  │  │    联机     │ │  界面   │  │   音效     │
│ (接口)  │  │  (接口)     │ │        │  │             │ │         │  │            │
│         │  │             │ │        │  │             │ │         │  │            │
│GetPiece │  │IsValidMove  │ │Think   │  │SendMove     │          │  │PlayMove    │
│SetPiece │  │IsGameOver   │          │  │PollNetwork  │          │  │PlayClick   │
│TakePiece│  │HasLegalMoves│          │  │Callbacks    │          │  │            │
└────┬─────┘  └─────────────┘ └────────┘  └─────────────┘ └─────────┘  └────────────┘
     │                                                                        │
     │                                                                  ┌─────▼──────┐
     │                                                                  │  Particle  │
┌────▼──────┐                                                           │    System  │
│   Piece   │ ← 抽象基类, 各棋种子类化                                    │    粒子特效 │
│  (棋子)   │    DiagonalChessPiece / GomokuPiece / 未来 ChessPiece 等    └────────────┘
│           │
│GetSymbol  │
│GetType    │
│GetValue   │
│Clone      │
└──────────┘
```

---

## 目录结构 / Directory Structure

```
DiagonalChess/                                   项目根目录
│
├── include/Engine/                              头文件
│   │
│   ├── Common.h                                 共享类型定义（通用声明）
│   │   ├── Side          阵营枚举（红方/黑方）
│   │   ├── AIDifficulty  AI 难度枚举（简单/中等/困难）
│   │   ├── NetState      联机状态枚举（离线/等待/连接中/已连接）
│   │   ├── MoveRecord    走棋记录结构体（起止坐标 + 棋子类型/阵营/被吃子 + 无吃子步数）
│   │   ├── AIMove        AI 走法结构体（起止坐标 + 评分）
│   │   ├── UIButton      按钮结构体（矩形区域 + 文本 + 悬停/禁用状态）
│   │   ├── Particle      粒子结构体（位置/速度/颜色/生命/大小）
│   │   ├── Move          走法结构体（起止坐标，轻量级四元组）
│   │   └── 棋盘常量      (DIAG=40 格距, ORIGIN_X=50 原点, WIN_W=1150 窗口宽,
│   │                       PIECE_R=20 棋子半径, DRAW_LIMIT=120 自然限着步数 ...)
│   │
│   ├── Piece.h                                   Piece 棋子抽象基类
│   │   ├── GetSide() / IsAlive() / IsSameSide()  阵营/存活/同阵营判断
│   │   ├── GetSymbol() → wstring     棋子符号（虚函数，各棋种实现）
│   │   ├── GetType()   → int         棋子类型（虚函数，各棋种定义自己的常量）
│   │   ├── GetValue()  → int         子力价值（虚函数）
│   │   └── Clone() → unique_ptr      深拷贝（虚函数，AI 搜索用）
│   │
│   ├── Piece/                                   棋子子类
│   │   ├── DiagonalChessPiece.h                 对角象棋棋子子类
│   │   │   ├── type: DChessPieceType  (NONE/CHARIOT/HORSE/ELEPHANT/ADVISOR/GENERAL/CANNON/SOLDIER)
│   │   │   ├── GetSymbol() → "帥"/"將"/"車"/"馬"/"相"/"仕"/"炮"/"兵" ...
│   │   │   └── GetValue()  → 10000/900/400/200/450/100
│   │   └── GomokuPiece.h                        五子棋棋子子类（黑白子，无类型区分）
│   │
│   ├── Board.h                                   Board 棋盘抽象接口
│   │   ├── GetRows() / GetCols()                 行列数
│   │   ├── GetPiece(r,c) → Piece*               获取棋子（只读/读写）
│   │   ├── SetPiece(r,c, unique_ptr<Piece>)     放置棋子（转移所有权）
│   │   ├── TakePiece(r,c) → unique_ptr<Piece>   取出棋子（取出所有权）
│   │   ├── Clear() / Reset()                    清空/重置
│   │   ├── IsInside(r,c) → bool                 坐标合法性检查
│   │   └── ForEachPiece(fn)                     遍历所有活子
│   │
│   ├── Rule.h                                   Rule 规则抽象基类
│   │   └── IsValidMove() / IsGameOver() / IsInCheck() / HasLegalMoves()
│   │                                            走法合法性/终局判定/将军/合法走法
│   │
│   ├── Core/                                    核心框架层
│   │   ├── Scene.h                    场景抽象基类（场景）
│   │   │   └── Update(dt) / Render()            更新/渲染
│   │   ├── Game.h                     游戏抽象基类（游戏）
│   │   │   └── Reset() / GetName() / IsRunning()重置/名称/运行状态
│   │   ├── IGameFactory.h             工厂接口（游戏工厂接口）
│   │   │   └── GetName() / Create()             名称/创建工厂方法
│   │   ├── GameManager.h              游戏管理器
│   │   │   ├── RegisterGame(factory)            注册游戏工厂
│   │   │   ├── StartGame(name)                  启动指定游戏
│   │   │   ├── ExitGame()                       退出当前游戏
│   │   │   ├── CurrentGame() → Game*            获取当前游戏指针
│   │   │   └── GetFactories() → vector<factory> 获取所有注册工厂
│   │   └── ChessPlatform.h            平台入口（平台主程序）
│   │       ├── Run()                             驱动主循环
│   │       └── RegisterGame(factory)             注册棋类工厂
│   │
│   ├── Board/                                   棋盘实现
│   │   ├── DiagonalChessBoard.h          对角象棋棋盘（实现 Board 接口）
│   │   │   ├── DiagonalChessPiece cells[9][9] 值类型存储（栈上，AI 拷贝快）
│   │   │   ├── bool occupied[9][9]            占位标记
│   │   │   ├── IsOccupied(r,c) / At(r,c)      快速读写棋子
│   │   │   ├── MovePieceInternal()            内部走子（连带更新 occupied）
│   │   │   ├── ClearCell() / OccupiedCell()   单格操作
│   │   │   ├── CopyTo(other)                  全盘深拷贝（AI 搜索用）
│   │   │   ├── GridToScreen(r, c)             坐标转换：网格 → 屏幕
│   │   │   └── ScreenToGrid(x, y)             坐标转换：屏幕 → 网格
│   │   └── GomokuBoard.h                  五子棋棋盘（15×15）
│   │       ├── int cells[15][15]             值存储
│   │       ├── GetCell() / SetCell()         读写格子
│   │       ├── IsEmpty() / IsOccupiedG()     空位/占位检测
│   │       ├── PlaceStone() / RemoveStone()  落子/悔棋
│   │       └── GridToScreen() / ScreenToGrid()坐标转换
│   │
│   ├── Rule/                                    规则引擎
│   │   ├── DiagonalChessRule.h            对角象棋规则引擎（对角象棋规则类）
│   │   │   ├── GetValidMoves(b, r, c)    获取某棋子全部合法走法
│   │   │   ├── IsValidMove()             走法是否合法（7 兵种分发）
│   │   │   ├── IsInCheck()               是否被将军
│   │   │   ├── IsCheckmate()             是否被绝杀
│   │   │   ├── HasLegalMoves()           是否存在合法走法
│   │   │   ├── WouldBeInCheck()          模拟走子后是否自将
│   │   │   ├── HasInsufficientMaterial() 子力不足和棋判断（双方均无攻击子力）
│   │   │   ├── SideHasNoAttack()         某方是否无攻击子力
│   │   │   └── CanXXXMove()              各兵种走法判定（7种）
│   │   └── GomokuRule.h                  五子棋规则引擎
│   │       ├── IsValidPlace()            落子合法性
│   │       ├── CheckWin(b, r, c, side)   五连判定（4方向扫描）
│   │       ├── CanWin(side)              是否存在五连可能
│   │       ├── IsBoardFull()             盘满检测
│   │       └── CountDir()                单方向连续子计数
│   │
│   ├── AI/                                      AI 人工智能
│   │   ├── AIPlayer.h                 AI 抽象基类（仅 Difficulty 设置接口）
│   │   │   └── SetDifficulty(d)      难度设置
│   │   └── DiagonalChessAI.h          对角象棋 AI（对角象棋 AI 类）
│   │       ├── Think()                主决策入口
│   │       ├── SetDifficulty(d)       难度设置（影响搜索深度）
│   │       ├── GetDifficulty() → enum 获取当前难度
│   │       ├── UpdateDepth()          根据难度更新搜索深度
│   │       └── Minimax / GenerateAllMoves / Evaluate /      搜索/生成走法/评估/
│   │           GetPieceValue / GetPositionBonus             棋子价值/位置奖励
│   │
│   ├── Network/NetworkManager.h            联机管理器（联机管理类）
│   │   ├── StartHost()               创建房间（监听 55001 端口）
│   │   ├── StartClient(ip)           加入房间
│   │   ├── Disconnect()              断开连接
│   │   ├── SendMove() / SendUndoXxx / SendRestartXxx /    走棋/悔棋/重开 请求与应答
│   │   │   SendSurrenderXxx / SendDrawXxx                   认输/和棋 请求与应答
│   │   ├── PollNetwork()             轮询收包（每帧调用）
│   │   ├── GetState() / GetNetSide() / IsNetMode() / GetLocalIP() 状态查询
│   │   └── 回调机制（std::function 成员变量）:
│   │       onMoveReceived / onUndoRequest / onRestartRequest /
│   │       onSurrenderRequest / onDrawRequest /
│   │       onUndoAccepted / onRestartAccepted /
│   │       onSurrenderAccepted / onDrawAccepted /
│   │       onRejected / onDisconnected
│   │
│   ├── UI/UIManager.h                      UI 管理器（界面管理类）
│   │   ├── Init(window, font, fontOk) 初始化绑定
│   │   ├── DrawBoard()               绘制棋盘背景 / 网格线 / 九宫斜线 / 星位点
│   │   ├── DrawPieces()              绘制棋子（阴影 / 底色 / 高光 / 选中框 / 可行走标记）
│   │   ├── DrawUI()                  绘制标题栏 / 回合指示 / 将军警告 / AI 思考提示
│   │   ├── DrawButtons() / DrawRequestPopups() 按钮 / 请求弹窗
│   │   ├── DrawMoveLog() / DrawGameOverEffect() 走棋记录 / 胜负结算
│   │   ├── DrawNetUI() / DrawTutorialPanel() / DrawSurrenderPopup() 联机/教程/认输弹窗
│   │   ├── DrawNotification()        通用通知系统（临时消息）
│   │   ├── DrawText() / DrawTextWithShadow() 文字渲染（含阴影）
│   │   └── UpdateHover() / SetNotification() 悬停检测 / 通知设置
│   │
│   ├── UI/MainMenuScene.h                  主菜单场景（前端平台 UI）
│   │   ├── 列出所有已注册的游戏工厂
│   │   ├── 点击卡片 → HasPickedGame() → GetPickedGame()
│   │   ├── ChessPlatform 读取 → GameManager::StartGame()
│   │   └── 800×600 独立窗口，游戏卡片带悬停高亮
│   │
│   ├── Audio/AudioManager.h               音效管理器（音效管理类）
│   │   ├── PlayMoveSound()           走子音效 (800Hz / 0.1s)
│   │   ├── PlayCaptureSound()        吃子音效 (600+900Hz / 0.15s)
│   │   ├── PlayWinSound()            胜利音效 (523→785Hz 滑音 / 0.8s)
│   │   ├── PlayLoseSound()           失败音效 (440→330Hz 滑音 / 0.6s)
│   │   ├── PlayDrawSound()           和棋音效 (440+554Hz / 0.5s)
│   │   ├── PlayClickSound()          点击音效 (1200Hz / 0.05s)
│   │   └── GenerateSounds()          程序化合成全部音频样本（无需外部文件）
│   │
│   ├── Effects/ParticleSystem.h           粒子特效系统（粒子系统类）
│   │   ├── Update(dt)                更新粒子位置 + 生命周期
│   │   ├── Draw(window)              渲染粒子
│   │   ├── CreateWinParticles(winner)胜利粒子（红/蓝双色 × 100 粒子）
│   │   ├── CreateDrawParticles()     和棋粒子（金色 × 80 粒子）
│   │   └── Clear()                   清空粒子
│   │
│   └── Games/                                  具体游戏实现
│       ├── DiagonalChess/                      对角象棋游戏
│       │   ├── DiagonalChessGame.h            对角象棋游戏类
│       │   │   ├── 组合模块: Board / Rule / AI / Network / UI / Audio / Particles
│       │   │   ├── Update(dt) / Render()     主更新/主渲染循环
│       │   │   ├── ProcessEvents()           事件处理（含窗口缩放 letterboxing）
│       │   │   ├── HandleBoardClick()        棋盘点击处理
│       │   │   ├── HandleButtonClick()       按钮点击处理
│       │   │   ├── ExecuteMove() / UndoMove()执行走棋/悔棋
│       │   │   ├── RestartGame() / DoSurrender() / DoDraw() 重开/认输/和棋
│       │   │   ├── CheckGameEnd() / DoAITurn()终局检测/AI 走棋（含 1.5s 思考延迟）
│       │   │   └── 状态: currentTurn / gameOver / winner / isDrawGame / surrendered /
│       │   │            agreedDraw / aiMode / aiSide / moveHistory / moveLogStrings...
│       │   └── DiagonalChessFactory.h         对角象棋工厂类
│       └── Gomoku/                             五子棋游戏
│           ├── GomokuGame.h                   五子棋游戏类
│           │   ├── 自包含渲染(self-contained): DrawBoard / DrawStones / DrawUI
│           │   ├── 15×15 棋盘，天元 + 四角星位标记
│           │   ├── 鼠标悬停预览落子位置（半透明棋子）
│           │   ├── ProcessEvents() / HandleBoardClick() / PlaceStone()
│           │   ├── CheckGameEnd(r,c,side)     五连检测 / 盘满和棋
│           │   ├── UndoMove() / RestartGame() / DoSurrender()
│           │   └── 本地双人对战（无 AI / 无联机 / 无音效）
│           └── GomokuFactory.h                 五子棋工厂类
│
├── src/Engine/                                  源代码（对应 .cpp 实现）
│   ├── Piece.cpp                                棋子基类 + DiagonalChessPiece + GomokuPiece 实现
│   ├── Board/DiagonalChessBoard.cpp             棋盘初始化 + 坐标转换 + 占位管理
│   ├── Board/GomokuBoard.cpp                    五子棋棋盘初始化
│   ├── Rule/DiagonalChessRule.cpp               规则引擎（走法/将军/终局判定）
│   ├── Rule/GomokuRule.cpp                      五子棋规则（五连/盘满判定）
│   ├── AI/DiagonalChessAI.cpp                   Minimax + Alpha-Beta 搜索
│   ├── Network/NetworkManager.cpp               TCP 联机 + 消息协议（15种）
│   ├── UI/UIManager.cpp                         界面渲染（棋盘/按钮/面板/教程/认输/通知弹窗）
│   ├── UI/MainMenuScene.cpp                     主菜单场景（游戏选择列表 UI）
│   ├── Audio/AudioManager.cpp                   音效程序化合成
│   ├── Effects/ParticleSystem.cpp               粒子物理更新 + 渲染
│   ├── Core/ChessPlatform.cpp                   平台主循环
│   ├── Core/GameManager.cpp                     游戏实例管理
│   ├── Games/DiagonalChess/DiagonalChessGame.cpp对角象棋（事件/走棋/联机/回合流程）
│   └── Games/Gomoku/GomokuGame.cpp              五子棋（点击/落子/五连检测/UI）
│
├── src/main.cpp                                 程序入口（注册工厂 → 启动平台 → 主菜单 → 游戏）
├── CMakeLists.txt                               CMake 构建配置（MSYS2 UCRT64 / Visual Studio）
├── build_local.bat                              MSYS2 UCRT64 本地编译脚本
├── build.bat                                    （已废弃，保留作参考）
└── README.md                                    本文件
```

---

## 核心模块详解 / Core Module Details

### 1. Board 棋盘模块

- 抽象接口 `Board`：`GetPiece` / `SetPiece` / `TakePiece` 采用**所有权转移**模式（`unique_ptr`），而非原地修改，便于 AI 搜索时安全地模拟走子
- **DiagonalChessBoard**：9×9 棋盘，`DiagonalChessPiece cells[9][9]` 值类型栈上存储，`CopyTo()` 全盘深拷贝，`MovePieceInternal()` 内部走子
- **GomokuBoard**：15×15 棋盘，`int cells[15][15]` 值存储，`PlaceStone()` / `RemoveStone()` 配合悔棋
- 坐标系转换：对角象棋使用 45° 菱形投影 `x = ORIGIN_X + (r+c)×DIAG`, `y = ORIGIN_Y + (c-r)×DIAG`；五子棋使用正交棋盘

### 2. Rule 规则引擎

**DiagonalChessRule**：
- 7 种兵种走法判定：车/马/象/士/将/炮/兵
- 蹩马腿检测（`IsBlockedHorse`）、塞象眼检测（`IsBlockedElephant`）、炮架计数（`CountPiecesBetween`）
- 将军判断：遍历对方棋子是否可攻击己方将/帅
- 模拟走子防自将：`WouldBeInCheck()` 在临时棋盘上测试
- 游戏结束条件（7种）：
  - **绝杀**（checkmate）：被将军且无合法走法
  - **困毙**（stalemate）：未被将军但无合法走法
  - **子力不足和棋**：双方均无攻击子力（车/马/象/炮/兵）
  - **自然限着和棋**：120 步内无吃子
  - **认输**
  - **双方同意和棋**
- 完全无状态，纯函数式，便于测试

**GomokuRule**：
- `CheckWin()` 四方向（横/纵/正斜/反斜）扫描五连
- `CanWin()` 预先判断某方是否仍可能五连（用于终局优化）
- `IsBoardFull()` 盘满和棋检测

### 3. AI 人工智能

- `AIPlayer` 为最简抽象接口（仅 `SetDifficulty()`），所有搜索逻辑在 `DiagonalChessAI`
- **搜索算法**：Minimax + Alpha-Beta 剪枝
- **三档难度**：简单(深度2) / 中等(深度3) / 困难(深度4)
- **评估函数**：棋子基础价值 + 位置奖励
  - 将/帅: 10000, 车: 900, 炮: 450, 马: 400, 象/士: 200, 兵: 100
  - 位置奖励：兵越前越值钱，马/车/炮居中加分
- AI 思考延迟 1.5 秒（模拟真实对战节奏）
- 五子棋当前无 AI（本地双人对战）

### 4. Network 联机模块

- **传输层**：TCP Socket（端口 55001）
- **协议**：15 种消息类型

| 编号 | 消息 | 说明 |
|------|------|------|
| 0 | move | 走棋同步 |
| 1 | undo_req | 悔棋请求 |
| 2 | undo_acc | 悔棋同意 |
| 3 | undo_rej | 悔棋拒绝 |
| 4 | restart_req | 重开请求 |
| 5 | restart_acc | 重开同意 |
| 6 | restart_rej | 重开拒绝 |
| 7 | undo_ack | 悔棋步数确认 |
| 8 | restart_ack | 重开确认 |
| 9 | surrender_req | 认输请求 |
| 10 | surrender_acc | 认输同意 |
| 11 | surrender_rej | 认输拒绝 |
| 12 | draw_req | 和棋请求 |
| 13 | draw_acc | 和棋同意 |
| 14 | draw_rej | 和棋拒绝 |

- **角色**：Host（执红先手） / Client（执黑后手）
- **回调机制**：所有网络事件通过 12 个 `std::function` 成员变量回调通知游戏层
- 支持：走棋同步、悔棋请求确认、重开请求确认、认输请求确认、和棋请求确认、超时断开重连
- 联机 UI：IP 输入框（支持退格删除 + Enter 确认）、本机 IP 显示、连接状态指示

### 5. UI 界面模块

**DiagonalChess 界面（UIManager）**：
- **棋盘渲染**：菱形背景、网格线、九宫斜线（对角线交叉）、星位圆点
- **棋子渲染**：双层圆（阴影+底色）、高光弧、选中高亮框、文名字体、可行走标记（绿色圆点/红色攻击圈）
- **信息面板**：标题栏、回合指示器（红黑圆点）、将军警告、AI 思考提示
- **按钮系统**：普通/悬停/禁用三态、4种按钮渲染样式
- **走棋记录**：可拖拽分隔条调整面板宽度、可滚动面板、交替行高亮
- **联机面板**：IP 输入框（支持实时编辑）、创建房间/加入房间/断开按钮、等待提示（显示本机 IP）、已连接状态
- **结算效果**：半透明遮罩 + 大字标题 + 小字说明 + 重新开始按钮，根据终局原因差异化显示（绝杀/困毙/困毙和棋/自然限着和棋/子力不足和棋/同意和棋/认输）
- **教程面板**：覆盖式半透明面板，25 条规则说明（含棋盘布局/兵种走法/胜负条件/和棋条件/联机说明/AI 设置）
- **认输弹窗**：居中面板、红方/黑方两个选项按钮、点击外部取消
- **通知系统**：`SetNotification(text, seconds)` 设置临时通知消息，`DrawNotification()` 渲染渐消文本（用于网络拒绝等提示）
- **窗口缩放**：`sf::View` viewport letterboxing 适配，棋盘内容保持比例缩放

**Gomoku 界面（GomokuGame 自包含渲染）**：
- 正交 15×15 棋盘，`GRID_SIZE = 40`，星位圆点（天元 + 四角）
- 棋子渲染：圆形 + 渐变底色 + 高光，文名字体（"⚫"/"⚪"）
- 鼠标悬停时半透明预览落子位置
- 简洁按钮系统：重新开始 / 悔棋 / 认输 / 返回
- 独立窗口（600×600），无联机/无音效/无粒子特效

### 6. Audio 音效模块

- **全部音效程序化合成**（`sf::SoundBuffer::loadFromSamples`），无需外部音频文件
- 6 种音效：走子(800Hz)、吃子(600+900Hz 双频)、胜利(上行滑音)、失败(下行滑音)、和棋(大三度双音)、点击(1200Hz 短促)
- 所有音效使用正弦波 + 包络线，生成 `std::vector<sf::Int16>` 样本
- 仅对角象棋使用，五子棋未接入音效

### 7. ParticleSystem 粒子特效系统

- 胜利粒子：红蓝双色 × 100 粒子，向上喷发，受重力下落，持续 3 秒
- 和棋粒子：金色 × 80 粒子，缓慢上升，持续 2.5 秒
- 粒子生命渐变：`alpha = (life / maxLife) × 255`
- 仅对角象棋使用，五子棋未接入

### 8. 平台层 Platform Layer

```
MainMenuScene + ChessPlatform + GameManager + IGameFactory
```

- **MainMenuScene**：前端菜单场景，列出所有注册的游戏，卡片式布局，悬停高亮，点击启动
- **ChessPlatform**：程序入口，拥有 `菜单 → 游戏 → 菜单` 的循环，通过 `RunScene()` 驱动任意 Scene
- **GameManager**：管理 `vector<unique_ptr<IGameFactory>>`，根据名称创建游戏实例，`GetFactories()` 供菜单读取
- **IGameFactory**：每个棋种提供一个工厂，返回 `unique_ptr<Game>`
- 扩展新棋种只需实现工厂接口并注册即可；菜单会自动显示新游戏

### 9. 启动流程 Launch Flow

```
main() → ChessPlatform::Run()
    ├→ 创建 MainMenuScene（列出所有注册的游戏）
    ├→ 用户点击游戏卡片
    ├→ MainMenuScene 关闭 → ChessPlatform 读取 launchTarget
    ├→ GameManager::StartGame(name) → 创建具体 Game 实例
    ├→ RunScene(game) → 驱动 Game 的 Update / Render 循环
    └→ Game 退出 → 回到 MainMenuScene（循环）
```

当前已注册游戏：
- **DiagonalChess**（对角象棋）：完整功能（AI/联机/音效/粒子）
- **Gomoku**（五子棋）：本地双人对战（无 AI/无联机/无音效）

---

## 数据流 / Data Flow

```
User clicks board / 用户点击棋盘
    ↓
DiagonalChessGame::ProcessEvents / 事件处理（含窗口缩放 letterboxing）
    ↓
DiagonalChessGame::HandleBoardClick(r, c) / 棋盘点击处理
    ↓
DiagonalChessRule::GetValidMoves(board, r, c) / 获取合法走法
    ├── IsValidMoveRaw() → CanChariotMove 车 / CanHorseMove 马 / ... 兵种分发
    └── WouldBeInCheck() → IsInCheckRaw()  ← 防自将过滤 / self-check filter
    ↓
User clicks target / 用户点击目标格
    ↓
DiagonalChessGame::ExecuteMove(from, to) / 执行走棋
    ├── MoveRecord push to stack / 走棋记录入栈 (moveHistory)
    ├── AudioManager::PlayMoveSound / PlayCaptureSound / 走子/吃子音效
    ├── NetworkManager::SendMove / 联机发送 (online mode / 联机模式)
    ├── ParticleSystem::CreateWinParticles / 胜利粒子 (if game ends / 如对局结束)
    ├── currentTurn toggle / 回合切换
    └── DiagonalChessRule::CheckGameEnd / 终局检测
        ├── HasInsufficientMaterial / 子力不足
        ├── DRAW_LIMIT check / 自然限着检测 (120 moves / 120步)
        └── HasLegalMoves() + IsInCheck() / 合法走法 + 将军检测
    ↓
If AI mode and AI's turn / AI 模式且轮到 AI:
    DiagonalChessAI::Think(board, aiSide, rule) / AI 决策
        ├── GenerateAllMoves / 全棋盘合法走法
        └── Minimax (depth 2-4, alpha-beta pruning / α-β剪枝)
    → ExecuteMove(best) after 1.5s delay / 延迟 1.5s 执行最佳走法
    ↓
Every frame / 每帧:
UIManager::DrawBoard + DrawPieces + DrawUI + DrawButtons + DrawMoveLog + ...
ParticleSystem::Update + Draw / 粒子更新 + 渲染
NetworkManager::PollNetwork / 轮询收包
```

---

## 编译 & 运行 / Build & Run

### 依赖 / Dependencies

- CMake 3.16+
- SFML 2.6.2
- MinGW-w64 (MSYS2 UCRT64) 或 Visual Studio 2019+
- Windows 系统字体：SimHei 黑体 / Microsoft YaHei 微软雅黑 / SimSun 宋体 / KaiTi 楷体

### 方式一：MSYS2 UCRT64 本地编译 / MSYS2 Build

使用 `build_local.bat`，需在 MSYS2 UCRT64 环境中运行：

```batch
build_local.bat
```

该脚本使用 g++ 直接编译所有 cpp 文件并链接 SFML 库（路径硬编码为 MSYS2 UCRT64 安装目录）。

### 方式二：CMake 构建 / CMake Build

```powershell
cmake -S . -B build
cmake --build build --config Debug
# EXE output at project root / EXE 输出在项目根目录: DiagonalChess.exe
```

### SFML 路径配置 / SFML Path Config

CMakeLists.txt 中 SFML 路径默认为 MSYS2 UCRT64 安装路径。若路径不同，修改 `CMakeLists.txt`：

```cmake
set(SFML_DIR "你的SFML路径 / your SFML path")
```

---

## 扩展新棋类示例 / Adding a New Game

以添加**中国象棋 Chinese Chess**为例 / Example:

### 1. 实现子类 / Implement Subclasses

```cpp
// include/Engine/Piece/ChineseChessPiece.h
class ChineseChessPiece : public Piece {
    ChessPieceType type; // KING/ROOK/KNIGHT/BISHOP/QUEEN/PAWN/CANNON
    std::wstring GetSymbol() const override;
    int GetType() const override;
    int GetValue() const override;
    std::unique_ptr<Piece> Clone() const override;
};

// include/Engine/Board/ChineseChessBoard.h
class ChineseChessBoard : public Board {
    // 10×9 布局 / 10x9 layout
};

// include/Engine/Rule/ChineseChessRule.h
class ChineseChessRule : public Rule {
    // 走法 / 将军 / 绝杀 / move / check / checkmate
};
```

### 2. 创建棋类 Game / Create the Game class

可选择两种模式：
- **完整模式**（如 DiagonalChessGame）：组合 Board + Rule + AI + Network + UIManager + Audio + Particles
- **简洁模式**（如 GomokuGame）：自包含渲染，不依赖 UIManager/Audio/Particles

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
    // ... 按需组合模块 / compose modules as needed
    sf::RenderWindow window;
};
```

### 3. 创建工厂 / Create the Factory

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

### 4. 注册 / Register

```cpp
// src/main.cpp
int main() {
    ChessPlatform platform;
    platform.RegisterGame(std::make_unique<DiagonalChessFactory>());  // 对角象棋
    platform.RegisterGame(std::make_unique<GomokuFactory>());         // 五子棋
    platform.RegisterGame(std::make_unique<ChineseChessFactory>());   // ← 新增 / new
    platform.Run();
    return 0;
}
```

无需修改任何核心代码 / No core code changes needed。主菜单自动显示新游戏。

---

## 设计模式 / Design Patterns

| 模式 | 中文名 | 应用位置 |
|------|--------|----------|
| **工厂模式** | Factory Pattern | `IGameFactory` → `DiagonalChessFactory` / `GomokuFactory` |
| **策略模式** | Strategy Pattern | `Rule` / `Board` 抽象基类，每个棋种提供不同实现 |
| **组合模式** | Composite Pattern | `DiagonalChessGame` 组合 Board + Rule + AI + Network + UI + Audio + Effects |
| **观察者模式** | Observer Pattern | `NetworkManager` 12 个回调（`std::function` 成员变量） |
| **模板方法** | Template Method | `Game::Update / Render` 定义接口，子类实现逻辑 |

## 设计原则 / Design Principles

- **SRP** 单一职责：每个模块职责唯一
- **OCP** 开闭原则：平台对扩展开放（注册新棋种），对修改封闭
- **LSP** 里氏替换：所有 Game 子类可替换使用
- **ISP** 接口隔离：Board / Rule / AI 各自独立的窄接口
- **DIP** 依赖反转：高层模块依赖抽象接口，不依赖具体实现

---

## 已知问题 / Known Issues

| # | 问题 Issue | 说明 Description |
|---|-----------|-----------------|
| 1 | 五子棋功能精简 | GomokuGame 无 AI、无联机、无音效、无粒子特效，仅为本地双人对战 |
| 2 | 窗口缩放部分支持 | DiagonalChessGame 支持缩放 letterboxing；GomokuGame 不支持 |
| 3 | AI 评估可优化 | 可加入更精确的位置评估表提升棋力 |
| 4 | 联机 UI 体验 | 仅支持 IP 直连，无房间列表 / 匹配功能 |
| 5 | 缺少单元测试 | 各模块未编写独立测试用例 |

---

## 五子棋补充说明 / Gomoku Notes

五子棋作为平台中第二个完整棋种，采用了更简化的架构：

- **不依赖共享 UI 组件**：GomokuGame 自行实现全部渲染逻辑（`DrawBoard` / `DrawStones` / `DrawUI` / `DrawGameOver`），有自己的 `RectButton` 结构
- **轻量级 Piece 子类**：`GomokuPiece` 无类型枚举，仅表示黑白子（`GetSymbol()` 返回空字符串，颜色由棋盘渲染侧区分）
- **规则引擎**：四方向五连扫描 + 盘满和棋 + 提前终局优化（`CanWin()`）
- **棋盘**：15×15，`GRID_SIZE = 40`，星位标记天元(7,7) + 四角(3,3)/(3,11)/(11,3)/(11,11)
- **交互**：鼠标悬停半透明预览，点击落子，支持悔棋（undo）和认输（surrender）
