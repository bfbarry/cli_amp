SRC_DIR := src
OBJ_DIR := build
CC := g++ -std=c++14 -stdlib=libc++
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
EXEC = cli_amp
CMAKE = -I/opt/homebrew/include -L/opt/homebrew/lib -lportaudio -lsoundio -ljack
#-framework AudioUnit -framework AudioToolbox -framework CoreAudio -framework CoreFoundation


$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CMAKE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) -o $@ -c $^ $(CMAKE)

$(OBJ_DIR):
	mkdir -p $@

install-deps:
	brew install cmake portaudio pulseaudio libsoundio jack nlohmann-json
.PHONY: install-deps

uninstall-deps:
	brew uninstall cmake portaudio pulseaudio libsoundio jack
.PHONY: uninstall-deps

clean:
	rm -f $(EXEC) *.o
.PHONY: clean