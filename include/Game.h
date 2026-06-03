/**
 * @file ChineseChess.h
 * @brief 中国象棋游戏的主要头文件，包含游戏的核心类和结构体定义
 */
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <stack>
#include <algorithm>

/**
 * @brief 棋子类型枚举
 */
enum class PieceType { NONE, CHARIOT, HORSE, ELEPHANT, ADVISOR, GENERAL, CANNON, SOLDIER };

/**
 * @brief 玩家阵营枚举
 */
enum class Side { RED, BLACK };

/**
 * @brief AI难度枚举
 */
enum class AIDifficulty { EASY, MEDIUM, HARD };

/**
 * @brief 网络状态枚举
 */
enum class NetState { OFFLINE, HOST_WAITING, CONNECTING, CONNECTED };

/**
 * @brief 棋子结构体
 */
struct Piece {
    PieceType type = PieceType::NONE;  // 棋子类型
    Side side = Side::RED;            // 棋子所属阵营
    bool alive = false;               // 棋子是否存活
};

/**
 * @brief 移动记录结构体
 */
struct MoveRecord {
    int fromR, fromC, toR, toC;       // 移动的起始和目标位置
    Piece movedPiece;                 // 被移动的棋子
    Piece capturedPiece;              // 被吃掉的棋子
    Side side;                        // 执行移动的阵营
    int prevMovesWithoutCapture;      // 之前无吃子回合数
};

/**
 * @brief AI移动结构体
 */
struct AIMove {
    int fromR, fromC, toR, toC;       // 移动的起始和目标位置
    int score;                        // 移动得分
};

/**
 * @brief 按钮结构体
 */
struct UIButton {
    sf::FloatRect bounds;            // 按钮边界
    std::wstring label;              // 按钮标签
    bool hovered;                    // 是否悬停
    bool disabled;                   // 是否禁用
};

/**
 * @brief 粒子结构体
 */
struct Particle {
    sf::Vector2f pos;                // 粒子位置
    sf::Vector2f vel;                // 粒子速度
    sf::Color color;                 // 粒子颜色
    float life;                      // 粒子当前生命值
    float maxLife;                   // 粒子最大生命值
    float size;                      // 粒子大小
};

/**
 * @brief 游戏主类
 */
class Game {
public:
    Game();
    ~Game();
    void run();

private:

    // 游戏主循环相关函数
    void processEvents();            // 处理事件
    void update(float dt);           // 更新游戏状态
    void render();                   // 渲染游戏画面



    // 棋盘相关函数
    void initBoard();                // 初始化棋盘
    void placePieces();              // 放置棋子


    
    // 坐标转换函数
    sf::Vector2f gridToScreen(int r, int c) const;  // 棋盘坐标转屏幕坐标
    sf::Vector2i screenToGrid(float sx, float sy) const;  // 屏幕坐标转棋盘坐标



    // 移动验证相关函数
    std::vector<sf::Vector2i> getValidMoves(int r, int c) const;  // 获取有效移动
    bool isValidMove(int fromR, int fromC, int toR, int toC) const;  // 检查移动是否有效
    bool isValidMoveB(const Piece b[9][9], int fromR, int fromC, int toR, int toC) const;  // 在指定棋盘上检查移动是否有效



    // 各种棋子的移动规则
    bool canChariotMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;  // 车的移动规则
    bool canHorseMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;  // 马的移动规则
    bool canElephantMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;  // 象的移动规则
    bool canAdvisorMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;  // 士的移动规则
    bool canGeneralMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;  // 将的移动规则
    bool canCannonMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;  // 炮的移动规则
    bool canSoldierMove(const Piece b[9][9], int fr, int fc, int tr, int tc, Side side) const;  // 兵的移动规则



    // 辅助函数
    bool isInPalace(int r, int c, Side side) const;  // 检查位置是否在九宫内
    bool isBlockedHorse(const Piece b[9][9], int fr, int fc, int tr, int tc) const;  // 检查马是否被阻挡
    bool isBlockedElephant(const Piece b[9][9], int fr, int fc, int tr, int tc) const;  // 检查象是否被阻挡
    int countPiecesBetween(const Piece b[9][9], int fr, int fc, int tr, int tc) const;  // 计算两点间的棋子数



    // 事件处理函数
    void handleBoardClick(int r, int c);  // 处理棋盘点击
    void handleButtonClick(float mx, float my);  // 处理按钮点击



    // 移动执行函数
    void executeMove(int fromR, int fromC, int toR, int toC);  // 执行移动
    void undoMove();  // 撤销移动
    void restartGame();  // 重新开始游戏



    // 游戏状态检查函数
    void checkGameEnd();  // 检查游戏是否结束
    bool isDraw() const;  // 检查是否平局
    bool hasLegalMoves(Side side) const;  // 检查是否有合法移动
    bool isInCheck(Side side) const;  // 检查是否被将军
    bool isInCheckB(const Piece b[9][9], Side side) const;  // 在指定棋盘上检查是否被将军
    bool isCheckmate(Side side) const;  // 检查是否被将死
    bool hasInsufficientMaterial() const;  // 检查是否子力不足
    bool wouldBeInCheck(int fromR, int fromC, int toR, int toC, Side side) const;  // 检查移动后是否会被将军
    bool wouldBeInCheckB(const Piece b[9][9], int fromR, int fromC, int toR, int toC, Side side) const;  // 在指定棋盘上检查移动后是否会被将军



    // 显示相关函数
    std::wstring getMoveString(const MoveRecord& move) const;  // 获取移动记录的字符串表示
    std::wstring getPieceName(PieceType type, Side side) const;  // 获取棋子名称

    void drawBoard();  // 绘制棋盘
    void drawPieces();  // 绘制棋子
    void drawUI();  // 绘制UI界面
    void drawMoveLog();  // 绘制移动记录
    void drawButtons();  // 绘制按钮
    void drawGameOverEffect();  // 绘制游戏结束特效
    void drawText(const std::wstring& text, float x, float y,
                  unsigned int size, sf::Color color, bool center = false);  // 绘制文本
    void drawTextWithShadow(const std::wstring& text, float x, float y,
                            unsigned int size, sf::Color color, bool center = false);  // 绘制带阴影的文本



    // 粒子系统
    void updateParticles(float dt);  // 更新粒子
    void drawParticles();  // 绘制粒子
    void createWinParticles(Side winner);  // 创建胜利粒子
    void createDrawParticles();  // 创建平局粒子



    // 音效相关函数
    void playMoveSound();  // 播放移动音效
    void playCaptureSound();  // 播放吃子音效
    void playWinSound();  // 播放胜利音效
    void playLoseSound();  // 播放失败音效
    void playDrawSound();  // 播放平局音效
    void playClickSound();  // 播放点击音效
    void initSounds();  // 初始化音效



    // 网络相关函数
    void startHost();  // 开始作为主机
    void startClient();  // 开始作为客户端
    void sendMove(int fromR, int fromC, int toR, int toC);  // 发送移动
    void doSurrender(Side side);  // 执行投降
    void doDraw();  // 执行平局
    void showSurrenderPanel();  // 显示投降面板
    void drawSurrenderPopup();  // 绘制投降弹窗
    void sendUndoRequest();  // 发送撤销请求
    void sendRestartRequest();  // 发送重新开始请求
    void sendSurrenderRequest();  // 发送投降请求
    void sendSurrenderResponse(bool accept);  // 发送投降响应
    void sendDrawRequest();  // 发送平局请求
    void sendDrawResponse(bool accept);  // 发送平局响应
    void applyUndoSteps(int steps);  // 应用撤销步骤
    void pollNetwork();  // 轮询网络
    void disconnectNetwork();  // 断开网络连接
    std::wstring getLocalIP() const;  // 获取本地IP
    void drawNetUI();  // 绘制网络UI
    void drawTutorialPanel();  // 绘制教程面板



    // AI相关函数
    void doAITurn();  // 执行AI回合
    std::vector<AIMove> generateAllMoves(const Piece b[9][9], Side side) const;  // 生成所有可能的移动
    int evaluate(const Piece b[9][9]) const;  // 评估棋盘局面
    int minimax(Piece b[9][9], int depth, int alpha, int beta, bool isMaximizing, Side aiSide);  // 极大极小算法
    void makeMoveAI(Piece b[9][9], const AIMove& move, Piece& captured) const;  // 执行AI移动
    void undoMoveAI(Piece b[9][9], const AIMove& move, const Piece& captured, const Piece& moved) const;  // 撤销AI移动
    int getPieceValue(PieceType type) const;  // 获取棋子价值
    int getPositionBonus(PieceType type, int r, int c, Side side) const;  // 获取位置奖励值



    // 游戏状态变量
    Piece board[9][9];               // 棋盘
    Side currentTurn;                // 当前回合
    int selectedR, selectedC;        // 选中的棋子位置
    bool pieceSelected;              // 是否选中了棋子
    std::vector<sf::Vector2i> validMoves;  // 有效移动列表
    bool gameOver;                   // 游戏是否结束
    Side winner;                     // 胜利方
    bool isDrawGame;                // 是否平局
    bool surrendered;                // 是否投降
    bool agreedDraw;                 // 是否同意平局
    float gameOverTimer;             // 游戏结束计时器

    std::stack<MoveRecord> moveHistory;  // 移动历史记录
    std::vector<std::wstring> moveLogStrings;  // 移动记录字符串
    int movesWithoutCapture;         // 无吃子回合数



    // SFML相关变量
    sf::RenderWindow window;         // 渲染窗口
    sf::Font font;                   // 字体
    sf::View view;                   // 视图
    bool fontLoaded;                 // 字体是否加载
    bool showTutorial;              // 是否显示教程
    int logScrollOffset;             // 日志滚动偏移
    float logDividerY;              // 日志分隔线Y坐标
    bool draggingLogDivider;        // 是否正在拖动日志分隔线



    // 按钮变量
    UIButton undoBtn;                // 撤销按钮
    UIButton restartBtn;             // 重新开始按钮
    UIButton aiBtn;                  // AI按钮
    UIButton difficultyBtn;          // 难度按钮
    UIButton gameOverRestartBtn;     // 游戏结束重新开始按钮
    UIButton onlineBtn;              // 在线按钮
    UIButton hostBtn;                // 主机按钮
    UIButton joinBtn;                // 加入按钮
    UIButton connectBtn;             // 连接按钮
    UIButton disconnectBtn;          // 断开按钮
    UIButton surrenderBtn;           // 投降按钮
    UIButton drawOfferBtn;           // 平局提议按钮
    UIButton undoAcceptBtn;         // 接受撤销按钮
    UIButton undoRejectBtn;         // 拒绝撤销按钮
    UIButton restartAcceptBtn;       // 接受重新开始按钮
    UIButton restartRejectBtn;       // 拒绝重新开始按钮
    UIButton tutorialBtn;            // 教程按钮
    UIButton surrenderRedBtn;        // 红方投降按钮
    UIButton surrenderBlackBtn;      // 黑方投降按钮
    UIButton surrenderAcceptBtn;     // 接受投降按钮
    UIButton surrenderRejectBtn;     // 拒绝投降按钮
    UIButton drawAcceptBtn;         // 接受平局按钮
    UIButton drawRejectBtn;         // 拒绝平局按钮



    // 网络相关变量
    NetState netState;               // 网络状态
    sf::TcpListener listener;        // TCP监听器
    sf::TcpSocket socket;            // TCP套接字
    std::wstring localIP;           // 本地IP
    std::wstring inputIP;           // 输入的IP
    bool showIPInput;               // 是否显示IP输入
    Side netSide;                   // 网络游戏中的阵营
    bool netMode;                   // 是否为网络模式
    bool receivingMove;             // 是否正在接收移动
    bool showSurrenderPopup;
    bool undoRequestSent;           // 是否已发送撤销请求
    bool undoRequestReceived;       // 是否已接收撤销请求
    bool restartRequestSent;        // 是否已发送重新开始请求
    bool restartRequestReceived;     // 是否已接收重新开始请求
    bool surrenderRequestSent;      // 是否已发送投降请求
    bool surrenderRequestReceived;  // 是否已接收投降请求
    bool drawRequestSent;          // 是否已发送平局请求
    bool drawRequestReceived;       // 是否已接收平局请求
    Side undoRequesterSide;        // 撤销请求方
    Side surrenderRequesterSide;    // 投降请求方
    std::wstring notificationText;  // 通知文本
    float notificationTimer;        // 通知计时器



    // AI相关变量
    bool aiMode;                    // 是否为AI模式
    Side aiSide;                    // AI阵营
    int aiDepth;                    // AI搜索深度
    AIDifficulty aiDifficulty;      // AI难度
    bool aiThinking;                // AI是否正在思考
    float aiDelayTimer;             // AI延迟计时器



    // 粒子系统变量
    std::vector<Particle> particles;
  // 粒子列表


    // 音效相关变量
    sf::SoundBuffer moveSoundBuffer;  // 移动音效缓冲
    sf::SoundBuffer captureSoundBuffer;  // 吃子音效缓冲
    sf::SoundBuffer winSoundBuffer;   // 胜利音效缓冲
    sf::SoundBuffer loseSoundBuffer;  // 失败音效缓冲
    sf::SoundBuffer drawSoundBuffer;  // 平局音效缓冲
    sf::SoundBuffer clickSoundBuffer;  // 点击音效缓冲
    sf::Sound moveSound;             // 移动音效
    sf::Sound captureSound;          // 吃子音效
    sf::Sound winSound;              // 胜利音效
    sf::Sound loseSound;             // 失败音效
    sf::Sound drawSound;             // 平局音效
    sf::Sound clickSound;            // 点击音效
    bool soundsLoaded;               // 音效是否加载



    // 常量定义
    static constexpr int DIAG = 40;   // 棋盘格子大小
    static constexpr float ORIGIN_X = 50.f;  // 棋盘原点X坐标
    static constexpr float ORIGIN_Y = 410.f;  // 棋盘原点Y坐标
    static constexpr int WIN_W = 1150;  // 窗口宽度
    static constexpr int WIN_H = 820;  // 窗口高度
    static constexpr int PIECE_R = 20;  // 棋子半径
    static constexpr int MAX_LOG = 20;  // 最大日志数
    static constexpr int DRAW_LIMIT = 120;  // 平局回合数限制
};
