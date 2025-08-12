#include <stdio.h>
#include <stdint.h>

#define CHPUT #0x00a2

__sfr __at 0x40 EXTIO;
__sfr __at 0x41 I8251_STAT;
__sfr __at 0x42 I8251_DATA;

__sfr __at 0x1c SPC700_RESET;

void main(void) __naked
{
__asm
	.db #0x41, #0x42, #0x10, #0x40, #0x00, #0x00, #0x00, #0x00
	.db #0x00, #0x00, #0x00, #0x00, #0x00, #0x00, #0x00, #0x00

	call	_doit
__endasm;
}

int putchar(int c) __naked
{
	(void)c;
__asm
	ld	a, l
	call	CHPUT
	di
	ld	de, #0
	ret
__endasm;
}

static inline uint8_t i8251_recv(void)
{
	while ((I8251_STAT & 0b00000010) == 0);
	return I8251_DATA;
}

static inline void i8251_send(uint8_t data)
{
	//while ((I8251_STAT & 0b00000001) == 0); // 無駄
	I8251_DATA = data;
}

static void outp(uint8_t data, uint8_t port) __naked
{
	(void)data;
	(void)port;
__asm
	push	bc
	ld	c, l
	out	(c), a
	pop	bc
	ret
__endasm;
}

static uint8_t inp(uint8_t port) __naked
{
	(void)port;
__asm
	push	bc
	ld	c, a
	in	a, (c)
	pop	bc
	ret
__endasm;
}

static void doit(void)
{
	uint8_t data;

	printf("\x0cSPC700:");

	(void)SPC700_RESET;
__asm
	ei
	halt
	halt
	halt
	halt
	di
__endasm;
	if ((inp(0x18) == 0xaa) && (inp(0x19) == 0xbb)) {
		printf("detected");
	} else {
		printf("not detected");
	}

	EXTIO = 0x69;

	while (1) {
		data = i8251_recv();
		switch (data >> 6) {
		case 0b00: /* WR */ /* 0b000000AA, DATA */
			outp(i8251_recv(), 0x18 | (data & 0b11));
			break;
		case 0b01: /* RD */ /* 0b010000AA */
			i8251_send(inp(0x18 | (data & 0b11)));
			break;
		case 0b11: /* RESET */
			(void)SPC700_RESET;
			break;
		}
	}
}
