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
unsigned char *filter_low(unsigned char *img, double *matrix, int size, int n_row, int n_col);
unsigned char *filter_lap(unsigned char *img, int *matrix, int size, int c, int n_row, int n_col);
int arrayMax(int *array, int size);
int arrayMin(int *array, int size);

int main(int argc, char *argv[])
{
  int   i, j, n_row, n_col;
  unsigned char  *ima, *blur;

  /* Read image */

  ima = bmpread("picture_3.bmp", &n_row, &n_col);
 
  /* Show the image size */

  printf("Number of rows = %d, Number of columns = %d\n", n_row, n_col);

  /* Blur with 3*3 low pass filter. */

  double filter_low3[9];
  for (int i = 0; i < 9; i++) { filter_low3[i] = 1.0 / 9.0; }
  ima = filter_low(ima, filter_low3, 3, n_row, n_col);
  bmpsave("picture03_fLow3.bmp", ima, n_row - 2, n_col - 2);

  /* Laplacian filter operation */

  n_row -= 2;
  n_col -= 2;

  int filter_lap1[9] = { 0, 1, 0, 1, -4, 1, 0, 1, 0 };
  blur = filter_lap(ima, filter_lap1, 3, -10, n_row, n_col);
  bmpsave("picture03_fLa1.bmp", blur, n_row - 2, n_col - 2);

  int filter_lap1diag[9] = { 1, 1, 1, 1, -8, 1, 1, 1, 1 };
  blur = filter_lap(ima, filter_lap1diag, 3, -10, n_row, n_col);
  bmpsave("picture03_fLa1diag.bmp", blur, n_row - 2, n_col - 2);

  int filter_lap2[9] = { 0, -1, 0, -1, 4, -1, 0, -1, 0 };
  blur = filter_lap(ima, filter_lap2, 3, 10, n_row, n_col);
  bmpsave("picture03_fLa2.bmp", blur, n_row - 2, n_col - 2);

  int filter_lap2diag[9] = { -1, -1, -1, -1, 8, -1, -1, -1, -1 };
  blur = filter_lap(ima, filter_lap2diag, 3, 10, n_row, n_col);
  bmpsave("picture03_fLa2diag.bmp", blur, n_row - 2, n_col - 2);

  return 0;
}

unsigned char *filter_low(unsigned char *img, double *matrix, int size, int n_row, int n_col)
{
	unsigned char *fil;
	int sizeto = size * size;
	int n_rowf = n_row - (size - 1), n_colf = n_col - (size - 1);
	fil = (unsigned char *)malloc(n_rowf * n_colf * sizeof(unsigned char));

	/* Filter operation*/
	int pos = 0, mid = size / 2 + 1;
	double temp = 0.0;
	for (int i = 0; i < n_rowf; i++)
	{
		for (int j = 0; j < n_colf; j++)
		{
			for (int p = 0; p < sizeto; p++)
			{
				pos = ((p / size + 1) - mid) * n_col + ((p % size) - mid) + 1;			//Calculate relative position of the center pixel.
				temp += matrix[p] * img[(i + mid - 1) * n_col + (j + mid - 1) + pos];		//Note that the center pixel should be in the original picture, 
																							//so we need a translation.
			}
			//The filter has to be divided by the total first, e.g. the value are decimal fractions for avarage low pass filter.
			fil[i * n_colf + j] = (int) temp;
			temp = 0;
		}
	}
	return fil;
}

unsigned char *filter_lap(unsigned char *img, int *matrix, int size, int c, int n_row, int n_col)
{
	unsigned char *fil;
	int *buff;
	int sizeto = size * size;
	int n_rowf = n_row - (size - 1), n_colf = n_col - (size - 1);
	fil = (unsigned char *)malloc(n_rowf * n_colf * sizeof(unsigned char));
	buff = (int *)malloc(n_rowf * n_colf * sizeof(int));

	/* Filter operation*/
	int pos = 0, mid = size / 2 + 1;
	int temp = 0;
	for (int i = 0; i < n_rowf; i++)
	{
		for (int j = 0; j < n_colf; j++)
		{
			for (int p = 0; p < sizeto; p++)
			{
				pos = ((p / size + 1) - mid) * n_col + ((p % size) - mid) + 1;			//Calculate relative position of the center pixel.
				temp += matrix[p] * img[(i + mid - 1) * n_col + (j + mid - 1) + pos];		//Note that the center pixel should be in the original picture, 
																							//so we need a translation.
				//if (temp < 0) { printf("OK"); }
			}
			//The filter has to be divided by the total first, e.g. the value are decimal fractions for avarage low pass filter.
			buff[i * n_colf + j] = temp;
			temp = 0;
		}
	}

	int sizef = n_rowf * n_colf;
	int fmax = arrayMax(buff, sizef);
	int fmin = arrayMin(buff, sizef);
	for (int i = 0; i < n_rowf; i++)
	{
		for (int j = 0; j < n_colf; j++)
		{
			int pix = i * n_colf + j;
			buff[pix] -= fmin;
			buff[pix] = (255.0 / fmax);
			buff[pix] = (int) buff[pix];
			buff[pix] = img[(i + mid - 1) * n_col + (j + mid - 1)] + c * buff[pix];
		}
	}
	int smax = arrayMax(buff, sizef);
	int smin = arrayMin(buff, sizef);
	for (int i = 0; i < n_rowf; i++)
	{
		for (int j = 0; j < n_colf; j++)
		{
			int pix = i * n_colf + j;
			buff[pix] -= smin;
			buff[pix] *= (255.0 / smax);
			fil[pix] = (unsigned char)buff[pix];
		}
	}

	return fil;
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

int arrayMin(int *array, int size)
{
	int temp = 0;
	for (int i = 0; i < size; i++)
	{
		if (temp < array[i])
			continue;
		else
			temp = array[i];
	}
	return temp;
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


