ARGS= -Wall -DNDEBUG -lws2_32
ARGS_ML= -ggdb -lws2_32
O_PATH= bin/PROG_MASTER.exe
PRG_NAME= PROG_MASTER.exe
SRC= $(wildcard source/*.c)
SRC_NET= $(wildcard source/net/*.c)
SRC_CMD= $(wildcard source/cmd/*.c)
SRC_CMD_HDD= $(wildcard source/cmd/hdd/*.c)
INCL_PATH= include

bw:
	cls
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(SRC_CMD_HDD) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)

b:
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(SRC_CMD_HDD) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)

dw:
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(SRC_CMD_HDD) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)
	gdb $(O_PATH)
d:
	gdb --args $(O_PATH) -Color -Time
	
d2:
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(SRC_CMD_HDD) $(ARGS_ML) -I $(INCL_PATH) -o $(O_PATH)
	drmemory -logdir drmemory_log $(O_PATH) -Time -Color -GeoIP
	
kill:
	taskkill /F /IM $(PRG_NAME)

r:
	run_all

bwr:
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(SRC_CMD_HDD) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)
	run_all

br:
	gcc $(SRC) $(SRC_NET) $(SRC_CMD) $(SRC_CMD_HDD) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)
	run_all
