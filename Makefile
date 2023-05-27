EMUL_DIR = ~/work/retro/px8/emul
VFMAN_DIR = ~/work/retro/px8/tools/vfman
VFLOPPY_DIR = ~/work/retro/px8/tools/vfloppy
SERIAL = /dev/ttyS0

dino: DINO.ROM

DINO.ROM: main.c dino_sprites.h cactus_sprite.h utils.lib
	zcc +cpm -lutils -subtype=px8 -create-app -odino main.c

dino.com: main.c dino_sprites.h cactus_sprite.h utils.lib
	zcc +cpm -lutils -subtype=px8 -odino.com main.c

utils.lib: utils.asm
	z80asm -xutils.lib utils.asm

run: DINO.ROM
	cp DINO.ROM $(EMUL_DIR)/BASIC.ROM
	wine $(EMUL_DIR)/px8.exe

deploy: dino.com
	rm -f dino.d88
	$(VFMAN_DIR)/vformat dino.d88
	$(VFMAN_DIR)/vfwrite dino.d88 dino.com
	$(VFLOPPY_DIR)/epspdv3 -s $(SERIAL) -0 dino.d88

trexpx4: trexpx4.com trexpx4.d88

trexpx4.com: trex_px4.asm
	z80asm.exe -b -s -l -m -g -reloc-info -otrexpx4.com trex_px4.asm

trexpx4.d88: trexpx4.com
	rm -f trexpx4.d88
	$(VFMAN_DIR)/vformat.exe trexpx4.d88
	$(VFMAN_DIR)/vfwrite.exe trexpx4.d88 trexpx4.com

ALL: dino trexpx4
