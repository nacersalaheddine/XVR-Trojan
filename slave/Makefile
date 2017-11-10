ARGS= -Wall -DNDEBUG -lws2_32 -mwindows
ARGS_C= -Wall -DNDEBUG -lws2_32 -mconsole
ARGS_ML= -ggdb -mconsole
PROG_NAME= SLAVE.exe
O_PATH= bin/SLAVE.exe
SRC= $(wildcard source/*.c)
SRC_NET= $(wildcard source/net/*.c)
SRC_CMD= $(wildcard source/commands/*.c)
SRC_ALL= $(SRC) $(SRC_NET) $(SRC_CMD)
INCL_PATH= include

bw:
	gcc $(SRC_ALL) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)

bcw:
	gcc $(SRC_ALL) $(ARGS_C) -I $(INCL_PATH) -o $(O_PATH)

b:
	gcc $(SRC_ALL) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)

bc:
	gcc $(SRC_ALL) $(ARGS_C) -I $(INCL_PATH) -w -o $(O_PATH)

	
d:
	gdb $(O_PATH)
	
d2:
	gcc $(SRC_ALL) $(ARGS_ML) -I $(INCL_PATH) -o $(O_PATH)
	drmemory -logdir drmemory_log $(O_PATH)
	
kill:
	taskkill /F /IM $(PROG_NAME)

r:
	run

br:
	gcc $(SRC_ALL) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)
	run

bcr:
	gcc $(SRC_ALL) $(ARGS_C) -I $(INCL_PATH) -w -o $(O_PATH)
	run
