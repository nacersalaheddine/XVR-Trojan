#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "sc.h"
#include "sc_error.h"
#include "types.h"

int sc_loopLastError = 0;
int sc_stopped = 1;
int sc_running = 0;
SDL_Window* sc_window = NULL;
SDL_Surface* sc_slave = NULL;
SDL_Surface* sc_screen = NULL;

SDL_Rect sc_slaveRect;
SDL_Rect sc_screenRect;
uint32 sc_firstTicks = 0;
uint32 sc_secondTicks = 0;

int sc_isUpdating = 0;
int sc_isDrawing = 0;

void SC_SetImage(uint8* data, uint32 len)
{
	if(!sc_running || sc_stopped)
	{
		return;
	}
	
	while(sc_isDrawing){}

	sc_isUpdating = 1;
	SDL_RWops* io = SDL_RWFromMem(data, len);

	if(!io)
	{
		sc_isUpdating = 0;

		return;
	}

	if(sc_slave)
	{
		SDL_FreeSurface(sc_slave);
	}

	sc_slave = SDL_LoadBMP_RW(io, 1);
	SDL_RWclose(io);

	if(!sc_slave)
	{
		sc_isUpdating = 0;

		return;
	}

	sc_slaveRect = sc_slave->clip_rect;
	sc_slaveRect.x = 0;
	sc_slaveRect.y = 0;

	sc_isUpdating = 0;
}

int SC_GetLoopLastError(void)
{
	return sc_loopLastError;
}

void SC_Update(void)
{

}

void SC_Draw(void)
{
	SDL_FillRect(sc_screen, NULL, WINDOW_CLEAR_COLOR);

	if(sc_slave)
	{
		while(sc_isUpdating){}
		sc_isDrawing = 1;
		SDL_BlitScaled(sc_slave, &sc_slaveRect, sc_screen, &sc_screenRect);
		sc_isDrawing = 0;
	}
}

void SC_Resize(void)
{
	int _w;
	int _h;

	SDL_GetWindowSize(sc_window, &_w, &_h);

	if(_w <= WINDOW_DEF_MIN_WIDTH || _h <= WINDOW_DEF_MIN_HEIGHT)
	{
		if(_w <= WINDOW_DEF_MIN_WIDTH)
		{
			_w = WINDOW_DEF_MIN_WIDTH;
		}

		if( _h <= WINDOW_DEF_MIN_HEIGHT)
		{
			_h = WINDOW_DEF_MIN_HEIGHT;
		}

		SDL_SetWindowSize(sc_window, _w++, _h++); //fixes buffer not reset... strange or its my mistake
		SDL_SetWindowSize(sc_window, _w, _h);
	}

	SDL_FreeSurface(sc_screen);
	sc_screen = SDL_GetWindowSurface(sc_window);
	sc_screenRect = sc_screen->clip_rect;
	sc_screenRect.x = 0;
	sc_screenRect.y = 0;
}

void SC_CapFPS(void)
{
	sc_secondTicks = SDL_GetTicks();

	if(sc_secondTicks - sc_firstTicks < (1000 / WINDOW_MAX_FPS))
	{
		SDL_Delay((1000 / WINDOW_MAX_FPS) - (sc_secondTicks - sc_firstTicks));
	}
}

void SC_DestroyWindow(void)
{
	if(sc_slave)
	{
		SDL_FreeSurface(sc_slave);
	}

	if(sc_screen)
	{
		SDL_FreeSurface(sc_screen);
	}

	if(sc_window)
	{
		SDL_DestroyWindow(sc_window);
	}

	sc_window = NULL;
	sc_slave = NULL;
	sc_screen = NULL;

	SDL_Delay(500); //fixes for me the crash when closing...
	SDL_Quit();

	sc_stopped = 1;
}

int SC_Loop(void* args)
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		sc_loopLastError = SC_ERROR_INIT;

		return SC_ERROR_INIT;
	}

	if(!(sc_window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_DEF_WIDTH, WINDOW_DEF_HEIGHT, SDL_WINDOW_RESIZABLE)))
	{
		sc_loopLastError = SC_ERROR_CREATEWINDOW;

		return sc_loopLastError;
	}

	sc_running = 1;
	sc_stopped = 0;
	SDL_Event event;
	sc_screen = SDL_GetWindowSurface(sc_window);
	sc_screenRect = sc_screen->clip_rect;
	sc_screenRect.x = 0;
	sc_screenRect.y = 0;

	while(sc_running)
	{
		sc_firstTicks = SDL_GetTicks();

		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
			{
				sc_running = 0;
				
				while(sc_isUpdating){};

				break;
			}else if(event.type == SDL_WINDOWEVENT){
				if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					SC_Resize();
				}
			}
		}

		SC_Update();
		SC_Draw();
		SDL_UpdateWindowSurface(sc_window);
		SC_CapFPS();
	}

	SC_DestroyWindow();

	sc_loopLastError = SC_ERROR_STOPED;
	sc_stopped = 1;

	return 0;
}

int SC_Start(void)
{
	if(sc_running || !sc_stopped)
	{
		return SC_ERROR_IS_ALREADY_RUNNING;
	}

	sc_loopLastError = SC_ERROR_NO_ERROR;

	if(!SDL_CreateThread(SC_Loop, "SC_Loop", (void*)NULL))
	{
		return SC_ERROR_CREATETHREAD;
	}
	
	return SC_ERROR_NO_ERROR;
}

void SC_Stop(void)
{
	if(!sc_running && sc_stopped)
	{
		return;
	}

	sc_running = 0;
}