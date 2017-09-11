#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

char *Usage = "print-rand-matrix -r rows -c cols\n";
#define ARGS "r:c:"

int Rows;
int Cols;

int main(int argc, char **argv)
{
  int c;
  int i;
  int j;
  double r;
  struct timeval tm;
  unsigned long seed;

  while((c = getopt(argc,argv,ARGS)) != EOF) {
    switch(c) {
      case 'r':
        Rows = atoi(optarg);
        break;
      case 'c':
        Cols = atoi(optarg);
        break;
      default:
        fprintf(stderr,"unrecognized command %c\n",
          (char)c);
        fprintf(stderr,"usage: %s",Usage);
        exit(1);
    }
  }

  if(Rows <= 0) {
    fprintf(stderr,"must enter rows\n");
    fprintf(stderr,"usage: %s",Usage);
    exit(1);
  }

  if(Cols <= 0) {
    fprintf(stderr,"must enter columns\n");
    fprintf(stderr,"usage: %s",Usage);
    exit(1);
  }

  gettimeofday(&tm,NULL);
  seed = tm.tv_sec + tm.tv_usec;
  srand48(seed);

  printf("%d %d\n",Rows,Cols);
  for(i=0; i < Rows; i++) {
    printf("# Row %d\n",i);
    for(j=0; j < Cols; j++) {
      r = drand48();
      printf("%f\n",r);
    }
  }

  FILE *fa = fopen("a.txt", "w");
  fprintf(fa,"%d %d\n",Rows,Cols);
  for(i=0; i < Rows; i++) {
    fprintf(fa,"# Row %d\n",i);
    for(j=0; j < Cols; j++) {
      r = drand48();
      fprintf(fa,"%f\n",r);
    }
  }
  fclose(fa);

  int tmp = Rows;
  Rows = Cols;
  Cols = tmp;
  FILE *fb = fopen("b.txt", "w");
  fprintf(fb,"%d %d\n",Rows,Cols);
  for(i=0; i < Rows; i++) {
    fprintf(fb,"# Row %d\n",i);
    for(j=0; j < Cols; j++) {
      r = drand48();
      fprintf(fa,"%f\n",r);
    }
  }
  fclose(fb);

  exit(0);
}

  

