ARGS= -Wall -shared -lSDL2
O_PATH = ../../XVR_SC.dll
INCL_PATH= ../include/
SRC= $(wildcard *.c)
PROGS= $(patsubst %.c,%.o,$(SRC))
OBJS= $(wildcard obj/*.o)

all: $(PROGS)

%.o: %.c
	gcc -c $< -I $(INCL_PATH) -o obj/$@

clean:
	PowerShell rm obj/*.o
	
b:
	gcc $(OBJS) -I $(INCL_PATH) $(ARGS) -o $(O_PATH)

bw: 
	gcc $(OBJS) -W -I $(INCL_PATH) $(ARGS) -o $(O_PATH)


