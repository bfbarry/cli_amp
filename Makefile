EXEC = cli_amp
CMAKE = -I/opt/homebrew/include -L/opt/homebrew/lib -lportaudio -lsoundio -ljack
#-framework AudioUnit -framework AudioToolbox -framework CoreAudio -framework CoreFoundation


$(EXEC): main.cpp
	g++ -std=c++14 -stdlib=libc++ -o $@ $^ $(CMAKE)
# $@ substitutes $(EXEC), $^ substitutes main.cpp

install-deps:
	brew install cmake portaudio pulseaudio libsoundio jack
.PHONY: install-deps

uninstall-deps:
	brew uninstall cmake portaudio pulseaudio libsoundio jack
.PHONY: uninstall-deps

clean:
	rm -f $(EXEC)
.PHONY: clean