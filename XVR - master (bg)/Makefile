ARGS= -Wall -DNDEBUG -lws2_32 -lgdi32
O_PATH= bin/PROG-ENG.exe
SRC= $(wildcard source/*.c)
SRC_CMD= $(wildcard source/cmd/*.c) $(wildcard source/sc/*.c)
INCL_PATH= include

bw:
	gcc $(SRC) $(SRC_CMD) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)

b:
	gcc $(SRC) $(SRC_CMD) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)

dw:
	gcc $(SRC) $(SRC_CMD) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)
	gdb $(O_PATH)
d:
	gcc $(SRC) $(SRC_CMD) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)
	gdb $(O_PATH)

r:
	run

bwr:
	gcc $(SRC) $(SRC_CMD) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)
	$(O_PATH)

br:
	gcc $(SRC) $(SRC_CMD) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)
	run
