#include "raylib.h"
#include <iostream>
#include <random>

const int WIDTH = 1000;
const int HEIGHT = WIDTH/16*10;
const int BALL_RADIUS = 10;
const int PADDLE_WIDTH = 25;
const int PADDLE_HEIGHT = 100;

int player_score = 0;
int cpu_score = 0;

class Ball{
public:
    int x, y;
    int x_vel, y_vel;
    int radius;

    Ball(double x, double y, int x_vel, int y_vel, int r): x(x), y(y), x_vel(x_vel), y_vel(y_vel), radius(r) {}

    void setXDirection(int xDir) {

    }

    void setYDirection() {

    }

    void checkScore() {
        if (x + radius >= GetScreenWidth()) {
            cpu_score++;
            resetBall();
        }
        if (x - radius <= 0) {
            player_score++;
            resetBall();
        }
    }

    void update() {
        x += x_vel;
        y += y_vel;

        if (y + radius >= GetScreenHeight() || y - radius <= 0) {
            y_vel *= -1;
        }
    }

    void draw() {
        DrawCircle(x, y, radius, LIGHTGRAY);
    }

    void resetBall() {
        x = GetScreenWidth()/2;
        y = GetScreenHeight()/2;

        int randSpeed[2] = {-1, 1};
        x_vel *= randSpeed[GetRandomValue(0, 1)];
        y_vel *= randSpeed[GetRandomValue(0, 1)];
    }
};

class Paddle{
protected:
    void LimitMovement() {
        if (y <= 0) {
            y = 0;
        }

        if (y + height >= GetScreenHeight()) {
            y = GetScreenHeight() - height;
        }
    }
public:
    int x, y, width, height;
    int yVelocity;

    Paddle(int x, int y, int w, int h) : x(x), y(y), width(w), height(h) {
        yVelocity = 7;
    }

    void move() {
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            if (y + height <= GetScreenHeight() || y >= 0) {
                y -= yVelocity;
            }
        }
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            if (y + height <= GetScreenHeight() || y >= 0) {
                y += yVelocity;
            }
        }
        LimitMovement();
    }

    void draw() {
        DrawRectangle(x, y, width, height, WHITE);

    }
};

class CpuPaddle : public Paddle {
public:
    CpuPaddle(int x, int y, int w, int h): Paddle(x, y, w, h) {}

    void move(int ball_y) {
        if (y + height/2 > ball_y) {
            y -= yVelocity;
        }
        if (y + height/2 <= ball_y) {
            y += yVelocity;
        }
        LimitMovement();
    }
};

Ball ball(WIDTH/2, HEIGHT/2, 8, 8, BALL_RADIUS);
Paddle player(WIDTH-PADDLE_WIDTH -5, HEIGHT/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT);
CpuPaddle comp(5, HEIGHT/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT);


void UpdateDrawFrame(void);     // Update and Draw one frame

int main()
{

    InitWindow(WIDTH, HEIGHT, "Ping Pong");
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    ball.resetBall();

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}

void UpdateDrawFrame(void)
{
    // updating all the sprites
    ball.update();
    player.move();
    comp.move(ball.y);

    // checking for collision
    if (CheckCollisionCircleRec(Vector2{static_cast<float>(ball.x),
                                        static_cast<float>(ball.y)},
                                ball.radius,
                                Rectangle{static_cast<float>(comp.x),
                                          static_cast<float>(comp.y),
                                          static_cast<float>(comp.width),
                                          static_cast<float>(comp.height)})) {
        ball.x_vel *= -1;
    }
    if (CheckCollisionCircleRec(Vector2{static_cast<float>(ball.x),
                                        static_cast<float>(ball.y)},
                                ball.radius,
                                Rectangle{static_cast<float>(player.x),
                                          static_cast<float>(player.y),
                                          static_cast<float>(player.width),
                                          static_cast<float>(player.height)})) {
        ball.x_vel *= -1;
    }
    ball.checkScore();

    // Draw
    BeginDrawing();

    ClearBackground(BLACK);
    DrawLine(WIDTH/2, 0, WIDTH/2, HEIGHT, GRAY);
    ball.draw();
    player.draw();
    comp.draw();
    DrawText(TextFormat("%d", cpu_score), WIDTH/4 - 20, 20, 80, WHITE);
    DrawText(TextFormat("%d", player_score), 3*WIDTH/4 - 20, 20, 80, WHITE);

    EndDrawing();
}

// TODO: fix the collision bug, make it more playable, end screen and start screen, maybe levels, and in future maybe multiplayer.
