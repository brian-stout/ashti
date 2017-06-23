CPPFLAGS+=-Wall -Wextra -Wpedantic
CPPFLAGS+=-Wwrite-strings -Wstack-usage=1024 -Wfloat-equal
CPPFLAGS+=-Waggregate-return -Winline

LDLIBS+=-lm

CFLAGS += -std=c11

BIN=ashti
OBJS=ashti.o directories.o

.PHONY: clean debug profile

$(BIN): $(OBJS)

debug: CFLAG+=-g
debug: $(BIN)

profile: CFLAGS+=-pg
profile: LDFLAGS+=-pg
profile: $(BIN)

clean:
	$(RM) $(OBJS) $(BIN)
