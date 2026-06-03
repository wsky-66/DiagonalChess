#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <stack>

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
    int prevMovesWithoutCapture;
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

struct Move {
    int fromR, fromC, toR, toC;
};

std::wstring GetPieceName(PieceType type, Side side);

static constexpr int DIAG = 40;
static constexpr float ORIGIN_X = 50.f;
static constexpr float ORIGIN_Y = 410.f;
static constexpr int WIN_W = 1150;
static constexpr int WIN_H = 820;
static constexpr int PIECE_R = 20;
static constexpr int MAX_LOG = 20;
static constexpr int DRAW_LIMIT = 120;
