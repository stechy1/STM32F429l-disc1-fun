
/* public domain Simple, Minimalistic JPEG writer - http://jonolick.com
*
* Quick Notes:
* 	Based on a javascript jpeg writer
* 	JPEG baseline (no JPEG progressive)
* 	Supports 1, 3 or 4 component input. (luminance, RGB or RGBX)
*
* Latest revisions:
*	1.52 (2012-22-11) Added support for specifying Luminance, RGB, or RGBA via comp(onents) argument (1, 3 and 4 respectively).
*	1.51 (2012-19-11) Fixed some warnings
*	1.50 (2012-18-11) MT safe. Simplified. Optimized. Reduced memory requirements. Zero allocations. No namespace polution. Approx 340 lines code.
*	1.10 (2012-16-11) compile fixes, added docs,
*		changed from .h to .cpp (simpler to bootstrap), etc
* 	1.00 (2012-02-02) initial release
*
* Basic usage:
*	char *foo = new char[128*128*4]; // 4 component. RGBX format, where X is unused
*	jo_write_jpg("foo.jpg", foo, 128, 128, 4, 90); // comp can be 1, 3, or 4. Lum, RGB, or RGBX respectively.
*
* */

//#ifndef JO_JPEG_HEADER_FILE_ONLY

#if defined(_MSC_VER) && _MSC_VER >= 0x1400
#define _CRT_SECURE_NO_WARNINGS // suppress warnings about fopen()
#endif


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "nanoJpegEncoder.h"
#include "OV7670_control.h"


//uint8_t SDRAM_JPEG[640*480*3] = { 0 };

extern uint8_t* gPtrJPEGImg;
extern __IO uint32_t gTime;
int mainA(int argc, char* argv[])
{
  unsigned char* buf;
  unsigned char* ptr;
  FILE* f;
  int size;

  f = fopen("exa.ppm", "rb");
  if (!f)
  {
    printf("Error opening the input file.\n");
    return 1;
  }
  fseek(f, 0, SEEK_END);
  size = (int)ftell(f);

  buf = (unsigned char*)malloc(size);
  fseek(f, 0, SEEK_SET);
  size = (int)fread(buf, 1, size, f);
  fclose(f);

  jo_write_jpg("exa_out.jpeg", buf, 22, 3, 3, 100);

  FILE *fp = fopen("exa_out.jpeg", "wb");
  if (!fp)
  {
    printf("Error opening the output file.\n");
    return false;
  }
  //fwrite(SDRAM_JPEG, sizeof(uint8_t), (gPtrJPEGImg - SDRAM_JPEG), fp);
  for (ptr = (uint8_t*)SDRAM_JPEG; ptr < gPtrJPEGImg; ptr++)
  {
    fputc(*ptr, fp);
    printf("ptr=%p 0x%x\n", ptr, *ptr);
  }
  fclose(fp);

  return 0;
}

void fwriteEx(const unsigned char *buf, int numberOfElements, int size)
{
  //int i;
  //static int cnt = 30;
  //unsigned char* ptr;
  //*
  //printf("b_%p 0x%x elements >%d size >%d\r\n", gPtrJPEGImg, *buf, numberOfElements,size);

  //for (i = 0;i<numberOfElements;i++)
  //  printf("0x%x ",buf[i]);

  while (numberOfElements--)
  {
    *gPtrJPEGImg++ = *buf++;

    //printf("f_%p 0x%x elements >%d size >%d\r\n", buf, *buf, numberOfElements,size);
    //printf("f_%d\r\n", numberOfElements);

    //if ((gPtrJPEGImg > (uint8_t*)(SDRAM_BANK_ADDR)) && (gPtrJPEGImg < (uint8_t*)(SDRAM_BANK_ADDR+IS42S16400J_SIZE)))
    //gPtrJPEGImg++;
    //else
    //  printf("E:>%p 0x%x. Line >%d\r\n", gPtrJPEGImg,*gPtrJPEGImg,__LINE__);
  }
  //*/
}

inline void putcEx(uint8_t c)
{
  //*/
  *gPtrJPEGImg++ = c;
  //printf("c_%p 0x%x\r\n", gPtrJPEGImg,c);
  //if ((gPtrJPEGImg > (uint8_t*)(SDRAM_BANK_ADDR)) && (gPtrJPEGImg < (uint8_t*)(SDRAM_BANK_ADDR+IS42S16400J_SIZE)))
  //gPtrJPEGImg++;
  //else
  //  printf("E_%p 0x%x\r\n", gPtrJPEGImg,c);
  //*/

}

static const unsigned char s_jo_ZigZag[] = { 0, 1, 5, 6, 14, 15, 27, 28, 2, 4, 7, 13, 16, 26, 29, 42, 3, 8, 12, 17, 25, 30, 41, 43, 9, 11, 18, 24, 31, 40, 44, 53, 10, 19, 23, 32, 39, 45, 52, 54, 20, 22, 33, 38, 46, 51, 55, 60, 21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49, 57, 58, 62, 63 };

static void jo_writeBits(int &bitBuf, int &bitCnt, const  unsigned short *bs)
{
  bitCnt += bs[1];
  bitBuf |= bs[0] << (24 - bitCnt);
  while (bitCnt >= 8)
  {
    unsigned char c = (bitBuf >> 16) & 255;
    putcEx(c);
    if (c == 255)
    {
      putcEx(0);
    }
    bitBuf <<= 8;
    bitCnt -= 8;
  }
}

static void jo_DCT(int &d0, int &d1, int &d2, int &d3, int &d4, int &d5, int &d6, int &d7)
{
  int tmp0 = d0 + d7;
  int tmp7 = d0 - d7;
  int tmp1 = d1 + d6;
  int tmp6 = d1 - d6;
  int tmp2 = d2 + d5;
  int tmp5 = d2 - d5;
  int tmp3 = d3 + d4;
  int tmp4 = d3 - d4;

  // Even part
  int tmp10 = tmp0 + tmp3;	// phase 2
  int tmp13 = tmp0 - tmp3;
  tmp0 = tmp1 + tmp2;		//tmp11
  tmp1 = tmp1 - tmp2;		//tmp12

  d0 = tmp10 + tmp0; 		// phase 3
  d4 = tmp10 - tmp0;

  //float zz1 = (tmp1 + tmp13) * 0.707106781f; // c4
  int z1 = ((tmp1 + tmp13) * (724)) >> 10; // c4
  d2 = tmp13 + z1; 		// phase 5
  d6 = tmp13 - z1;

  // Odd part
  tmp10 = tmp4 + tmp5; 		// phase 2
  //tmp0 = tmp5 + tmp6;
  tmp1 = tmp6 + tmp7;

  // The rotator is modified from fig 4-8 to avoid extra negations.
  //float zz5 = (tmp10 - tmp1) * 0.382683433f; // c6 z5
  int z5 = ((tmp10 - tmp1) * 392) >> 10; // c6 z5

  //int ttmp2 = tmp10 * 0.541196100f + z5; // c2-c6	z2
  tmp2 = ((tmp10 * 554) >> 10) + z5; // c2-c6	z2

  tmp4 = ((tmp1 * 1337) >> 10) + z5; // c2+c6		z4
  //int ttmp4 = tmp1 * 1.306562965f + z5; // c2+c6		z4

  //tmp3 = (tmp5 + tmp6) * 0.707106781f; // c4		z3
  tmp3 = ((tmp5 + tmp6) * 724)>>10; // c4		z3

  z1 = tmp7 + tmp3;		// phase 5
  z5 = tmp7 - tmp3;

  d5 = z5 + tmp2;			// phase 6
  d3 = z5 - tmp2;
  d1 = z1 + tmp4;
  d7 = z1 - tmp4;
}


static void jo_calcBits(int val, unsigned short bits[2])
{
  int tmp1 = val < 0 ? -val : val;
  val = val < 0 ? val - 1 : val;
  bits[1] = 1;
  while (tmp1 >>= 1)
  {
    ++bits[1];
  }
  bits[0] = val & ((1 << bits[1]) - 1);
}

//static int jo_processDU(uint8_t *fp, int &bitBuf, int &bitCnt, int *CDU, float *fdtbl, int DC, const  unsigned short HTDC[256][2], const unsigned short HTAC[256][2])
static int jo_processDU(uint8_t *fp, int &bitBuf, int &bitCnt, int *CDU, int *fdtbl, int DC, const  unsigned short HTDC[256][2], const unsigned short HTAC[256][2])
{
  const unsigned short EOB[2] = { HTAC[0x00][0], HTAC[0x00][1] };
  const unsigned short M16zeroes[2] = { HTAC[0xF0][0], HTAC[0xF0][1] };

  // DCT rows
  //for (int dataOff = 0; dataOff < 64; dataOff += 8)
  {
    //jo_DCT(CDU[dataOff], CDU[dataOff + 1], CDU[dataOff + 2], CDU[dataOff + 3], CDU[dataOff + 4], CDU[dataOff + 5], CDU[dataOff + 6], CDU[dataOff + 7]);
    jo_DCT(CDU[0], CDU[1], CDU[2], CDU[3], CDU[4], CDU[5], CDU[6], CDU[7]);
    jo_DCT(CDU[8], CDU[9], CDU[10], CDU[11], CDU[12], CDU[13], CDU[14], CDU[15]);
    jo_DCT(CDU[16], CDU[17], CDU[18], CDU[19], CDU[20], CDU[21], CDU[22], CDU[23]);
    jo_DCT(CDU[24], CDU[25], CDU[26], CDU[27], CDU[28], CDU[29], CDU[30], CDU[31]);
    jo_DCT(CDU[32], CDU[33], CDU[34], CDU[35], CDU[36], CDU[37], CDU[38], CDU[39]);
    jo_DCT(CDU[40], CDU[41], CDU[42], CDU[43], CDU[44], CDU[45], CDU[46], CDU[47]);
    jo_DCT(CDU[48], CDU[49], CDU[50], CDU[51], CDU[52], CDU[53], CDU[54], CDU[55]);
    jo_DCT(CDU[56], CDU[57], CDU[58], CDU[59], CDU[60], CDU[61], CDU[62], CDU[63]);
  }


  // DCT columns
  //for (int dataOff = 0; dataOff < 8; ++dataOff)
  {
    jo_DCT(CDU[0], CDU[8], CDU[16], CDU[24], CDU[32], CDU[40], CDU[48], CDU[56]);
    jo_DCT(CDU[1], CDU[9], CDU[17], CDU[25], CDU[33], CDU[41], CDU[49], CDU[57]);
    jo_DCT(CDU[2], CDU[10], CDU[18], CDU[26], CDU[34], CDU[42], CDU[50], CDU[58]);
    jo_DCT(CDU[3], CDU[11], CDU[19], CDU[27], CDU[35], CDU[43], CDU[51], CDU[59]);
    jo_DCT(CDU[4], CDU[12], CDU[20], CDU[28], CDU[36], CDU[44], CDU[52], CDU[60]);
    jo_DCT(CDU[5], CDU[13], CDU[21], CDU[29], CDU[37], CDU[45], CDU[53], CDU[61]);
    jo_DCT(CDU[6], CDU[14], CDU[22], CDU[30], CDU[38], CDU[46], CDU[54], CDU[62]);
    jo_DCT(CDU[7], CDU[15], CDU[23], CDU[31], CDU[39], CDU[47], CDU[55], CDU[63]);
  }
  // Quantize/descale/zigzag the coefficients
  int DU[64];
  for (int i = 0; i < 64; ++i)
  {
    //float v = (CDU[i] * fdtbl[i]);
    //DU[s_jo_ZigZag[i]] = (int)(v < 0 ? (v - 0.5f) : (v + 0.5f));

    DU[s_jo_ZigZag[i]] = ((int32_t)CDU[i] * (int32_t)fdtbl[i])/1000000;

    //float v = (CDU[i] * fdtbl[i]);
    //DU[s_jo_ZigZag[i]] = (int)(v < 0 ? (v - 0.5f) : (v + 0.5f));
    //DU[s_jo_ZigZag[i]] = (int)(v < 0 ? ceilf(v - 0.5f) : floorf(v + 0.5f));
  }

  //printf("%d %d %d\r\n",CDU[32],fdtbl[32],DU[s_jo_ZigZag[32]]);
  // Encode DC
  int diff = DU[0] - DC;
  if (diff == 0)
  {
    jo_writeBits(bitBuf, bitCnt, HTDC[0]);
  }
  else
  {
    unsigned short bits[2];
    jo_calcBits(diff, bits);
    jo_writeBits(bitBuf, bitCnt, HTDC[bits[1]]);
    jo_writeBits(bitBuf, bitCnt, bits);
  }

  // Encode ACs
  int end0pos = 63;
  for (; (end0pos > 0) && (DU[end0pos] == 0); --end0pos)
  {
  }
  // end0pos = first element in reverse order !=0
  if (end0pos == 0)
  {
    jo_writeBits(bitBuf, bitCnt, EOB);
    return DU[0];
  }
  for (int i = 1; i <= end0pos; ++i)
  {
    int startpos = i;
    for (; DU[i] == 0 && i <= end0pos; ++i)
    {
    }
    int nrzeroes = i - startpos;
    if (nrzeroes >= 16)
    {
      int lng = nrzeroes >> 4;
      for (int nrmarker = 1; nrmarker <= lng; ++nrmarker)
        jo_writeBits(bitBuf, bitCnt, M16zeroes);

      nrzeroes &= 15;
    }
    unsigned short bits[2];
    jo_calcBits(DU[i], bits);
    jo_writeBits(bitBuf, bitCnt, HTAC[(nrzeroes << 4) + bits[1]]);
    jo_writeBits(bitBuf, bitCnt, bits);
  }
  if (end0pos != 63)
  {
    jo_writeBits(bitBuf, bitCnt, EOB);
  }
  return DU[0];
}

bool jo_write_jpg(const char *filename, const void *data, int width, int height, int comp, int quality)
{
  //static float YDU[64];
  //static float UDU[64];
  //static float VDU[64];

  static int YDU[64];
  static int UDU[64];
  static int VDU[64];
  static bool writeHeader = true;

  //constants that don't pollute global namespace
  static const  unsigned char std_dc_luminance_nrcodes[] = { 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
  static const  unsigned char std_dc_luminance_values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
  static const unsigned char std_ac_luminance_nrcodes[] = { 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
  static const unsigned char std_ac_luminance_values[] =
  {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa
  };
  static const unsigned char std_dc_chrominance_nrcodes[] = { 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
  static const unsigned char std_dc_chrominance_values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
  static const unsigned char std_ac_chrominance_nrcodes[] = { 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
  static const unsigned char std_ac_chrominance_values[] =
  {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa
  };
  // Huffman tables
  static const unsigned short YDC_HT[256][2] = { { 0, 2 }, { 2, 3 }, { 3, 3 }, { 4, 3 }, { 5, 3 }, { 6, 3 }, { 14, 4 }, { 30, 5 }, { 62, 6 }, { 126, 7 }, { 254, 8 }, { 510, 9 } };
  static const unsigned short UVDC_HT[256][2] = { { 0, 2 }, { 1, 2 }, { 2, 2 }, { 6, 3 }, { 14, 4 }, { 30, 5 }, { 62, 6 }, { 126, 7 }, { 254, 8 }, { 510, 9 }, { 1022, 10 }, { 2046, 11 } };
  static const unsigned short YAC_HT[256][2] =
  {
    { 10, 4 }, { 0, 2 }, { 1, 2 }, { 4, 3 }, { 11, 4 }, { 26, 5 }, { 120, 7 }, { 248, 8 }, { 1014, 10 }, { 65410, 16 }, { 65411, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 12, 4 }, { 27, 5 }, { 121, 7 }, { 502, 9 }, { 2038, 11 }, { 65412, 16 }, { 65413, 16 }, { 65414, 16 }, { 65415, 16 }, { 65416, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 28, 5 }, { 249, 8 }, { 1015, 10 }, { 4084, 12 }, { 65417, 16 }, { 65418, 16 }, { 65419, 16 }, { 65420, 16 }, { 65421, 16 }, { 65422, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 58, 6 }, { 503, 9 }, { 4085, 12 }, { 65423, 16 }, { 65424, 16 }, { 65425, 16 }, { 65426, 16 }, { 65427, 16 }, { 65428, 16 }, { 65429, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 59, 6 }, { 1016, 10 }, { 65430, 16 }, { 65431, 16 }, { 65432, 16 }, { 65433, 16 }, { 65434, 16 }, { 65435, 16 }, { 65436, 16 }, { 65437, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 122, 7 }, { 2039, 11 }, { 65438, 16 }, { 65439, 16 }, { 65440, 16 }, { 65441, 16 }, { 65442, 16 }, { 65443, 16 }, { 65444, 16 }, { 65445, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 123, 7 }, { 4086, 12 }, { 65446, 16 }, { 65447, 16 }, { 65448, 16 }, { 65449, 16 }, { 65450, 16 }, { 65451, 16 }, { 65452, 16 }, { 65453, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 250, 8 }, { 4087, 12 }, { 65454, 16 }, { 65455, 16 }, { 65456, 16 }, { 65457, 16 }, { 65458, 16 }, { 65459, 16 }, { 65460, 16 }, { 65461, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 504, 9 }, { 32704, 15 }, { 65462, 16 }, { 65463, 16 }, { 65464, 16 }, { 65465, 16 }, { 65466, 16 }, { 65467, 16 }, { 65468, 16 }, { 65469, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 505, 9 }, { 65470, 16 }, { 65471, 16 }, { 65472, 16 }, { 65473, 16 }, { 65474, 16 }, { 65475, 16 }, { 65476, 16 }, { 65477, 16 }, { 65478, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 506, 9 }, { 65479, 16 }, { 65480, 16 }, { 65481, 16 }, { 65482, 16 }, { 65483, 16 }, { 65484, 16 }, { 65485, 16 }, { 65486, 16 }, { 65487, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 1017, 10 }, { 65488, 16 }, { 65489, 16 }, { 65490, 16 }, { 65491, 16 }, { 65492, 16 }, { 65493, 16 }, { 65494, 16 }, { 65495, 16 }, { 65496, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 1018, 10 }, { 65497, 16 }, { 65498, 16 }, { 65499, 16 }, { 65500, 16 }, { 65501, 16 }, { 65502, 16 }, { 65503, 16 }, { 65504, 16 }, { 65505, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 2040, 11 }, { 65506, 16 }, { 65507, 16 }, { 65508, 16 }, { 65509, 16 }, { 65510, 16 }, { 65511, 16 }, { 65512, 16 }, { 65513, 16 }, { 65514, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 65515, 16 }, { 65516, 16 }, { 65517, 16 }, { 65518, 16 }, { 65519, 16 }, { 65520, 16 }, { 65521, 16 }, { 65522, 16 }, { 65523, 16 }, { 65524, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 2041, 11 }, { 65525, 16 }, { 65526, 16 }, { 65527, 16 }, { 65528, 16 }, { 65529, 16 }, { 65530, 16 }, { 65531, 16 }, { 65532, 16 }, { 65533, 16 }, { 65534, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
  };
  static const unsigned short UVAC_HT[256][2] =
  {
    { 0, 2 }, { 1, 2 }, { 4, 3 }, { 10, 4 }, { 24, 5 }, { 25, 5 }, { 56, 6 }, { 120, 7 }, { 500, 9 }, { 1014, 10 }, { 4084, 12 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 11, 4 }, { 57, 6 }, { 246, 8 }, { 501, 9 }, { 2038, 11 }, { 4085, 12 }, { 65416, 16 }, { 65417, 16 }, { 65418, 16 }, { 65419, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 26, 5 }, { 247, 8 }, { 1015, 10 }, { 4086, 12 }, { 32706, 15 }, { 65420, 16 }, { 65421, 16 }, { 65422, 16 }, { 65423, 16 }, { 65424, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 27, 5 }, { 248, 8 }, { 1016, 10 }, { 4087, 12 }, { 65425, 16 }, { 65426, 16 }, { 65427, 16 }, { 65428, 16 }, { 65429, 16 }, { 65430, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 58, 6 }, { 502, 9 }, { 65431, 16 }, { 65432, 16 }, { 65433, 16 }, { 65434, 16 }, { 65435, 16 }, { 65436, 16 }, { 65437, 16 }, { 65438, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 59, 6 }, { 1017, 10 }, { 65439, 16 }, { 65440, 16 }, { 65441, 16 }, { 65442, 16 }, { 65443, 16 }, { 65444, 16 }, { 65445, 16 }, { 65446, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 121, 7 }, { 2039, 11 }, { 65447, 16 }, { 65448, 16 }, { 65449, 16 }, { 65450, 16 }, { 65451, 16 }, { 65452, 16 }, { 65453, 16 }, { 65454, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 122, 7 }, { 2040, 11 }, { 65455, 16 }, { 65456, 16 }, { 65457, 16 }, { 65458, 16 }, { 65459, 16 }, { 65460, 16 }, { 65461, 16 }, { 65462, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 249, 8 }, { 65463, 16 }, { 65464, 16 }, { 65465, 16 }, { 65466, 16 }, { 65467, 16 }, { 65468, 16 }, { 65469, 16 }, { 65470, 16 }, { 65471, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 503, 9 }, { 65472, 16 }, { 65473, 16 }, { 65474, 16 }, { 65475, 16 }, { 65476, 16 }, { 65477, 16 }, { 65478, 16 }, { 65479, 16 }, { 65480, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 504, 9 }, { 65481, 16 }, { 65482, 16 }, { 65483, 16 }, { 65484, 16 }, { 65485, 16 }, { 65486, 16 }, { 65487, 16 }, { 65488, 16 }, { 65489, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 505, 9 }, { 65490, 16 }, { 65491, 16 }, { 65492, 16 }, { 65493, 16 }, { 65494, 16 }, { 65495, 16 }, { 65496, 16 }, { 65497, 16 }, { 65498, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 506, 9 }, { 65499, 16 }, { 65500, 16 }, { 65501, 16 }, { 65502, 16 }, { 65503, 16 }, { 65504, 16 }, { 65505, 16 }, { 65506, 16 }, { 65507, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 2041, 11 }, { 65508, 16 }, { 65509, 16 }, { 65510, 16 }, { 65511, 16 }, { 65512, 16 }, { 65513, 16 }, { 65514, 16 }, { 65515, 16 }, { 65516, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 16352, 14 }, { 65517, 16 }, { 65518, 16 }, { 65519, 16 }, { 65520, 16 }, { 65521, 16 }, { 65522, 16 }, { 65523, 16 }, { 65524, 16 }, { 65525, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
    { 1018, 10 }, { 32707, 15 }, { 65526, 16 }, { 65527, 16 }, { 65528, 16 }, { 65529, 16 }, { 65530, 16 }, { 65531, 16 }, { 65532, 16 }, { 65533, 16 }, { 65534, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
  };
  static const  int YQT[] = { 16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55, 14, 13, 16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62, 18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99 };
  static const  int UVQT[] = { 17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24, 26, 56, 99, 99, 99, 99, 99, 47, 66, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 };
  //static const float aasf[] = { 1.0f * 2.828427125f, 1.387039845f * 2.828427125f, 1.306562965f * 2.828427125f, 1.175875602f * 2.828427125f, 1.0f * 2.828427125f, 0.785694958f * 2.828427125f, 0.541196100f * 2.828427125f, 0.275899379f * 2.828427125f };

  //if (!data || !filename || !width || !height || comp > 4 || comp < 1 || comp == 2)
  //{
  //printf("Input data is missing. %d\r\n",__LINE__);
  //return false;
  //}

  uint8_t *fp = NULL; //= fopen(filename, "wb");
  //if (!fp) {
  //    return false;
  //}

  quality = quality ? quality : 90;
  quality = quality < 1 ? 1 : quality > 100 ? 100 : quality;
  quality = quality < 50 ? 5000 / quality : 200 - quality * 2;
  //QUALITY = 90
  static unsigned char YTable[64];
  static unsigned char UVTable[64];



  //fixed QUALITY = 90
  static float fdtbl_UV[64] = { 0.041667, 0.022530, 0.019134, 0.011812, 0.006250, 0.007955, 0.011548, 0.022653,
                                0.022530, 0.016243, 0.013795, 0.005895, 0.004506, 0.005735, 0.008326, 0.016332,
                                0.019134, 0.013795, 0.006657, 0.004068, 0.004784, 0.006088, 0.008839, 0.017338,
                                0.011812, 0.005895, 0.004068, 0.004520, 0.005315, 0.006765, 0.009821, 0.019265,
                                0.006250, 0.004506, 0.004784, 0.005315, 0.006250, 0.007955, 0.011548, 0.022653,
                                0.007955, 0.005735, 0.006088, 0.006765, 0.007955, 0.010124, 0.014698, 0.028832,
                                0.011548, 0.008326, 0.008839, 0.009821, 0.011548, 0.014698, 0.021339, 0.041858,
                                0.022653, 0.016332, 0.017338, 0.019265, 0.022653, 0.028832, 0.041858, 0.082107
                              };

  static float fdtbl_Y[64] = { 0.041667, 0.045060, 0.047835, 0.035435, 0.025000, 0.019887, 0.023097, 0.037755,
                               0.045060, 0.032486, 0.022992, 0.019160, 0.018024, 0.009558, 0.013877, 0.029695,
                               0.031890, 0.022992, 0.024408, 0.016272, 0.011959, 0.011070, 0.012627, 0.031524,
                               0.035435, 0.025547, 0.020340, 0.015067, 0.010630, 0.007959, 0.012276, 0.032108,
                               0.031250, 0.022530, 0.013667, 0.009664, 0.008929, 0.007232, 0.010999, 0.030204,
                               0.031819, 0.016386, 0.011070, 0.010408, 0.009943, 0.009642, 0.012781, 0.032036,
                               0.023097, 0.012809, 0.011049, 0.011554, 0.010999, 0.012249, 0.017782, 0.041858,
                               0.032362, 0.018147, 0.018251, 0.019265, 0.020594, 0.028832, 0.039864, 0.082107
                             };

  //fixed QUALITY = 90
  static int idtbl_UV[64] = { 41667, 22530, 19134, 11812, 6250, 7955, 11548, 22653,
                              22530, 16243, 13795, 5895, 4506, 5735, 8326, 16332,
                              19134, 13795, 6657, 4068, 4784, 6088, 8839, 17338,
                              11812, 5895, 4068, 4520, 5315, 6765, 9821, 19265,
                              6250, 4506, 4784, 5315, 6250, 7955, 11548, 22653,
                              7955, 5735, 6088, 6765, 7955, 10124, 14698, 28832,
                              11548, 8326, 8839, 9821, 11548, 14698, 21339, 41858,
                              22653, 16332, 17338, 19265, 22653, 28832, 41858, 82107
                            };

  static int idtbl_Y[64] = { 41667, 45060, 47835, 35435, 25000, 19887, 23097, 37755,
                             45060, 32486, 22992, 19160, 18024, 9558, 13877, 29695,
                             31890, 22992, 24408, 16272, 11959, 11070, 12627, 31524,
                             35435, 25547, 20340, 15067, 10630, 7959, 12276, 32108,
                             31250, 22530, 13667, 9664, 8929, 7232, 10999, 30204,
                             31819, 16386, 11070, 10408, 9943, 9642, 12781, 32036,
                             23097, 12809, 11049, 11554, 10999, 12249, 17782, 41858,
                             32362, 18147, 18251, 19265, 20594, 28832, 39864, 82107
                           };

  // Write Headers
  if (writeHeader == true)
  {
    writeHeader == false;
    for (int i = 0; i < 64; ++i)
    {
      int yti = (YQT[i] * quality + 50) / 100;
      YTable[s_jo_ZigZag[i]] = yti < 1 ? 1 : yti > 255 ? 255 : yti;
      int uvti = (UVQT[i] * quality + 50) / 100;
      UVTable[s_jo_ZigZag[i]] = uvti < 1 ? 1 : uvti > 255 ? 255 : uvti;
    }
    static const unsigned char head0[] = { 0xFF, 0xD8, 0xFF, 0xE0, 0, 0x10, 'J', 'F', 'I', 'F', 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0xFF, 0xDB, 0, 0x84, 0 };
    fwriteEx(head0, sizeof(head0), 1);
    fwriteEx(YTable, sizeof(YTable), 1);
    putcEx(1);
    fwriteEx(UVTable, sizeof(UVTable), 1);
    const unsigned char head1[] = { 0xFF, 0xC0, 0, 0x11, 8, height >> 8, height & 0xFF, width >> 8, width & 0xFF, 3, 1, 0x11, 0, 2, 0x11, 1, 3, 0x11, 1, 0xFF, 0xC4, 0x01, 0xA2, 0 };
    fwriteEx(head1, sizeof(head1), 1);
    fwriteEx(std_dc_luminance_nrcodes + 1, sizeof(std_dc_luminance_nrcodes) - 1, 1);
    fwriteEx(std_dc_luminance_values, sizeof(std_dc_luminance_values), 1);
    putcEx(0x10); // HTYACinfo
    fwriteEx(std_ac_luminance_nrcodes + 1, sizeof(std_ac_luminance_nrcodes) - 1, 1);
    fwriteEx(std_ac_luminance_values, sizeof(std_ac_luminance_values), 1);
    putcEx(1); // HTUDCinfo
    fwriteEx(std_dc_chrominance_nrcodes + 1, sizeof(std_dc_chrominance_nrcodes) - 1, 1);
    fwriteEx(std_dc_chrominance_values, sizeof(std_dc_chrominance_values), 1);
    putcEx(0x11); // HTUACinfo
    fwriteEx(std_ac_chrominance_nrcodes + 1, sizeof(std_ac_chrominance_nrcodes) - 1, 1);
    fwriteEx(std_ac_chrominance_values, sizeof(std_ac_chrominance_values), 1);
    static const  unsigned char head2[] = { 0xFF, 0xDA, 0, 0xC, 3, 1, 0, 2, 0x11, 3, 0x11, 0, 0x3F, 0 };
    fwriteEx(head2, sizeof(head2), 1);
    //printf("Write jpeg header is done. >%p >%d Line=%d\r\n", (gPtrJPEGImg - 1), *(gPtrJPEGImg - 1), __LINE__);
  }
  else
    gPtrJPEGImg = (uint8_t*)(SDRAM_JPEG + 607); //LENGHT of header

  //    printf("Write jpeg header is done. >%p >%d len>%d >%dms Line=%d\r\n", (gPtrJPEGImg - 1), *(gPtrJPEGImg - 1), (int)(gPtrJPEGImg - SDRAM_JPEG), gTime, __LINE__);

  /*---
  uint8_t *ptr = (uint8_t*)SDRAM_JPEG;
  for (int i = 0;i<30;i++)
  printf("%p 0x%x ",&ptr[i],ptr[i]);
  ---*/

  // Encode 8x8 macroblocks
  const  unsigned char *imageData = (const unsigned char *)data;
  int DCY = 0, DCU = 0, DCV = 0;
  static int bitBuf = 0;
  static int bitCnt = 0;
  int ofsG = comp > 1 ? 1 : 0, ofsB = comp > 1 ? 2 : 0;
  int p;
  int r;
  int g;
  int b;
  int pom;
  uint16_t tmp;
  uint32_t n = 0;

  for (int y = 0; y < height; y += 8)     //480
  {

    //printf("Write %d %d %p %p %p %p %d %dms\r\n",__LINE__,p,&imageData[p + 0],gPtrJPEGImg,(uint8_t*)SDRAM_JPEG,(uint8_t*)(SDRAM_BANK_ADDR+IS42S16400J_SIZE),y,(gTime-n));
    n = gTime;
    for (int x = 0; x < width; x += 8)    //640
    {
      for (int row = y, pos = 0; row < y + 8; ++row)
      {
        pom = row*width*comp;
        for (int col = 3*x; col < 3*(x + 8); ++col,++col,++col, ++pos)    //x+8
        {
          p = pom + col;
          /*---
          if (row >= height)
          {
          p -= width*comp*(row + 1 - height);
          printf("%d %d\r\n",p,__LINE__);
          }

          if (col >= width)
          {
          p -= comp*(col + 1 - width);
          printf("%d %d\r\n",p,__LINE__);
          }

          ---*/
          //if ((p >= 0) && (p <= 3*640*480) && (pos < 128))
          //{
          //r = imageData[p + 0]; //RGB888 RGB565
          //g = imageData[p + ofsG];
          //b = imageData[p + ofsB];

          r = imageData[p++]; //RGB888 RGB565
          g = imageData[p++];
          b = imageData[p];

          //YDU[pos] = +0.29900f*r + 0.58700f*g + 0.11400f*b - 128;
          //UDU[pos] = -0.16874f*r - 0.33126f*g + 0.50000f*b;
          //VDU[pos] = +0.50000f*r - 0.41869f*g - 0.08131f*b;

          //YDU[pos] = ((float)((+299 * r + 587 * g + 114 * b) / 1000)) - 128.0f;
          //UDU[pos] = ((float)((-169 * r - 331 * g + 500 * b) / 1000));
          //VDU[pos] = ((float)((+500 * r - 419 * g - 81 * b) / 1000));

          //YDU[pos] = (((+299 * r + 587 * g + 114 * b) >> 10)) - 128;
          //UDU[pos] = (b >> 1) + (((-169 * r - 331 * g) >> 10));
          //VDU[pos] = (r >> 1) + (((-419 * g - 81 * b) >> 10));

          YDU[pos] = (((+306 * r + 601 * g + 116 * b) >> 10)) - 128;
          UDU[pos] = (b >> 1) + (((-173 * r - 338 * g) >> 10));
          VDU[pos] = (r >> 1) + (((-429 * g - 83 * b) >> 10));

          //}
          //else
          //printf("Out of range %d %d %d %p\r\n",__LINE__,p,pos,&imageData[p + 0]);

        }

      }


      DCY = jo_processDU(fp, bitBuf, bitCnt, YDU, idtbl_Y, DCY, YDC_HT, YAC_HT);
      DCU = jo_processDU(fp, bitBuf, bitCnt, UDU, idtbl_UV, DCU, UVDC_HT, UVAC_HT);
      DCV = jo_processDU(fp, bitBuf, bitCnt, VDU, idtbl_UV, DCV, UVDC_HT, UVAC_HT);
    }
    //printf("r=%d g=%d b=%d ",r,g,b);
  }

  // Do the bit alignment of the EOI marker
  static const unsigned short fillBits[] = { 0x7F, 7 };
  jo_writeBits(bitBuf, bitCnt, fillBits);

  // EOI
  putcEx(0xFF);
  putcEx(0xD9);

  //fclose(fp);

  return true;
}
