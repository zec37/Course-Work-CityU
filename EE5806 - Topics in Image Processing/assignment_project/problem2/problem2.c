/*************************************************************************
Sample C Programs for Computer Project of Digital Image Processing.
These programs provide image reading and writing functions.
Students should modify and extend these programs for the project.
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

unsigned char *bmpread(char *imname, int *n_row, int *n_col);
void bmpsave(char *imname, unsigned char *ima, int n_row, int n_col);
void *histogram_cal(int *len, unsigned char *ima, int n_row, int n_col);
unsigned char *histogram_draw(int *length, int n_row, int n_col);
void histogram(char *imname, int *length, unsigned char *his, unsigned char *img, int n_row, int n_col);
unsigned char *bitplane(int bit, unsigned char *img, int n_row, int n_col);
void modification(int bit, unsigned char *bitpln, unsigned char *img, int n_row, int n_col);

int main(int argc, char *argv[])
{
  int   i, j, n_row, n_col, pix;
  int	length[256] = { 0 };
  unsigned char  *ima, *bitrec, *bit8, *bit7, *bit1, *his;

  /* Read image */

  ima = bmpread("picture_3.bmp", &n_row, &n_col);
 
  /* Show the image size */

  printf("Number of rows = %d, Number of columns = %d\n", n_row, n_col);

  /* Bit plane slicing*/

  bit8 = bitplane(8, ima, n_row, n_col);
  bit7 = bitplane(7, ima, n_row, n_col);
  bit1 = bitplane(1, ima, n_row, n_col);

  /* Reconstruction using bitplane7 and bitplane8 */

  bitrec = (unsigned char *)malloc(n_row * n_col * sizeof(unsigned char));
  for (int i = 0; i < n_row; i++)
  {
	  for (int j = 0; j < n_col; j++)
	  {
		  pix = i * n_row + j;
		  bitrec[pix] = bit8[pix] / 255 * 128 + bit7[pix] / 255 * 64;
	  }
  }

  bmpsave("picture03_tmp.bmp", bitrec, n_row, n_col);
  bmpsave("picture03_tmp8.bmp", bit8, n_row, n_col);
  bmpsave("picture03_tmp1.bmp", bit1, n_row, n_col);

  /* Modification by setting LSB = 0 and MSB = 0 */

  his = (unsigned char *)malloc(256 * 1024 * sizeof(unsigned char));

  modification(8, bitrec, ima, n_row, n_col);
  bmpsave("picture03_tmp8_modified.bmp", bitrec, n_row, n_col);
  histogram("picture03_his8mod.bmp", length, his, bitrec, n_row, n_col);

  modification(1, bitrec, ima, n_row, n_col);
  bmpsave("picture03_tmp1_modified.bmp", bitrec, n_row, n_col);
  histogram("picture03_his1mod.bmp", length, his, bitrec, n_row, n_col);

  return 0;
}

void modification(int bit, unsigned char *bitpln, unsigned char *img, int n_row, int n_col)
{
	int remove = pow(2, (bit - 1));
	for (int i = 0; i < n_row; i++)
	{
		for (int j = 0; j < n_col; j++)
		{
			bitpln[i * n_row + j] = img[i * n_row + j] & ~remove;
		}
	}
}

//This counts the bitplane from LSB to MSB.
unsigned char *bitplane(int bit, unsigned char *img, int n_row, int n_col)
{
	int pix;
	unsigned char *bitpln;
	bitpln = (unsigned char *)malloc(n_row * n_col * sizeof(unsigned char));
	for (int i = 0; i < n_row; i++) 
	{
		for (int j = 0; j < n_col; j++) 
		{ 
			pix = i * n_col + j;
			bitpln[pix] = img[pix] >> (bit - 1) & 1; 
			bitpln[pix] *= 255;
		}
	}
	return bitpln;
}

void *histogram_cal(int *len, unsigned char *img, int n_row, int n_col)
{
	int pix;
	for (int i = 0; i < n_row; i++)
	{
		for (int j = 0; j < n_col; j++)
		{
			pix = (int)img[i * n_col + j];
			len[pix]++;
		}
	}
	return len;
}

unsigned char *histogram_draw(int *length, int n_row, int n_col)
{
	unsigned char *his;
	double size = n_row * n_col;
	double temp;

	his = (unsigned char *)malloc(256 * 1024 * sizeof(unsigned char));

	if (his == NULL) {
		printf("Not enough memory\n");  exit(1);
	}

	for (int c = 0; c < 256; c++)
	{
		temp = (double)length[c] / size;
		length[c] = temp * 1024;
	}

	for (int l = 0; l < 256; l++)
	{
		for (int i = 0; i < length[l]; i++)
		{
			his[l * 1024 + i] = 255 + '0';
		}
	}
	return his;
}

void histogram(char *imname, int *length, unsigned char *his, unsigned char *img, int n_row, int n_col)
{
	histogram_cal(length, img, n_row, n_col);
	his = histogram_draw(length, n_row, n_col);
	bmpsave(imname, his, 256, 1024);
}

unsigned char *bmpread(char *imname, int *n_row, int *n_col)
{
  int            i, ncols, nrows, n_bit, n_tab, inv, mskip;
  short          word1;
  unsigned int   uint1;
  unsigned char  byte1, byte2, *ima;
  FILE           *imdata;

  imdata = fopen(imname, "rb");
  if (imdata == NULL) {
    printf("%s open error.\n", imname);  exit(0);
  }

  fread(&byte1, 1, 1, imdata);  fread(&byte2, 1, 1, imdata);
  if (byte1 != 'B' || byte2 != 'M') {
    printf("Not a BMP file.\n");  exit(0);
  }

  fseek(imdata, 18, 0);

  fread(&ncols, 4, 1, imdata);  fread(&nrows, 4, 1, imdata);
  if (nrows < 0) {nrows = -nrows;  inv = 1;}  else inv = 0;

  fseek(imdata, 2, 1);
  fread(&word1, 2, 1, imdata);   n_bit = word1;
  if (n_bit != 8) {
    printf("n_bit = %d not supported.\n", n_bit);  
	exit(0);
  }

  fread(&uint1, 4, 1, imdata);
  if (uint1 != 0) {
    printf("Compression not supported.\n");  exit(0);
  }

  fseek(imdata, 12, 1);
  fread(&uint1, 4, 1, imdata);  n_tab = uint1;
  fread(&uint1, 4, 1, imdata);
  fseek(imdata, 4*256, 1);

  ima = (unsigned char *)malloc(nrows*ncols*sizeof(unsigned char));
  if (ima == NULL) {
    printf("Not enough memory\n");  exit(1);
  }

  mskip = ncols/4;  if (4*mskip < ncols) mskip++;  mskip = 4*mskip - ncols;

  if (inv == 1) {
    for (i=0; i<nrows; i++) {
      fread(&(ima[i*ncols]), 1, ncols, imdata);
      fseek(imdata, mskip, SEEK_CUR);
    }
  }
  else {
    for (i=nrows-1; i>=0; i--) {
      fread(&(ima[i*ncols]), 1, ncols, imdata);
      fseek(imdata, mskip, SEEK_CUR);
    }
  }

  fclose(imdata);

  *n_col = ncols;  *n_row = nrows;

  return ima;
}


void bmpsave(char *imname, unsigned char *ima, int n_row, int n_col)
{
  int            i, j, mskip;
  unsigned int   uint1;
  short          word1;
  unsigned char  byte1, byte2;
  FILE           *imdata;

  imdata = fopen(imname, "wb");
  if (imdata == NULL) {
    printf("%s open error.\n", imname);  exit(0);
  }

  byte1 = 'B';  byte2 = 'M';
  fwrite(&byte1, 1, 1, imdata);  fwrite(&byte2, 1, 1, imdata);
  uint1 = 54 + 4*256 + n_col*n_row;  fwrite(&uint1, 4, 1, imdata);
  word1 = 0;   fwrite(&word1, 2, 1, imdata);  fwrite(&word1, 2, 1, imdata);
  uint1 = 54 + 4*256;  fwrite(&uint1, 4, 1, imdata);  
  uint1 = 40;  fwrite(&uint1, 4, 1, imdata);
  fwrite(&n_col, 4, 1, imdata);  fwrite(&n_row, 4, 1, imdata);
  word1 = 1;  fwrite(&word1, 2, 1, imdata);
  word1 = 8;  fwrite(&word1, 2, 1, imdata);
  uint1 = 0;  fwrite(&uint1, 4, 1, imdata);

  mskip = n_col/4;  if (4*mskip < n_col) mskip++;  mskip = 4*mskip - n_col;
  uint1 = (n_col+mskip)*n_row;  fwrite(&uint1, 4, 1, imdata);
  j = 11811;  fwrite(&j, 4, 1, imdata);  fwrite(&j, 4, 1, imdata);
  uint1 = 256;   fwrite(&uint1, 4, 1, imdata);
  uint1 = 0;     fwrite(&uint1, 4, 1, imdata);

  for (i=0; i<256; i++) {
    byte1 = i;
    fwrite(&byte1, 1, 1, imdata);  fwrite(&byte1, 1, 1, imdata);
    fwrite(&byte1, 1, 1, imdata);  fwrite(&byte1, 1, 1, imdata);
  }

  byte1 = 0;
  for (i=n_row-1; i>=0; i--) {
    fwrite(&(ima[i*n_col]), 1, n_col, imdata);
    if (mskip != 0) {
      for (j=0; j<mskip; j++) fwrite(&byte1, 1, 1, imdata);
    }
  }

  fclose(imdata);
}


