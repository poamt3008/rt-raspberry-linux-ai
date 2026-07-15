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
          -Imodels \
          -Iexternal

LDFLAGS = -pthread -lm -lgpiod

###############################################################################
# Application Sources
###############################################################################

SRC = \
    app/main.c \
    drivers/camera.c \
    drivers/gpio.c \
    drivers/image_proc.c \
    drivers/network_info.c \
    tasks/task_capture.c \
    tasks/task_process.c \
    tasks/task_inference.c \
    tasks/task_network.c \
    core/time_utils.c \
    core/capture_mailbox.c \
    core/inference_mailbox.c \
    core/inference_log.c \
    models/network.c \
    models/weights.c \
    models/input_loader.c\
    core/shared_result_client.c 

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

TEST_IMAGE_PROC_TARGET = tests/test_image_proc

TEST_IMAGE_PROC_SRC = \
    tests/test_image_proc.c \
    drivers/image_proc.c

TEST_PIPELINE_TARGET = tests/test_pipeline

TEST_PIPELINE_SRC = \
    tests/test_pipeline.c \
    drivers/image_proc.c \
    models/network.c \
    models/weights.c

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

test_image_proc:
	$(CC) $(TEST_IMAGE_PROC_SRC) $(CFLAGS) -o $(TEST_IMAGE_PROC_TARGET) $(LDFLAGS)

test_pipeline:
	$(CC) $(TEST_PIPELINE_SRC) $(CFLAGS) -o $(TEST_PIPELINE_TARGET) $(LDFLAGS)

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
	rm -f $(TEST_IMAGE_PROC_TARGET)
	rm -f $(TEST_PIPELINE_TARGET)

###############################################################################
# Phony Targets
###############################################################################

.PHONY: all clean test_network test_gpio test_image_proc test_pipeline