.SUFFIXES: .rel .ihx .rom

all: spc700.rom

.c.rel:
	sdcc -mz80 --sdcccall 1 --opt-code-speed $<

.rel.ihx:
	sdcc -mz80 --no-std-crt0 --code-loc 0x4000 -o $@ $<

.ihx.rom:
	objcopy -I ihex -O binary $< $@
	dd if=/dev/zero of=$@ bs=1k seek=15 count=1

clean:
	$(RM) spc700.asm spc700.ihx spc700.lk spc700.lst spc700.map spc700.noi spc700.rel spc700.rom spc700.sym
