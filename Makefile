# Makefile (https://stackoverflow.com/a/2484343)

CC = gcc
LD = gcc

BFLAGS = -O0 -ggdb #-O3
CFLAGS = -I. -std=gnu99 -Wall -Wextra -pedantic
WITH = -pthread -lrt -lm

MODULES   := src # deps
SRC_DIR   := $(addprefix ./,$(MODULES))
BUILD_DIR := $(addprefix .build/,$(MODULES))

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
OBJ       := $(patsubst ./%.c,.build/%.o,$(SRC))
INCLUDES  := $(addprefix -I,$(SRC_DIR))

vpath %.c $(SRC_DIR)

define make-goal
$1/%.o: %.c
	$(CC) $(BFLAGS) $(CFLAGS) $(INCLUDES) -c $$< -o $$@
endef

.PHONY: all checkdirs clean docs

all: checkdirs docs rt-app.x86_64

rt-app.x86_64: $(OBJ)
	$(LD) $^ -o $@ $(WITH)

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR) html rt-app.x86_64

docs:
	@doxygen

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))