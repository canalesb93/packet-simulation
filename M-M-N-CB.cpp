//========================================================== file = mmc.c  =====
//=  Problema 3 Simulacion M/M/C                                               =
//==============================================================================
//=----------------------------------------------------------------------------=
//=  Build: gcc -o mmc mmc.c -lm                                               =
//=----------------------------------------------------------------------------=
//=  Execute: ./mmc                                                            =
//=----------------------------------------------------------------------------=
//=  History: ITESM (20/03/2010) - cbarbaji                                    =
//==============================================================================
//----- Includes ---------------------------------------------------------------
#include <stdio.h>              
#include <stdlib.h>             
#include <math.h>               

//----- Constantes --------------------------------------------------------------
#define SIM_TIME   1.0e2          // Tiempo de simulacion
#define LAMBDA 40                  // Razon de llegada de mensajes
#define MIU  8                    // Razon de servicio de mensajes
#define MAX_SERVERS  2            // Numero de servidores

//----- Prototipos de Funciones --------------------------------------------------
double expVA(double x);          // Generar variable aleatoria exponencial con media x
unsigned int isMinTime(double x);// Nos da la posicion de valor minimo a x si existe, sino da 1024
unsigned int MinTime();          // Nos da elvalor minimo del vactor de tdep
void initialize();               // inicializa el vector tdep con 0's

//----- Variables Globales -------------------------------------------------------
double tdep[MAX_SERVERS];        // tiempo para siguiente evento (salida)

//===== Main program ===========================================================
int main(void)
{
  double   end_time = SIM_TIME;  // Tiempo total a simular
  double   Ta = 1.0/LAMBDA;      // Tiempo promedio de llegada entre mensajes
  double   Ts = 1.0/MIU;         // Tiempo promedio de servicio por mensaje

  double   time = 0.0;           // Tiempo actual de simulacion
  double   tarr = 0.0;           // Tiempo para siguiente evento (llegada)  
  int      n = 0;                // Numero de paquetes en sistema
  unsigned int ms = MAX_SERVERS; // Numero de paquetes en sistema
  unsigned int id = 0;           // Id de paquetes en sistema llegada
  unsigned int id2 = 0;          // Id de paquetes en sistema salida
  unsigned int aux1 = 0;         // variable auxiliar
  unsigned int aux2 = 0;         // variable auxiliar

  unsigned int c = 0;            // Numero de servicios completos  
  double   s = 0.0;              // Integral del numero de paquetes en sistema
  double   tn = time;            // Tiempo de ultimo evento
  double   tb;                   // Tiempo de ultimo inicio de servicio
  double   x;                    // Throughput
  double   u;                    // Utilizacion
  double   l;                    // Media de paquetes en sistema
  double   w;                    // Media de tiempo de residencia  
  double   b = 0.0;              // Tiempo total de ocupacion
   
  initialize();
  
  // Ciclo principal
  while (time < end_time)
  {
    aux1 = isMinTime(tarr);
    if (id==0)
      aux1=0;
    if (aux1 != 1024)                // *** Evento (llegada) ***
    {
      time = tarr;
      s = s + n * (time - tn);  // actualiza area bajo curva de s
      n++;
      id++;   
      tn = time;                // tn = tiepo de ultimo evento para el siguiente evento
      printf("Llegada de mensaje %u en T= %.3f sec --- Paquetes en Fila: %u\n",id,time,n);
      tarr = time + expVA(Ta);
      if (n == 1)
      {
        tb = time;              // tb= incio de tiempo de servicio de ultimo paquete en fila
        tdep[aux1] = time + expVA(Ts);
      }
    }
    else                        // *** Evento (salida) ***
    {
      aux2 = MinTime();
      time = tdep[aux2];
      s = s + n * (time - tn);  // actualiza area bajo curva de s
      n--;
      tn = time;                // tn = tiempo de ultimo evento para el siguiente evento
      id2++;
      printf("Termina mensaje %u en T= %.3f sec --- Server %d --- Paquetes en Fila: %u\n",id2,time,aux2,n);
      c++;                      // Incrementa numero de servicios completos
      if (n > 0){
        tdep[aux2] = time + expVA(Ts);           
      }
      else
      {
        tdep[aux2] = SIM_TIME;
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
  printf("=            *** Resultadso de Simulacion MMC ***             = \n");
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

//==============================================================================
//=  Nos da la posicion de valor minimo a x si existe, sino da 1024            =
//==============================================================================
unsigned int isMinTime(double x)
{
  unsigned int i = 0;
  unsigned int result = 1024;
  for (i = 0; i < MAX_SERVERS; i++){
     if (x < tdep[i]){
      result = i;
      break;
     }
  }  
  return result;
}

//==============================================================================
//=  Nos da elvalor minimo del vactor de tdep                                  =
//==============================================================================
unsigned int MinTime(double x)
{
  unsigned int i = 0;
  unsigned int result = 1;
  unsigned int min = 99999;
  for (i = 0; i < MAX_SERVERS; i++){
     if (tdep[i] < min){
      min = tdep[i];
      result = i;      
     }
  }  
  return result;
}

//==============================================================================
//=  inicializa el vector tdep con 0's                                         =
//==============================================================================
void initialize()
{
  unsigned int i = 0;
  for (i = 0; i< MAX_SERVERS; i++){
    tdep[i] = 0;
  }    
}