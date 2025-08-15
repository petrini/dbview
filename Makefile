TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -f ./mynewdb.db -n
	./$(TARGET) -f ./mynewdb.db
	./$(TARGET) -f ./mynewdb.db -a "Timmy H.,123 Sheshire Ln.,120"
	./$(TARGET) -f ./mynewdb.db -a "TheRusty,Olazabal 673, 500"
	./$(TARGET) -f ./mynewdb.db -a "Oxi,Las Heras 543, 250"
	./$(TARGET) -f ./mynewdb.db -l
	./$(TARGET) -f ./mynewdb.db -h "Oxi,300"
	./$(TARGET) -f ./mynewdb.db -l

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -Wall -Wextra -Wpedantic -Werror -c $< -o $@ -Iinclude


