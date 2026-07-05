###############################################################################
# Project
###############################################################################

TARGET = rt-linux-ai

###############################################################################
# Compiler
###############################################################################

CC = gcc

CFLAGS = -Wall -Wextra \
          -Iinclude \
          -Idrivers \
          -Itasks \
          -Icore \
          -Imodels

LDFLAGS = -pthread -lm -lgpiod

###############################################################################
# Application Sources
###############################################################################

SRC = \
    app/main.c \
    drivers/camera.c \
    tasks/task_capture.c \
    tasks/task_inference.c \
    core/time_utils.c \
    models/network.c \
    models/weights.c \
    models/input_loader.c\
    drivers/gpio.c

OBJ = $(SRC:.c=.o)

###############################################################################
# Test Sources
###############################################################################

TEST_TARGET = tests/test_network

TEST_SRC = \
    tests/test_network.c \
    models/network.c \
    models/weights.c \
    models/input_loader.c

TEST_GPIO_TARGET = tests/test_gpio

TEST_GPIO_SRC = \
    tests/test_gpio.c \
    drivers/gpio.c

###############################################################################
# Build Application
###############################################################################

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

###############################################################################
# Build Test
###############################################################################

test_network:
	$(CC) $(TEST_SRC) $(CFLAGS) -o $(TEST_TARGET) $(LDFLAGS)

test_gpio:
	$(CC) $(TEST_GPIO_SRC) $(CFLAGS) -o $(TEST_GPIO_TARGET) $(LDFLAGS)

###############################################################################
# Generic Rule
###############################################################################

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

###############################################################################
# Clean
###############################################################################

clean:
	rm -f $(OBJ)
	rm -f $(TARGET)
	rm -f $(TEST_TARGET)
	rm -f $(TEST_GPIO_TARGET)

###############################################################################
# Phony Targets
###############################################################################

.PHONY: all clean test_network test_gpio