TARGET_1 := server
OBJECTS_1 := server.o encode_decode.o
TARGET_2 := client
OBJECTS_2 := client.o encode_decode.o

CC := g++
CFLAGS = -std=c++11 -g -Wall
LIBS := -lpthread


default: $(TARGET_1) $(TARGET_2)
all: default

%.o: %.cpp
	@echo compiling $< '--->'
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_1): server.o encode_decode.o
	@echo Linking $< '--->'
	$(CC) $(OBJECTS_1) -Wall $(LIBS) -o $@

$(TARGET_2): client.o encode_decode.o
	@echo Linking $< '--->'
	$(CC) $(OBJECTS_2) -Wall $(LIBS) -o $@


.PHONY: default all clean

clean:
	-rm -f *.o
	-rm -f $(TARGET_1) $(TARGET_2)
