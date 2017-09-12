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
void gamma(unsigned char *ima, int n_row, int n_col, double ga);
unsigned char *eq(int *eq_stat, unsigned char *ima, int n_row, int n_col);
void *histogram_cal(int *len, unsigned char *ima, int n_row, int n_col);
unsigned char *histogram_draw(int *length, int n_row, int n_col); 
void histogram(char *imname, int *length, unsigned char *his, unsigned char *img, int n_row, int n_col);
int arrayMax(int *array, int size);

int main(int argc, char *argv[])
{
  int   i, j, n_row, n_col;
  int	length[256] = { 0 };
  int	eq_stat[256] = { 0 };
  unsigned char  *ima1, *ima2, *his;
  unsigned char	 *eq_t;

  /* Read image */

  ima1 = bmpread("picture_2.bmp", &n_row, &n_col);
  ima2 = bmpread("picture_2.bmp", &n_row, &n_col);

  /* Show the image size */

  printf("Number of rows = %d, Number of columns = %d\n", n_row, n_col);

  /* Save histogram of original picture */

  his = (unsigned char *)malloc(256 * 256 * sizeof(unsigned char));
  histogram("picture_his.bmp", length, his, ima1, n_row, n_col);

  /* Equalization of original picture */
  histogram_cal(eq_stat, ima1, n_row, n_col);
  eq_t = eq(eq_stat, ima1, n_row, n_col);
  bmpsave("picture01_eqo.bmp", eq_t, n_row, n_col);
  histogram("picture01_eqo_his.bmp", length, his, eq_t, n_row, n_col);
  if (sizeof(eq_t) == sizeof(ima1)) { printf("OK"); }

  /* Gamma function */

  gamma(ima1, n_row, n_col, 0.4);
  gamma(ima2, n_row, n_col, 2);

  /* Save output */

  bmpsave("picture01_tmp1.bmp", ima1, n_row, n_col);
  bmpsave("picture01_tmp2.bmp", ima2, n_row, n_col);

  /* Equalization for gamma operated picture */

  histogram_cal(eq_stat, ima1, n_row, n_col);
  eq_t = eq(eq_stat, ima1, n_row, n_col);
  bmpsave("picture01_eq1.bmp", eq_t, n_row, n_col);
  histogram("picture01_eq1_his.bmp", length, his, eq_t, n_row, n_col);

  histogram_cal(eq_stat, ima2, n_row, n_col);
  eq_t = eq(eq_stat, ima2, n_row, n_col);
  bmpsave("picture01_eq2.bmp", eq_t, n_row, n_col);
  histogram("picture01_eq2_his.bmp", length, his, eq_t, n_row, n_col);


  /* Save histogram of gamma operated picture */

  histogram("picture01_tmp1_his.bmp", length, his, ima1, n_row, n_col);
  histogram("picture01_tmp2_his.bmp", length, his, ima2, n_row, n_col);

  return 0;
}

void histogram(char *imname, int *length, unsigned char *his, unsigned char *img, int n_row, int n_col)
{
	histogram_cal(length, img, n_row, n_col);
	his = histogram_draw(length, n_row, n_col);
	bmpsave(imname, his, 256, 256);
}

void gamma(unsigned char *ima, int n_row, int n_col, double ga)
{
	for (int i = 0; i < n_row; i++)
	{
		for (int j = 0; j < n_col; j++)
		{
			double intensity = (double) ima[i * n_col + j];
			intensity = (intensity + 0.5) / 256;
			intensity = pow(intensity, ga);
			intensity = intensity * 256 - 0.5;
			ima[i * n_col + j] = (int) intensity;
		}
	}
}

void *histogram_cal(int *len, unsigned char *img, int n_row, int n_col)
{
	int pix;
	for (int i = 0; i < n_row; i++)
	{
		for (int j = 0; j < n_col; j++)
		{
			pix = (int) img[i * n_col + j];
			len[pix]++;
		}
	}

	return len;
}

int arrayMax(int *array, int size)
{
	int temp = 0;
	for (int i = 0; i < size; i++)
	{
		if (temp > array[i])
			continue;
		else
			temp = array[i];
	}
	return temp;
}

unsigned char *histogram_draw(int *length, int n_row, int n_col)
{
	unsigned char *his;
	double temp;
	double lmax = arrayMax(length, 256)

	his = (unsigned char *)malloc(256 * 256 * sizeof(unsigned char));

	if (his == NULL) {
		printf("Not enough memory\n");  exit(1);
	}

	for (int c = 0; c < 256; c++)
	{
		temp = (double)length[c] / lmax;
		length[c] = temp * 256;
	}

	for (int l = 0; l < 256; l++)
	{
		for (int i = 0; i < length[l]; i++)
		{
			his[l * 256 + i] = 255 + '0';
		}
	}
	return his;
}

unsigned char *eq(int *eq_stat, unsigned char *ima, int n_row, int n_col) 
{
	unsigned char	*eq;
	int		proj[256] = {0};
	int		pix;
	int		t = 0;
	double	p;
	double	size = n_row * n_col;

	eq = (unsigned char *)malloc(n_row * n_col * sizeof(unsigned char));

	for (int i = 0; i < 256; i++)
	{
		t += eq_stat[i];
		p = t / size * 256;
		proj[i] = (int) (p + 0.5);
		if (proj[i] > 255) { proj[i]--; }
	}

	for (int i = 0; i < n_row; i++)
	{
		for (int j = 0; j < n_col; j++)
		{
			pix = (int) i * n_col + j;
			eq[pix] = proj[ima[pix]];
		}
	}
	return eq;
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


