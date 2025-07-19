CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

SRCS = main.c trend.c correlation.c \
       slabinfo/slabinfolist.c \
       vmstat/vmstatlist.c

OBJS = $(SRCS:.c=.o)
TARGET = kernel_leak_detector

INCLUDES = -I./slabinfo -I./vmstat

all: $(TARGET)

$(TARGET): $(OBJS)
    $(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

%.o: %.c
    $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
    rm -f $(OBJS) $(TARGET)

.PHONY: all clean
