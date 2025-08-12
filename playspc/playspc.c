#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <windows.h>

typedef struct {
	uint32_t cbSize;
	uint32_t transmitType;
	uint32_t bScript700;
	int32_t (__stdcall *pCallbackRead)(int32_t);
	void (__stdcall *pCallbackWrite)(int32_t, int32_t);
} cb_t;

typedef void (__stdcall *loadf_t)(void *);
typedef int32_t (__stdcall *trans_t)(cb_t *);

static int port;

static __stdcall void apc700_write(int32_t addr, int32_t data)
{
	uint8_t buf[2] = { 0b00000000 | addr, data };

	write(port, buf, 2);
}

static __stdcall int32_t apc700_read(int32_t addr)
{
	uint8_t buf[1] = { 0b01000000 | addr };

	write(port, buf, 1);
	read(port, buf, 1);

	return buf[0];
}

int main(int argc, char *argv[])
{
	HMODULE dll;
	loadf_t loadf;
	trans_t trans;
	char pname[16];
	int file;
	struct stat st;
	void *spcdat;
	ssize_t rd;
	uint8_t buf[1] = { 0b11000000 };
	cb_t cb = {
		.cbSize = sizeof(cb),
		.transmitType = 3,
		.bScript700 = 0,
		.pCallbackRead = apc700_read,
		.pCallbackWrite = apc700_write
	};

	if (argc != 3) {
		fprintf(stderr, "usage: %s <comport> <spcfile>", argv[0]);
		return 1;
	}

	if (!(dll = LoadLibrary("snesapu.dll"))) {
		fprintf(stderr, "LoadLibrary(\"snesapu.dll\") failed\n");
		return 1;
	}
	if (!(loadf = (loadf_t)GetProcAddress(dll, "LoadSPCFile"))) {
		fprintf(stderr, "GetProcAddress(\"LoadSPCFile\") failed\n");
		return 1;
	}
	if (!(trans = (trans_t)GetProcAddress(dll, "TransmitSPCEx"))) {
		fprintf(stderr, "GetProcAddress(\"TransmitSPCEx\") failed\n");
		return 1;
	}

	snprintf(pname, sizeof(pname), "\\\\.\\%s", argv[1]);
	if ((port = open(pname, O_RDWR | O_BINARY)) < 0) {
		perror(argv[1]);
		return 1;
	}

	if ((file = open(argv[2], O_RDONLY | O_BINARY)) < 0) {
		perror(argv[2]);
		return 1;
	}
	if (fstat(file, &st) < 0) {
		perror("fstat()");
		return 1;
	}
	if (st.st_size < 66048) {
		st.st_size = 66048;
	}
	if (!(spcdat = calloc(1, st.st_size))) {
		perror("calloc()");
		return 1;
	}
	if ((rd = read(file, spcdat, st.st_size)) < 0) {
		perror("read()");
		return 1;
	}
	close(file);

	printf("read %d bytes.\n", rd);

	printf("resetting...");
	fflush(stdout);

	write(port, buf, 1);
	while ((apc700_read(0x0) != 0xaa) || (apc700_read(0x1) != 0xbb));

	printf(" done.\n");

	printf("sending...");
	fflush(stdout);

	loadf(spcdat);
	trans(&cb);

	printf(" done.\n");

	free(spcdat);
	close(port);
	FreeLibrary(dll);

	return 0;
}
