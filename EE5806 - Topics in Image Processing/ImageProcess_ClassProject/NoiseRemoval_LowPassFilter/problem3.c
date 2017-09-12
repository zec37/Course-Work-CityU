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
unsigned char *filter_mid(unsigned char *img, int size, int n_row, int n_col);
int median(int *array, int size);
void swap(int *array, int a, int b);

int main(int argc, char *argv[])
{
  int   i, j, n_row, n_col;
  unsigned char  *ima, *blur;

  /* Median function test*/
  int a[] = { 1,8,6,4,5,7,2,3,9 };
  int testvar = median(a, 9);

  /* Read image */

  ima = bmpread("picture_2.bmp", &n_row, &n_col);
 
  /* Show the image size */

  printf("Number of rows = %d, Number of columns = %d\n", n_row, n_col);

  /* Add noise */

  int size = n_row * n_col;
  int noise = size / 2;
  int pix, sORp;
  srand(time(0));
  for (int i = 0; i < noise; i++)
  {
	  pix = (int)((double)rand() / ((double)RAND_MAX + 1) * size);
	  sORp = (int)((double)rand() / ((double)RAND_MAX + 1) * 2);
	  ima[pix] = sORp * 255;
  }

  /* Save output */

  bmpsave("picture02_tmp.bmp", ima, n_row, n_col);

  /* Low pass filter */

  double filter_low3[9];
  for (int i = 0; i < 9; i++) { filter_low3[i] = 1.0 / 9.0; }
  blur = filter_low(ima, filter_low3, 3, n_row, n_col);
  bmpsave("picture02_fLow3.bmp", blur, n_row - 2, n_col - 2);

  double filter_low7[49];
  for (int i = 0; i < 49; i++) { filter_low7[i] = 1.0 / 49.0; }
  blur = filter_low(ima, filter_low7, 7, n_row, n_col);
  bmpsave("picture02_fLow7.bmp", blur, n_row - 6, n_col - 6);

  /* Median filter */

  blur = filter_mid(ima, 3, n_row, n_col);
  bmpsave("picture02_fMid3.bmp", blur, n_row - 2, n_col - 2);
  blur = filter_mid(ima, 7, n_row, n_col);
  bmpsave("picture02_fMid7.bmp", blur, n_row - 6, n_col - 6);

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

unsigned char *filter_mid(unsigned char *img, int size, int n_row, int n_col)
{
	unsigned char *fil;
	int sizeto = size * size;
	int n_rowf = n_row - (size - 1), n_colf = n_col - (size - 1);
	fil = (unsigned char *)malloc(n_rowf * n_colf * sizeof(unsigned char));

	/* Filter operation*/
	int pos = 0, mid = size / 2 + 1;
	int *sort;
	sort = (int *)malloc(sizeto * sizeof(int));
	int count = 0;
	for (int i = 0; i < n_rowf; i++)
	{
		for (int j = 0; j < n_colf; j++)
		{
			for (int p = 0; p < sizeto; p++)
			{
				pos = ((p / size + 1) - mid) * n_col + ((p % size) - mid) + 1;			//Calculate relative position of the center pixel.
				sort[p] = img[(i + mid - 1) * n_col + (j + mid - 1) + pos];
			}
			fil[i * n_colf + j] = median(sort, sizeto);
			count++;
		}
	}
	return fil;
}

int median(int *array, int size)	//size must be odd in this function.
{
	int mid = size / 2;
	int Rcount = 0;
	while(1)
	{
		for (int i = 0; i < mid; i++)
		{
			if (array[i] > array[i + 1]) { swap(array, i, i + 1); }
		}
		int temp = array[mid];
		int test = 1;
		while(temp <= array[mid + test])
		{
			if (test == (mid - Rcount + 1)) { return temp; }
			test++;
		}
		swap(array, mid, mid + test);
		swap(array, mid + test, size - 1 - Rcount);
		Rcount++;
	}
}

void swap(int *array, int a, int b)
{
	int temp = array[b];
	array[b] = array[a];
	array[a] = temp;
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


