// Esta funcion implementa los eventos de interaccion entre agentes

# include <stdlib.h>
# include <time.h>
# include <math.h>

# define N 1000

float* Dinamica(float Po, float d, float SAT, float ancho, float DR, int distr_tipo, int distr_corr, float media_inf, float media_sup){


float thrD[N], thrU[N];
float C[N]; int O[N];
float S[4];
float *pS = &S[0];

//float Sd, So, Su;
int Nd, No, Nu;
int t = 0;
int i;
int j;

//for (i = 0; i < 100000*N; i++) {}
srand(time(NULL) + clock());                                 //Solo inicializo el generador una vez para evitar que ante repeticiones muy frecuentes inicialice con la misma hora


// i) INICIALIZACION DE UMBRALES
if (distr_tipo == 1) {                                                                                          //FIJA
    for (i = 0; i < N; i++) {thrD[i] = media_inf; thrU[i] = media_sup;}
}
else if (distr_tipo == 2) {                                                                                     //PLANA
    for (i = 0; i < N; i++) {
        thrU[i] = media_sup + ancho*(2*((float)rand()/RAND_MAX)-1);
        if (distr_corr == 1) {thrD[i] = thrU[i] - media_sup + media_inf;}            //correlacionada
        else if (distr_corr == 2) {thrD[i] = -thrU[i] + media_sup + media_inf;}                                       //anticorrelacionada
        else {thrD[i] = media_inf + ancho*(2*((float)rand()/RAND_MAX)-1);};
    }
}
/*else {                                                                                                          //GAUSSIANA
    for (i = 0; i < N; i++) {
    // randn!!!!!! thrU[i] = media_sup + (1/2)*ancho_sup*randn(N,1);
    N_extremos = sum(thrU > media_sup + ancho_sup);
    while N_extremos ~= 0                                      //reubico umbrales demasiado extremos
        thrU(thrU > media_sup + ancho_sup) = media_sup + (1/2)*ancho_sup*randn(N_extremos,1);
        N_extremos = sum(thrU > media_sup + ancho_sup);
    end
    if distr_corr == 1                                             //correlacionada
        thrD = thrU - media_sup + media_inf;
    elseif distr_corr == 2                                         //anticorrelacionada
        thrD = -thrU + media_sup + media_inf;
    else                                                           //independiente
        thrD = media_inf + (1/2)*ancho_inf*randn(N,1);
        N_extremos = sum(thrD < media_inf - ancho_inf);
        while N_extremos ~= 0
            thrD(thrD < media_inf - ancho_inf) = media_inf + (1/2)*ancho_inf*randn(N_extremos,1);
            N_extremos = sum(thrD < media_inf - ancho_inf);
        end
    end*/


// ii) INICIALIZACION DE CONVICCIONES Y OPINIONES
Nd = 0; No = 0; Nu = 0;
for (i = 0; i < N; i++) {
    if (((float)rand()/RAND_MAX) <= Po) {
        C[i] = thrD[i] + (thrU[i] - thrD[i])*((float)rand()/RAND_MAX);
        O[i] = 0;
        No++;
    }
    else if (((float)rand()/RAND_MAX) <= 0.5) {
        C[i] = -SAT + (SAT + thrD[i])*((float)rand()/RAND_MAX);
        O[i] = -1;
        Nd++;
    }
    else {
        C[i] = thrU[i] + (SAT - thrU[i])*((float)rand()/RAND_MAX);
        O[i] = 1;
        Nu++;
    }
}


// iii) DINAMICA
if (No == 0 || No == N) {                                                       // Chequeo que el estado inicial no sea estacionario
    S[0] = (float)Nd/N; S[1] = (float)No/N; S[2] = (float)Nu/N; S[3] = (float)t;
    return pS;
}

while (1) {
    t = t + 1;
    i = rand() % N;
    j = i; while (j==i) {j = rand() % N;}

    if (O[i] == O[j]) {  // Opiniones iguales
        if (fabs(C[i]-C[j]) <= 2*d){                                           // CRUCES: Si estan muy cerca convergen al promedio
            C[i] = (C[i] + C[j])/2;
            C[j] = C[i];
        }
        else if (C[i] > C[j]) {
            C[i] = C[i] - d;
            C[j] = C[j] + d;
        }
        else {
            C[i] = C[i] + d;
            C[j] = C[j] - d;
        }
        switch (O[i]) {                                                         // THR: Actualizo opiniones (en este caso no es necesario chequear fin while)
        case 0:
            if (C[i] < thrD[i]) {
                O[i] = -1; Nd++; No--;
                break;
            }
            if (C[i] > thrU[i]) {
                O[i] = 1; Nu++; No--;
                break;
            }
            if (C[j] < thrD[i]) {
                O[j] = -1; Nd++; No--;
                break;
            }
            if (C[j] > thrU[j]) {
                O[j] = 1; Nu++; No--;
                break;
            }
            break;
        case -1:
            if (C[i] > thrD[i]) {
                O[i] = 0; No++; Nd--;
            }
            else if (C[j] > thrD[j]) {
                O[j] = 0; No++; Nd--;
            }
            break;
        case 1:
            if (C[i] < thrU[i]) {
                O[i] = 0; No++; Nu--;
            }
            else if (C[j] < thrU[j]) {
                O[j] = 0; No++; Nu--;
            }
            break;
        }
    }

    else if (O[i] != 0 && O[j] != 0) {   // No hay indecisos (opiniones diferentes)
        if (O[i] == 1) {
            C[i] = C[i] + d; if (C[i] > SAT) {C[i] = SAT;}                         // SAT: Saturo la conviccion
            C[j] = C[j] - d; if (C[j] < -SAT) {C[j] = -SAT;}
        }
        else {
            C[i] = C[i] - d; if (C[i] < -SAT) {C[i] = -SAT;}
            C[j] = C[j] + d; if (C[j] > SAT) {C[j] = SAT;}
        }
    }

    else {   // Hay indecisos (opiniones diferentes)
        if (O[j] == 0) {
            if (O[i] == 1) {
                C[i] = C[i] - d;
                C[j] = C[j] + DR*d;
                if (C[i] < thrU[i]) {O[i] = 0; No++; Nu--;}                          // THR
                if (C[j] > thrU[j]) {O[j] = 1; Nu++; No--; if(C[j] > SAT) {C[j] = SAT;}}    // SAT
            }
            else {
                C[i] = C[i] + d;
                C[j] = C[j] - DR*d;
                if (C[i] > thrD[i]) {O[i] = 0; No++; Nd--;}
                if (C[j] < thrD[j]) {O[j] = -1; Nd++; No--; if(C[j] < -SAT) {C[j] = -SAT;}}
            }
        }
        else if (O[j] == 1) {
            C[i] = C[i] + DR*d;
            C[j] = C[j] - d;
            if (C[i] > thrU[i]) {O[i] = 1; Nu++; No--; if(C[i] > SAT) {C[i] = SAT;}}
            if (C[j] < thrU[j]) {O[j] = 0; No++; Nu--;}
        }
        else {
            C[i] = C[i] - DR*d;
            C[j] = C[j] + d;
            if (C[i] < thrD[i]) {O[i] = -1; Nd++; No--; if(C[i] < -SAT) {C[i] = -SAT;}};
            if (C[j] > thrD[j]) {O[j] = 0; No++; Nd--;}
        }
        if (No == 0 || No == N) {
            S[0] = (float)Nd/N; S[1] = (float)No/N; S[2] = (float)Nu/N; S[3] = (float)t;
            return pS; break;
        }
    }
}
}
