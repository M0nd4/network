# -*- coding: utf-8 -*-
"""
Created on Sun May 17 23:10:50 2015
Pasqualetti Guillermo D.
Este algoritmo implementa el modelo basado en agentes de Balenzuela et al 2014
ACLARACIONES
"""


# 0) PRELIMINARES
import ctypes as ct
import numpy as np
import multiprocessing
import time
import os
import pickle
import gzip

tic = time.time()
print time.strftime("%d/%b - %H:%M:%S")


#LibDinamicaC = ct.WinDLL("\DinamicaaaCDLL.dll")
#LibDinamicaC = ct.cdll.LoadLibrary('./Funciones/DinamicaaaCDLL/bin/Release/DinamicaaaCDLL.dll')
LibDinamica = ct.CDLL('./libnetwork.so')
Dinamica = LibDinamica.Dinamica
Dinamica.argtypes = [ct.c_float, ct.c_float, ct.c_float, ct.c_float, ct.c_float, ct.c_int, ct.c_int, ct.c_float, ct.c_float]
Dinamica.restype = ct.POINTER(ct.c_float)
def DinamicaPy(Po, d, SAT, ancho, DR, THR0, THR1, THR2, THR3):            # Necesaria ya que processing usa pickle y no se puede picklear un puntero
    pS = Dinamica(Po, d, SAT, ancho, DR, THR0, THR1, THR2, THR3)
    S[0] = pS[0]; S[1] = pS[1]; S[2] = pS[2]; S[3] = pS[3]
    return S
    
    
# 1) PARAMETROS
SAT = 3                                                                # Maxima conviccion posible (saturacion)
DR = 2                                                                 # D relativo de los indecisos
N = 1000                                                               # Numero de agentes

N_ENS = 200
THR = (1,1,-1,1)                                   # Umbral: [fija/plana/gaussiana, corr/anticorr/indep, media inferior, media superior]; a centro inferior ponerle el signo explicitamente; la gaussiana esta truncada en dos desviaciones estandar y los agentes son reubicados con distribuciones uniformes alrededor de ambas medias
ANCHO = np.array([0]); #np.arange(0,0.50001,0.05)  #np.arange(0,0.950001,0.05)                             #0.95:0.02:0.99              # semiancho o 2*desviacion estandar inferior, semiancho o 2*desviacion estandar superior
D = np.arange(0.01,1.0001,0.01)                                                       # Influenciabilidad. Valores originales: [0.001 0.01 0.1 0.3 0.4] Pero 0.001 parece ser igual a 0.01
PO = np.arange(0,1.0001,0.01)   #np.array([0.2, 0.5])                                                           # Fraccion inicial de indecisos
n_proc = 5
nom_arch = "ResultadosTestPAR{}{}Nens{}.pklz".format(THR[0], THR[1], N_ENS)
nom_var = ("Sd", "So", "Su", "t", "PO", "D", "ANCHO", "THR", "N_ENS", "N", "SAT", "DR", "t_ejecucion")


# 2) INICIALIZACION
S = np.zeros([4])
So = np.zeros((PO.size,D.size,ANCHO.size,N_ENS),dtype=np.float)
Su = np.zeros_like(So)
Sd = np.zeros_like(So)
t  = np.zeros_like(So)


# 3) EJECUCION
pool = multiprocessing.Pool(processes = n_proc)

print "N = " + str(N),
print "; THR = " + str(THR); print "---------------------------------"; print " ";
i_ancho = -1
for ancho in ANCHO:
    tic_ancho = time.time()
    i_ancho = i_ancho + 1
    print "Ancho = " + "%.2f"%(ancho)
    
    i_D = -1
    for d in D:
        i_D = i_D + 1
    
        i_Po = -1
        for Po in PO:
            i_Po = i_Po + 1
            msg = "D = " + "%.2f"%(d) + "; Po = " + "%.2f"%(Po); print "\r" + msg,
                                                         
            #pS = Dinamica(Po, d, SAT, ancho, DR, THR[0], THR[1], THR[2], THR[3])            
            if __name__=="__main__":                                # Necesario para que ande en Windows
                resultados = [pool.apply_async(DinamicaPy, args=(Po,d,SAT,ancho,DR,THR[0],THR[1],THR[2],THR[3],)) for n_ens in range(N_ENS)]
                n_ens = -1            
                for p in resultados:
                    n_ens = n_ens + 1
                    salida = p.get()
                    Sd[i_Po,i_D,i_ancho,n_ens] = salida[0]
                    So[i_Po,i_D,i_ancho,n_ens] = salida[1]
                    Su[i_Po,i_D,i_ancho,n_ens] = salida[2]
                    t[i_Po,i_D,i_ancho,n_ens] = salida[3]
                                                        
                
    toc = time.time() - tic; toc_ancho = time.time() - tic_ancho
    msg = "Tiempo: " + "%.2f"%(toc_ancho/60) + " min." if toc_ancho/60 > 1 else "Tiempo: " + "%.2f"%(toc_ancho) + " seg."; print "\r" + msg + "    "
    msg = "Transcurrido: " + "%.2f"%(toc/3600) + " hs." if toc/3600 > 1 else "Transcurrido: " + "%.2f"%(toc/60) + " min."; print msg
    msg = "Restante: " + "%.2f"%(toc_ancho/3600*(ANCHO.size-i_ancho-1)) + " hs." if toc_ancho/3600*(ANCHO.size-i_ancho-1) > 1 else "Restante: " + "%.2f"%(toc_ancho/60*(ANCHO.size-i_ancho-1)) + " min."; print msg; print " "

pool.close()
pool.join()


# 4) ALMACENAMIENTO
print "---------------------------------";
t_ejecucion = time.time() - tic
os.chdir('Resultados')
datos = (Sd, So, Su, t, PO, D, ANCHO, THR, N_ENS, N, SAT, DR, t_ejecucion, nom_var)
file = gzip.open(nom_arch, 'wb')
pickle.dump(datos, file)
file.close()
os.chdir(os.pardir)
print " "; print "DATOS GUARDADOS CON EXITO!"
print nom_arch; print " "
print time.strftime("%d/%b - %H:%M:%S")
toc = time.time() - tic; msg = "Tiempo total: " + "%.2f"%(toc/3600) + " hs." if toc/3600 > 1 else "Tiempo total: " + "%.2f"%(toc/60) + " min."; print msg; print " "
