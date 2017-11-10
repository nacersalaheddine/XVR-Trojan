#ifndef __SC_H
#define __SC_H

#ifdef main
# undef main
#endif /* main */

#define WINDOW_MAX_FPS 60
#define WINDOW_DEF_WIDTH 800
#define WINDOW_DEF_HEIGHT 600
#define WINDOW_DEF_MIN_WIDTH 600
#define WINDOW_DEF_MIN_HEIGHT 400
#define WINDOW_TITLE "Screen Capture"
#define WINDOW_CLEAR_COLOR 0x292D3E //#292D3E

#define SC_SCREEN_IMAGE "ScreenCap.bmp"
#define SC_UPDATE_TIMER_VALUE 0.01

#endif