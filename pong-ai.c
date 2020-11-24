#define _CRT_SECURE_NO_WARNINGS

// Includes
#include <time.h>
#include <stdbool.h>
#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "opengl32.lib")
//Global Variables
float y = 0.0f;
float compY = 0.0f;
float incX = -0.01f;
float incY = 0.01f;
float ballX = 0.0f;
float ballY = -0.9f;
bool wUp = true;
bool sUp = true;
bool movingup = false;
bool movingdown = false;
int p1score = 0;
int p2score = 0;
DWORD threadId[3];
HDC hDC;
HWND hWnd;
HANDLE thread1;
HANDLE thread2;
HANDLE thread3;

// Function Declarations

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
void UpdateOpenGL(HDC hDC);

void UpdateScore(){
    char newtext[32];
    char tostring[4];
    ZeroMemory(newtext, 32);
    ZeroMemory(tostring, 4);
    strcat(newtext, "OpenGL Pong - [score: ");
    sprintf(tostring, "%d", p1score);
    strcat(newtext, tostring);
    strcat(newtext, " - ");
    sprintf(tostring, "%d", p2score);
    strcat(newtext, tostring);
    strcat(newtext, "]");
    SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)&newtext);
}

ULONG WINAPI MoveComp1Paddle(LPVOID p){
    while(1){
        if(ballX > 0.75){
            if(ballY < compY){
                compY -= 0.01f;
            }else if(ballY > compY){
                compY += 0.01f;
            }
        }

        //UpdateOpenGL(hDC);
        Sleep(15);
    }
    return 0;
}

ULONG WINAPI MoveComp2Paddle(LPVOID p){
    while(1){
        if(ballX < -0.67){
            if(ballY < y){
                y -= 0.01f;
            }else if(ballY > y){
                y += 0.01f;
            }
        }

        //UpdateOpenGL(hDC);
        Sleep(15);
    }
    return 0;
}


ULONG WINAPI MoveBall ( LPVOID p) {
    while(1){

        ballY += incY;
        ballX += incX;

            //reflect at player paddle //& add momentum
        if(ballX <= -0.85f && ballX >= -0.86f){
            if(ballY >= y-0.12f && ballY <= y+0.12f){
                incX = -incX;
                incY *= 2.17;
                //if(movingup) incY += 0.005;
                //else if(movingdown) incY -= 0.005;
            }
        }

            //reflect at comp1 paddle
        if(ballX >= 1.05f && ballY >= compY-0.12f && ballY <= compY+0.12f){
            incX = -incX;
            incY *= 1/2.17;
            }


        if(ballX >= 1.07f){
            //MessageBox(NULL, "Comp1 scores a point!\nPress OK to continue playing.", "Comp1 Scores.", MB_OK);
            p1score++;
        }

            //reflect at top & bottom of screen
        if(ballY <= -0.979f || ballY >=0.95f){
            incY = -incY;
        }

        if(ballX <= -1.0f){
            //MessageBox(NULL, "Computer 2 scores a point.\nPress OK to continue.", "Computer Scores", MB_OK);
            p2score++;
        }


            //if win
        if (ballX <= -1.0f || ballX >= 1.07f)
        {

            float rndX = (rand() % 100) / 500;
            float rndY = (rand() % 100) / 500;
            float rndDirection = rand() % 32767;
            //if (rndDirection < 32767/2) {rndDirection = -1;} else {rndDirection = 1;}

            UpdateScore();

            //cleanup
            DWORD exitcode;
            DWORD exitcode2;
            DWORD exitcode3;
            GetExitCodeThread(thread1, &exitcode);
            GetExitCodeThread(thread2, &exitcode2);
            GetExitCodeThread(thread3, &exitcode3);


            //re-initialize
            y = 0.0f;
            compY = 0.0f;
            incX = -0.01f + rndX + rndY;
            incY = 0.01f + rndY;
            ballX = 0.0f + rndX;
            ballY = -0.9f + rndY;
            thread1 = CreateThread(NULL, 0, MoveBall, 0, 0, &threadId[0]);
            thread2 = CreateThread(NULL, 0, MoveComp1Paddle, 0, 0, &threadId[1]);
            thread3 = CreateThread(NULL, 0, MoveComp2Paddle, 0, 0, &threadId[2]);
            ExitThread(exitcode);
            ExitThread(exitcode2);
            ExitThread(exitcode3);

        }

        //UpdateOpenGL(hDC);
        Sleep(12);

    }

    return 0;
}


// WinMain

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int iCmdShow)
{
    WNDCLASS wc;
    HGLRC hRC;
    MSG msg;
    BOOL quit = FALSE;
    thread1 = CreateThread(NULL, 0, MoveBall, 0, 0, &threadId[0]);
    thread2 = CreateThread(NULL, 0, MoveComp1Paddle, 0, 0, &threadId[1]);
    thread3 = CreateThread(NULL, 0, MoveComp2Paddle, 0, 0, &threadId[2]);
    // register window class
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "PongGL";
    RegisterClass( &wc );


    // create main window, 512x384
    hWnd = CreateWindow(
        "PongGL", "OpenGL Pong",
        WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
        100, 100, 1200, 800,
        NULL, NULL, hInstance, NULL );

    // enable OpenGL for the window
    EnableOpenGL( hWnd, &hDC, &hRC );

    // program main loop
    while ( !quit )
    {

        // check for messages
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )  )
        {

            // handle or dispatch messages
            if ( msg.message == WM_QUIT )
            {
                quit = TRUE;
            }
            else
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }

        }
        else
        {
            UpdateOpenGL(hDC);

        }

    }

    // shutdown OpenGL
    DisableOpenGL( hWnd, hDC, hRC );

    // destroy the window explicitly
    DestroyWindow( hWnd );

    return msg.wParam;

}

// Window Procedure

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {

    case WM_CREATE:
        return 0;

    case WM_CLOSE:
        PostQuitMessage( 0 );
        return 0;

    case WM_DESTROY:
        return 0;

    case WM_KEYDOWN:
        switch ( wParam )
        {

/*
        case 38:
            //CreateThread(NULL,0,MoveUp,0,0,&threadId[0]);
            movingup = true;
            wUp = false;
            while(wUp == false){
                if(y < 0.89f){
                    if(!GetAsyncKeyState(38)){
                        wUp = true;
                        movingup = false;
                    }
                    UpdateOpenGL(hDC);
                    y += 0.03f;
                    Sleep(10);
                }else{
                    y = 0.88f;
                    wUp = true;
                }

            }
            //y += 0.01f;
            break;
        case 40:
            movingdown = true;
            sUp = false;
            while(sUp == false){
                if(y >= -0.9f){
                    if(!GetAsyncKeyState(40)){
                        sUp = true;
                        movingdown = false;
                    }
                    UpdateOpenGL(hDC);
                    y -= 0.03f;
                    Sleep(10);
                }else{
                    y = -0.89f;
                    sUp = true;
                }
            }
            //y -= 0.01f;
            break;
*/

        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
            break;

        }
        return 0;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );

    }
}

// Enable OpenGL

void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int format;

    // get the device context (DC)
    *hDC = GetDC( hWnd );

    // set the pixel format for the DC
    ZeroMemory( &pfd, sizeof( pfd ) );
    pfd.nSize = sizeof( pfd );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    format = ChoosePixelFormat( *hDC, &pfd );
    SetPixelFormat( *hDC, format, &pfd );

    // create and enable the render context (RC)
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );

}

// Disable OpenGL

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( hRC );
    ReleaseDC( hWnd, hDC );
}

void UpdateOpenGL(HDC hDC){
            glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
            glClear( GL_COLOR_BUFFER_BIT );

            glPushMatrix();
            glBegin( GL_POLYGON );
            glColor3f( 0.0f, 0.0f, 1.0f ); glVertex2f( -0.97f, -0.1f+y );
            glColor3f( 0.0f, 0.0f, 1.0f ); glVertex2f( -0.97f,  0.1f+y );
            glColor3f( 0.0f, 0.0f, 1.0f ); glVertex2f( -1.0f,   0.1f+y );
            glColor3f( 0.0f, 0.0f, 1.0f ); glVertex2f( -1.0f,  -0.1f+y );
            glEnd();

            glBegin( GL_POLYGON );
            glColor3f( 0.0f, 1.0f, 0.0f ); glVertex2f( -0.08f+ballX, -0.03f+ballY );
            glColor3f( 0.0f, 1.0f, 0.0f ); glVertex2f( -0.08f+ballX, 0.01f+ballY );
            glColor3f( 0.0f, 1.0f, 0.0f ); glVertex2f( -0.11f+ballX,  0.01f+ballY );
            glColor3f( 0.0f, 1.0f, 0.0f ); glVertex2f( -0.11f+ballX, -0.03f+ballY );
            glEnd();

            glBegin( GL_POLYGON );
            glColor3f( 0.0f, 0.0f, 1.0f ); glVertex2f( 0.97f, -0.1f+compY );
            glColor3f( 0.0f, 0.0f, 1.0f ); glVertex2f( 0.97f,  0.1f+compY );
            glColor3f( 0.0f, 0.0f, 1.0f ); glVertex2f( 1.0f,   0.1f+compY );
            glColor3f( 0.0f, 0.0f, 1.0f ); glVertex2f( 1.0f,  -0.1f+compY );
            glEnd();

            glPopMatrix();

            SwapBuffers( hDC );
            Sleep(11);
}
