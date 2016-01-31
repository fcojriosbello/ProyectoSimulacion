/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/object.h"
#include "ns3/global-value.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include <ns3/average.h>
#include <ns3/error-model.h>
#include "ns3/point-to-point-module.h"
#include "ns3/gnuplot.h"
#include "Observador.h"
#include <cmath>

#define T_STUDENT 2.2622  //GENERICA, habrá que cambiarla al final
#define SIMULACIONES 1  //HAbrá que modificarlo

#define CSMA 0
#define WIFI 1

#define NODOS_SEDE2 30  //Número de nodos en la sede 2. Será fijo ya que sólo
                        //nos interesa medir en un sentido (problema simétrico).


//Simulación simple para el servicio VoIP usando CSMA
void
simulacionCSMA (uint32_t nCsma, Time ton, Time toff, uint32_t sizePkt, DataRate dataRate, 
    double prob_error_pkt1, double prob_error_pkt2, double prob_error_pkt3, std::string p2p_dataRate, 
    std::string p2p_delay, double& retardo, double& porcentaje, double& tasa);

//Simulación simple para el servicio VoIP usando WIFI
void
simulacionWifi (uint32_t nWifi, Time ton, Time toff, uint32_t sizePkt, DataRate dataRate, double prob_error_pkt,
      std::string p2p_dataRate, std::string p2p_delay, double& retardo, double& porcentaje, double& tasa);

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
  double perrorCSMA     = 1e-10;
  //double perrorALOHA    = 1e-4;
  double porcentaje     = 0.0;
  double retardo        = 0.0;
  double tasa           = 0.0;
  //std::string protocolo [] = {'CSMA', 'ALOHA', 'TONKEN RING'}

  Average<double> acu_porcentaje;
  Average<double> acu_retardo; 
  Average<double> acu_tasa;

  Gnuplot plotPorcentaje;
  plotPorcentaje.SetTitle("Porcentaje de paquetes erróneos");
  plotPorcentaje.SetLegend( "Número de nodos en la sede origen", "Porcentaje de Paquetes erróneos (%)");
  
  Gnuplot plotRetardo;
  plotRetardo.SetTitle("Retardo medio");
  plotRetardo.SetLegend( "Número de nodos en la sede origen", "Retardo medio (ms)");
  
  Gnuplot plotTasa;
  plotTasa.SetTitle("Tasa efectiva");
  plotTasa.SetLegend( "Número de nodos en la sede orgien", "Tasa efectiva (Mbps)");
  
  /* Hay que cambiar prot < 1 por prot <= WIFI */
  for (int prot = CSMA; prot < 1; prot++)
  {
    std::stringstream sstm;
  //  sstm << "Protocolo: " << protocolo[prot];
    if (prot == CSMA)
      sstm << "Protocolo: " << "CSMA";
    /*else if (prot == WIFI)
      sstm << "Protocolo: " << "WIFI";
    */
    std::string titleProt = sstm.str();

    // Dataset para CSMA: porcentaje de errores, retardo medio y tasa efectiva media
    Gnuplot2dDataset datosPorcentajeCSMA;
    datosPorcentajeCSMA.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    datosPorcentajeCSMA.SetErrorBars(Gnuplot2dDataset::Y);
    datosPorcentajeCSMA.SetTitle(titleProt);
    
    Gnuplot2dDataset datosRetardoCSMA;
    datosRetardoCSMA.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    datosRetardoCSMA.SetErrorBars(Gnuplot2dDataset::Y);
    datosRetardoCSMA.SetTitle(titleProt);
    
    Gnuplot2dDataset datosTasaCSMA;
    datosTasaCSMA.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    datosTasaCSMA.SetErrorBars(Gnuplot2dDataset::Y);
    datosTasaCSMA.SetTitle(titleProt);
    
    /*
    // Dataset para ALOHA
    Gnuplot2dDataset datosPorcentajeALOHA;
    datosPorcentajeALOHA.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    datosPorcentajeALOHA.SetErrorBars(Gnuplot2dDataset::Y);
    datosPorcentajeALOHA.SetTitle(titleProt);
    
    Gnuplot2dDataset datosRetardoALOHA;
    datosRetardoALOHA.SetErrorBars(Gnuplot2dDataset::Y);
    datosRetardoALOHA.SetTitle(titleProt);
    
    Gnuplot2dDataset datosTasaALOHA;
    datosTasaALOHA.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    datosTasaALOHA.SetErrorBars(Gnuplot2dDataset::Y);
    datosTasaALOHA.SetTitle(titleProt);
    */

  for (int numNodos = 10; numNodos < 100; numNodos+=10)
  {
    NS_LOG_DEBUG("Número de nodos " << numNodos);
    for(uint32_t numSimulaciones = 0; numSimulaciones < SIMULACIONES; numSimulaciones++)
    {
      NS_LOG_DEBUG("Número de simulación " << numSimulaciones);
      if (prot==CSMA){
        NS_LOG_DEBUG("Protocolo: CSMA");
        //simulacionWifi (numNodos, Time("0.150s"), Time("0.650s"), (uint32_t)40, DataRate("64kbps"), 1e-6, "2Mbps", 
              //"2ms", retardo, porcentaje, tasa);
        simulacionCSMA (numNodos, Time("0.150s"), Time("0.650s"), (uint32_t)40, DataRate("64kbps"),perrorCSMA, 
              perrorCSMA, 1e-6, "2Mbps", "2ms", retardo, porcentaje, tasa);
        acu_porcentaje.Update(porcentaje);
        acu_retardo.Update(retardo);
        acu_tasa.Update(tasa);
      }
      /*else if (prot==ALOHA)
      simulacionALOHA();
      acu_porcentaje.Update(porcentaje);
      acu_retardo.Update(retardo);
      acu_tasa.Update(tasa);
      */

    }
    
    // Añadimos los datos al punto para las tres gráficas
    if(acu_porcentaje.Count() > 0)
      datosPorcentajeCSMA.Add(numNodos, acu_porcentaje.Mean(), CalculaZ(acu_porcentaje.Var()));
    acu_porcentaje.Reset();
    
    if(acu_retardo.Count() > 0)
      datosRetardoCSMA.Add(numNodos, acu_retardo.Mean(), CalculaZ(acu_retardo.Var()));
    acu_retardo.Reset();
    
    if(acu_tasa.Count() > 0)
      datosTasaCSMA.Add(numNodos, acu_tasa.Mean(), CalculaZ(acu_tasa.Var()));
    acu_tasa.Reset();
    
    }
    
    // Añadimos los dataset a cada gráfica
    plotPorcentaje.AddDataset(datosPorcentajeCSMA);
    //plotPorcentaje.AddDataset(datosPorcentajeALOHA);
    plotRetardo.AddDataset(datosRetardoCSMA);
    //plotPorcentaje.AddDataset(datosTasaALOHA);
    plotTasa.AddDataset(datosTasaCSMA);
    //plotPorcentaje.AddDataset(datosTasaALOHA);
  }

std::ofstream fichero1("proyecto-1.plt");
plotPorcentaje.GenerateOutput(fichero1);
fichero1 << "pause -1" << std::endl;
fichero1.close();

std::ofstream fichero2("proyecto-2.plt");
plotRetardo.GenerateOutput(fichero2);
fichero2 << "pause -1" << std::endl;
fichero2.close();

std::ofstream fichero3("proyecto-3.plt");
plotTasa.GenerateOutput(fichero3);
fichero3 << "pause -1" << std::endl;
fichero3.close();

return 0;
}


//Simulación simple para el servicio VoIP usando CSMA
void
simulacionCSMA (uint32_t nCsma,
                Time ton,
                Time toff,
                uint32_t sizePkt,
                DataRate dataRate,
                double prob_error_pkt1,
                double prob_error_pkt2,
                double prob_error_pkt3,
                std::string p2p_dataRate,
                std::string p2p_delay,
                double& retardo,
                double& porcentaje,
                double& tasa)
{
NS_LOG_FUNCTION(nCsma << ton << toff << sizePkt << dataRate << prob_error_pkt1 << prob_error_pkt2 
  << prob_error_pkt3 << p2p_dataRate << p2p_delay << retardo << porcentaje << tasa);

  // Creamos los modelos de errores y le asociamos los parámetros
  Ptr<RateErrorModel> modelo_error1 = CreateObject<RateErrorModel> ();
  Ptr<RateErrorModel> modelo_error2 = CreateObject<RateErrorModel> ();
  Ptr<RateErrorModel> modelo_error3 = CreateObject<RateErrorModel> ();

  modelo_error1->SetRate(prob_error_pkt1);
  modelo_error1->SetUnit(RateErrorModel::ERROR_UNIT_BIT);

  modelo_error2->SetRate(prob_error_pkt2);
  modelo_error2->SetUnit(RateErrorModel::ERROR_UNIT_BIT);

  modelo_error3->SetRate(prob_error_pkt3);
  modelo_error3->SetUnit(RateErrorModel::ERROR_UNIT_BIT);

  // Nodos que pertenecen a la red de área local de la sede 1
  // Como primer nodo añadimos el encaminador de la operadora.
  // Como último nodo añadimos el sumidero.
  NodeContainer csmaNodes1;
  csmaNodes1.Create (nCsma + 2);

  // Nodos que pertenecen a la red de área local de la sede 2
  // Como primer nodo añadimos el encaminador de la operadora.
  // Como último nodo añadimos el sumidero.
  // Tendrá un número de nodos fijos ya que sólo 
  // nos interesa medir en un sentido (problema simétrico).
  NodeContainer csmaNodes2;
  csmaNodes2.Create (NODOS_SEDE2 + 2);

  // Nodos que pertenecen al enlace punto a punto.
  // Nodo cero de cada red csma.
  NodeContainer p2pNodes;
  p2pNodes.Add (csmaNodes1.Get(0));
  p2pNodes.Add (csmaNodes2.Get(0));

  // Instalamos el dispositivo de red en los nodos de la LAN
  CsmaHelper csma;
  NetDeviceContainer csmaDevices1;
  NetDeviceContainer csmaDevices2;
  csma.SetChannelAttribute ("DataRate", StringValue ("10Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));
  csmaDevices1 = csma.Install (csmaNodes1);
  csmaDevices2 = csma.Install (csmaNodes2);

  // Instalamos el dispositivo en los nodos punto a punto
  PointToPointHelper pointToPoint;
  NetDeviceContainer p2pDevices;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue(p2p_dataRate));//cap enlace
  pointToPoint.SetChannelAttribute ("Delay", StringValue (p2p_delay)); //retardo prop
  pointToPoint.SetQueue ("ns3::DropTailQueue");
  p2pDevices = pointToPoint.Install (p2pNodes); 

  //Configuramos el error del canal en las interfaces de la sede 1
  for (uint32_t k = 0; k < nCsma+2; k++ )
  csmaDevices1.Get (k)->SetAttribute ("ReceiveErrorModel", PointerValue (modelo_error1));

  //Configuramos el error del canal en las interfaces de la sede 2
  for (uint32_t k = 0; k < NODOS_SEDE2+2; k++ )
  csmaDevices2.Get (k)->SetAttribute ("ReceiveErrorModel", PointerValue (modelo_error3));

  //Configuramos el error del enlace p2p 
  p2pDevices.Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (modelo_error2));
  p2pDevices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (modelo_error2));

  // Instalamos la pila TCP/IP en todos los nodos
  // Los nodos p2p también estan incluidos en los 
  // contenedores de nodos de las sedes.
  InternetStackHelper stack;
  stack.Install (csmaNodes1);
  stack.Install (csmaNodes2);

  // Asignamos direcciones a cada una de las interfaces
  Ipv4AddressHelper address;
  //Subred 192.168.1.0 para la sede 1
  Ipv4InterfaceContainer csmaInterfaces1;
  address.SetBase ("192.168.1.0", "255.255.255.0");
  csmaInterfaces1 = address.Assign (csmaDevices1);
  //Subred 192.168.2.0 para la sede 2
  Ipv4InterfaceContainer csmaInterfaces2;
  address.SetBase ("192.168.2.0", "255.255.255.0");
  csmaInterfaces2 = address.Assign (csmaDevices2); 
  //Subred 10.0.1.0 para el enlace p2p
  Ipv4InterfaceContainer p2pInterfaces;
  address.SetBase ("10.0.1.0", "255.255.255.0");
  p2pInterfaces = address.Assign (p2pDevices);

  // Calculamos las rutas del escenario.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  // Establecemos un sumidero para los paquetes en el puerto 9.
  // Los sumideros estarán ubicados en los últimos nodos de las 
  // subredes csma (en ambas sedes).
  uint16_t port = 9;
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  ApplicationContainer appSink1 = sink.Install (csmaNodes1.Get (nCsma + 1));
  ApplicationContainer appSink2 = sink.Install (csmaNodes2.Get (NODOS_SEDE2 + 1)); 

  // Instalamos un cliente OnOff en los equipos de la sede 1.
  OnOffHelper VoIP1 ("ns3::UdpSocketFactory",
          Address (InetSocketAddress (csmaInterfaces2.GetAddress (NODOS_SEDE2 + 1), port)));

  // Creamos las variables aleatorias para los tiempos de on y off
  Ptr<ExponentialRandomVariable> tonExponencial = CreateObject<ExponentialRandomVariable> ();
  Ptr<ExponentialRandomVariable> toffExponencial = CreateObject<ExponentialRandomVariable> ();
  // Especificamos las medias de estas variables
  tonExponencial->SetAttribute("Mean", DoubleValue(ton.GetDouble()/1e9));
  toffExponencial->SetAttribute("Mean", DoubleValue(toff.GetDouble()/1e9));
  // Asociamos las variables aleatorias al cliente OnOff
  VoIP1.SetAttribute("OnTime", PointerValue(tonExponencial));
  VoIP1.SetAttribute("OffTime", PointerValue(toffExponencial));
  VoIP1.SetAttribute("PacketSize", UintegerValue(sizePkt));
  VoIP1.SetAttribute("DataRate", DataRateValue(dataRate));

  NodeContainer clientes1;

  for (uint32_t i = 1; i <= nCsma; i++)
    clientes1.Add(csmaNodes1.Get(i));

  //Instalamos la aplicación On/Off en todos y cada uno de 
  //los nodos de la red de área local de la sede 1.
  ApplicationContainer clientApps1 = VoIP1.Install (clientes1);
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (20.0));

  // Instalamos un cliente OnOff en los equipos de la sede 2.
  OnOffHelper VoIP2 ("ns3::UdpSocketFactory",
          Address (InetSocketAddress (csmaInterfaces1.GetAddress (nCsma + 1), port)));

  // Asociamos las variables aleatorias al cliente OnOff
  VoIP2.SetAttribute("OnTime", PointerValue(tonExponencial));
  VoIP2.SetAttribute("OffTime", PointerValue(toffExponencial));
  VoIP2.SetAttribute("PacketSize", UintegerValue(sizePkt));
  VoIP2.SetAttribute("DataRate", DataRateValue(dataRate));

  NodeContainer clientes2;

  for (uint32_t i = 1; i <= NODOS_SEDE2; i++)
    clientes2.Add(csmaNodes2.Get(i));

  //Instalamos la aplicación On/Off en todos y cada uno de 
  //los nodos de la red de área local de la sede 2.
  ApplicationContainer clientApps2 = VoIP2.Install (clientes2);
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (20.0));

  //Objeto observador para obtener los resultados de las simulaciones.
  Observador observador;

  for (uint32_t i = 0; i < clientApps1.GetN(); i++)
    //Conectamos las trazas al observador para todos los clientes de la sede 1.
    clientApps1.Get(i)->GetObject<OnOffApplication>()->TraceConnectWithoutContext ("Tx", 
                                  MakeCallback(&Observador::PktGenerado, &observador));

  //Conectamos la traza del sumidero al observador.
  appSink2.Get(0)->GetObject<PacketSink>()->TraceConnectWithoutContext ("Rx", 
                  MakeCallback(&Observador::PktRecibido, &observador));

  for (uint32_t j = 1; j <= nCsma; j++)
  {
    //Aprovechamos para cambiar el numero maximo de reintentos de tx.
    csmaDevices1.Get(j)->GetObject<CsmaNetDevice>()->SetBackoffParams (Time ("1us"), 10, 1000, 10, 8);
  }

    for (uint32_t j = 1; j <= NODOS_SEDE2; j++)
  {
    //Aprovechamos para cambiar el numero maximo de reintentos de tx.
    csmaDevices2.Get(j)->GetObject<CsmaNetDevice>()->SetBackoffParams (Time ("1us"), 10, 1000, 10, 8);
  }

  observador.SetTamPkt(sizePkt);

  // Lanzamos la simulación
  Simulator::Run ();
  Simulator::Destroy ();

  retardo = observador.GetMediaTiempos()/1e3;
  porcentaje = observador.GetPorcentajePktsPerdidos();
  tasa = observador.GetTasaMedia()/1e6;


  NS_LOG_INFO("Retardo de transmisión medio: " <<  retardo << "ms");
  NS_LOG_INFO("Porcentaje de paquetes perdidos: " << porcentaje << "%");
  NS_LOG_INFO("Tasa media efectiva: " << tasa << "Mbps");

}

void
simulacionWifi (uint32_t nWifi, Time ton, Time toff, uint32_t sizePkt, DataRate dataRate, double prob_error_pkt,
      std::string p2p_dataRate, std::string p2p_delay, double& retardo, double& porcentaje, double& tasa)
{
  NS_LOG_FUNCTION (nWifi << ton << toff << sizePkt << dataRate << prob_error_pkt << p2p_dataRate
          << p2p_delay << retardo << porcentaje << tasa);
  
  // Nodos que pertenecen a la red WAN de la sede 1.
  // Como primer nodo añadimos el encaminador de la operadora.
  // Como último nodo añadimos el sumidero.
  NodeContainer wifiNodes1;
  wifiNodes1.Create(nWifi+2);

  // Nodos que pertenecen a la red WAN de la sede 2.
  // Como primer nodo añadimos el encaminador de la operadora.
  // Como último nodo añadimos el sumidero.
  // Tendrá un número de nodos fijos ya que sólo 
  // nos interesa medir en un sentido (problema simétrico).
  NodeContainer wifiNodes2;
  wifiNodes2.Create(NODOS_SEDE2+2);


  // Nodos que pertenecen al enlace punto a punto.
  // Nodo cero de cada red csma.
  NodeContainer p2pNodes;
  p2pNodes.Add (wifiNodes1.Get(0));
  p2pNodes.Add (wifiNodes2.Get(0));

  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss("ns3::LogDistancePropagationLossModel", "Exponent", DoubleValue(3.0));
  Ptr<YansWifiChannel> canalWifi1 = wifiChannel.Create();
  Ptr<YansWifiChannel> canalWifi2 = wifiChannel.Create();

  // Creamos el modelo de nivel físico de los nodos de la sede 1
  YansWifiPhyHelper wifiPhy1;
  // Establecemos el modelo de error para el canal wifi de la sede 1
  wifiPhy1.SetErrorRateModel("ns3::NistErrorRateModel");
  // Le asociamos el wifi para la sede 1
  wifiPhy1.SetChannel(canalWifi1);

  // Creamos el modelo de nivel físico de los nodos de la sede 2
  YansWifiPhyHelper wifiPhy2;
  // Establecemos el modelo de error para el canal wifi de la sede 2
  wifiPhy2.SetErrorRateModel("ns3::NistErrorRateModel");
  // Le asociamos el wifi para la sede 1
  wifiPhy2.SetChannel(canalWifi2);

  // Creamos el modelo de nivel de enlace de los nodos de la sede 1
  NqosWifiMacHelper wifiMac;
  // Seleccionamos modo AdHoc y sin soporte de QoS
  wifiMac.SetType("ns3::AdhocWifiMac", "QosSupported", BooleanValue(false));

  // Definimos el estandar a usar en ambas sedes.
  WifiHelper wifi;
  wifi.SetStandard(WIFI_PHY_STANDARD_80211a);

  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", 
      StringValue("OfdmRate9Mbps"), "ControlMode", StringValue("OfdmRate9Mbps"));

  // Añadimos el netDevice Wifi a cada nodo de la sede 1
  NetDeviceContainer wifiDevices1 = wifi.Install(wifiPhy1, wifiMac, wifiNodes1);

  // Añadimos el netDevice Wifi a cada nodo de la sede 2
  NetDeviceContainer wifiDevices2 = wifi.Install(wifiPhy2, wifiMac, wifiNodes2);

  // Creamos el modelo de error para el enlace p2p
  Ptr<RateErrorModel> modelo_error = CreateObject<RateErrorModel> ();
  modelo_error->SetRate(prob_error_pkt);
  modelo_error->SetUnit(RateErrorModel::ERROR_UNIT_BIT);

  // Instalamos el dispositivo en los nodos punto a punto
  PointToPointHelper pointToPoint;
  NetDeviceContainer p2pDevices;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue(p2p_dataRate));//cap enlace
  pointToPoint.SetChannelAttribute ("Delay", StringValue (p2p_delay)); //retardo prop
  pointToPoint.SetQueue ("ns3::DropTailQueue");
  p2pDevices = pointToPoint.Install (p2pNodes);  

  //Configuramos el error del enlace p2p 
  p2pDevices.Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (modelo_error));
  p2pDevices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (modelo_error));

  // Instalamos la pila TCP/IP en los nodos
  InternetStackHelper stack;
  stack.Install (wifiNodes1);
  stack.Install (wifiNodes2);


  // Asignamos direcciones a cada una de las interfaces
  Ipv4AddressHelper address;
  //Subred 192.168.1.0 para la sede 1
  Ipv4InterfaceContainer interfacesWifi1;
  address.SetBase ("192.168.1.0", "255.255.255.0");
  interfacesWifi1 = address.Assign (wifiDevices1);
  //Subred 192.168.2.0 para la sede 2
  Ipv4InterfaceContainer interfacesWifi2;
  address.SetBase ("192.168.2.0", "255.255.255.0");
  interfacesWifi2 = address.Assign (wifiDevices2); 
  //Subred 10.0.1.0 para el enlace p2p
  Ipv4InterfaceContainer p2pInterfaces;
  address.SetBase ("10.0.1.0", "255.255.255.0");
  p2pInterfaces = address.Assign (p2pDevices);

  // Calculamos las rutas
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Creamos el modelo de movilidad para la sede 1
  MobilityHelper ayudanteMovilidad1;
  Ptr < ListPositionAllocator > localizaciones1 = CreateObject < ListPositionAllocator > ();

    //Añadimos las posibles localizaciones para los nodos de la sede 1.
  for(uint32_t i = 0; i <= sqrt(nWifi); i++)
    {
      for (uint32_t j = 0; j <= sqrt(nWifi); j++)
        localizaciones1->Add(Vector(i, j, 0));
    }
  
  ayudanteMovilidad1.SetPositionAllocator(localizaciones1);
  ayudanteMovilidad1.SetMobilityModel
    ("ns3::ConstantPositionMobilityModel");

  ayudanteMovilidad1.Install(wifiNodes1);


  // Creamos el modelo de movilidad para la sede 2
  MobilityHelper ayudanteMovilidad2;
  Ptr < ListPositionAllocator > localizaciones2 = CreateObject < ListPositionAllocator > ();

  //Añadimos las posibles localizaciones para los nodos de la sede 2.
  for(uint32_t i = 0; i <= sqrt(nWifi); i++)
    {
      for (uint32_t j = 0; j <= sqrt(nWifi); j++)
        localizaciones2->Add(Vector(i, j, 0));
    }
  
  ayudanteMovilidad2.SetPositionAllocator(localizaciones2);
  ayudanteMovilidad2.SetMobilityModel
    ("ns3::ConstantPositionMobilityModel");

  ayudanteMovilidad2.Install(wifiNodes2);



  uint16_t port = 9;

  PacketSinkHelper sink ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  ApplicationContainer appSink1 = sink.Install (wifiNodes1.Get (nWifi + 1));
  ApplicationContainer appSink2 = sink.Install (wifiNodes2.Get (NODOS_SEDE2 + 1)); 

  appSink1.Start(Seconds(1.0));
  appSink1.Stop(Seconds(23.0)); 

  appSink2.Start(Seconds(1.0));
  appSink2.Stop(Seconds(23.0)); 


  // Instalamos un cliente OnOff en los equipos de la sede 1.
  OnOffHelper VoIP1 ("ns3::UdpSocketFactory",
          Address (InetSocketAddress (interfacesWifi2.GetAddress (NODOS_SEDE2 + 1), port)));

  // Creamos las variables aleatorias para los tiempos de on y off
  Ptr<ExponentialRandomVariable> tonExponencial = CreateObject<ExponentialRandomVariable> ();
  Ptr<ExponentialRandomVariable> toffExponencial = CreateObject<ExponentialRandomVariable> ();
  // Especificamos las medias de estas variables
  tonExponencial->SetAttribute("Mean", DoubleValue(ton.GetDouble()/1e9));
  toffExponencial->SetAttribute("Mean", DoubleValue(toff.GetDouble()/1e9));
  // Asociamos las variables aleatorias al cliente OnOff
  VoIP1.SetAttribute("OnTime", PointerValue(tonExponencial));
  VoIP1.SetAttribute("OffTime", PointerValue(toffExponencial));
  VoIP1.SetAttribute("PacketSize", UintegerValue(sizePkt));
  VoIP1.SetAttribute("DataRate", DataRateValue(dataRate));

  NodeContainer clientes1;

  for (uint32_t i = 1; i <= nWifi; i++)
    clientes1.Add(wifiNodes1.Get(i));

  //Instalamos la aplicación On/Off en todos y cada uno de 
  //los nodos de la red de área local de la sede 1.
  ApplicationContainer clientApps1 = VoIP1.Install (clientes1);
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (20.0));

  // Instalamos un cliente OnOff en los equipos de la sede 2.
  OnOffHelper VoIP2 ("ns3::UdpSocketFactory",
          Address (InetSocketAddress (interfacesWifi1.GetAddress (nWifi + 1), port)));

  // Asociamos las variables aleatorias al cliente OnOff
  VoIP2.SetAttribute("OnTime", PointerValue(tonExponencial));
  VoIP2.SetAttribute("OffTime", PointerValue(toffExponencial));
  VoIP2.SetAttribute("PacketSize", UintegerValue(sizePkt));
  VoIP2.SetAttribute("DataRate", DataRateValue(dataRate));

  NodeContainer clientes2;

  for (uint32_t i = 1; i <= NODOS_SEDE2; i++)
    clientes2.Add(wifiNodes2.Get(i));

  //Instalamos la aplicación On/Off en todos y cada uno de 
  //los nodos de la red de área local de la sede 2.
  ApplicationContainer clientApps2 = VoIP2.Install (clientes2);
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (20.0));

  Observador observador;

  for (uint32_t i = 0; i < clientApps1.GetN(); i++)
    //Conectamos las trazas al observador para todos los clientes de la sede 1.
    clientApps1.Get(i)->GetObject<OnOffApplication>()->TraceConnectWithoutContext ("Tx", 
                                  MakeCallback(&Observador::PktGenerado, &observador));

  //Conectamos la traza del sumidero al observador.
  appSink2.Get(0)->GetObject<PacketSink>()->TraceConnectWithoutContext ("Rx", 
                  MakeCallback(&Observador::PktRecibido, &observador));

  observador.SetTamPkt(sizePkt);

  Simulator::Run();
  Simulator::Destroy();

  retardo = observador.GetMediaTiempos()/1e3;
  porcentaje = observador.GetPorcentajePktsPerdidos();
  tasa = observador.GetTasaMedia()/1e6;


  NS_LOG_INFO("Retardo de transmisión medio: " <<  retardo << "ms");
  NS_LOG_INFO("Porcentaje de paquetes perdidos: " << porcentaje << "%");
  NS_LOG_INFO("Tasa media efectiva: " << tasa << "Mbps");

}
