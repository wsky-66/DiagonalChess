# Chess Collection Platform / 棋类游戏大全平台

基于 SFML 2.6.2 + C++17 的可扩展棋类游戏平台。当前内置**对角象棋**（Diagonal Chess），并预留了插件式架构方便后续扩展中国象棋、国际象棋、五子棋、围棋、军棋等棋种。

An extensible chess game platform built with SFML 2.6.2 + C++17. Currently features **Diagonal Chess**, with a plugin architecture ready for Chinese Chess, Chess, Gomoku, Go, and more.

---

## 设计目标 / Design Goals

原项目存在典型的 **God Object** 反模式——单个 Game 类同时承担棋盘管理、棋子规则、胜负判定、AI、联机、UI、粒子特效、音效、历史记录、回合管理等十余项职责，导致：

- 类体积过大（Game.cpp 高达 2459 行）
- 耦合严重，无法独立测试
- 几乎无法扩展新棋种

重构后采用**四层组件化架构**，将原 Game 类拆分为 10 个独立模块，每个模块可通过接口替换。

The original project suffered from the classic **God Object** anti-pattern—a single `Game` class handling board management, piece rules, win/loss detection, AI, networking, UI, particle effects, audio, history, turn management, and more. This resulted in:

- Bloated class (Game.cpp: 2459 lines)
- Heavy coupling, impossible to unit test
- Nearly impossible to add new games

After refactoring, a **4-layer component architecture** splits the original Game class into 10 independent modules, each replaceable via interfaces.

---

## 架构总览

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
            │ 对角象棋游戏  │ │ 中国象棋游戏 │ │ 五子棋游戏  │ ← 具体棋类
            │ DiagonalChessGame       │ │ ChineseChessGame      │ │  Gomoku ... │
            └───────┬──────┘ └─────────────┘ └─────────────┘
                    │
     ┌──────────────┼──────────────┬──────────────┬──────────────┬──────────────┐
     │              │              │              │              │              │
┌────▼────┐  ┌──────▼──────┐ ┌────▼───┐  ┌──────▼──────┐ ┌────▼────┐  ┌─────▼──────┐
│  Board  │  │    Rule     │ │   AI   │  │  Network    │ │   UI    │  │   Audio    │
│  棋盘   │  │    规则     │ │  决策  │  │    联机     │ │  界面   │  │   音效     │
│ (接口)  │  │  (接口)     │ │        │  │             │ │         │  │            │
│         │  │             │ │        │  │             │ │         │  │            │
│GetPiece│  │IsValidMove│ │Think  │  │SendMove  │          │  │PlayMove  │
│SetPiece│  │IsGameOver │          │  │PollNetwork│          │  │PlayClick │
│MovePiece│ │HasLegalMvs│          │  │Callbacks  │          │  │            │
└────┬─────┘  └─────────────┘ └────────┘  └─────────────┘ └─────────┘  └────────────┘
     │                                                                        │
     │                                                                  ┌─────▼──────┐
     │                                                                  │  Particle  │
┌────▼──────┐                                                           │    System  │
│   Piece   │ ← 抽象基类, 各棋种子类化                                    │    粒子特效 │
│  (棋子)   │    DiagonalChessPiece / 未来可以还有 ChessPiece 等           └────────────┘
│           │
│GetSymbol│
│GetType  │
│GetValue │
│Clone    │
└──────────┘
```

---

## 目录结构

```
testchess/                                       项目根目录
│
├── include/Engine/                              头文件
│   │
│   ├── Common.h                                 共享类型定义（通用声明）
│   │   ├── Side          阵营枚举（红方/黑方）
│   │   ├── AIDifficulty  AI 难度枚举（简单/中等/困难）
│   │   ├── NetState      联机状态枚举（离线/等待/连接中/已连接）
│   │   ├── MoveRecord    走棋记录结构体（起止坐标 + 棋子类型/阵营）
│   │   ├── AIMove        AI 走法结构体（起止坐标 + 评分）
│   │   ├── UIButton      按钮结构体（矩形区域 + 文本 + 悬停/禁用状态）
│   │   ├── Particle      粒子结构体（位置/速度/颜色/生命/大小）
│   │   ├── Move          走法结构体（起止坐标）
│   │   └── 棋盘常量      (DIAG=40 格距, ORIGIN_X=50 原点, WIN_W=1150 窗口宽,
│   │                       PIECE_R=20 棋子半径, DRAW_LIMIT=120 自然限着步数 ...)
│   │
│   ├── Piece.h                                   Piece 棋子抽象基类
│   │   ├── GetSide() / IsAlive() / IsSameSide()  阵营/存活/同阵营判断
│   │   ├── GetSymbol() → wstring     棋子符号（虚函数，各棋种实现）
│   │   ├── GetType()   → int         棋子类型（虚函数，各棋种定义自己的常量）
│   │   ├── GetValue()  → int         子力价值（虚函数）
│   │   ├── Clone() → unique_ptr      深拷贝（虚函数，AI 搜索用）
│   │   └── DiagonalChessPiece        对角象棋棋子子类
│   │       ├── type: DChessPieceType  (NONE/CHARIOT/HORSE/ELEPHANT/ADVISOR/GENERAL/CANNON/SOLDIER)
│   │       ├── GetSymbol() → "帥"/"將"/"車"/"馬"/"相"/"仕"/"炮"/"兵" ...
│   │       └── GetValue()  → 10000/900/400/200/450/100
│   │
│   ├── Board.h                                   Board 棋盘抽象接口
│   │   ├── GetRows() / GetCols()                 行列数
│   │   ├── GetPiece(r,c) → Piece*               获取棋子（只读/读写）
│   │   ├── SetPiece(r,c, unique_ptr<Piece>)     放置棋子
│   │   ├── TakePiece(r,c) → unique_ptr<Piece>   取出棋子
│   │   ├── Clear() / Reset()                    清空/重置
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
│   │   │   └── CurrentGame() → Game*            获取当前游戏指针
│   │   └── ChessPlatform.h            平台入口（平台主程序）
│   │       ├── Run()                             驱动主循环
│   │       └── RegisterGame(factory)             注册棋类工厂
│   │
│   ├── Board/DiagonalChessBoard.h          对角象棋棋盘（实现 Board 接口）
│   │   ├── DiagonalChessPiece cells[9][9] 值类型存储（栈上，AI 拷贝快）
│   │   ├── bool occupied[9][9]            占位标记
│   │   ├── IsOccupied(r,c) / At(r,c)      快速读写棋子
│   │   ├── MovePieceInternal()            内部走子（连带更新 occupied）
│   │   ├── ClearCell() / OccupiedCell()   单格操作
│   │   ├── CopyTo(other)                  全盘深拷贝（AI 搜索用）
│   │   ├── GridToScreen(r, c)             坐标转换：网格 → 屏幕
│   │   └── ScreenToGrid(x, y)             坐标转换：屏幕 → 网格
│   │
│   ├── Rule/DiagonalChessRule.h            对角象棋规则引擎（对角象棋规则类）
│   │   ├── GetValidMoves(b, r, c)    获取某棋子全部合法走法
│   │   ├── IsValidMove()             走法是否合法（7 兵种分发）
│   │   ├── IsInCheck()               是否被将军
│   │   ├── IsCheckmate()             是否被绝杀
│   │   ├── HasLegalMoves()           是否存在合法走法
│   │   ├── WouldBeInCheck()          模拟走子后是否自将
│   │   ├── HasInsufficientMaterial() 子力不足和棋判断
│   │   ├── IsGameOver()              游戏是否结束
│   │   └── CanXXXMove()              各兵种走法判定
│   │       CanChariotMove 车 / CanHorseMove 马 / CanElephantMove 象 /
│   │       CanAdvisorMove 士 / CanGeneralMove 将 / CanCannonMove 炮 / CanSoldierMove 兵 +
│   │       IsBlockedHorse 蹩马腿 / IsBlockedElephant 塞象眼 / CountPiecesBetween 炮架计数
│   │
│   ├── AI/                                      AI 人工智能
│   │   ├── AIPlayer.h                 AI 抽象基类（AI 玩家基类）
│   │   │   └── Think(board,side,rule)→AIMove   决策入口
│   │   └── DiagonalChessAI.h          对角象棋 AI（对角象棋 AI 类）
│   │       ├── Think()                主决策入口
│   │       ├── SetDifficulty(d)       难度设置（影响搜索深度）
│   │       └── Minimax / GenerateAllMoves / Evaluate /      搜索/生成走法/评估/
│   │           GetPieceValue / GetPositionBonus /            棋子价值/位置奖励/
│   │           MakeMoveAI / UndoMoveAI                       走子/悔棋
│   │
│   ├── Network/NetworkManager.h            联机管理器（联机管理类）
│   │   ├── StartHost()               创建房间（监听 55001 端口）
│   │   ├── StartClient(ip)           加入房间
│   │   ├── Disconnect()              断开连接
│   │   ├── SendMove()…               走棋/悔棋/重开/认输/和棋 请求与应答
│   │   ├── PollNetwork()             轮询收包（每帧调用）
│   │   └── 回调机制（std::function）:
│   │       onMoveReceived 收到走子 / onUndoRequestReceived 收到悔棋请求 /
│   │       onRestartRequestReceived 收到重开请求 / onSurrenderRequestReceived 收到认输请求 /
│   │       onDrawRequestReceived 收到和棋请求 / onUndoAccepted 悔棋被同意 /
│   │       onRestartAccepted 重开被同意 / onSurrenderAccepted 认输被同意 /
│   │       onDrawAccepted 和棋被同意 / onRejected 被拒绝 / onDisconnected 断开连接
│   │
│   ├── UI/UIManager.h                      UI 管理器（界面管理类）
│   │   ├── Init(window, font)        初始化绑定
│   │   ├── DrawBoard()               绘制棋盘背景 / 网格线 / 九宫斜线 / 星位点
│   │   ├── DrawPieces()              绘制棋子（阴影 / 底色 / 高光 / 选中框 / 可行走标记）
│   │   ├── DrawUI()                  绘制标题栏 / 回合指示 / 将军警告 / AI 思考提示
│   │   ├── DrawButtons() / DrawRequestPopups() 按钮 / 请求弹窗
│   │   ├── DrawMoveLog() / DrawGameOverEffect()
│   │   ├── DrawNetUI() / DrawTutorialPanel() / DrawSurrenderPopup()
│   │   └── DrawText() / UpdateHover() 文字渲染 / 悬停检测
│   │
│   ├── UI/MainMenuScene.h                  主菜单场景（前端平台 UI）
│   │   ├── 列出所有已注册的游戏工厂
│   │   ├── 点击卡片 → HasPickedGame() → GetPickedGame()
│   │   └── ChessPlatform 读取→ GameManager::StartGame()
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
│   │   ├── Draw()                    渲染粒子
│   │   ├── CreateWinParticles()      胜利粒子（红/蓝双色 × 100 粒子）
│   │   ├── CreateDrawParticles()     和棋粒子（金色 × 80 粒子）
│   │   └── Clear()                   清空粒子
│   │
│   └── Games/DiagonalChess/                  对角象棋游戏
│       ├── DiagonalChessGame.h            对角象棋游戏（对角象棋游戏类）
│       │   ├── 组合模块: Board / Rule / AI / Network / UI / Audio / Particles
│       │   ├── Update(dt) / Render()     主更新/主渲染循环
│       │   ├── ProcessEvents()           事件处理
│       │   ├── HandleBoardClick()        棋盘点击处理
│       │   ├── HandleButtonClick()       按钮点击处理
│       │   ├── ExecuteMove() / UndoMove()执行走棋/悔棋
│       │   ├── RestartGame() / DoSurrender() / DoDraw()
│       │   │                             重开/认输/和棋
│       │   ├── CheckGameEnd() / DoAITurn()终局检测/AI 走棋
│       │   └── 状态: currentTurn 当前回合/ gameOver 游戏结束/
│       │            winner 胜者/ isDrawGame 和棋/ surrendered 认输/
│       │            agreedDraw 同意和棋/ aiMode AI 模式/
│       │            aiSide AI 阵营/ moveHistory 历史栈/
│       │            moveLogStrings 走棋记录...
│       └── DiagonalChessFactory.h         对角象棋工厂（对角象棋工厂类）
│           └── Create() → make_unique<DiagonalChessGame>()
│
├── src/Engine/                                  源代码（对应 .cpp 实现）
│   ├── Piece.cpp                                棋子基类 + 对角象棋棋子实现
│   ├── Board/DiagonalChessBoard.cpp             棋盘初始化 + 坐标转换 + 占位管理
│   ├── Rule/DiagonalChessRule.cpp               规则引擎（走法/将军/终局判定）
│   ├── AI/DiagonalChessAI.cpp                   Minimax + Alpha-Beta 搜索
│   ├── Network/NetworkManager.cpp               TCP 联机 + 消息协议
│   ├── UI/UIManager.cpp                         界面渲染（棋盘/按钮/面板/教程/认输弹窗）
│   ├── UI/MainMenuScene.cpp                     主菜单场景（游戏选择列表 UI）
│   ├── Audio/AudioManager.cpp                   音效程序化合成
│   ├── Effects/ParticleSystem.cpp               粒子物理更新 + 渲染
│   ├── Core/ChessPlatform.cpp                   平台主循环
│   ├── Core/GameManager.cpp                     游戏实例管理
│   └── Games/DiagonalChess/DiagonalChessGame.cpp对角象棋（事件/走棋/联机/回合流程）
│
├── src/main.cpp                                 程序入口（注册工厂 → 启动平台 → 主菜单 → 游戏）
├── CMakeLists.txt                               CMake 构建配置
└── README.md                                    本文件
```

---

## 核心模块详解

### 1. Board 棋盘模块

```
Board（棋盘抽象基类） → DiagonalChessBoard（对角象棋棋盘）
```

- 管理 9×9 二维 Piece 数组
- 提供初始棋子布局 `PlacePieces()`（双方各 16 枚棋子）
- 坐标系转换：`GridToScreen(行, 列)` / `ScreenToGrid(x, y)`
- 斜 45° 棋盘映射公式：`x = ORIGIN_X + (r+c)×DIAG`, `y = ORIGIN_Y + (c-r)×DIAG`

### 2. Rule 规则引擎

```
Rule（规则抽象基类） → DiagonalChessRule（对角象棋规则引擎）
```

- 7 种兵种走法判定：车/马/象/士/将/炮/兵
- 蹩马腿检测、塞象眼检测、炮架计数
- 将军判断：遍历对方所有棋子是否能攻击到己方将/帅
- 模拟走子防自将：`WouldBeInCheck()` 在临时棋盘上测试
- 游戏结束条件：绝杀/困毙/子力不足/自然限着(120步)
- 完全无状态，纯函数式，便于测试

### 3. AI 人工智能

```
AIPlayer（AI 抽象基类） → DiagonalChessAI（对角象棋 AI）
```

- **搜索算法**：Minimax + Alpha-Beta 剪枝
- **三档难度**：简单(深度2) / 中等(深度3) / 困难(深度4)
- **评估函数**：棋子基础价值 + 位置奖励
  - 将/帅: 10000, 车: 900, 炮: 450, 马: 400, 象/士: 200, 兵: 100
  - 位置奖励：兵越前越值钱，马/车/炮居中加分
- 每步决策延迟 1.5 秒（模拟思考）

### 4. Network 联机模块

```
NetworkManager（联机管理器）
```

- **传输层**：TCP Socket（端口 55001）
- **协议**：14 种消息类型（move/undo/restart/surrender/draw 的 request/accept/reject + ack）
- **角色**：Host（执红先手） / Client（执黑后手）
- **回调机制**：所有网络事件通过 `std::function` 回调通知游戏层
- 支持：走棋同步、悔棋请求确认、重开请求确认、认输请求确认、和棋请求确认、超时断开重连

### 5. UI 界面模块

```
UIManager（界面管理器）
```

- **棋盘渲染**：菱形背景、网格线、九宫斜线（对角线交叉）、星位小圆点
- **棋子渲染**：双层圆（阴影+底色）、高光弧、选中高亮框、文名字体、可行走标记（绿色圆点/红色攻击圈）
- **信息面板**：标题栏、回合指示器（红黑圆点）、将军警告、AI 思考提示、通知弹窗
- **按钮系统**：正常/悬停/禁用三态、统一绘制函数
- **走棋记录**：可拖拽分隔条、可滚动面板、交替行高亮
- **联机面板**：IP 输入框、创建房间/加入房间/断开按钮、等待提示（显示本机IP）、已连接状态
- **结算效果**：半透明遮罩、大字标题（绝杀/困毙/认输/和棋）、小字说明、重新开始按钮
- **教程面板**：覆盖式半透明面板，18 条规则说明
- **认输弹窗**：居中面板、红方/黑方两个选项按钮、点击外部取消

### 6. Audio 音效模块

```
AudioManager（音效管理器）
```

- **全部音效程序化合成**（`sf::SoundBuffer::loadFromSamples`），无需外部音频文件
- 6 种音效：走子(800Hz)、吃子(600+900Hz 双频)、胜利(上行滑音)、失败(下行滑音)、和棋(大三度双音)、点击(1200Hz 短促)
- 所有音效使用正弦波 + 包络线，生成 `std::vector<sf::Int16>` 样本

### 7. ParticleSystem 粒子特效系统

```
ParticleSystem（粒子系统）
```

- 胜利粒子：红蓝双色 × 100 粒子，向上喷发，受重力下落，持续 3 秒
- 和棋粒子：金色 × 80 粒子，缓慢上升，持续 2.5 秒
- 粒子属性：位置/速度/颜色/生命/最大生命/大小
- Alpha 渐消：`alpha = (life / maxLife) × 255`

### 8. 平台层 Platform Layer

```
MainMenuScene + ChessPlatform + GameManager + IGameFactory
```

- **MainMenuScene**：前端菜单场景，列出所有注册的游戏，点击启动
- **ChessPlatform**：程序入口，拥有菜单→游戏→菜单的循环，通过 `RunScene()` 驱动任意 Scene
- **GameManager**：管理 `vector<unique_ptr<IGameFactory>>`，根据名称创建游戏实例
- **IGameFactory**：每个棋种提供一个工厂，返回 `unique_ptr<Game>`
- 扩展新棋种只需实现工厂接口并注册即可；菜单会自动显示新游戏

### 9. 启动流程 Launch Flow

```
main() → ChessPlatform::Run()
    ├→ 显示 MainMenuScene（列出所有注册的游戏）
    ├→ 用户点击游戏卡片
    ├→ MainMenuScene 关闭 → ChessPlatform 读取 launchTarget
    ├→ GameManager::StartGame(name) → 创建具体 Game 实例
    ├→ RunScene(game) → 驱动 Game 的 Update / Render 循环
    └→ Game 退出 → 回到 MainMenuScene（循环）
```

---

## 数据流 / Data Flow

```
User clicks board / 用户点击棋盘
    ↓
DiagonalChessGame::ProcessEvents / 事件处理
    ↓
DiagonalChessGame::HandleBoardClick(r, c) / 棋盘点击处理
    ↓
DiagonalChessRule::GetValidMoves(board, r, c) / 获取合法走法
    ├── IsValidMove() → CanChariotMove 车 / CanHorseMove 马 / ... 兵种分发
    └── WouldBeInCheck() → IsInCheckB()  ← 防自将过滤 / self-check filter
    ↓
User clicks target / 用户点击目标格
    ↓
DiagonalChessGame::ExecuteMove(from, to) / 执行走棋
    ├── MoveRecord push to stack / 走棋记录入栈 (moveHistory)
    ├── AudioManager::PlayMoveSound / PlayCaptureSound / 走子/吃子音效
    ├── NetworkManager::SendMove / 联机发送 (online mode / 联机模式)
    ├── ParticleSystem::CreateWinParticles / 胜利粒子 (if capture general / 如吃将)
    ├── currentTurn toggle / 回合切换
    └── DiagonalChessRule::CheckGameEnd / 终局检测
        ├── HasInsufficientMaterial / 子力不足
        ├── DRAW_LIMIT check / 自然限着检测 (120 moves / 120步)
        └── HasLegalMoves + IsInCheck / 合法走法 + 将军检测
    ↓
If AI mode and AI's turn / AI 模式且轮到 AI:
    DiagonalChessAI::Think(board, aiSide, rule) / AI 决策
        ├── GenerateAllMoves / 全棋盘合法走法
        └── Minimax (depth 2-4, alpha-beta pruning / α-β剪枝)
    → ExecuteMove(best) / 执行最佳走法
    ↓
Every frame / 每帧:
UIManager::DrawBoard / 绘制棋盘 + DrawPieces / 绘制棋子 + DrawUI / 绘制界面 + DrawButtons / 绘制按钮
ParticleSystem::Update / 粒子更新 + Draw / 粒子渲染
```

---

## 编译 & 运行 / Build & Run

### 依赖 / Dependencies

- Visual Studio 2019+
- CMake 3.16+
- SFML 2.6.2 (located at `../SFML-2.6.2` relative path / 位于相对路径 `../SFML-2.6.2`)
- Windows system fonts: SimHei 黑体 / Microsoft YaHei 微软雅黑 / SimSun 宋体

### 构建 / Build

```powershell
cmake -S . -B build
cmake --build build --config Debug
# EXE output at project root / EXE 输出在项目根目录: DiagonalChess.exe
```

### SFML 路径配置 / SFML Path Config

If SFML is not at `../SFML-2.6.2` / 若 SFML 不在此路径, modify `CMakeLists.txt` line 8 / 修改第 8 行:

```cmake
set(SFML_DIR "your SFML path / 你的SFML路径")
```

---

## 扩展新棋类示例 / Adding a New Game

以添加**中国象棋 Chinese Chess**为例 / Example:

### 1. 创建棋类 Game / Create the Game class

```cpp
// include/Engine/Games/ChineseChess/ChineseChessGame.h
// 中国象棋游戏类
class ChineseChessGame : public Game {
public:
    void Update(float dt) override;     // 更新
    void Render() override;             // 渲染
    void Reset() override;              // 重置
    std::string GetName() const override { return "ChineseChess"; }
    bool IsRunning() const override { return window.isOpen(); }
private:
    ChineseChessBoard board;            // 10×9 棋盘 / board
    ChineseChessRule rule;              // 中国象棋规则 / rules
    ChineseChessAI ai;                  // AI
    NetworkManager network;             // 复用现有联机 / reuse network
    UIManager ui;                       // 复用现有 UI / reuse UI
    AudioManager audio;                 // 复用现有音效 / reuse audio
    ParticleSystem particles;           // 复用现有粒子 / reuse particles
    sf::RenderWindow window;            // 窗口
    sf::Font font;                      // 字体
    // ...
};
```

### 2. 创建工厂 / Create the Factory

```cpp
// include/Engine/Games/ChineseChess/ChineseChessFactory.h
// 中国象棋工厂类
class ChineseChessFactory : public IGameFactory {
public:
    std::string GetName() const override { return "ChineseChess"; }
    std::unique_ptr<Game> Create() override {
        return std::make_unique<ChineseChessGame>();
    }
};
```

### 3. 注册 / Register

```cpp
// src/main.cpp
int main() {
    ChessPlatform platform;
    platform.RegisterGame(std::make_unique<DiagonalChessFactory>());    // 对角象棋
    platform.RegisterGame(std::make_unique<ChineseChessFactory>());     // ← 新增 / new
    platform.Run();
    return 0;
}
```

无需修改任何核心代码 / No core code changes needed.

---

## 设计模式

| 模式 | 中文名 | 应用位置 |
|------|--------|----------|
| **工厂模式** | Factory Pattern | `IGameFactory` → `DiagonalChessFactory` |
| **策略模式** | Strategy Pattern | `Rule` / `AIPlayer` 抽象基类，每个棋种提供不同实现 |
| **组合模式** | Composite Pattern | `DiagonalChessGame` 组合 Board + Rule + AI + Network + UI + Audio + Effects |
| **观察者模式** | Observer Pattern | `NetworkManager` 回调机制（`std::function`） |
| **模板方法** | Template Method | `Game::Update / Render` 定义接口，子类实现逻辑 |

## 设计原则

- **SRP** 单一职责：每个模块职责唯一
- **OCP** 开闭原则：平台对扩展开放（注册新棋种），对修改封闭
- **LSP** 里氏替换：所有 Game 子类可替换使用
- **ISP** 接口隔离：Board / Rule / AI 各自独立的窄接口
- **DIP** 依赖反转：高层模块依赖抽象接口，不依赖具体实现

---

## 已知问题 / Known Issues

| # | 问题 Issue | 说明 Description |
|---|-----------|-----------------|
| 1 | 联机 reject 按钮不完整 | 悔棋/重开拒绝的消息发送需补全协议 |
| 2 | 缺少游戏选择菜单 | ChessPlatform 直接启动 "DiagonalChess"，需增加 MainMenuScene |
| 3 | 窗口缩放未适配 | 棋盘/棋子/UI 大小硬编码，未响应窗口大小变化 |
| 4 | AI 评估可优化 | 可加入更精确的位置评估表提升棋力 |
