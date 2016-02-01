#include "ns3/gnuplot.h"
#include "ns3/object.h"
#include "ns3/core-module.h"
#include <ns3/average.h>
#include <ns3/data-rate.h>

#include "simulacionCSMA.h"
#include "simulacionWifi.h"
#include <cmath>

#define T_STUDENT 2.7765  
#define SIMULACIONES 5

#define CSMA 0
#define WIFI 1

#define MAX_NODOS 100
#define PASO_NODOS 10

#define MOD1 0
#define MOD2 1
#define MOD3 2


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("fuentes09");

// Nos calcula la z para el intervalo de confianza
double
CalculaZ(double varianza)
{
    double z = T_STUDENT*sqrt(varianza/(SIMULACIONES));
    return z;
}

int
main (int argc, char *argv[])
{
  GlobalValue::Bind("ChecksumEnabled", BooleanValue(true));
  Time::SetResolution (Time::US);

  //Variables para definir la modalidad de L3VPN a usar
  double p2p_prob_error_bit;
  std::string p2p_dataRate; 
  std::string p2p_delay;

  //Parámetros de los enlaces csma
  double csma_perror     = 1e-10;
  std::string csma_dataRate = "10Mbps"; 
  std::string csma_delay = "6560ns";

  //Parámetros de wifi
  std::string wifi_dataRate = "9Mbps"; 

  //Parámetros de VoIP
  Time ton = Time("0.150s");
  Time toff = Time("0.650s");
  uint32_t sizePkt = 40;
  DataRate dataRate = DataRate("64kbps");

  //Variables para obtener los resultados de las simulaciones simples.
  double porcentaje     = 0.0;
  double retardo        = 0.0;
  double tasa           = 0.0;

  //Acumuladores para obtener resultados asociados a un número de nodos
  Average<double> acu_porcentaje;
  Average<double> acu_retardo; 
  Average<double> acu_tasa;


  //----------------------Parámetros de entrada-------------------------
  CommandLine cmd;
  //Parámetros VoIP
  cmd.AddValue ("ton", "Tiempo del periodo en on de VoIP", ton);
  cmd.AddValue ("toff", "Tiempo del periodo en off de VoIP", toff);
  cmd.AddValue ("sizePkt", "Tamaño del paquete VoIP", sizePkt);
  cmd.AddValue ("dataRate", "Tasa de envío durante el periodo en on de VoIP", dataRate);
  //Parámetros csma
  cmd.AddValue ("csma_perror", "Probabilidad de error de bit para los enlaces csma", csma_perror);
  cmd.AddValue ("csma_dataRate", "Tasa del enlace csma", csma_dataRate);
  cmd.AddValue ("csma_delay", "Retardo del enlace csma", csma_delay);
  //Parámetros wifi
  cmd.AddValue ("wifi_dataRate", "Tasa del enlace wifi (6, 9, 12, 18, 24, 36, 48 o 54Mbps)", wifi_dataRate);  

  cmd.Parse (argc,argv);

  NS_LOG_INFO ("Se han configurado los siguientes argumentos de entrada:" << std::endl <<
               "     -ton:             " << ton.GetDouble()/1e6 << "s" << std::endl <<
               "     -toff:            " << toff.GetDouble()/1e6 << "s" << std::endl <<
               "     -sizePkt:         " << sizePkt << "B" << std::endl <<
               "     -dataRate:        " << dataRate.GetBitRate()/1e3 << "kbps" << std::endl <<
               "     -csma_perror:     " << csma_perror << std::endl <<
               "     -csma_dataRate:   " << csma_dataRate << std::endl <<
               "     -csma_delay:      " << csma_delay << std::endl <<
               "     -wifi_dataRate:   " << wifi_dataRate);

  std::stringstream wifi_modulation;
  wifi_modulation << "OfdmRate" << wifi_dataRate;
  wifi_dataRate = wifi_modulation.str();
  //-----------------------------------------------------------------------


  //Por cada modalidad de L3VPN deberemos obetener 3 gráficas con
  //2 curvas cada una.
  for (int modalidad = MOD1; modalidad <= MOD3; modalidad++)
  {
    //Cambiamos los parámetros del enlace p2p en función de la modalidad.
    if (modalidad == MOD1)
    {
      p2p_prob_error_bit = 1e-5;
      p2p_dataRate = "2Mbps"; 
      p2p_delay = "140ms";
    }
    else if (modalidad == MOD2)
    {
      p2p_prob_error_bit = 1e-6;
      p2p_dataRate = "7Mbps"; 
      p2p_delay = "70ms";      
    }
    else
    {
      p2p_prob_error_bit = 1e-8;
      p2p_dataRate = "20Mbps"; 
      p2p_delay = "30ms";      
    }

    NS_LOG_DEBUG("Modalidad L3VPN: " << modalidad);
    NS_LOG_DEBUG("Prob. error de bit (enlace p2p): " << p2p_prob_error_bit);
    NS_LOG_DEBUG("Tasa (enlace p2p): " << p2p_dataRate);
    NS_LOG_DEBUG("Retardo (enlace p2p): " << p2p_delay);
    NS_LOG_DEBUG("---------------------------------------");
    

    //Preparamos las 3 gráficas
    Gnuplot plotPorcentaje;
    plotPorcentaje.SetTitle("Porcentaje de paquetes erróneos");
    plotPorcentaje.SetLegend( "Número de nodos en la sede origen", "Porcentaje de Paquetes erróneos (%)");
  
    Gnuplot plotRetardo;
    plotRetardo.SetTitle("Retardo medio");
    plotRetardo.SetLegend( "Número de nodos en la sede origen", "Retardo medio (ms)");
  
    Gnuplot plotTasa;
    plotTasa.SetTitle("Tasa efectiva");
    plotTasa.SetLegend( "Número de nodos en la sede orgien", "Tasa efectiva (Kbps)");
  
    // Por cada protocolo debemos obtener 3 curvas (una para cada gráfica). 
    for (int prot = CSMA; prot <= WIFI; prot++)
    {
      //std::stringstream sstm;
      std::string titleProt;

      if (prot == CSMA)
      {
        //sstm << "Protocolo: CSMA";
        titleProt = "Protocolo: CSMA";
        NS_LOG_DEBUG("Protocolo: CSMA");
      }
     else if (prot == WIFI)
      {
        //sstm << "Protocolo: WIFI";
        titleProt = "Protocolo: WIFI";
        NS_LOG_DEBUG("Protocolo: WIFI");
      }

      NS_LOG_DEBUG("---------------------------------------");
    
      //std::string titleProt = sstm.str();

      // Datasets: porcentaje de errores, retardo medio y tasa efectiva media
      // Preparamos las curvas.
      Gnuplot2dDataset datosPorcentaje;
      datosPorcentaje.SetStyle(Gnuplot2dDataset::LINES_POINTS);
      datosPorcentaje.SetErrorBars(Gnuplot2dDataset::Y);
      datosPorcentaje.SetTitle(titleProt);
    
      Gnuplot2dDataset datosRetardo;
      datosRetardo.SetStyle(Gnuplot2dDataset::LINES_POINTS);
      datosRetardo.SetErrorBars(Gnuplot2dDataset::Y);
      datosRetardo.SetTitle(titleProt);
    
      Gnuplot2dDataset datosTasa;
      datosTasa.SetStyle(Gnuplot2dDataset::LINES_POINTS);
      datosTasa.SetErrorBars(Gnuplot2dDataset::Y);
      datosTasa.SetTitle(titleProt);
    

      for (int numNodos = 10; numNodos <= MAX_NODOS; numNodos += PASO_NODOS)
      {
        NS_LOG_DEBUG("Número de nodos " << numNodos);

        for(uint32_t numSimulaciones = 0; numSimulaciones < SIMULACIONES; numSimulaciones++)
        {

         NS_LOG_DEBUG("Número de simulación " << numSimulaciones);

         if (prot==CSMA){
           NS_LOG_DEBUG("Protocolo: CSMA");

           simulacionCSMA (numNodos, ton, toff, sizePkt, dataRate, csma_perror, csma_dataRate, csma_delay,
              p2p_prob_error_bit, p2p_dataRate, p2p_delay, retardo, porcentaje, tasa);
          }
           else if (prot == WIFI)
          {
           NS_LOG_DEBUG("Protocolo: WIFI");
           simulacionWifi (numNodos, ton, toff, sizePkt, dataRate, wifi_dataRate,
              p2p_prob_error_bit, p2p_dataRate, p2p_delay, retardo, porcentaje, tasa);
          }
        
          acu_porcentaje.Update(porcentaje);
          acu_retardo.Update(retardo);
          acu_tasa.Update(tasa);
        }
    
    
        // Añadimos los datos al punto para las tres gráficas
        if(acu_porcentaje.Count() > 0)
         datosPorcentaje.Add(numNodos, acu_porcentaje.Mean(), CalculaZ(acu_porcentaje.Var()));
        acu_porcentaje.Reset();
    
        if(acu_retardo.Count() > 0)
          datosRetardo.Add(numNodos, acu_retardo.Mean(), CalculaZ(acu_retardo.Var()));
        acu_retardo.Reset();
    
        if(acu_tasa.Count() > 0)
          datosTasa.Add(numNodos, acu_tasa.Mean(), CalculaZ(acu_tasa.Var()));
       acu_tasa.Reset();
      }
    
      // Añadimos los dataset a cada gráfica
      plotPorcentaje.AddDataset(datosPorcentaje);    
      plotRetardo.AddDataset(datosRetardo);
      plotTasa.AddDataset(datosTasa);
    }

    //Pasamos la primera gráfica a un archivo en función de la modalidad simulada.
    if (modalidad == MOD1)
    {
      std::ofstream fichero1("proyecto_mod1-1.plt");
      plotPorcentaje.GenerateOutput(fichero1);
      fichero1 << "pause -1" << std::endl;
      fichero1.close();
    }
    else if (modalidad == MOD2)
    {
      std::ofstream fichero1("proyecto_mod2-1.plt");
      plotPorcentaje.GenerateOutput(fichero1);
      fichero1 << "pause -1" << std::endl;
      fichero1.close();
    } 
    else
    {
      std::ofstream fichero1("proyecto_mod3-1.plt");
      plotPorcentaje.GenerateOutput(fichero1);
      fichero1 << "pause -1" << std::endl;
      fichero1.close();
    }

    //Pasamos la segunda gráfica a un archivo en función de la modalidad simulada.
    if (modalidad == MOD1)
    {
      std::ofstream fichero2("proyecto_mod1-2.plt");
      plotRetardo.GenerateOutput(fichero2);
      fichero2 << "pause -1" << std::endl;
      fichero2.close();
    }
    else if (modalidad == MOD2)
    {
      std::ofstream fichero2("proyecto_mod2-2.plt");
      plotRetardo.GenerateOutput(fichero2);
      fichero2 << "pause -1" << std::endl;
      fichero2.close();
    } 
    else
    {
      std::ofstream fichero2("proyecto_mod3-2.plt");
      plotRetardo.GenerateOutput(fichero2);
      fichero2 << "pause -1" << std::endl;
      fichero2.close();
    }

    //Pasamos la tercera gráfica a un archivo en función de la modalidad simulada.
    if (modalidad == MOD1)
    {
      std::ofstream fichero3("proyecto_mod1-3.plt");
      plotTasa.GenerateOutput(fichero3);
      fichero3 << "pause -1" << std::endl;
      fichero3.close();
    }
    else if (modalidad == MOD2)
    {
      std::ofstream fichero3("proyecto_mod2-3.plt");
      plotTasa.GenerateOutput(fichero3);
      fichero3 << "pause -1" << std::endl;
      fichero3.close();
    } 
    else
    {
      std::ofstream fichero3("proyecto_mod3-3.plt");
      plotTasa.GenerateOutput(fichero3);
      fichero3 << "pause -1" << std::endl;
      fichero3.close();
    }
  }

return 0;
}