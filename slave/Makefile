ARGS= -Wall -DNDEBUG -lws2_32 -lgdi32 -mwindows
ARGS_C= -Wall -DNDEBUG -lws2_32 -lgdi32 -mconsole
ARGS_ML= -ggdb -lws2_32 -lgdi32 -mconsole
O_PATH= bin/PROG_SLAVE.exe
PRG_NAME= PROG_SLAVE.exe
SRC= $(wildcard source/*.c)
SRC_NET= $(wildcard source/net/*.c)
SRC_CMD= $(wildcard source/cmd/*.c)
INCL_PATH= include

bw:
	cls
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(ARGS_C) -I $(INCL_PATH) -o $(O_PATH)

b:
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)

bc:
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(ARGS_C) -I $(INCL_PATH) -w -o $(O_PATH)

dw:
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)
	gdb $(O_PATH)
d:
	gdb $(O_PATH)

d2:
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(ARGS_ML) -I $(INCL_PATH) -o $(O_PATH)
	drmemory -logdir drmemory_log $(O_PATH)

kill:
	taskkill /F /IM $(PRG_NAME)

r:
	run

bwr:
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)
	$(O_PATH)

br:
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)
	run

bcr:
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(ARGS_C) -I $(INCL_PATH) -w -o $(O_PATH)
	run
