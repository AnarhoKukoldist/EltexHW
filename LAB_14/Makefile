.PHONY: 2 2/client 2/server

2: 2/server 2/client

2/client: 2/obj/client.o 2/obj/Communication.o 2/obj/UsersList.o 2/obj/UI.o 2/obj/Msg_history.o
	gcc -Wall -Wextra $^ -o $@ -lncurses 

2/server: 2/obj/server.o 2/obj/Communication.o 2/obj/UsersList.o
	gcc -Wall -Wextra $^ -o $@

2/obj/%.o : 2/src/%.c
	gcc -Wall -Wextra -MMD -c $^ -o $@ -I ./2/include -g

