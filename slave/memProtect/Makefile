ARGS=
PROG_NAME= PROG.exe
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
	gdb $(O_PATH)
	
d2:
	gcc $(SRC) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)
	drmemory -logdir drmemory_log $(O_PATH)
	
kill:
	taskkill /F /IM $(PROG_NAME)

r:
	run

bwr:
	gcc $(SRC) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)
	$(O_PATH)

br:
	gcc $(SRC) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)
	run
