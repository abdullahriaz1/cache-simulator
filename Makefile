EXECBIN  = cacher
SOURCES  = $(wildcard *.c)
HEADERS  = $(wildcard *.h)
OBJECTS  = $(SOURCES:%.c=%.o)
FORMATS  = $(SOURCES:%.c=.format/%.c.fmt) $(HEADERS:%.h=.format/%.h.fmt)

CC       = clang
FORMAT   = clang-format
CFLAGS   = -Wall -Wpedantic -Werror -Wextra -DDEBUG

.PHONY: all clean format

all: $(EXECBIN)

$(EXECBIN): $(OBJECTS)
	$(CC) -o $@ $^ -pthread

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(EXECBIN) $(OBJECTS)

nuke: clean
	rm -rf .format

format: $(FORMATS)

.format/%.c.fmt: %.c
	mkdir -p .format
	$(FORMAT) -i $<
	touch $@

.format/%.h.fmt: %.h
	mkdir -p .format
	$(FORMAT) -i $<
	touch $@