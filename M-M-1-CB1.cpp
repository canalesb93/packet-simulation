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

//----- Constantes --------------------------------------------------------------
#define SIM_TIME   1.0e3        // Tiempo de simulacion
#define LAMBDA     7            // Razon de llegada de mensajes
#define MIU        8            // Razon de servicio de mensajes

//----- Prototipos de Funciones --------------------------------------------------
double expVA(double x);        // Generar variable aleatoria exponencial con media x

//===== Main program ===========================================================
int main(void)
{
  double   end_time = SIM_TIME; // Tiempo total a simular
  double   Ta = 1.0/LAMBDA;     // Tiempo promedio de llegada entre mensajes
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
   
  // Ciclo principal
  while (time < end_time)
  {
    if (tarr < tdep)                // *** Evento (llegada) ***
    {
      time = tarr;
      s = s + n * (time - tn);  // actualiza area bajo curva de s
      n++;
      id++;   
      tn = time;                // tn = tiepo de ultimo evento para el siguiente evento
      printf("Llegada de mensaje %u en T= %.3f sec --- Paquetes en Concentrador: %u\n",id,time,n);
      tarr = time + expVA(Ta);
      if (n == 1)
      {
        tb = time;              // tb= incio de tiempo de servicio de ultimo paquete en fila
        tdep = time + expVA(Ts);
      }
    }
    else                        // *** Evento (salida) ***
    {
      time = tdep;
      s = s + n * (time - tn);  // actualiza area bajo curva de s
      n--;
      tn = time;                // tn = tiempo de ultimo evento para el siguiente evento
      id2++;
      printf("Termina mensaje %u en T= %.3f sec --- Paquetes en Concentrador: %u\n",id2,time,n);
      c++;                      // Incrementa numero de servicios completos
      if (n > 0)
        tdep = time + expVA(Ts);
      else
      {
        tdep = SIM_TIME;
        b = b + time - tb;      // Actualizar tiempo de servicio
      }
    }
  }

  // Procesa Resultados
  x = c / time;   // Procesa throughput
  u = b / time;   // Procesa utilizacion del concentrador
  l = s / time;   // Computa numero promedio de paquetes en sistema
  w = l / x;      // Procesa tiempo promedio de estancia en sistema

  // Imprime resultados
  printf("=============================================================== \n");
  printf("=            *** Resultadso de Simulacion MM1 ***             = \n");
  printf("=============================================================== \n");
  printf("=  Tiempo total simulado                    = %.2f s  \n", end_time);
  printf("=============================================================== \n");
  printf("=  Parametros de sistema:                                       \n");
  printf("=    Razon de llegada de paquetes           = %.2f paq/s \n",1.0/Ta);
  printf("=    Razon de servicio de los paquetes      = %.2f paq/s \n",1.0/Ts);
  printf("=============================================================== \n");
  printf("=  Resultados:                                                  \n");
  printf("=    Numero de paquetes atendidos           = %d paq         \n", c);
  printf("=    Throughput                             = %.2f paq/s     \n", x);
  printf("=    Utilizacion                            = %.2f %% \n",100.0 * u);
  printf("=    Numero promedio de paquetes en sistema = %.2f paq       \n", l);
  printf("=    Tiempo promedio de estancia en sistema = %.2f s         \n", w);
  printf("=============================================================== \n");
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
