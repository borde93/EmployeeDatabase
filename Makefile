TARGET = bin/dbview

# *wildcard searches for all the matching filenames (ex all .c files -> *.c)
SRC = $(wildcard src/*.c)

# $(patsubst pattern,replacement,text) 
# Finds whitespace-separated words in text that match pattern and replaces them with replacement.
# Here pattern may contain a ‘%’ which acts as a wildcard, matching any number of any characters within a word.
# If replacement also contains a ‘%’, the ‘%’ is replaced by the text that matched the ‘%’ in pattern.
# Only the first ‘%’ in the pattern and replacement is treated this way; any subsequent ‘%’ is unchanged."
#
# In this case, it subs
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -f ./mynewdb.db -n
	./$(TARGET) -f ./mynewdb.db -a "John S., 123 Robson st., 38"
	./$(TARGET) -f ./mynewdb.db -a "Betty L., 123 Smithe st., 25"
	./$(TARGET) -f ./mynewdb.db -a "Luffy P.K., 23 Ocean rd., 10"
	./$(TARGET) -f ./mynewdb.db -l
	./$(TARGET) -f ./mynewdb.db -r "Betty L."
	./$(TARGET) -f ./mynewdb.db -l


default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude
