./bin/dataReader: ./obj/main.o ./obj/semaphoreControl.o ./obj/messageControl.o
	cc ./obj/main.o ./obj/semaphoreControl.o ./obj/messageControl.o -o ./bin/dataReader

./obj/main.o: ./src/main.c ./inc/protos.h
	cc ./src/main.c -c -o ./obj/main.o

./obj/semaphoreControl.o: ./src/semaphoreControl.c
	cc ./src/semaphoreControl.c -c -o ./obj/semaphoreControl.o

./obj/messageControl.o: ./src/messageControl.c
	cc ./src/messageControl.c -c -o ./obj/messageControl.o
