EXEC = cli_amp
CMAKE = -I./lib/portaudio/include ./lib/portaudio/lib/.libs/libportaudio.a -L/opt/homebrew/lib -lsoundio -ljack -framework AudioUnit -framework AudioToolbox -framework CoreAudio -framework CoreFoundation


$(EXEC): main.cpp
	g++ -std=c++14 -stdlib=libc++ -o $@ $^ $(CMAKE)
# $@ substitutes $(EXEC), $^ substitutes main.cpp

install-deps:
	mkdir -p lib
	curl https://files.portaudio.com/archives/pa_stable_v190700_20210406.tgz | tar -zx -C lib
	cd lib/portaudio && ./configure && make
# $(MAKE) for current dir, multithreaded -j 
.PHONY: install-deps

uninstall-deps:
	cd lib && $(MAKE) uninstall
	rm -rf lib/portaudio
.PHONY: uninstall-deps

clean:
	rm -f $(EXEC)
.PHONY: clean