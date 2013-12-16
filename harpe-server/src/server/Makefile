SRC = $(wildcard *.c*)
OBJ = $(SRC:.cpp=.o) $(SRC:*.cpp=.o)

SUBDIRS = 

CLEANDIRS = $(SUBDIRS:%=clean-%)

.PHONY: subdirs $(SUBDIRS)
.PHONY: subdirs $(CLEANDIRS)

#all: $(OBJ) subdirs
all: $(OBJ)

#subdirs: $(SUBDIRS)
     
$(SUBDIRS):
	$(MAKE) -C $@

.cpp.o:
	$(CC) $(FLAGS) -o $@ -c $^;
	@cp $@ $(OBJ_DIR)

.c.o :
	$(CC) $(FLAGS) -o $@ -c $^;
	@cp $@ $(OBJ_DIR)


clean: $(CLEANDIRS)
	@rm -f *.o
	@rm -f $(EXEC)

$(CLEANDIRS): 
	$(MAKE) -C $(@:clean-%=%) clean
	@rm -f *.o
	@rm -f $(EXEC)
