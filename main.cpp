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
    GAMEPLAY
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
    GuiLoadStyleDark();
    SetTargetFPS(60);
    ToggleFullscreen();

    int width = GetScreenWidth();
    int height = GetScreenHeight();

    Color wall = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
    Color gridLine = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));

    Appscreen currentScreen = Appscreen::MAIN_MENU;

    GuiSetStyle(DEFAULT, TEXT_SIZE, 80);

    int gridMap[ROWS][COLS] = {0};

    //gridMap[3][5]=1;
    //gridMap[4][5]=1;
    //gridMap[5][5]=1;
    Color wallColor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL));

    Unit activeUnit[MAX_UNITS] = {
        {2,1,true,true},{2,3,true,true},{2,5,true,true},
        {11,1,false,true},{11,3,false,true},{11,5,false,true}
    };

    int selectIndex = -1;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        switch (currentScreen)
        {
        case Appscreen::MAIN_MENU:
        {
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

                    if (gridMap[row][col] == 1)
                    {
                        DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, wallColor);
                    }

                    DrawRectangleLines(x, y, TILE_SIZE, TILE_SIZE, gridLine);
                }
            }

            // selecting pieces

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
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
                        
                    }
                    else
                    {

                        int colDist = abs(clickRow - activeUnit[selectIndex].row);
                        int rowDist = abs(clickCol - activeUnit[selectIndex].col);

                        int totalDist = colDist + rowDist;

                        if (totalDist == 1 || (colDist == 1 && rowDist ==1))
                        {
                            activeUnit[selectIndex].col = clickCol;
                            activeUnit[selectIndex].row = clickRow;
                            selectIndex = -1;
                        }
                        // logic to kill player
                        
                    }   
                }
            }

            // drawing units
            for (int i = 0; i < MAX_UNITS; i++)
            {
                if (!activeUnit[i].isalive)
                {
                    continue;
                }

                int x = xOffset + (activeUnit[i].col *TILE_SIZE);
                int y = yOffset + (activeUnit[i].row *TILE_SIZE);
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
                    DrawRectangle(x,y,TILE_SIZE,TILE_SIZE,YELLOW);
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
            
            GuiDrawRectangle(Rectangle{float(width / 2) - 400, float(height / 2) - 100, float(width) - 400, float(height) - 200}, 1, gridLine, wall);
            if (GuiButton(Rectangle{20, 20, 150, 70}, "BACK"))
            {
                currentScreen = Appscreen::MAIN_MENU;
            }
            break;

        }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}