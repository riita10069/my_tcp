#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#define uchar unsigned char

main(argc, argv)
	int             argc;
	char           *argv[];
{
	char            datafile1[30], datafile2[30], **av;
	FILE           *fp1, *fp2;
	unsigned       *buff1, *buff2;
	long            filesize(), countbit(), compare(), diff = 0;
	unsigned long   lgth1, lgth2, wordsize, len;

	av = argv;
	if (argc > 2) {
		--argc;
		while ((*++av)[0] == '-') {
			switch ((*av)[1]) {
			default:
				printf("Illegal option: %s... usage: difftest filename1 filename2\n", *(av));
				exit(1);
				break;
			}
			argc--;
		}
		if (argc < 2)
			printf("required two filenames\n");
		else {
			strcpy(datafile1, (*av));
			strcpy(datafile2, (*++av));
		}
		if ((fp1 = fopen(datafile1, "rb")) == NULL) {
			printf("%s not exist\n");
			exit(1);
		}
		if ((fp2 = fopen(datafile2, "rb")) == NULL) {
			printf("%s not exist\n");
			exit(1);
		}
		lgth1 = filesize(fp1);
		lgth2 = filesize(fp2);
		printf("%s = %lubytes, %s = %lubytes\n", datafile1, lgth1, datafile2, lgth2);
		wordsize = 32000;
		buff1 = (unsigned *) malloc(wordsize * sizeof(unsigned));
		buff2 = (unsigned *) malloc(wordsize * sizeof(unsigned));
		if (buff1 == NULL)
			exit(1);
		if (buff2 == NULL)
			exit(1);
		if (lgth1 >= lgth2) {
			len = lgth2;
			while (len > wordsize * 2) {
				ffread(buff1, 2, wordsize, fp1);
				ffread(buff2, 2, wordsize, fp2);
				diff = diff + compare(buff1, buff2, wordsize);
				len = len - wordsize * 2;
			}
			ffread(buff1, 2, len / 2, fp1);
			ffread(buff2, 2, len / 2, fp2);
			diff = diff + compare(buff1, buff2, len / 2);
			if (len % 2)
				diff = diff + countbit((unsigned) (fgetc(fp1) ^ fgetc(fp2)));
			diff = diff + (long) ((lgth1 - lgth2) * 8);
		} else {
			len = lgth1;
			while (len > wordsize * 2) {
				ffread(buff1, 2, wordsize, fp1);
				ffread(buff2, 2, wordsize, fp2);
				diff = diff + compare(buff1, buff2, wordsize);
				len = len - wordsize * 2;
			}
			ffread(buff1, 2, len / 2, fp1);
			ffread(buff2, 2, len / 2, fp2);
			diff = diff + compare(buff1, buff2, len / 2);
			if (len % 2)
				diff = diff + countbit((unsigned) (fgetc(fp1) ^ fgetc(fp2)));
			diff = diff + (long) ((lgth2 - lgth1) * 8);
		}
		printf("difference is %ld bits\n", diff);
		/* fcloseall(); */
		fclose(fp1);
		fclose(fp2);
	} else
		printf("wrong argument\n");
}


long 
filesize(fpnt)
	FILE           *fpnt;
{
	long            lgth;

	fseek(fpnt, 0L, 2);
	lgth = ftell(fpnt);
	fseek(fpnt, 0L, 0);
	return lgth;
}


long 
countbit(word)
	unsigned        word;
{
	unsigned        pat = 0x01;
	long            ret = 0;
	int             i;

	for (i = 0; i < sizeof(int)*8; i++) {
		if (word & pat)
			ret++;
		pat = pat << 1;
	}
	return ret;
}


long 
compare(buf1, buf2, size)
	unsigned       *buf1, *buf2;
	unsigned long   size;
{
	long            ren = 0;
	unsigned long   i;

	for (i = 0; i < size; i++) {
		if ((buf1[i] - buf2[i]) == 0)
			continue;
		ren = ren + countbit(buf1[i] ^ buf2[i]);
	}
	return ren;
}


ffread(buf, size, item, fp)
	unsigned       *buf;
	int             size;
	unsigned long   item;
	FILE           *fp;
{
	unsigned long   i;

	for (i = 0; i < item; i++) {
		buf[i] = getw(fp);
	}
}
