//========================================================== file = cmm1.c =====
//=  Problema 1 Simulacion M/M/1                                               =
//==============================================================================
//=----------------------------------------------------------------------------=
//=  Build: gcc -o cmm1 cmm1.c -lm                                             =
//=----------------------------------------------------------------------------=
//=  Execute: ./cmm1                                                           =
//=----------------------------------------------------------------------------=
//=  History: ITESM (14/03/2010) - cbarbaji                                    =
//==============================================================================
//----- Includes ---------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cstring>
// #include <string>
#include <cctype>
#include <stack>
#include <queue>
#include <list>
#include <vector>
#include <map>
#include <sstream>
#include <cmath>
#include <bitset>
#include <utility>
#include <set>
#include <numeric>
#include <ctime>
#include <fstream>


using namespace std;

//----- Constantes --------------------------------------------------------------
#define SIM_TIME   1          // Tiempo de simulacion
#define LAMBDA     2              // Razon de llegada de mensajes
#define MIU        0.000222       // Razon de servicio de mensajes
#define K          5              // Cantidad de usuarios maximos
#define N          15             // Cantidad de usuarios maximos
#define MTU        1500           // Tamaño máximo de paquete
#define R          0.04166666667  // Tiempo por cliente por marco


struct client {
  unsigned int id;
  double arrival;
  double departure;
  int current_frame;
  client(unsigned int i, double a) : id(i), arrival(a), departure(-1.0), current_frame(0) {}
};

//----- Prototipos de Funciones --------------------------------------------------
double expVA(double x);        // Generar variable aleatoria exponencial con media x

//===== Main program ===========================================================
int main()
{
  double   end_time = SIM_TIME; // Tiempo total a simular segundos

  double   Ta = 1.0/LAMBDA;     // Tiempo promedio de llegada clientes
  double   Ts = 1.0/MIU;        // Tiempo promedio de servicio por mensaje

  double   time = 0.0;          // Tiempo actual de simulacion
  double   tarr = 0.0;          // Tiempo para siguiente evento (llegada)
  double   tdep = SIM_TIME;     // tiempo para siguiente evento (salida)
  unsigned int n = 0;           // Numero de paquetes en sistema
  unsigned int id = 0;          // Id de paquetes en sistema llegada
  unsigned int id2 = 0;         // Id de paquetes en sistema salida

  unsigned int c = 0;           // Numero de servicios completos
  double   b = 0.0;             // Tiempo total de ocupacion
  double   s = 0.0;             // Integral del numero de paquetes en sistema
  double   tn = time;           // Tiempo de ultimo evento
  double   tb;                  // Tiempo de ultimo inicio de servicio
  double   x;                   // Throughput
  double   u;                   // Utilizacion
  double   l;                   // Media de paquetes en sistema
  double   w;                   // Media de tiempo de residencia  

  unsigned int k = 0;

  
  vector<int> frames;
  queue<client> clients;
  
  // Abrir archivos
  // Extrae los frame
  ifstream framesFile;
  framesFile.open ("jurassic.txt");
  while(!framesFile.eof()){
    int frame;
    framesFile >> frame;
    frames.push_back(frame);
  }
  framesFile.close();

  // Extraer delays
  ifstream delaysFile;
  delaysFile.open ("amazon_delays.txt");
  
  delaysFile.close();

  // for(int i = 0; i < frames.size(); i++){
  //   printf("%d\n", frames[i]);
  // }


  // Ciclo principal
  while (time < end_time)
  {
    // cout << time << " " << tarr << endl;

    if(time <= 0.49 && time >= 0.5) cout << time << endl;
    if (time == tarr)                // *** Evento (llegada) de cliente ***
    {

      clients.push(client(id, tarr));

      tarr += Ta;                 // se aumenta por el promedio de llegadas 
      printf("%.3f\n", tarr);

      //s = s + n * (time - tn);  // actualiza area bajo curva de s
      k++;
      id++;   
      tn = time;                // tn = tiepo de ultimo evento para el siguiente evento
      printf("Llegada de un cliente %u en T= %.3f sec --- Paquetes en Concentrador: %u\n",id,time,n);
      // tarr = time + expVA(Ta);

    }
    // else                        // *** Evento (salida) ***
    // {
    //   time = tdep;
    //   //s = s + n * (time - tn);  // actualiza area bajo curva de s
    //   n--;
    //   tn = time;                // tn = tiempo de ultimo evento para el siguiente evento
    //   id2++;
    //   printf("Termina mensaje %u en T= %.3f sec --- Paquetes en Concentrador: %u\n",id2,time,n);
    //   c++;                      // Incrementa numero de servicios completos
    //   if (n > 0)
    //     tdep = time + expVA(Ts);
    //   else
    //   {
    //     tdep = SIM_TIME;
    //     b = b + time - tb;      // Actualizar tiempo de servicio
    //   }
    // }

    //printf("%.3f\n", time);
    time += 0.001;
  }

  // Procesa Resultados
  // x = c / time;   // Procesa throughput
  // u = b / time;   // Procesa utilizacion del concentrador
  // l = s / time;   // Computa numero promedio de paquetes en sistema
  // w = l / x;      // Procesa tiempo promedio de estancia en sistema

  // Imprime resultados
  printf("=============================================================== \n");
  printf("=            *** Resultadso de Simulacion MM1 ***             = \n");
  printf("=============================================================== \n");
  printf("=  Tiempo total simulado                    = %.2f s  \n", end_time);
  printf("=============================================================== \n");
  printf("=  Parametros de sistema:                                       \n");
  printf("=    Razon de llegada de paquetes           = %.6f paq/s \n",1.0/Ta);
  printf("=    Razon de servicio de los paquetes      = %.6f paq/s \n",1.0/Ts);
  printf("=============================================================== \n");
  printf("=  Resultados:                                                  \n");
  printf("=    Numero de paquetes atendidos           = %d paq         \n", c);
  printf("=    Throughput                             = %.2f paq/s     \n", x);
  printf("=    Utilizacion                            = %.2f %% \n",100.0 * u);
  printf("=    Numero promedio de paquetes en sistema = %.2f paq       \n", l);
  printf("=    Tiempo promedio de estancia en sistema = %.2f s         \n", w);
  printf("=============================================================== \n");

  delaysFile.close();

  return 0;
}

//==============================================================================
//=  Funcion que genera VA exponenciales usando el metodo inverso              =
//=    - Input:  x (media del valor de la distribucion)                        =
//=    - Output: valor de VA exponencial                                       =
//==============================================================================
double expVA(double x)
{
  double z;  // numero aleatorio z uniforme entre 0 y 1   
  do
  {
    z = ((double) rand() / RAND_MAX);
  }
  while ((z == 0) || (z == 1));

  return(-x * log(z));
}
