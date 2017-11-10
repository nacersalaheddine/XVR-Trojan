#ifndef __INPUT_H
#define __INPUT_H

#define INPUT_TYPE_NONE 0
#define INPUT_TYPE_USER 1
#define INPUT_TYPE_MASTER 2

#define INPUT_MAX 256

extern void input_Gets_Init(void);
extern char* input_Gets(void);
extern int input_Get(int type, char** str);
extern int input_IsKeyPressed(unsigned char key);

#endif