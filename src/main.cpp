#include "Engine/Core/ChessPlatform.h"
#include "Engine/Games/DiagonalChess/DiagonalChessFactory.h"

int main() {
    ChessPlatform platform;
    platform.RegisterGame(std::make_unique<DiagonalChessFactory>());
    platform.Run();
    return 0;
}
