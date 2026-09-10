#include "raylib.h"
#include <array>

constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;
constexpr float groundY = 640.0f;

constexpr int pipeCount = 3;
constexpr float pipeWidth = 85.0f;
constexpr float pipeGap = 165.0f;
constexpr float pipeCapWidth = 105.0f;
constexpr float pipeCapHeight = 28.0f;
constexpr float pipeSpacing = 420.0f;
constexpr float pipeSpeed = 180.0f;

enum class GameState {
    Waiting,
    Playing,
    GameOver
};

class Bird {
private:
    Vector2 position{ 260.0f, 320.0f };
    float velocityY = 0.0f;

    static constexpr float gravity = 900.0f;
    static constexpr float jumpForce = -350.0f;
    static constexpr float radius = 22.0f;

public:
    void Reset() {
        position = {260.0f, 320.0f};
        velocityY = 0.0f;
    }

    void Jump() {
        velocityY = jumpForce;
    }

    void Update(float deltaTime) {
        velocityY += gravity * deltaTime;
        position.y += velocityY * deltaTime;

        if (position.y < radius) {
            position.y = radius;
            velocityY = 0.0f;
        }
    }

    void Draw(const Texture2D& texture) const {
        float angle = velocityY * 0.08f;

        if (angle < -25.0f) angle = -25.0f;
        if (angle > 75.0f) angle = 75.0f;

        Rectangle source{
            0.0f, 0.0f,
            static_cast<float>(texture.width),
            static_cast<float>(texture.height)
        };

        Rectangle destination{position.x, position.y, 64.0f, 48.0f};
        Vector2 origin{32.0f, 24.0f};

        DrawTexturePro(texture, source, destination, origin, angle, WHITE);
    }

    bool HitGround() const {
        return position.y + radius >= groundY;
    }

    Vector2 GetPosition() const {
        return position;
    }

    float GetRadius() const {
        return radius;
    }

    float GetX() const {
        return position.x;
    }
};

class Pipe {
private:
    float x = 0.0f;
    float gapY = 250.0f;
    bool scored = false;

public:
    void Reset(float newX) {
        x = newX;
        gapY = static_cast<float>(GetRandomValue(170, 470));
        scored = false;
    }

    void Update(float deltaTime) {
        x -= pipeSpeed * deltaTime;
    }

    void Draw() const {
        const Color pipeGreen{76, 150, 72, 255};
        const Color pipeDark{43, 100, 52, 255};
        const Color pipeLight{130, 190, 91, 255};

        float topHeight = gapY - pipeGap / 2.0f;
        float bottomY = gapY + pipeGap / 2.0f;
        float capX = x - (pipeCapWidth - pipeWidth) / 2.0f;

        Rectangle topBody{ x, 0.0f, pipeWidth, topHeight };
        Rectangle bottomBody{x, bottomY, pipeWidth, groundY + 10.0f - bottomY};
        Rectangle topCap{ capX, topHeight - pipeCapHeight, pipeCapWidth, pipeCapHeight };
        Rectangle bottomCap{ capX, bottomY, pipeCapWidth, pipeCapHeight };

        DrawRectangleRec(topBody, pipeGreen);
        DrawRectangleRec(bottomBody, pipeGreen);
        DrawRectangleRec(topCap, pipeGreen);
        DrawRectangleRec(bottomCap, pipeGreen);

        DrawRectangleRec({ x + 12.0f, bottomY + pipeCapHeight,
                  9.0f, groundY + 10.0f - bottomY - pipeCapHeight }, pipeLight);

        DrawRectangleLinesEx(topBody, 4.0f, pipeDark);
        DrawRectangleLinesEx(bottomBody, 4.0f, pipeDark);
        DrawRectangleLinesEx(topCap, 4.0f, pipeDark);
        DrawRectangleLinesEx(bottomCap, 4.0f, pipeDark);
    }

    bool Collides(const Bird& bird) const {
        float topHeight = gapY - pipeGap / 2.0f;
        float bottomY = gapY + pipeGap / 2.0f;
        float capX = x - (pipeCapWidth - pipeWidth) / 2.0f;

        Rectangle topPipe{x, 0.0f, pipeWidth, topHeight};
        Rectangle bottomPipe{x, bottomY, pipeWidth, groundY - bottomY};
        Rectangle topCap{capX, topHeight - pipeCapHeight, pipeCapWidth, pipeCapHeight};
        Rectangle bottomCap{capX, bottomY, pipeCapWidth, pipeCapHeight};

        Vector2 birdPosition = bird.GetPosition();
        float birdRadius = bird.GetRadius();

        return CheckCollisionCircleRec(birdPosition, birdRadius, topPipe) ||
            CheckCollisionCircleRec(birdPosition, birdRadius, bottomPipe) ||
            CheckCollisionCircleRec(birdPosition, birdRadius, topCap) ||
            CheckCollisionCircleRec(birdPosition, birdRadius, bottomCap);
    }

    bool IsOutside() const {
        return x + pipeCapWidth < 0.0f;
    }

    bool ShouldAddScore(float birdX) {
        if (!scored && x + pipeWidth < birdX) {
            scored = true;
            return true;
        }
        return false;
    }

    float GetX() const {
        return x;
    }
};

void DrawCenteredText(const char* text, int y, int size, Color color) {
    int width = MeasureText(text, size);
    DrawText(text, (screenWidth - width) / 2, y, size, color);
}

void DrawCloud(float x, float y, float scale) {
    const Color cloudColor{ 235, 248, 220, 255 };

    DrawCircleV({x, y }, 35.0f * scale, cloudColor);
    DrawCircleV({x + 38.0f * scale, y - 10.0f * scale }, 43.0f * scale, cloudColor);
    DrawCircleV({x + 80.0f * scale, y }, 32.0f * scale, cloudColor);
    DrawRectangleRec({x, y, 80.0f * scale, 30.0f * scale }, cloudColor);
}

void DrawBackground() {
    ClearBackground({ 101, 185, 205, 255 });

    DrawCloud(20.0f, groundY - 150.0f, 1.0f);
    DrawCloud(230.0f, groundY - 135.0f, 1.2f);
    DrawCloud(500.0f, groundY - 155.0f, 1.0f);
    DrawCloud(730.0f, groundY - 140.0f, 1.2f);
    DrawCloud(1010.0f, groundY - 150.0f, 1.1f);
    DrawCloud(1210.0f, groundY - 135.0f, 1.0f);

    const Color backTrees{ 105, 184, 103, 255 };
    const Color frontTrees{ 65, 150, 79, 255 };

    DrawRectangle(0, static_cast<int>(groundY - 90.0f), screenWidth, 90, backTrees);

    for (int x = -40, i = 0; x < screenWidth + 80; x += 80, i++) {
        int y = static_cast<int>(groundY - 90.0f - (i % 2) * 12.0f);
        DrawCircle(x, y, 58.0f, backTrees);
        DrawCircle(x + 45, y + 5, 52.0f, backTrees);
    }

    DrawRectangle(0, static_cast<int>(groundY - 55.0f), screenWidth, 55, frontTrees);

    for (int x = -30, i = 0; x < screenWidth + 80; x += 75, i++) {
        int y = static_cast<int>(groundY - 58.0f - (i % 3) * 8.0f);
        DrawCircle(x, y, 50.0f, frontTrees);
        DrawCircle(x + 40, y + 7, 46.0f, frontTrees);
    }
}

void ResetGame(Bird& bird, std::array<Pipe, pipeCount>& pipes, int& score) {
    bird.Reset();
    score = 0;

    for (int i = 0; i < pipeCount; i++) {
        pipes[i].Reset(screenWidth + 180.0f + i * pipeSpacing);
    }
}

int main() {
    InitWindow(screenWidth, screenHeight, "Flappy Bird - raylib");
    SetTargetFPS(60);

    Texture2D birdTexture = LoadTexture("assets/bird.png");
    SetTextureFilter(birdTexture, TEXTURE_FILTER_POINT);

    Bird bird;
    std::array<Pipe, pipeCount> pipes;
    GameState state = GameState::Waiting;
    int score = 0;
    int bestScore = 0;

    ResetGame(bird, pipes, score);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        if (deltaTime > 0.05f) deltaTime = 0.05f;

        bool jumpPressed = IsKeyPressed(KEY_SPACE) ||
            IsKeyPressed(KEY_UP) ||
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (state == GameState::Waiting && jumpPressed) {
            bird.Jump();
            state = GameState::Playing;
        }
        else if (state == GameState::Playing) {
            if (jumpPressed) bird.Jump();

            bird.Update(deltaTime);

            for (Pipe& pipe : pipes) {
                pipe.Update(deltaTime);

                if (pipe.Collides(bird)) state = GameState::GameOver;

                if (pipe.ShouldAddScore(bird.GetX())) {
                    score++;
                    if (score > bestScore) bestScore = score;
                }

                if (pipe.IsOutside()) {
                    pipe.Reset(pipe.GetX() + pipeSpacing * pipeCount);
                }
            }

            if (bird.HitGround()) state = GameState::GameOver;
        }
        else if (state == GameState::GameOver &&
            (jumpPressed || IsKeyPressed(KEY_R))) {
            ResetGame(bird, pipes, score);
            bird.Jump();
            state = GameState::Playing;
        }

        BeginDrawing();
        DrawBackground();

        DrawRectangle(0, static_cast<int>(groundY), screenWidth,
            screenHeight - static_cast<int>(groundY), BEIGE);

        DrawRectangle(0, static_cast<int>(groundY), screenWidth, 10,
            { 65, 150, 79, 255 });

        for (const Pipe& pipe : pipes) {
            pipe.Draw();
        }

        bird.Draw(birdTexture);
        DrawText(TextFormat("BEST: %d", bestScore), 20, 20, 30, DARKBLUE);

        if (state == GameState::Waiting) {
            DrawCenteredText("FLAPPY BIRD", 150, 50, DARKBLUE);
            DrawCenteredText("Press SPACE to start", 215, 25, DARKGRAY);
        }
        else {
            DrawCenteredText(TextFormat("%d", score), 25, 50, WHITE);
        }

        if (state == GameState::GameOver) {
            DrawCenteredText("GAME OVER", 160, 50, RED);
            DrawCenteredText(TextFormat("Score: %d", score), 220, 30, DARKBLUE);
            DrawCenteredText("Press SPACE or R to restart", 265, 23, DARKGRAY);
        }

        EndDrawing();
    }

    UnloadTexture(birdTexture);
    CloseWindow();
    return 0;
}
