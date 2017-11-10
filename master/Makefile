ARGS=  -Wall -DNDEBUG -lws2_32
PROG_NAME= MASTER.exe
O_PATH= bin/MASTER.exe
SRC= $(wildcard source/*.c)
SRC_CMD= $(wildcard source/commands/*.c)
SRC_NET= $(wildcard source/net/*.c)
SRC_SERV_CMD= $(wildcard source/server/*.c)
SRC_SERV_HDD= $(wildcard source/server/hdd/*.c)
SRC_ALL= $(SRC) $(SRC_CMD) $(SRC_NET) $(SRC_SERV_CMD) $(SRC_SERV_HDD)
INCL_PATH= include

bw:
	gcc $(SRC_ALL) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)

b:
	gcc $(SRC_ALL) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)

dw:
	gcc $(SRC_ALL) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)
	gdb $(O_PATH)
	
d:
	gdb --args $(O_PATH) -color -time
	
d2:
	gcc $(SRC_ALL) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)
	drmemory -logdir drmemory_log $(O_PATH) -time -color -geoip
	
kill:
	taskkill /F /IM $(PROG_NAME)

r:
	run

bwr:
	gcc $(SRC_ALL) $(ARGS) -I $(INCL_PATH) -o $(O_PATH)
	$(O_PATH)

br:
	gcc $(SRC_ALL) $(ARGS) -I $(INCL_PATH) -w -o $(O_PATH)
	run
