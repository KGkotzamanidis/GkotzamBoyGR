CC=g++
FLAGS=-fdiagnostics-color=always -g -Wall
LINKER=-lSDL3 -lSDL3_image

SOURCE=$(wildcard ./src/*.cpp)
EXECname=GkotzamBoyGR.o

OUTPUT=-o $(EXECname)

clean:
	rm -f $(EXECname)
run:
	./$(EXECname)
build:
	$(CC) $(FLAGS) $(SOURCE) $(LINKER) $(OUTPUT)