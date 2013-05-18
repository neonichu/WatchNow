PEBBLE_SDK=$(HOME)/Downloads/PebbleKit-v1.1/Pebble

.PHONY: all clean

all: waf
	./waf configure build

clean:
	rm -f lib include pebble_app.ld tools waf wscript
	rm -f resources/wscript
	rm -rf build

waf:
	$(PEBBLE_SDK)/tools/create_pebble_project.py --symlink-only $(PEBBLE_SDK)/sdk .
