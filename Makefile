EXEC = cli_amp
CMAKE = -I/opt/homebrew/include -L/opt/homebrew/lib -lportaudio -lsoundio -ljack -lsimdjson
#-framework AudioUnit -framework AudioToolbox -framework CoreAudio -framework CoreFoundation


$(EXEC): main.o audio_io.o
	g++ -std=c++14 -stdlib=libc++ -o $@ $^ $(CMAKE)
# $@ substitutes $(EXEC), $^ substitutes inputs

main.o: main.cpp
	g++ -std=c++14 -stdlib=libc++ -c $^ $(CMAKE)

audio_io.o: audio_io.cpp
	g++ -std=c++14 -stdlib=libc++ -c $^ $(CMAKE)

install-deps:
	brew install cmake portaudio pulseaudio libsoundio jack
.PHONY: install-deps

uninstall-deps:
	brew uninstall cmake portaudio pulseaudio libsoundio jack
.PHONY: uninstall-deps

clean:
	rm -f $(EXEC) *.o
.PHONY: clean