AR		= $(CROSS_COMPILE)ar
CC		= $(CROSS_COMPILE)gcc
CXX		= $(CROSS_COMPILE)g++
LDSHFLAGS	= -rdynamic -shared 
ARFLAGS		= rcv
CFLAGS		= -Wall -g -fPIC -I../src
CXXFLAGS	= -Wall -g -fPIC -I../src
LDFLAGS		= -L.. -liotexemb -lcurl

SRCS=$(wildcard *.c) $(wildcard *.cpp)
OBJDIR=../objs
SCRIPTS=../scripts
TARGETS=$(foreach src, $(SRCS), $(basename $(src)))

.PHONY:all clean objdir test no_int128
.SILENT:clean

all:objdir $(TARGETS)


objdir:
	@mkdir -p $(OBJDIR)

clean:
	$(RM) -rf *.o a.out depend.d $(TARGETS) $(OBJDIR)

$(TARGETS):
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -o $(OBJDIR)/$@ $^ $(LDFLAGS)

no_int128:
	@make clean && make CFLAGS="-Wall -g -fPIC -I../src -D_NO_128INT_"

# Examples
account_meta: account_meta.o

depend.d:$(wildcard *.h *.c)
	$(CC) $(CFLAGS) -MM $^ > $@

-include depend.d