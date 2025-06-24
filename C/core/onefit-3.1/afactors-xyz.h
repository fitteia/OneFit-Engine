// afactors-xyz.h

struct Atomo;

typedef struct Atomo {
        int     numero;
        char    nome[10];
        double  x;
        double  y;
        double  z;

} Atomo;

void r6medio( FILE *fin, FILE *fout);
void Afactors(FILE *fin, FILE *fout, int e1, int e2);
double  distancia(Atomo A1, Atomo A2);
double  angulo(Atomo A1, Atomo A2, Atomo A3, Atomo A4);
FILE *openf(char fname[], char modei[]);

