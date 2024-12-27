#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define GRID_SIZE 3
#define CELL_SIZE (WINDOW_WIDTH / GRID_SIZE)

typedef enum { NONE, PLAYER_X, PLAYER_O } Player;
typedef enum { MENU, GAME, RESULT } GameState;

void drawGrid(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
    for (int i = 1; i < GRID_SIZE; i++) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, WINDOW_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, WINDOW_WIDTH, i * CELL_SIZE);
    }
}

void drawMarker(SDL_Renderer *renderer, int row, int col, Player player) {
    int x = col * CELL_SIZE;
    int y = row * CELL_SIZE;
    if (player == PLAYER_X) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
        SDL_RenderDrawLine(renderer, x, y, x + CELL_SIZE, y + CELL_SIZE);
        SDL_RenderDrawLine(renderer, x + CELL_SIZE, y, x, y + CELL_SIZE);
    } else if (player == PLAYER_O) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue color
        SDL_Rect rect = { x, y, CELL_SIZE, CELL_SIZE };
        SDL_RenderDrawRect(renderer, &rect);
    }
}

Player checkWinner(Player board[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != NONE) {
            return board[i][0];
        }
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != NONE) {
            return board[0][i];
        }
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != NONE) {
        return board[0][0];
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != NONE) {
        return board[0][2];
    }
    return NONE;
}

bool isBoardFull(Player board[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == NONE) {
                return false;
            }
        }
    }
    return true;
}

void showResult(SDL_Renderer *renderer, const char *message) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    SDL_Color textColor = { 255, 255, 255, 255 }; // White color
    // Here you would typically use SDL_ttf to render the text
    // For simplicity, we'll just draw a rectangle as a placeholder for the message
    SDL_Rect rect = { 200, 275, 200, 50 };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for the rectangle
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);
}

bool checkButtonClick(int x, int y, SDL_Rect *button) {
    return (x >= button->x && x <= button->x + button->w &&
            y >= button->y && y <= button->y + button->h);
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Jogo da Velha", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    GameState gameState = MENU;
    Player board[GRID_SIZE][GRID_SIZE] = { NONE };
    Player currentPlayer = PLAYER_X;
    bool quit = false;
    SDL_Event e;

    SDL_Rect playButton = { 250, 275, 100, 50 };

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (gameState == MENU && e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (checkButtonClick(x, y, &playButton)) {
                    gameState = GAME;
                    memset(board, NONE, sizeof(board));
                    currentPlayer = PLAYER_X;
                }
            } else if (gameState == GAME && e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                int row = y / CELL_SIZE;
                int col = x / CELL_SIZE;
                if (board[row][col] == NONE) {
                    board[row][col] = currentPlayer;
                    currentPlayer = (currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;

                    Player winner = checkWinner(board);
                    if (winner != NONE) {
                        gameState = RESULT;
                        showResult(renderer, (winner == PLAYER_X) ? "Player X Wins!" : "Player O Wins!");
                    } else if (isBoardFull(board)) {
                        gameState = RESULT;
                        showResult(renderer, "It's a Draw!");
                    }
                }
            }
        }

        if (gameState == MENU) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for button
            SDL_RenderFillRect(renderer, &playButton);

            SDL_RenderPresent(renderer);
        } else if (gameState == GAME) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
            SDL_RenderClear(renderer);

            drawGrid(renderer);

            for (int i = 0; i < GRID_SIZE; i++) {
                for (int j = 0; j < GRID_SIZE; j++) {
                    if (board[i][j] != NONE) {
                        drawMarker(renderer, i, j, board[i][j]);
                    }
                }
            }

            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
