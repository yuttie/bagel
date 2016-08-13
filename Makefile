########################################
## Macro Definition
########################################
DEBUG    = -O2
CC       = gcc
CFLAGS   = -Wall -std=c99 $(DEBUG)
CXX      = g++
CXXFLAGS = -Wall $(DEBUG)
LD       = gcc
LDFLAGS  = $(DEBUG)
INCLUDES = -I.
LIBS     = -lX11 -lXi
RM       = rm -f
PROF     = gprof
TARGET   = xwarpptr
SRCS     = main.c
OBJS     = $(SRCS:.c=.o)

########################################
## Rules
########################################
.PHONY: all rebuild clean run profile
all: $(TARGET)
rebuild: clean $(TARGET)
clean:
	$(RM) $(TARGET) $(OBJS)
	$(RM) gmon.out tags *~
run: $(TARGET)
	./$(TARGET)
profile:
	$(PROF) $(TARGET) gmon.out | less
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

# Dependency(Generated by 'gcc -MM *.c')
main.o: main.c

