#include "Board.h"
#include "Window.h"
#include "Colour.h"
#include <memory.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Rendering info */
#define WIDTH 600
#define HEIGHT 600
#define BOARDER 10
Vec2f container = {0.5f, 0.0f};
Vec3f colours[] = {GREY, LIGHT_GRAY, FOUR, LIGHT_ORANGE, ORANGE, DARK_ORANGE};

Board Bd_NewBoard()
{
	int i;
	Board board;
	memset(board.tiles, 0, SIZE * sizeof(int));
	
	srand(time(0));
	int placeCount = 0;
	while (placeCount < 2)
	{
		int pos = rand() % SIZE;
		if (board.tiles[pos] == 0)
		{
			board.tiles[pos] = 1;
			placeCount++;
		}
	}
	return board;
}

void DrawBackground()
{
	Vec2i start = {-(WIDTH/2.0f), 0};
	Vec2i size = {WIDTH, HEIGHT};
	WM_DrawRect(start, size, DarkGrey);
}

static int numbers = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
void DrawTile(int index, int value)
{
	/* Draw box */
	int tileWidth = ((WIDTH - BOARDER) / ROWS) - (BOARDER);
	int tileHeight = ((HEIGHT - BOARDER) / COLUMNS) - (BOARDER);
	int stepX = tileWidth + BOARDER;
	int stepY = tileHeight + BOARDER;
	int x = (int)((index % ROWS) * stepX) - (WIDTH/2) + BOARDER;
	int y = (int)(index / ROWS) * stepY + BOARDER;
	Vec2i start = {x, y};
	Vec2i size = {tileWidth, tileHeight};
	WM_DrawRect(start, size, value <= 5 ? colours[value] : Yellow);
	
	if (value > 0)
	{
		/* Draw number */
		char numberStr[80];
		sprintf(numberStr, "%i", value < 11 ? numbers[value] : (int)pow(2, value));
		Vec2i textPosition = {x + (tileWidth / 2.0f), y + (tileHeight / 2.0f)};
		WM_DrawText(numberStr, textPosition, "Testing.ttf", 46, 
			value <= 2 ? FirstNumber : White);
	}
}

void Bd_DrawBoard(Board board)
{
	int i;
	WM_StartContainer(container);
	DrawBackground();
	for (i = 0; i < SIZE; i++)
		DrawTile(i, board.tiles[i]);
}

#define IN_BOUNDS(pos) pos.x >= 0 && pos.x < ROWS && pos.y >= 0 && pos.y < COLUMNS
#define STEP(pos, step) pos.x += step.x; pos.y += step.y;
#define POS_TO_INDEX(pos) pos.y * ROWS + pos.x
int Scan(Board* board, Vec2i start, Vec2i dir)
{
	int movedFlag = 0;
	Vec2i pos = start;
	while (IN_BOUNDS(pos))
	{
		int index = POS_TO_INDEX(pos);
		int value = board->tiles[index];
		if (value != 0)
		{
			Vec2i backStep = {-dir.x, -dir.y};
			Vec2i samplePos = pos;
			int resultValue = value;
			while (IN_BOUNDS(samplePos))
			{
				STEP(samplePos, backStep);
				int sampleIndex = POS_TO_INDEX(samplePos);
				int sample = board->tiles[sampleIndex];
				if (sample != 0)
				{
					if (sample == value)
					{
						STEP(samplePos, backStep);
						resultValue++;
					}
					break;
				}
			}

			STEP(samplePos, dir);
			int newIndex = POS_TO_INDEX(samplePos);
			if (newIndex != index)
			{
				board->tiles[index] = 0;
				board->tiles[newIndex] = resultValue;
				movedFlag = 1;
			}
		}
		STEP(pos, dir);
	}
	return movedFlag;
}

static Vec2i startPos[]   = {{0, 0}, {0, 3},  {0, 0},  {3, 0} };
static Vec2i directions[] = {{0, 1}, {0, -1}, {1, 0},  {-1, 0}};
static Vec2i stepDirs[]   = {{1, 0}, {1, 0},  {0, 1},  {0, 1} };
void Bd_Move(Board* board, int dir)
{
	int movedFlag = 0;
	Vec2i start = startPos[dir];
	Vec2i scanStep = directions[dir];
	Vec2i step = stepDirs[dir];
	while(IN_BOUNDS(start))
	{
		if (Scan(board, start, scanStep))
			movedFlag = 1;
		STEP(start, step);
	}

	if (movedFlag == 1)
	{
		int hasPlaced = 0, tryCount = 0;
		while (!hasPlaced && tryCount < 100)
		{
			int pos = rand() % SIZE;
			if (board->tiles[pos] == 0)
			{
				board->tiles[pos] = 1;
				hasPlaced = 1;
			}
			tryCount++;
		}
	}
}

