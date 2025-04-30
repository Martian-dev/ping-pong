#include "raylib.h"
#include <iostream>

const int WIDTH = 1000;
const int HEIGHT = WIDTH/16*10;
const int BALL_RADIUS = 10;
const int PADDLE_WIDTH = 20;
const int PADDLE_HEIGHT = 100;
const int BALL_SPEED = 8;
const int PADDLE_SPEED = 7;

Color Blue = Color{73, 106, 164, 255};
Color Dark_Blue = Color{65, 91, 138, 255};
Color Light_Blue = Color{109, 143, 191, 150};

int player_score = 0;
int cpu_score = 0;

enum class CollisionType {
    None,
    Top,
    Middle,
    Bottom
};

struct Contact {
    CollisionType type;
    float penetration;
};

class Ball{
public:
    double x, y;
    double x_vel, y_vel;
    int radius;

    Ball(double x, double y, double x_vel, double y_vel, int r): x(x), y(y), x_vel(x_vel), y_vel(y_vel), radius(r) {}

    void collideWithPaddle(Contact const& contact) {
        x += contact.penetration;
        x_vel *= -1;

        if (contact.type == CollisionType::Top) {
            y_vel = -0.75 * BALL_SPEED;
        }
        else if (contact.type == CollisionType::Bottom) {
            y_vel = 0.75 * BALL_RADIUS;
        }
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

    void draw() const {
        DrawCircle(x, y, radius, WHITE);
    }

    void resetBall() {
        x = WIDTH/2;
        y = HEIGHT/2;

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

        if (y + height >= HEIGHT) {
            y = HEIGHT - height;
        }
    }
public:
    double x, y, width, height;
    double yVelocity;

    Paddle(double x, double y, double w, double h) : x(x), y(y), width(w), height(h) {
        yVelocity = PADDLE_SPEED;
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
        DrawRectangle(x, y, width, height, LIGHTGRAY);

    }
};

class CpuPaddle : public Paddle {
public:
    CpuPaddle(double x, double y, double w, double h): Paddle(x, y, w, h) {}

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

Ball ball(WIDTH/2, HEIGHT/2, BALL_SPEED, 0, BALL_RADIUS);
Paddle player(WIDTH-PADDLE_WIDTH -10, HEIGHT/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT);
CpuPaddle comp(10, HEIGHT/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT);


void UpdateDrawFrame(void);     // Update and Draw one frame

Contact CheckPaddleCollision(Ball const& b, Paddle const& p) {
    double ballright = b.x - b.radius;
    double ballleft = b.x + b.radius;
    double balltop = b.y - b.radius;
    double ballbottom = b.y + b.radius;

    double paddleleft = p.x - p.width;
    double paddleright = p.x + 2*p.width;
    double paddletop = p.y;
    double paddlebottom = p.y + p.height;

    Contact contact{};

    if (ballright < paddleleft)
        return contact;
    if (ballleft > paddleright)
        return contact;
    if (balltop > paddlebottom)
        return contact;
    if (ballbottom < paddletop)
        return contact;

    double paddleUpperRange = paddletop + (p.height/3);
    double paddleLowerRange = paddlebottom - (p.height/3);

    if (b.x_vel < 0 ) // left paddle
        contact.penetration = paddleright - ballleft;
    else if (b.x_vel > 0) // right paddle
        contact.penetration = paddleleft - ballright;

    if ((balltop >= paddletop) && (ballbottom <= paddleUpperRange))
        contact.type = CollisionType::Top;
    else if ((balltop >= paddleLowerRange) && (ballbottom <= paddlebottom))
        contact.type = CollisionType::Bottom;
    else
        contact.type = CollisionType::Middle;

    return contact;
}

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
//    if (CheckCollisionCircleRec(Vector2{static_cast<float>(ball.x),
//                                        static_cast<float>(ball.y)},
//                                ball.radius,
//                                Rectangle{static_cast<float>(comp.x),
//                                          static_cast<float>(comp.y),
//                                          static_cast<float>(comp.width),
//                                          static_cast<float>(comp.height)})) {
//        ball.x_vel *= -1;
//    }
//    if (CheckCollisionCircleRec(Vector2{static_cast<float>(ball.x),
//                                        static_cast<float>(ball.y)},
//                                ball.radius,
//                                Rectangle{static_cast<float>(player.x),
//                                          static_cast<float>(player.y),
//                                          static_cast<float>(player.width),
//                                          static_cast<float>(player.height)})) {
//        ball.x_vel *= -1;
//    }

    if (
            Contact contact = CheckPaddleCollision(ball, player);
            contact.type != CollisionType::None
            ) {
        ball.collideWithPaddle(contact);
    }
    if (
            Contact contact = CheckPaddleCollision(ball, comp);
            contact.type !=  CollisionType::None
            ) {
        ball.collideWithPaddle(contact);
    }

    ball.checkScore();
    std::cout << ball.y_vel << ball.x_vel << std::endl;

    // Draw
    BeginDrawing();

    ClearBackground(Blue);
    DrawRectangle(WIDTH/2, 0, WIDTH/2, HEIGHT, Dark_Blue);
    DrawCircle(WIDTH/2, HEIGHT/2, HEIGHT/3, Light_Blue);
    DrawLine(WIDTH/2, 0, WIDTH/2, HEIGHT, LIGHTGRAY);
    ball.draw();
    player.draw();
    comp.draw();
    DrawText(TextFormat("%d", cpu_score), WIDTH/4 - 20, 20, 80, WHITE);
    DrawText(TextFormat("%d", player_score), 3*WIDTH/4 - 20, 20, 80, WHITE);

    EndDrawing();
}

// TODO: Random ball spawning, delay after scoring, make it more playable, end screen and start screen, maybe levels, and in future maybe multiplayer.
