#include "raylib.h"

int main()
{
    constexpr int screenWidth = 960;
    constexpr int screenHeight = 540;

    InitWindow(screenWidth, screenHeight, "Flappy Bird - raylib");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(SKYBLUE);

        DrawRectangle(
            0,
            screenHeight - 80,
            screenWidth,
            80,
            BEIGE
        );

        DrawCircle(220, screenHeight / 2, 22, YELLOW);
        DrawCircle(228, screenHeight / 2 - 6, 5, BLACK);

        DrawText(
            "raylib is connected!",
            30,
            30,
            30,
            DARKBLUE
        );

        DrawText(
            "Press ESC to close",
            30,
            70,
            20,
            DARKGRAY
        );

        EndDrawing();
    }

    CloseWindow();
    return 0;
}