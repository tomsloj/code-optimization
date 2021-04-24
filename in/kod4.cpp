int i;
int j;
int a;
int b[20];
int c[20];
for (i=1; i<20; i++){
for (j=1; j<20; j++){a = 5; b[j] = a * i;
}
a = 5 * b[i];
c[i] = a - 10;
}