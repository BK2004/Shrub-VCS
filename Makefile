CC = g++
CLIBS = -lz -lcrypto
CARGS = 
DIRS = argparse commands libs
SRC_DIR = ./src
BUILD_DIR = ./build
OUT = app.exe
PREGEN = ./src/commands/gen_commands.sh ./src/argparse/gen_argparse.sh
SRC_FILES = $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

$(BUILD_DIR)/$(OUT): pregen $(OBJ_FILES)
	$(CC) -o $@ $(CARGS) $(OBJ_FILES) $(CLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p `echo $@ | rev | cut -d'/' -f2- | rev`
	$(CC) -o $@ -I $(SRC_DIR) -c $< $(CLIBS)

pregen: $(PREGEN)
	for script in $(PREGEN) ; do \
		$$script ; \
	done
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)