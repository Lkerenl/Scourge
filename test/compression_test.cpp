#include <lz4.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include "elf64/aplib.h"

#ifndef CB_CALLCONV
# if defined(AP_DLL)
#  define CB_CALLCONV __stdcall
# elif defined(__GNUC__)
#  define CB_CALLCONV
# else
#  define CB_CALLCONV __cdecl
# endif
#endif


static unsigned get_file_size(FILE * fp)
{
  unsigned long saved = ftell(fp);
  unsigned size = 0;
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, saved, SEEK_SET);
  return size;
}

static unsigned int ratio(unsigned int x, unsigned int y)
{
	if (x <= UINT_MAX / 100) {
		x *= 100;
	}
	else {
		y /= 100;
	}

	if (y == 0) {
		y = 1;
	}

	return x / y;
}

int CB_CALLCONV callback(unsigned int insize, unsigned int inpos,
                         unsigned int outpos, void *cbparam)
{
	(void) cbparam;
	printf("\rcompressed %u -> %u bytes (%u%% done)", inpos, outpos,
	       ratio(inpos, insize));

	return 1;
}

bool ap_compress_data(uint8_t * data, uint8_t ** packed_data, unsigned int data_size, unsigned int * packed_size)
{
  clock_t clocks;
  uint8_t * workmem = NULL;
  if((*packed_data = (uint8_t *) malloc(aP_max_packed_size(data_size))) == NULL||
     (workmem = (uint8_t *) malloc(aP_workmem_size(data_size))) == NULL)
  {
    fprintf(stderr, "Can't alloction memory\n");
    return false;
  }

  clocks = clock();
  *packed_size = aPsafe_pack(data, *packed_data, data_size, workmem, callback, NULL);
  clocks = clock() - clocks;

  if(*packed_size == APLIB_ERROR)
  {
    fprintf(stderr, "an error occured while compressing\n" );
    return false;
  }
  printf("\rCompressed %u -> %u bytes (%u%%) in %.2f seconds\n",
       data_size, *packed_size, ratio(data_size, *packed_size),
       (double) clocks / (double) CLOCKS_PER_SEC);
  free(workmem);
  return true;
}

bool ap_decompress_data(uint8_t * packed_data, uint8_t ** data, unsigned int packed_size, unsigned int * depacked_size)
{
  size_t size;
  clock_t clocks;

  *depacked_size = aPsafe_get_orig_size(packed_data);
  if(*depacked_size == APLIB_ERROR)
  {
    fprintf(stderr, "compressed data error\n");
    return false;
  }

  if((*data = (uint8_t *) malloc(*depacked_size)) == NULL)
  {
    fprintf(stderr, "Can't alloction memory\n");
    return false;
  }

  clocks = clock();
  size = aPsafe_depack(packed_data, packed_size, *data, *depacked_size);
  clocks = clock() - clocks;

  if(size != *depacked_size)
  {
    fprintf(stderr, "an error occured while decompressing\n" );
    return false;
  }

  printf("Decompressed %u -> %u bytes in %.2f seconds\n",
       packed_size, size,
       (double) clocks / (double) CLOCKS_PER_SEC);

  return true;
}

// bool lz4_compress_data()


int main(int argc, char const *argv[]) {
  if (argc<3)
  {
    fprintf(stderr, "Usage: ./compression_tester <filename> <packedname>\n");
    return -1;
  }

  FILE * fp = fopen(argv[1],"rb");
  FILE * fpout = fopen(argv[2], "wb");
  if(fp == NULL||fpout == NULL)
  {
    fprintf(stderr, "Can't open file: %s \n", argv[1]);
    return -1;
  }

  unsigned size = get_file_size(fp);

  uint8_t * data = (uint8_t *) malloc(size);
  fread(data, size, 1, fp);

  uint8_t * packed_data = NULL;
  unsigned packed_size;

  ap_compress_data(data, &packed_data, size, &packed_size);
  fwrite(packed_data, packed_size, 1, fpout);

  uint8_t * depacked_data = NULL;
  unsigned depacked_size;
  ap_decompress_data(packed_data, &depacked_data, packed_size, &depacked_size);

  assert(depacked_size == size);
  assert(memcmp(data, depacked_data,size) == 0);

  free(data);
  free(depacked_data);
  free(packed_data);
  fclose(fp);
  fclose(fpout);

  return 0;
}
