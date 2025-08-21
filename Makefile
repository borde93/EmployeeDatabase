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
	./$(TARGET) -f ./mynewdb.db

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude
