#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

int cellSize = 30;
int cellCount = 25;
int offSet = 75;

double lastUpdateTime = 0;
bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
        
    }
    return false;
}

bool eventTrigged(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake
{
public:
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool addSegment = false;

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offSet + x * cellSize, offSet + y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true)
        {
            
            addSegment = false;
        }
        else
        {
            body.pop_back();
        }
    }

    void Reset()
    {
        body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        direction = {1, 0};
    }
};

class Food
{
public:
    Vector2 position = {5, 6};

    Texture2D texture;
    Food(const std::deque<Vector2>& snakeBody, int cellSize)
    {
        Image img = LoadImage("images/food.png");
        ImageResize(&img, cellSize, cellSize);
        texture = LoadTextureFromImage(img);
        UnloadImage(img);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw()
    {
        DrawTexture(texture, offSet + position.x * cellSize, offSet + position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game
{
public:
    Snake snakey = Snake();
    Food food;
    bool running = true;
    int score = 0;

    Game() : food(snakey.body, cellSize) {}
    
    void Draw()
    {
        food.Draw();
        snakey.Draw();
    }

    void Update()
    {
        if(running)
        {
        snakey.Update();
        CheckCollisions();
        CheckCollisionWithEdges();
        CheckCollisionWithTail();
        }
    }

    void CheckCollisions()
    {
        if (Vector2Equals(snakey.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snakey.body);
            snakey.addSegment = true;
            score ++;
        }
    }

    void CheckCollisionWithEdges()
    {
        if(snakey.body[0].x == cellCount || snakey.body[0].x == -1)
        {
            GameOver();
        }
        if(snakey.body[0].y == cellCount || snakey.body[0].y == -1)
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snakey.Reset();
        food.position = food.GenerateRandomPos(snakey.body);
        running = false;
        score = 0;
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snakey.body;
        headlessBody.pop_front();
        if(ElementInDeque(snakey.body[0], headlessBody))
        {
            GameOver();
        }
    }
};
int main()
{

    InitWindow(2*offSet + cellSize * cellCount, 2*offSet + cellSize * cellCount, "SNAKEY SNAKE");
    SetTargetFPS(60);
    Game game = Game();

    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        if (eventTrigged(0.2))
        {
            game.Update();
        }

        if (IsKeyPressed(KEY_UP) && game.snakey.direction.y != 1)
        {
            game.snakey.direction = {0, -1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snakey.direction.y != -1)
        {
            game.snakey.direction = {0, 1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snakey.direction.x != 1)
        {
            game.snakey.direction = {-1, 0};
            game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snakey.direction.x != -1)
        {
            game.snakey.direction = {1, 0};
            game.running = true;
        }

        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offSet-5, (float)offSet-5, (float)cellSize*cellCount+10, (float)cellSize*cellCount+10}, 5, darkGreen);
        DrawText("Snakey Snake", offSet - 5, 20, 40, darkGreen);
        DrawText(TextFormat("%i",game.score), offSet - 5, offSet + cellSize * cellCount + 10, 40, darkGreen);
        game.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}