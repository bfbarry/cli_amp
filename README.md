# Usage (macOS)
As of this writing, works on Mac OS Ventura 13.2

```bash
$ make install-deps
$ make
$ ./cli_amp
```

The amp settings (io devices, and dsp parameters) are defined in `config.json`

# TODO 
- Tests
  - For filters, maybe test the frequency response?
- Other pedals (reverb)
- cellular automaton based psychedelic visualizer

# troubleshooting
- sometimes audio on Mac OS is keeping data in the cache even though I am using the portaudio cleanup pattern
  - in this case `$ sudo killall coreaudiod` seems to help
# notes to self
- initally used the portaudio build from host website, but this relies on  "_Gestalt"
- For VsCode, add `/opt/homebrew/include` to C/C++ Edit configurations (UI)

## important dirs
- `/opt/homebrew/Cellar`: brew libs
- `/opt/homebrew/lib`: symlinks to brew `.dylib`s and `.a`s: 
- `/opt/homebrew/include`: symlinks to brew headers

## some commands
- To start jack now and restart at login:
  `brew services start jack`
- Or, if you don't want/need a background service you can just run:
  `/opt/homebrew/opt/jack/bin/jackd -X coremidi -d coreaudio`
- To start pulseaudio now and restart at login:
  `brew services start pulseaudio`
- Or, if you don't want/need a background service you can just run:
  `/opt/homebrew/opt/pulseaudio/bin/pulseaudio --exit-idle-time\=-1 --verbose`