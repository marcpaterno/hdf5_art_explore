# Top-level makefile for h5fnal

SOURCE_DIR = src
TEST_DIR = test

all: src test

.PHONY: src test

src:
	@$(MAKE) -C $(SOURCE_DIR)

test:
	@$(MAKE) -C $(TEST_DIR)

check:
	@$(MAKE) -C $(TEST_DIR) check

clean:
	@$(MAKE) -C $(SOURCE_DIR) clean
	@$(MAKE) -C $(TEST_DIR) clean
