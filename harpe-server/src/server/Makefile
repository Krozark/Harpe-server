SRC = $(wildcard *.c*)
OBJ = $(SRC:.cpp=.o) $(SRC:*.cpp=.o)

all: $(OBJ)
#main

.cpp.o:
	$(CC) $(FLAGS) -o "$@" -c "$<" 
	@mv $@ "$(OBJ_DIR)"

.c.o:
	$(CC) $(FLAGS) -o "$@" -c "$<"
	@mv $@ "$(OBJ_DIR)"


#main: main.cpp 
#	$(CC) $^ $(FLAGS) -o $@


clean:
	@rm -f *.o
	@rm -f *.exe
	@rm -f *.cc
	@rm -f *.hh
	@rm -f *.output

