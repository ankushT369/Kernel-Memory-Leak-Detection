CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

SRCS = main.c trend.c correlation.c \
       slabinfo/slabinfolist.c \
       vmstat/vmstatlist.c

BUILD_DIR = build
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = kernel_leak_detector

INCLUDES = -I./slabinfo -I./vmstat

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
