/* Decompress Dell BIOS into its compoments from its .hdr files
 * Written by: Jordan Hargrave
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <limits.h>

#define BUFFER_SIZE 0x40000

typedef struct {
  uint8_t  sig[4];
  uint32_t len1;
  uint32_t len2;
  uint16_t len3;
  uint8_t  txt[13];
} __attribute__((packed)) nhdr_t;

typedef struct {
  uint8_t	hdr_sig[4];
  uint8_t	hdr_len;
  uint8_t	hdr_maj_ver;
  uint8_t	hdr_min_ver;
  uint8_t	hdr_num_sys;
  char		hdr_check[40];
  uint8_t	hdr_version[3];
  uint8_t	hdr_flags;
  uint8_t	hdr_rsvd[6];
  uint16_t	compat_flags;
  uint16_t	sysids[1];
} __attribute__((packed)) rbuhdr_t;

/* Decompress code.. uses a weird RLE */
uint decomp(uint8_t *source, uint8_t *dest, uint sourcesize)
{
  uint x, y, pos, size;

  x = y = 0;
  while (x < sourcesize) {
    if (y > BUFFER_SIZE) {
      printf("Bigger buffer..\n");
      return 0;
    }
    if (source[x] & 0x0f) {
      size = (source[x] & 0xF)+1;
      pos = y - ((uint)source[x+1] | ((uint)(source[x] & 0xf0) << 4)) - 1;
      while (size--)
	dest[y++] = dest[pos++];
      x += 2;
    }
    else {
      size = (source[x++] >> 4) + 1;
      while (size--)
	dest[y++] = source[x++];
    }
  }
  printf("decomp: %x = %x\n", sourcesize, y); 
  return y;
}

void dump(uint8_t *buf, int len)
{
  int i,j,c;

  for (i=0; i<len; i+=16) {
    for (j=0; j<16; j++)
      printf("%.2x ", buf[i+j]);
    printf("  ");
    for (j=0; j<16; j++) {
      c = buf[i+j];
      if  (c < ' ' || c > 'z') c = '.';
      printf("%c", c);
    }
    printf("\n");
  }
}

int main(int argc, char *argv[])
{
  int fd;
  rbuhdr_t *phdr;
  size_t sz;
  int ofd,len, i, id, ilen;
  uint8_t *outbuf, *inbuf, *start;
  char name[PATH_MAX];
  int off,ntab=1;
  uint8_t sig[4] = { 0x00, 0x01, 0x1b, 0x00 };

  setbuf(stdout, NULL);
  if ((fd = open(argv[1], O_RDONLY)) < 0)
    return -1;
  sz = lseek(fd, 0, SEEK_END);
  inbuf = mmap(0, sz, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);

  if (inbuf == (void *)-1)
    return -1;

  phdr = (rbuhdr_t *)inbuf;
  printf("Got HDR: %c%c%c%c\n", phdr->hdr_sig[0],phdr->hdr_sig[1],phdr->hdr_sig[2],phdr->hdr_sig[3]);
  printf("header size: %x\n", phdr->hdr_len);
  printf("Hdr Ver: %u.%u\n", phdr->hdr_maj_ver, phdr->hdr_min_ver);
  if (phdr->hdr_maj_ver < 2)
	printf("Version: %c%c%c\n", phdr->hdr_version[0],phdr->hdr_version[1],phdr->hdr_version[2]);
  else
	printf("Version: %u.%u.%u\n",phdr->hdr_version[0],phdr->hdr_version[1],phdr->hdr_version[2]);
  printf("System IDS: ");
  for (i=0; i<phdr->hdr_num_sys; i++) {
    id = (phdr->sysids[i] & 0xFF) | ((phdr->sysids[i] & 0xF800) >> 3);
    printf("%.4x,",id);
  }
  printf("\n");

  outbuf = malloc(BUFFER_SIZE);
  memset(outbuf, 0, BUFFER_SIZE);

  start = inbuf + phdr->hdr_len;
  if (phdr->hdr_maj_ver >= 2)
	start += 0x1000;
  while ((start - inbuf) < sz) {
    if (memcmp(start, sig, 4) == 0) {
      /* Newer BIOS show source file name of blob */
      nhdr_t *h = (nhdr_t *)start;

      printf("source: %s\n", h->txt);
      ilen = h->len1;
      off = 0x1B;
    }
    else {
      ilen = *(uint16_t *)start;
      off = 2;
    }
    len = decomp(start+off,outbuf,ilen);
    dump(outbuf, 256);

    /* Export ACPI DSDT and SSDT */
    if (!strncmp(outbuf, "DSDT", 4)) {
	snprintf(name, sizeof(name), "%s.DSDT.%x", argv[1], ntab++);
	ofd = open(name, O_WRONLY|O_CREAT, 0644);
	write(ofd, outbuf, len);
	close(ofd);
    }
   if (!strncmp(outbuf, "SSDT", 4)) {
	snprintf(name, sizeof(name), "%s.SSDT.%x", argv[1], ntab++);
	ofd = open(name, O_WRONLY|O_CREAT, 0644);
	write(ofd, outbuf, len);
	close(ofd);
    }
    start += off+ilen;
  }
}
