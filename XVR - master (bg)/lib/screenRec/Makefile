ARGS= -lgdi32
O_PATH= bin/PROG.exe
SRC= $(wildcard source/*.c)
INCL_PATH= include

bw:
	gcc $(SRC) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)

b:
	gcc $(SRC) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)

dw:
	gcc $(SRC) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)
	gdb $(O_PATH)
d:
	gcc $(SRC) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)
	gdb $(O_PATH)

r:
	run

bwr:
	gcc $(SRC) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)
	$(O_PATH)

br:
	gcc $(SRC) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)
	run
