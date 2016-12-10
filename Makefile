CC = g++
VPATH = src
CXXFLAG = --std=c++11 -I include -Ofast
HEADERS = $(wildcard include/*.h)
LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
OBJ = main.o Circle.o Game.o
EXEC = LD37
all : ll
%.o : %.cpp
	$(CC) -c $< -o $@ $(CXXFLAG)
ll : $(OBJ) $(HEADERS)
	$(CC) -o $(EXEC) $(OBJ) $(LIBS)
.PHONY : clean
clean :
	rm $(EXEC) $(OBJ)
