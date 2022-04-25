GXX = gcc --std=c2x
INCLUDE = -Iinclude
LINKS = 

# Get the library.
INC = $(shell find include/ -type f -name '*.h')
SRC = $(shell find src/ -type f -name '*.c')
OBJ = $(patsubst src/%.c, build/obj/%.o, $(SRC))

# Get test files.
TEST_SRC = $(shell find test/ -type f -name '*.c')
TEST_BIN = $(patsubst test/%.c, build/bin/%, $(TEST_SRC))



# Define PHONY calls.
# clear: Clears all build files.
# loc: Shows amount of lines of code.
# Display: Shows the makefile variables [for debug purposes].
.PHONY: clear loc display


all: build $(TEST_BIN)
build: $(OBJ)


build/bin/%: test/%.c $(OBJ)
	@mkdir -p "$(@D)"
	@echo Compiling "$<"
	@$(GXX) "$<" $(OBJ) -o "$@" $(INCLUDE) $(LINKS)


build/obj/%.o: src/%.c
	@mkdir -p "$(@D)"
	@echo Compiling "$<"
	@$(GXX) -c "$<" -o "$@" $(INCLUDE) $(LINKS)


clear:
	@rm -rf build


loc:
	@wc -l $(INC) $(SRC) $(TEST_SRC)


display:
	@echo SRC=$(SRC)
	@echo OBJ=$(OBJ)
	@echo TEST_SRC=$(TEST_SRC)
	@echo TEST_BIN=$(TEST_BIN)