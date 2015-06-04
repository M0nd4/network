# include <stdlib.h>
# include <stdio.h>
//# include "DinamicaC.c"
# include "Dinamica.c"


//int DinamicaC(float Po, float d, float SAT, float ancho, float DR, int distr_tipo, int distr_corr, float media_inf, float media_sup);


int main() {

//srand(time(NULL) + clock());
# define N_ENS 20
int n_ens;
float Po = 0.01;
float d = 0.01;
float SAT = 3;
float ancho = 0;
float DR = 2;
int distr_tipo = 1;
int distr_corr = 1;
float media_inf = -1;
float media_sup = 1;
float* pS;
float S[4][N_ENS];

for (n_ens = 0; n_ens < N_ENS; n_ens++) {
    pS = Dinamica(Po, d, SAT, ancho, DR, distr_tipo, distr_corr, media_inf, media_sup);
    //printf("%.2f %.2f %.2f %f", *pS, *(pS+1), *(pS+2), *(pS+3));
    S[0][n_ens] = *pS; S[1][n_ens] = *(pS+1); S[2][n_ens] = *(pS+2); S[3][n_ens] = *(pS+3);
}


for(n_ens = 0; n_ens < N_ENS; n_ens++) {
    printf("%f  " "%f  " "%f  " "%f  ", S[0][n_ens], S[1][n_ens], S[2][n_ens], S[3][n_ens]);
    printf("\n");
}

return 0;
}


//usar ++
//CALCULAR EL SIGNO DE C(I)-C(J) Y USARLO PARA EVITAR VARIOS IF?
// usar const: mejor la calidad del codigo ya que el compilador te tira posibles errores. ademas permite optimizaciones: mkedio al dope pero mejora
// como trabajo con solo dos decimales?!: hacerlo para las varibles que valen la pena: las que se van a comparar mucho. Por ejemplo Opinion!
// usar semilla reproducible?!
