#include <cstdio>
#include <cstring>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "styles/dark/style_dark.h"
#include <cmath>

constexpr int TILE_SIZE = 120;
constexpr int COLS = 14;
constexpr int ROWS = 7;
constexpr int MAX_UNITS = 6;

enum class Appscreen
{
    MAIN_MENU,
    RULES,
    GAMEPLAY,
    VICTORY,
    DEFEAT
};

struct Unit
{
    int col, row;
    bool isblue;
    bool isalive;
};

int main()
{

    InitWindow(800, 450, "Game");
    InitAudioDevice();
    GuiLoadStyleDark();
    SetTargetFPS(60);
    ToggleFullscreen();

    Sound boop = LoadSound("assets\\boop.mp3");
    Sound move = LoadSound("assets\\move.mp3");
    Sound music = LoadSound("assets\\music.mp3");
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    Color wall = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
    Color gridLine = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));

    Appscreen currentScreen = Appscreen::MAIN_MENU;

    GuiSetStyle(DEFAULT, TEXT_SIZE, 80);

    int gridMap[ROWS][COLS] = {0};

    Color wallColor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL));

    Unit activeUnit[MAX_UNITS] = {
        {2, 1, true, true}, {2, 3, true, true}, {2, 5, true, true}, {11, 1, false, true}, {11, 3, false, true}, {11, 5, false, true}};

    int selectIndex = -1;
    bool isPlayerTurn = true;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        switch (currentScreen)
        {
        case Appscreen::MAIN_MENU:
        {
            if (!IsSoundPlaying(music))
            {
                PlaySound(music);
            }
            
            
            const char *title = "BOOP";
            int textWidth = MeasureText(title, 120);
            Color titleColor = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
            DrawText(title, GetScreenWidth() / 2 - textWidth / 2, 150, 120, titleColor);
            if (GuiButton(Rectangle{float(width / 2) - 100, 370, 200, 100}, "PLAY"))
            {
                currentScreen = Appscreen::GAMEPLAY;
            }
            if (GuiButton(Rectangle{float(width / 2) - 100, 500, 200, 100}, "RULES"))
            {
                currentScreen = Appscreen::RULES;
            }

            break;
        }

        case Appscreen::GAMEPLAY:
        {   
            StopSound(music);
            int gridWidth = COLS * TILE_SIZE;
            int gridHeight = ROWS * TILE_SIZE;

            int xOffset = (GetScreenWidth() - gridWidth) / 2;
            int yOffset = ((GetScreenHeight() - gridHeight) / 2) + 20;

            // drawing grid
            for (int row = 0; row < ROWS; row++)
            {
                for (int col = 0; col < COLS; col++)
                {
                    int index = (row * COLS) + col;
                    int x = xOffset + (col * TILE_SIZE);
                    int y = yOffset + (row * TILE_SIZE);

                    // if (gridMap[row][col] == 1)
                    //{
                    //     DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, wallColor);
                    // }

                    DrawRectangleLines(x, y, TILE_SIZE, TILE_SIZE, gridLine);
                }
            }

            int blueUnit = 3;
            int redUnit = 3;

            for (int i = 0; i < MAX_UNITS; i++)
            {
                if (activeUnit[i].isblue && !activeUnit[i].isalive)
                {
                    blueUnit -= 1;
                }
                else if (!activeUnit[i].isblue && !activeUnit[i].isalive)
                {
                    redUnit -= 1;
                }

                if (blueUnit == 0)
                {
                    currentScreen = Appscreen::DEFEAT;
                }
                else if (redUnit == 0)
                {
                    currentScreen = Appscreen::VICTORY;
                }
            }

            // selecting pieces
            /*if (isPlayerTurn)
            {
                DrawText("Blue's Turn", float(width / 2) - float(MeasureText("Blue's Turn",120) / 2), 10, 120, BLUE);
            }
            else
            {
                DrawText("Red's Turn", float(width / 2) - float(MeasureText("Red's Turn",120) / 2), 10, 120, RED);
            }
            */
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isPlayerTurn)
            {

                int mouseX = GetMouseX();
                int mouseY = GetMouseY();

                int adjustX = mouseX - xOffset;
                int adjustY = mouseY - yOffset;

                int clickCol = adjustX / TILE_SIZE;
                int clickRow = adjustY / TILE_SIZE;

                if (clickCol >= 0 && clickCol < COLS && clickRow >= 0 && clickRow < ROWS)
                {
                    // check if a unit is standing there or not
                    int clickedIndex = -1;
                    for (int i = 0; i < MAX_UNITS; i++)
                    {
                        if (!activeUnit[i].isalive)
                        {
                            continue;
                        }

                        if (activeUnit[i].col == clickCol && activeUnit[i].row == clickRow)
                        {
                            clickedIndex = i;
                            break;
                        }
                    }

                    if (clickedIndex != -1)
                    {
                        if (activeUnit[clickedIndex].isblue)
                        {
                            selectIndex = clickedIndex;
                        }
                        else if (selectIndex != -1)
                        {

                            int colDist = abs(clickRow - activeUnit[selectIndex].row);
                            int rowDist = abs(clickCol - activeUnit[selectIndex].col);

                            int totalDist = colDist + rowDist;

                            if (totalDist == 1 || (colDist == 1 && rowDist == 1))
                            {
                                activeUnit[clickedIndex].isalive = false;
                                PlaySound(boop);
                                activeUnit[selectIndex].col = clickCol;
                                activeUnit[selectIndex].row = clickRow;
                                selectIndex = -1;
                                isPlayerTurn = false;
                            }
                        }
                    }
                    else
                    {
                        if (selectIndex != -1)
                        {
                            int colDist = abs(clickRow - activeUnit[selectIndex].row);
                            int rowlDist = abs(clickCol - activeUnit[selectIndex].col);

                            int totalDist = colDist + rowlDist;

                            if (totalDist == 1 || (colDist == 1 && rowlDist == 1))
                            {
                                activeUnit[selectIndex].row = clickRow;
                                activeUnit[selectIndex].col = clickCol;
                                PlaySound(move);
                                selectIndex = -1;
                                isPlayerTurn = false;
                            }
                        }
                    }
                }
            }
            if (!isPlayerTurn)
            {
                int redIndex = -1;
                for (int i = 0; i < MAX_UNITS; i++)
                {
                    if (activeUnit[i].isalive && !activeUnit[i].isblue)
                    {
                        if (activeUnit[i].isalive && !activeUnit[i].isblue)
                        {
                            redIndex = i;
                            break;
                        }
                    }
                }

                if (redIndex != -1)
                {
                    int targetIndex = -1;
                    int minDist = 6767;

                    for (int i = 0; i < MAX_UNITS; i++)
                    {
                        if (activeUnit[i].isalive && activeUnit[i].isblue)
                        {
                            int colDist = abs(activeUnit[i].col - activeUnit[redIndex].col);
                            int rowDist = abs(activeUnit[i].row - activeUnit[redIndex].row);
                            int totalDist = colDist + rowDist;

                            if (totalDist < minDist)
                            {
                                minDist = totalDist;
                                targetIndex = i;
                            }
                        }
                    }

                    if (targetIndex != -1)
                    {
                        int colDist = abs(activeUnit[targetIndex].col - activeUnit[redIndex].col);
                        int rowDist = abs(activeUnit[targetIndex].row - activeUnit[redIndex].row);

                        if (minDist == 1 || (colDist == 1 && rowDist == 1))
                        {
                            activeUnit[targetIndex].isalive = false;
                            PlaySound(boop);
                            activeUnit[redIndex].col = activeUnit[targetIndex].col;
                            activeUnit[redIndex].row = activeUnit[targetIndex].row;
                        }

                        else
                        {
                            if (colDist > rowDist)
                            {
                                if (activeUnit[targetIndex].col > activeUnit[redIndex].col)
                                {
                                    activeUnit[redIndex].col += 1;
                                }
                                else
                                {
                                    activeUnit[redIndex].col -= 1;
                                }
                            }
                            else
                            {
                                if (activeUnit[targetIndex].row > activeUnit[redIndex].row)
                                {
                                    activeUnit[redIndex].row += 1;
                                }
                                else
                                {
                                    activeUnit[redIndex].row -= 1;
                                }
                            }
                        }
                    }
                }

                isPlayerTurn = true;
            }

            // drawing units
            for (int i = 0; i < MAX_UNITS; i++)
            {
                if (!activeUnit[i].isalive)
                {
                    continue;
                }

                int x = xOffset + (activeUnit[i].col * TILE_SIZE);
                int y = yOffset + (activeUnit[i].row * TILE_SIZE);
                int padding = 15;
                Color unitColor;
                if (activeUnit[i].isblue)
                {
                    unitColor = BLUE;
                }
                else
                {
                    unitColor = RED;
                }

                DrawRectangle(x + padding, y + padding, TILE_SIZE - (padding * 2), TILE_SIZE - (padding * 2), unitColor);

                if (i == selectIndex)
                {
                    DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, WHITE);
                    DrawRectangle(x + padding, y + padding, TILE_SIZE - (padding * 2), TILE_SIZE - (padding * 2), unitColor);
                }
            }
            if (GuiButton(Rectangle{20, 20, 150, 70}, "BACK"))
            {
                currentScreen = Appscreen::MAIN_MENU;
            }

            break;
        }

        case Appscreen::RULES:
        {
            StopSound(music);
            DrawRectangleLines(float(width / 2) - 800, float(height / 2) - 400, 1600, 800, gridLine);
            if (GuiButton(Rectangle{20, 20, 150, 70}, "BACK"))
            {
                currentScreen = Appscreen::MAIN_MENU;
            }
            break;
        }

        case Appscreen::DEFEAT:
            StopSound(music);
            DrawText("YOU LOST !!!!", float(width / 2) - float(MeasureText("YOU LOST !!!!", 200) / 2), float(height / 2) - float(200 / 2), 200, DARKGRAY);

            if (GuiButton(Rectangle{20, 20, 150, 70}, "BACK"))
            {
                currentScreen = Appscreen::MAIN_MENU;
            }
            break;

        case Appscreen::VICTORY:
            StopSound(music);
            DrawText("YOU WON !!!!", float(width / 2) - float(MeasureText("YOW WON !!!!", 200) / 2), float(height / 2) - float(200 / 2), 200, GOLD);

            if (GuiButton(Rectangle{20, 20, 150, 70}, "BACK"))
            {
                currentScreen = Appscreen::MAIN_MENU;
            }
            break;
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}