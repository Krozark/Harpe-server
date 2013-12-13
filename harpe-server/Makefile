export CC = g++
INCPATH = -I$(CURDIR)/include -I$(CURDIR)/src 
LIBS = -lsocket -lpthread 
#-lmysqlcppconn 
export DEFINES = -DCOLORS -DNDEBUG
export FLAGS = -Wall -og -std=c++0x $(INCPATH) $(LIBS) $(DEFINES)
export TOP = $(CURDIR)
export OBJ_DIR = $(TOP)/obj

SRC = $(wildcard *.c*)
OBJ = $(SRC:.cpp=.o) $(SRC:*.cpp=.o)

SUBDIRS = src obj

export EXEC = harpe-server


CLEANDIRS = $(SUBDIRS:%=clean-%)

.PHONY: subdirs $(SUBDIRS)
.PHONY: subdirs $(CLEANDIRS)

all: src $(OBJ) obj

doc : doc/html 

doc/html :
	cd doc && doxygen

src:
	$(MAKE) -C $@

obj:
	$(MAKE) -C $@

.cpp.o:
	$(CC) $(FLAGS) -o $@ -c $^;
	@mv $@ "$(OBJ_DIR)"

.c.o :
	$(CC) $(FLAGS) -o $@ -c $^;
	@mv $@ "$(OBJ_DIR)"


clean: $(CLEANDIRS)

$(CLEANDIRS): 
	$(MAKE) -C $(@:clean-%=%) clean
	@rm -f *.o
	#@rm -f $(SHARED)
	#@rm -f $(STATIC)
	@rm -f $(EXEC)

