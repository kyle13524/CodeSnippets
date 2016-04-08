./bin/encodeInput: ./obj/encodeInput.o ./obj/records.o
	cc ./obj/encodeInput.o ./obj/records.o -o ./bin/encodeInput

./obj/encodeInput.o: ./src/encodeInput.c ./inc/protos.h
	cc ./src/encodeInput.c -c -o ./obj/encodeInput.o

./obj/records.o: ./src/records.c ./inc/protos.h
	cc ./src/records.c -c -o ./obj/records.o