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
#define SIM_TIME   1000             // Tiempo de simulacion segundos
#define LAMBDA     2                // Razon de llegada de clientes
#define MIU        0.000222         // Razon de servicio de paquetes
#define MTU        1500             // Tamaño máximo de paquete
#define R          0.0416            // Tiempo por cliente por marco 1/24
#define CAPACIDAD_CLIENTES 135       // Capacidad de clientes que se pueden atender
#define CAPACIDAD_BUFFER 50      // Tiempo por cliente por marco

#define ARCHIVO_VIDEO "jurassic.txt"
#define ARCHIVO_DELAYS "amazon_delays.txt"


struct client {
  unsigned int id;
  double arrival;
  double departure;
  double frame_time;
  int current_frame;
  client(unsigned int i, double a, double time) : id(i), arrival(a), departure(-1.0), frame_time(time), current_frame(0) {}
};

struct packet {
  unsigned int client_id;
  int length;
  packet(unsigned int id, int l) : client_id(id), length(l) {}
  // double frame_id;
};

//----- Prototipos de Funciones --------------------------------------------------
double expVA(double x);        // Generar variable aleatoria exponencial con media x

bool double_equals(double a, double b, double epsilon = 0.00000001)
{
    return abs(a - b) < epsilon;
}

//===== Main program ===========================================================
int main()
{
  double   end_time = SIM_TIME; // Tiempo total a simular segundos

  double   Ta = 1.0/LAMBDA;     // Tiempo promedio de llegada clientes
  double   Ts = 1.0/MIU;        // Tiempo promedio de servicio por mensaje

  double   time = 0.0;          // Tiempo actual de simulacion
  double   tarr = 0.0;          // Tiempo para siguiente evento (llegada)
  double   tdep = SIM_TIME;     // tiempo para siguiente evento (salida)
  unsigned int id = 0;          // Id de paquetes en sistema llegada
  unsigned int id2 = 0;         // Id de paquetes en sistema salida

  unsigned int c = 0;           // Numero de servicios completos
  double   bc = 0.0;            // Tiempo total de ocupacion clientes
  double   bb = 0.0;            // Tiempo total de ocupacion buffer
  double   s = 0.0;             // Integral del numero de paquetes en sistema
  double   sc = 0.0;             // Integral del numero de paquetes en sistema
  double   tbc = -1.0;          // tiempo ocupacion inicio cliente
  double   tbb = -1.0;          // tiempo ocupacion inicio buffer

  double   x;                   // Throughput
  double   xc;                   // Throughput clientes
  double   fc;                   // Clientes perdidos %
  double   uc;                  // Utilizacion cliente
  double   ub;                  // Utilizacion buffer
  double   l;                   // Media de paquetes en sistema
  double   lc;                   // Media de paquetes en sistema
  double   w;                   // Media de tiempo de residencia  
  double   wc;                   // Media de tiempo de residencia  

  unsigned int K = CAPACIDAD_CLIENTES;
  unsigned int N = CAPACIDAD_BUFFER;

  unsigned int packets_sent = 0;
  unsigned int clients_attended = 0;
  unsigned int clients_lost = 0;

  vector<int> frames;

  double next_client_arr = 0.0;
  // vector<client> clients;
  // vector<packet> buffer;

  queue<pair<double, client> > clients;
  queue<pair<double, packet> > buffer;
  
  // Abrir archivos
  // Extrae los frame
  ifstream framesFile;
  framesFile.open (ARCHIVO_VIDEO); // Carga archivo de marcos de video
  while(!framesFile.eof()){
    int frame;
    framesFile >> frame;
    frames.push_back(frame);
  }
  framesFile.close();

  // Extraer delays
  ifstream delaysFile;
  delaysFile.open (ARCHIVO_DELAYS); // Carga archivo de retrazos
  
  // Ciclo principal
  while (time < end_time){
    bool newClient = false, clientPacket = false, bufferAction = false;
    double last_time = time;

    if(!clients.empty() && !buffer.empty()){

      if(clients.front().first <= buffer.front().first && clients.front().first <= next_client_arr){
        clientPacket = true;
      } else if(next_client_arr < clients.front().first && next_client_arr < buffer.front().first){
        newClient = true;
      } else {
        bufferAction = true;
      }


    } else if(!clients.empty()){
      if(clients.front().first < next_client_arr){
        clientPacket = true;
      } else {
        newClient = true;
      }
    } else if(!buffer.empty()){
      if(buffer.front().first < next_client_arr){
        bufferAction = true;
      } else {
        newClient = true;
      }
    } else {
      newClient = true;
    }

    if(clientPacket){
      client current = clients.front().second;
      double time = clients.front().first;

      if(time > SIM_TIME) break;

      clients.pop();

      int frame_size = frames[current.current_frame];
      int frame_count = ceil(frame_size / (double)MTU);

      int delay_int;
      double delay;

      if(!delaysFile.eof()){
        delaysFile >> delay_int;
        delay = delay_int * 0.000001;
      } else {
        delay = delay_int * 0.000100;
      } 
      if(buffer.size()+frame_count <= N){
        if(buffer.empty()){
          tbb = time;
        }
        for(int i = 0; i < frame_count; i++){
          double dep = time + (buffer.size()+1) * (double)MIU + delay;
          int size = frame_size%MTU;
          frame_size-=MTU;
          buffer.push(make_pair(dep, packet(current.id, size)));
        }
        if(current.current_frame % 500 == 0)
          printf("Envia frame %d el cliente %d\n", current.current_frame, current.id);
        current.current_frame++;
      }


      if(current.current_frame > frames.size()){
        printf("Cliente %d ha terminado en timepo %.3f\n", current.id, time);
        clients_attended++;
      } else {
        clients.push(make_pair(time+R, current));
      }

    }
    if(bufferAction){
      time = buffer.front().first;
      if(time > SIM_TIME) break;
      packet current = buffer.front().second;
      // printf("Paquete de tamaño %d enviado a cliente %d ==========>\n", current.length, current.client_id);
      packets_sent++;
      if(packets_sent%4000==0)
        printf("%d packets enviados. Actual al cliente %d.\n", packets_sent, current.client_id);


      buffer.pop();

    }

    if(clients.size() < K && newClient){
      time = next_client_arr;
      if(time > SIM_TIME) break;
      if(clients.empty()){
        tbc = time;
      }
      clients.push(make_pair(time+R, client(id, next_client_arr, 0)));
      id++;
      printf("Llega cliente %d al tiempo %.3f\n", id, time);
      next_client_arr += Ta;                 // se aumenta por el promedio de llegadas 
    } else if(newClient && clients.size()>=K) {
      next_client_arr += Ta;                 // se aumenta por el promedio de llegadas 
      clients_lost++;
    }

    s = s + buffer.size() * (time - last_time);  // actualiza area bajo curva de s
    sc = sc + clients.size() * (time - last_time);  // actualiza area bajo curva de s

    if(clients.empty() && time <= SIM_TIME && tbc >= 0){
      bc += (time - tbc);
      tbc = -1.0;
    }

    if(buffer.empty() && time <= SIM_TIME && tbb >= 0){
      bb += (time - tbb);
      tbb = -1.0;
    }


  }
  time = SIM_TIME;
  if(tbc >= 0 )
    bc += (time - tbc);
  if(tbb >= 0)
    bb += (time - tbb);

  // Procesa Resultados
  x = packets_sent / time;   // Procesa throughput
  xc = clients_attended / time;   // Procesa throughput
  fc = clients_lost/(double)(clients_lost+clients_attended);   // Procesa throughput
  ub = bb / time;   // Procesa utilizacion del concentrador
  uc = bc / time;   // Procesa utilizacion del concentrador
  l = s / time;   // Computa numero promedio de paquetes en sistema
  lc = sc / time;   // Computa numero promedio de paquetes en sistema
  w = l / x;      // Procesa tiempo promedio de estancia en sistema
  wc = lc / xc;      // Procesa tiempo promedio de estancia en sistema

  // Imprime resultados
  printf("============================================================================= \n");
  printf("=            *** Resultados de Simulacion Videos ***                        = \n");
  printf("============================================================================= \n");
  printf("=  Tiempo total simulado                                = %.2f s  \n", end_time);
  printf("============================================================================= \n");
  printf("=  Parametros de sistema:                                       \n");
  printf("=    Razon de llegada de clientes                       = %.6f clientes/s \n",1.0/Ta);
  printf("=    Razon de servicio de los paquetes                  = %.6f paq/s \n",1.0/Ts);
  printf("============================================================================= \n");
  printf("=  Resultados:                                                    \n");
  printf("=    Numero de paquetes atendidos                       = %d paq              \n", packets_sent);
  printf("=    Throughput del buffer                              = %.2f paq/s          \n", x);
  printf("=    Utilizacion del buffer                             = %.2f %%             \n",100.0 * ub);
  printf("=    Numero promedio de paquetes en sistema             = %.2f paq            \n", l);
  printf("=    Tiempo promedio de estancia de paquetes en sistema = %.2f s  \n", w);
  printf("=                                                                           = \n");
  printf("=    Numero de clientes atendidos                       = %d clientes         \n", clients_attended);
  printf("=    Porcentaje de clientes perdidos                    = %.2f %%         \n",100.0 * fc);
  printf("=    Throughput de la cola de clientes                  = %.2f clientes/s          \n", xc);
  printf("=    Utilizacion de la cola de clientes                 = %.2f %%             \n",100.0 * uc);
  printf("=    Numero promedio de clientes en sistema             = %.2f clientes            \n", lc);
  printf("=    Tiempo promedio de estancia de clientes en sistema = %.2f s  \n", wc);
  printf("============================================================================= \n");

  delaysFile.close();

  return 0;
}
