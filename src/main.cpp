#include "Engine/Core/ChessPlatform.h"
#include "Engine/Games/DiagonalChess/DiagonalChessFactory.h"
#include "Engine/Games/Gomoku/GomokuFactory.h"

int main() {
    ChessPlatform platform;
    platform.RegisterGame(std::make_unique<DiagonalChessFactory>());
    platform.RegisterGame(std::make_unique<GomokuFactory>());
    platform.Run();
    return 0;
}
