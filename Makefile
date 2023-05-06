CC		= gcc

CFLAGS	= -Wall -g
LDFLAGS	= -lncursesw -lnvidia-ml

PNAME	= tsumon

# Files to be compiled
SRCDIR	=  ./src ./src/panels ./src/setup
VPATH	= $(SRCDIR)
SRC_C	= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
OBJS	= $(notdir $(patsubst %.c, %.o, $(SRC_C)))

# Rules to make executable
$(PNAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(PNAME) $^ $(LDFLAGS)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

oclean:
	rm *.o