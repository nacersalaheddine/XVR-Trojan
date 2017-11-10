#ifndef __SC_H
#define __SC_H

#define SC_LIB_SDL2 "SDL2.dll"
#define SC_LIB_SDL2_WEBLINK "https://www.libsdl.org/download-2.0.php"
#define SC_LIB_XVR_SC "XVR_SC.dll"

extern int SC_CanRun;
extern void SC_LoadLibrary(void);
extern void SC_FreeLibrary(void);
extern int SC_Start(void);
extern void SC_Stop(void);
extern void SC_SetImage(unsigned char* data, unsigned int len);
extern int SC_GetLastError(void);

#endif