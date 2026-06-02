#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <stack>
#include <algorithm>

enum class PieceType { NONE, CHARIOT, HORSE, ELEPHANT, ADVISOR, GENERAL, CANNON, SOLDIER };

enum class Side { RED, BLACK };

enum class AIDifficulty { EASY, MEDIUM, HARD };

enum class NetState { OFFLINE, HOST_WAITING, CONNECTING, CONNECTED };

struct Piece {
    PieceType type = PieceType::NONE;
    Side side = Side::RED;
    bool alive = false;
};

struct MoveRecord {
    int fromR, fromC, toR, toC;
    Piece movedPiece;
    Piece capturedPiece;
    Side side;
};

struct AIMove {
    int fromR, fromC, toR, toC;
    int score;
};

struct UIButton {
    sf::FloatRect bounds;
    std::wstring label;
    bool hovered;
    bool disabled;
};

struct Particle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::Color color;
    float life;
    float maxLife;
    float size;
};

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

    void initBoard();
    void placePieces();

    sf::Vector2f gridToScreen(int r, int c) const;
    sf::Vector2i screenToGrid(float sx, float sy) const;

    std::vector<sf::Vector2i> getValidMoves(int r, int c) const;
    bool isValidMove(int fromR, int fromC, int toR, int toC) const;
    bool isValidMoveB(const Piece b[9][9], int fromR, int fromC, int toR, int toC) const;

    bool canChariotMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool canHorseMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool canElephantMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool canAdvisorMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool canGeneralMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool canCannonMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool canSoldierMove(const Piece b[9][9], int fr, int fc, int tr, int tc, Side side) const;

    bool isInPalace(int r, int c, Side side) const;
    bool isBlockedHorse(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool isBlockedElephant(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    int countPiecesBetween(const Piece b[9][9], int fr, int fc, int tr, int tc) const;

    void handleBoardClick(int r, int c);
    void handleButtonClick(float mx, float my);

    void executeMove(int fromR, int fromC, int toR, int toC);
    void undoMove();
    void restartGame();

    void checkGameEnd();
    bool isDraw() const;
    bool hasLegalMoves(Side side) const;
    bool isInCheck(Side side) const;
    bool isInCheckB(const Piece b[9][9], Side side) const;
    bool isCheckmate(Side side) const;
    bool hasInsufficientMaterial() const;
    bool wouldBeInCheck(int fromR, int fromC, int toR, int toC, Side side) const;

    std::wstring getMoveString(const MoveRecord& move) const;
    std::wstring getPieceName(PieceType type, Side side) const;

    void drawBoard();
    void drawPieces();
    void drawUI();
    void drawMoveLog();
    void drawButtons();
    void drawGameOverEffect();
    void drawText(const std::wstring& text, float x, float y,
                  unsigned int size, sf::Color color, bool center = false);
    void drawTextWithShadow(const std::wstring& text, float x, float y,
                            unsigned int size, sf::Color color, bool center = false);

    void updateParticles(float dt);
    void drawParticles();
    void createWinParticles(Side winner);
    void createDrawParticles();

    void playMoveSound();
    void playCaptureSound();
    void playWinSound();
    void playLoseSound();
    void playDrawSound();
    void playClickSound();
    void initSounds();

    void startHost();
    void startClient();
    void sendMove(int fromR, int fromC, int toR, int toC);
    void sendUndoRequest();
    void sendRestartRequest();
    void applyUndoSteps(int steps);
    void pollNetwork();
    void disconnectNetwork();
    std::wstring getLocalIP() const;
    void drawNetUI();

    void doAITurn();
    std::vector<AIMove> generateAllMoves(const Piece b[9][9], Side side) const;
    int evaluate(const Piece b[9][9]) const;
    int minimax(Piece b[9][9], int depth, int alpha, int beta, bool isMaximizing, Side aiSide);
    void makeMoveAI(Piece b[9][9], const AIMove& move, Piece& captured) const;
    void undoMoveAI(Piece b[9][9], const AIMove& move, const Piece& captured, const Piece& moved) const;
    int getPieceValue(PieceType type) const;
    int getPositionBonus(PieceType type, int r, int c, Side side) const;

    Piece board[9][9];
    Side currentTurn;
    int selectedR, selectedC;
    bool pieceSelected;
    std::vector<sf::Vector2i> validMoves;
    bool gameOver;
    Side winner;
    bool isDrawGame;
    float gameOverTimer;

    std::stack<MoveRecord> moveHistory;
    std::vector<std::wstring> moveLogStrings;
    int movesWithoutCapture;

    sf::RenderWindow window;
    sf::Font font;
    bool fontLoaded;

    UIButton undoBtn;
    UIButton restartBtn;
    UIButton aiBtn;
    UIButton difficultyBtn;
    UIButton gameOverRestartBtn;
    UIButton onlineBtn;
    UIButton hostBtn;
    UIButton joinBtn;
    UIButton connectBtn;
    UIButton disconnectBtn;

    UIButton undoAcceptBtn;
    UIButton undoRejectBtn;
    UIButton restartAcceptBtn;
    UIButton restartRejectBtn;

    NetState netState;
    sf::TcpListener listener;
    sf::TcpSocket socket;
    std::wstring localIP;
    std::wstring inputIP;
    bool showIPInput;
    Side netSide;
    bool netMode;
    bool receivingMove;
    bool undoRequestSent;
    bool undoRequestReceived;
    bool restartRequestSent;
    bool restartRequestReceived;
    Side undoRequesterSide;

    bool aiMode;
    Side aiSide;
    int aiDepth;
    AIDifficulty aiDifficulty;
    bool aiThinking;
    float aiDelayTimer;

    std::vector<Particle> particles;

    sf::SoundBuffer moveSoundBuffer;
    sf::SoundBuffer captureSoundBuffer;
    sf::SoundBuffer winSoundBuffer;
    sf::SoundBuffer loseSoundBuffer;
    sf::SoundBuffer drawSoundBuffer;
    sf::SoundBuffer clickSoundBuffer;
    sf::Sound moveSound;
    sf::Sound captureSound;
    sf::Sound winSound;
    sf::Sound loseSound;
    sf::Sound drawSound;
    sf::Sound clickSound;
    bool soundsLoaded;

    static constexpr int DIAG = 40;
    static constexpr float ORIGIN_X = 50.f;
    static constexpr float ORIGIN_Y = 410.f;
    static constexpr int WIN_W = 1150;
    static constexpr int WIN_H = 820;
    static constexpr int PIECE_R = 20;
    static constexpr int MAX_LOG = 20;
    static constexpr int DRAW_LIMIT = 120;
};
