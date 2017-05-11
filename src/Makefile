#
# Makefile
#
EXE = demolito
VERSION = "\"$(shell git show -s --format=%ci | cut -d\  -f1)\""
CC = gcc
PGOBENCH = ./$(EXE) search 13 1

OBJS = bitboard.o eval.o gen.o htable.o magic.o main.o move.o position.o pst.o search.o smp.o sort.o test.o \
	types.o uci.o zobrist.o
	
warnings = yes
debug=no
static = no
popcount = yes
lto = yes

CFLAGS += -std=gnu11 -m64 -O3 -march=native -DVERSION=$(VERSION)
LDFLAGS += -lpthread -lm

#Warnings.
ifeq ($(warnings),yes)
	CFLAGS += -Wall -Wextra -Wshadow -Wcast-align -Wunused -Wdeprecated \
		-Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations
else
	CFLAGS += -w
endif

#Debug.
ifeq ($(debug),yes)
	CFLAGS += -g
	LDFLAGS += -g
else
	CFLAGS += -DNDEBUG
	LDFLAGS += -DNDEBUG
endif

#Static.
ifeq ($(static),yes)
	CFLAGS += -static
	LDFLAGS += -static
endif

ifeq ($(popcount),yes)
	CFLAGS += -mpopcnt -msse3
	LDFLAGS += -mpopcnt -msse3
endif

#LTO.
ifeq ($(debug),no)
ifeq ($(lto),yes)
	CFLAGS += -flto
	LDFLAGS += $(CFLAGS)
endif
endif


.PHONY: $(EXE) pgo strip clean
default: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(LDFLAGS)
	
pgo:
	$(MAKE) clean
	$(MAKE) CFLAGS='$(CFLAGS) -fprofile-generate' LDFLAGS='$(LDFLAGS) -lgcov' $(EXE)
	$(PGOBENCH)
	@rm -f *.o
	$(MAKE) CFLAGS='$(CFLAGS) -fprofile-use -fno-peel-loops -fno-tracer' LDFLAGS='$(LDFLAGS) -lgcov' $(EXE)
	@rm -f *.gcda 

strip:
	strip $(EXE)

clean:
	rm -f $(EXE) $(EXE).exe *.o *.gcda
