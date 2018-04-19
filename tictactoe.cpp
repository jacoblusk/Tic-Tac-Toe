#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <cstdlib>
#include <math.h>

#define CLASS_NAME "TicTacToeClass"
#define WINDOW_NAME "Tic Tac Toe"

#define X_VALUE 1
#define O_VALUE (-X_VALUE)

#define NO_WINNER X_VALUE * 8

//These should be the same
#define GAME_HEIGHT 512
#define GAME_WIDTH  512

using namespace std;

class GAMESTATE {
  private:
    BOOL gameOver;
    UINT16 clicks;
    INT8 board[3][3];
    BOOL isXTurn;

  public:
    GAMESTATE(void);
    void SetBoardValue(UINT8 x, UINT8 y, INT8 value);
    INT8 GetBoardValue(UINT8 x, UINT8 y);
    BOOL IsXTurn(void);
    void ToggleTurn(void);
    INT CheckIfSomeoneWon(void);
    void SetGameState(BOOL state);
    BOOL GetGameState(void);
    void Reset(void);
};

GAMESTATE::GAMESTATE(void) {
    this->Reset();
}

void GAMESTATE::SetBoardValue(UINT8 x, UINT8 y, INT8 value) {
    this->board[y][x] = value;
}

void GAMESTATE::SetGameState(BOOL state) {
    this->gameOver = state;
}

INT8 GAMESTATE::GetBoardValue(UINT8 x, UINT8 y) {
    return this->board[y][x];
}

BOOL GAMESTATE::GetGameState(void) {
    return this->gameOver;
}

void GAMESTATE::ToggleTurn(void) {
    this->isXTurn = !this->isXTurn;
}

void GAMESTATE::Reset(void) {
    this->gameOver = FALSE;
    for(int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
            this->board[x][y] = 0;
        }
    }
}

BOOL GAMESTATE::IsXTurn(void) {
    return this->isXTurn;
}

INT GAMESTATE::CheckIfSomeoneWon(void) {
    //Check if there are any winning rows.
    //This only works because X_VALUE and Y_VALUE are opposite signs.
    for(INT y = 0; y < 3; y++) {
        INT rowCount = 0;
        for(INT x = 0; x < 3; x++) {
            rowCount += this->board[y][x];
        }

        if(rowCount == (X_VALUE * 3)) {
            return X_VALUE;
        } else if(rowCount == (O_VALUE * 3)) {
            return O_VALUE;
        }
    }

    for(INT y = 0; y < 3; y++) {
        INT rowCount = 0;
        for(INT x = 0; x < 3; x++) {
            rowCount += this->board[x][y];
        }

        if(rowCount == (X_VALUE * 3)) {
            return X_VALUE;
        } else if(rowCount == (O_VALUE * 3)) {
            return O_VALUE;
        }
    }

    //Check for diagonals.
    //This will return 0 if they're all 0, but since that doesn't indicate
    //a winner, then it doesn't matter.
    if(this->board[0][0] == this->board[1][1]
            && this->board[1][1] == this->board[2][2]
            && this->board[0][0] == this->board[2][2]) {
        return this->board[0][0];
    } else if(this->board[0][2] == this->board[1][1]
            && this->board[1][1] == this->board[2][0]
            && this->board[0][2] == this->board[2][0]) {
        return this->board[0][2];
    }

    //Check if it's a tie;
    for(INT y = 0; y < 3; y++) {
        for(INT x = 0; x < 3; x++) {
            if(this->board[x][y] == 0) {
                return 0;
            }
        }
    }

    //No winner!
    return NO_WINNER;
}

BOOL PaintGameState(HWND hWnd, GAMESTATE *gameState);

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch(uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_LBUTTONUP: {
            GAMESTATE *gameState = (GAMESTATE *)(GetWindowLongPtr(hWnd, 0));
            if(gameState && !gameState->GetGameState()) {
                RECT rect;
                LONG width, height;
                INT16 xRegion, yRegion;
                INT16 xPos = GET_X_LPARAM(lParam);
                INT16 yPos = GET_Y_LPARAM(lParam);
                INT whoWon;

                GetClientRect(hWnd, &rect);
                width = rect.right - rect.left;
                height = rect.bottom - rect.top;

                /*      Game Board
                 *   _________________
                 *  |     |     |     |
                 *  |     |     |     |
                 *  |_____|_____|_____|
                 *  |     |     |     |
                 *  |     |     |     |
                 *  |_____|_____|_____|
                 *  |     |     |     |
                 *  |     |     |     |
                 *  |_____|_____|_____|
                 *
                 */

                yRegion = xPos / (width / 3);
                xRegion = yPos / (height / 3);
                if(!gameState->GetBoardValue(xRegion, yRegion)) {
                    if(gameState->IsXTurn()) {
                        gameState->SetBoardValue(xRegion, yRegion, X_VALUE);
                        gameState->ToggleTurn();
                    } else {
                        gameState->SetBoardValue(xRegion, yRegion, O_VALUE);
                        gameState->ToggleTurn();
                    }
                }

                whoWon = gameState->CheckIfSomeoneWon();
                if(whoWon == X_VALUE) {
                    cout << "X won!" << endl;
                    gameState->SetGameState(TRUE);
                } else if(whoWon == O_VALUE) {
                    cout << "O won!" << endl;
                    gameState->SetGameState(TRUE);
                } else if(whoWon == NO_WINNER) {
                    cout << "Cat's Game!" << endl;
                    gameState->SetGameState(TRUE);
                }

                RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
            } else {
                if(gameState) {
                    gameState->Reset();
                    RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
                } else {
                    cerr << "Failed to get game state." << endl;
                }
            }
        } break;
        case WM_PAINT: {
            GAMESTATE *gameState = (GAMESTATE *)GetWindowLongPtr(hWnd, 0);
            if(gameState) {
                PaintGameState(hWnd, gameState);
            }
        } break;
        default: {
            result = DefWindowProc(hWnd, uMsg, wParam, lParam);
        } break;
    }

    return result;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
            LPSTR lpCmdLine, int nCmdShow) {

    BOOL getMessageResult;
    DWORD errorCode;
    BOOL running = TRUE;
    GAMESTATE gameState;
    MSG message;
    WNDCLASSA windowClass = {};

    windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WinProc;
    windowClass.hInstance = hInstance;
    windowClass.cbWndExtra = sizeof(GAMESTATE);
    windowClass.lpszClassName = CLASS_NAME;

    if(!RegisterClass(&windowClass)) {
        cerr << "Failed to call RegisterClass." << endl;
        return EXIT_FAILURE;
    }

    HWND window = CreateWindowEx(
            (DWORD) 0,
            CLASS_NAME,
            WINDOW_NAME,
            WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            GAME_WIDTH,
            GAME_HEIGHT,
            (HWND) NULL,
            (HMENU) NULL,
            hInstance,
            (LPVOID) NULL
    );

    /*
     * If the previous value is zero and the function succeeds, the return
     * value is zero, but the function does not clear the last error
     * information. To determine success or failure, clear the last error
     * information by calling SetLastError with 0, then call SetWindowLongPtr.
     * Function failure will be indicated by a return value of zero and
     * a GetLastError result that is nonzero.
     * https://msdn.microsoft.com/en-us/library/windows/desktop/ms644898(v=vs.85).aspx
     */

    SetLastError(0);
    if(!SetWindowLongPtr(window, 0, (LONG_PTR)&gameState) &&
        (errorCode = GetLastError()) != 0) {
        DWORD errorCode = GetLastError();
        cerr << "Failed to set game state, error: " << errorCode << endl;
        return EXIT_FAILURE;
    }

    //Check if window is NULL
    if(!window) {
        cerr << "Failed to call CreateWindowEx" << endl;
        return EXIT_FAILURE;
    }

    while(running) {
        while((getMessageResult = PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) != 0) {
            if (getMessageResult == -1 ||
                message.message == WM_QUIT) {
                cerr << "Recieved a -1 message result or WM_QUIT." << endl;
                running = FALSE;
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }

    return EXIT_SUCCESS;
}


BOOL PaintGameState(HWND hWnd, GAMESTATE *gameState) {
    HDC deviceContext;
    PAINTSTRUCT paint;
    HBRUSH brush;
    RECT rect;
    LONG width, height;

    //Colors defined in this order 0x00bbggrr
    //Can use RGB macro.
    brush = CreateSolidBrush(RGB(0, 0, 255));

    GetClientRect(hWnd, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

    deviceContext = BeginPaint(hWnd, &paint);

    //Draw Background
    SelectObject(deviceContext, GetStockObject(BLACK_BRUSH));
    Rectangle(deviceContext, rect.left, rect.top, rect.right, rect.bottom);

    //Next we draw the game board.
    SelectObject(deviceContext, GetStockObject(WHITE_BRUSH));
    SelectObject(deviceContext, GetStockObject(WHITE_PEN));

    Rectangle(deviceContext,
        0,
        height / 3 - 8,
        width,
        height / 3 + 8
    );

    Rectangle(deviceContext,
        0,
        (height / 3) * 2 - 8,
        width,
        (height / 3) * 2 + 8
    );

    Rectangle(deviceContext,
        width / 3 - 8,
        0,
        width / 3 + 8,
        height
    );

    Rectangle(deviceContext,
        (width / 3) * 2 - 8,
        0,
        (width / 3) * 2 + 8,
        height
    );

    SelectObject(deviceContext, GetStockObject(BLACK_PEN));
    //Now we draw the X's and O's.
    for(INT y = 0; y < 3; y++) {
        for(INT x = 0; x < 3; x++) {
            if(gameState->GetBoardValue(y, x) == X_VALUE) {
                //draw X
                SelectObject(deviceContext, GetStockObject(WHITE_BRUSH));
                SelectObject(deviceContext, GetStockObject(WHITE_PEN));
                POINT points1[4];
                POINT points2[4];

                INT pixelsUp = 48 * sinf(M_PI/4);

                INT midpointX = (width / 3) * x + ((width / 3) / 2);
                INT midpointY = (height / 3) * y + ((height / 3) / 2);

                INT p1X = pixelsUp - (8 * cosf(M_PI / 4));
                INT p1Y = pixelsUp + (8 * sinf(M_PI / 4));

                INT p2X = pixelsUp + (8 * cosf(M_PI / 4));
                INT p2Y = pixelsUp - (8 * sinf(M_PI / 4));

                points1[0] = (POINT) {
                    midpointX + p1X, midpointY + p1Y
                };

                points1[1] = (POINT) {
                    midpointX + p2X, midpointY + p2Y
                };

                points1[2] = (POINT) {
                    midpointX - p1X, midpointY - p1Y
                };

                points1[3] = (POINT) {
                    midpointX - p2X, midpointY - p2Y
                };

                //Draw second rectangle
                points2[0] = (POINT) {
                    midpointX + p1X, midpointY - p1Y
                };

                points2[1] = (POINT) {
                    midpointX - p2X, midpointY + p2Y
                };

                points2[2] = (POINT) {
                    midpointX - p1X, midpointY + p1Y
                };

                points2[3] = (POINT) {
                    midpointX + p2X, midpointY - p2Y
                };

                Polygon(deviceContext,
                        points1,
                        4
                );

                Polygon(deviceContext,
                        points2,
                        4
                );

            } else if(gameState->GetBoardValue(y, x) == O_VALUE) {
                //draw O
                SelectObject(deviceContext, GetStockObject(WHITE_BRUSH));
                Ellipse(deviceContext,
                    (width / 3) * x + ((width / 3) / 2) - 48,
                    (height / 3) * y + ((height / 3) / 2) - 48,
                    (width / 3) * x + ((width / 3) / 2) + 48,
                    (height / 3) * y + ((height / 3) / 2) + 48
                );

                SelectObject(deviceContext, GetStockObject(BLACK_BRUSH));
                Ellipse(deviceContext,
                    (width / 3) * x + ((width / 3) / 2) - 32,
                    (height / 3) * y + ((height / 3) / 2) - 32,
                    (width / 3) * x + ((width / 3) / 2) + 32,
                    (height / 3) * y + ((height / 3) / 2) + 32
                );
            }

            //Don't draw anything if there isn't a change.
        }
    }

    //Game is over
    if(gameState->GetGameState()) {
        char xWonText[] = "X Player Won";
        char oWonText[] = "O Player Won";
        char catsGameText[] = "Cat's Game";
        char restartText[] = "Click to Restart the Game!";
        RECT textLocation;

        SelectObject(deviceContext, brush);

        LONG midpointX = (rect.right - rect.left) / 2;
        LONG midpointY = (rect.bottom - rect.top) / 2;

        Rectangle(deviceContext,
                midpointX / 2 + rect.right - width / 2,
                midpointY / 2 + rect.bottom - height / 2,
                midpointX / 2 + rect.right - (width / 2) * 2,
                midpointY / 2 + rect.bottom - (height / 2) * 2
        );

        textLocation.right = rect.right;
        textLocation.top = height / 3;
        textLocation.left = 0;
        textLocation.bottom = height / 3 + 16;

        SetTextColor(deviceContext, RGB(255, 255, 255));
        SetBkColor(deviceContext, RGB(0, 0, 255));

        INT whoWon = gameState->CheckIfSomeoneWon();
        char *text;
        if(whoWon == X_VALUE) {
            text = xWonText;
        } else if(whoWon == O_VALUE) {
            text = oWonText;
        } else {
            text = catsGameText;
        }

        INT result = DrawText(deviceContext,
                 text,
                 strlen(text),
                 &textLocation,
                 DT_CENTER
        );

        textLocation.top = height / 2;
        textLocation.bottom = height / 2 + 16;

        result = DrawText(deviceContext,
                 restartText,
                 strlen(restartText),
                 &textLocation,
                 DT_CENTER
        );

        if(!result) {
            cerr << "Failed to DrawText." << endl;
        }
    }

    EndPaint(hWnd, &paint);
    DeleteObject(brush);
    return TRUE;
}
