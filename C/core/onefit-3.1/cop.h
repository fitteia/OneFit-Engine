// cop.
struct Operacao;

typedef struct Operacao {
  int i;
  int j;
  int t;
  double c;
  double (*fptr)(Operacao *x, double y[]);
} Operacao;

void maincop(int argc, char *argv[]);
void options(char argu[]);
void operacao(Operacao *col,char *argu);
double Abs_(Operacao *x, double y[]);
double dif(Operacao *x, double y[]);
double mult( Operacao *x, double y[]);
double divi(Operacao *x, double y[]);
double soma(Operacao *x, double y[]);
double Pow_(Operacao *x, double y[]);
double sim(Operacao *x, double y[]);
double ln_(Operacao *x, double y[]);
double Log_(Operacao *x, double y[]);
double Sin_(Operacao *x, double y[]);
double id(Operacao *x, double y[]);
double Exp_(Operacao *x, double y[]);
double Cos_(Operacao *x, double y[]);

