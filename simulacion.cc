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
#define ALOHA 1
#define TOKEN_RING 2 //Se modificará el valor cuando tengamos más protocolos

#define MAXWIFI 18
#define NWIFI 100
#define PUNTOSDIST 25


//Simulación simple para el servicio VoIP usando CSMA
void
simulacionCSMA (uint32_t nCsma, Time ton, Time toff, uint32_t sizePkt, DataRate dataRate, double prob_error_pkt, double& retardo, double& porcentaje, double& tasa);
void
simulacionWifi (uint32_t num_nodos, Time ton, Time toff, uint32_t sizePkt, 
                DataRate dataRate, double& retardo, double& porcentaje, double& tasa);

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
  //double perrorCSMA     = 1e-10;
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
  plotPorcentaje.SetLegend( "Número de nodos", "Porcentaje de Paquetes erróneos (%)");
  
  Gnuplot plotRetardo;
  plotRetardo.SetTitle("Retardo medio");
  plotRetardo.SetLegend( "Número de nodos", "Retardo medio (ms)");
  
  Gnuplot plotTasa;
  plotTasa.SetTitle("Tasa efectiva");
  plotTasa.SetLegend( "Número de nodos", "Tasa efectiva (Mbps)");
  
  /* Hay que cambiar prot < 1 por prot < 3 */
  for (int prot = CSMA; prot < 1; prot++)
  {
    std::stringstream sstm;
  //  sstm << "Protocolo: " << protocolo[prot];
    if (prot == CSMA)
      sstm << "Protocolo: " << "CSMA";
    /*else if (prot == ALOHA)
      sstm << "Protocolo: " << "ALOHA";
    */
    std::string titleProt = sstm.str();

    // Dataset para CSMA: porcentaje de errores, retardo medio y tasa efectiva media
    Gnuplot2dDataset datosPorcentajeCSMA;
    datosPorcentajeCSMA.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    datosPorcentajeCSMA.SetErrorBars(Gnuplot2dDataset::Y);
    datosPorcentajeCSMA.SetTitle(titleProt);
    
    Gnuplot2dDataset datosRetardoCSMA;
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
        simulacionWifi (numNodos, Time("0.150s"), Time("0.650s"), (uint32_t)40, DataRate("64kbps"), retardo, porcentaje, tasa);
        //simulacionCSMA (numNodos, Time("0.150s"), Time("0.650s"), (uint32_t)40, DataRate("64kbps"),perrorCSMA, retardo, porcentaje, tasa);
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
                double prob_error_pkt,
                double& retardo,
                double& porcentaje,
                double& tasa)
{
NS_LOG_FUNCTION(nCsma << ton << toff << sizePkt << dataRate << prob_error_pkt << retardo << porcentaje << tasa);

  // Creamos el modelo de error y le asociamos los parametros
  Ptr<RateErrorModel> modelo_error = CreateObject<RateErrorModel> ();

  modelo_error->SetRate(prob_error_pkt);
  modelo_error->SetUnit(RateErrorModel::ERROR_UNIT_BIT);

  // Nodos que pertenecen a la red de área local
  // Como primer nodo añadimos el sumidero.
  NodeContainer csmaNodes;
  csmaNodes.Create (nCsma);

  // Nodos que pertenecen al enlace punto a punto
  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  // Como primer nodo añadimos el encaminador que proporciona acceso
  // al enlace punto a punto.
  csmaNodes.Add (p2pNodes.Get (1));

  // Instalamos el dispositivo en los nodos punto a punto
  PointToPointHelper pointToPoint;
  NetDeviceContainer p2pDevices;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue("2Mbps"));//cap enlace
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms")); //retardo prop
  p2pDevices = pointToPoint.Install (p2pNodes); 

  // Instalamos el dispositivo de red en los nodos de la LAN
  CsmaHelper csma;
  NetDeviceContainer csmaDevices;
  csma.SetChannelAttribute ("DataRate", StringValue ("10Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));
  csmaDevices = csma.Install (csmaNodes);

  //Configuramos el error del canal en las interfaces
  for (uint32_t k = 0; k < nCsma; k++ )
  csmaDevices.Get (k)->SetAttribute ("ReceiveErrorModel", PointerValue (modelo_error));

  // Instalamos la pila TCP/IP en todos los nodos
  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install (p2pNodes.Get (0));

  // Asignamos direcciones a cada una de las interfaces
  Ipv4AddressHelper address;
  Ipv4InterfaceContainer csmaInterfaces;
  address.SetBase ("192.168.1.0", "255.255.255.0");
  csmaInterfaces = address.Assign (csmaDevices);
  Ipv4InterfaceContainer p2pInterfaces;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  p2pInterfaces = address.Assign (p2pDevices);

  // Calculamos las rutas del escenario.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  // Establecemos un sumidero para los paquetes en el puerto 9 del nodo
  //     aislado del enlace punto a punto
  uint16_t port = 9;
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  ApplicationContainer app = sink.Install (p2pNodes.Get (0));

  // Instalamos un cliente OnOff en uno de los equipos de la red de área local
  OnOffHelper VoIP ("ns3::UdpSocketFactory",
                        Address (InetSocketAddress (p2pInterfaces.GetAddress (0), port)));

  // Creamos las variables aleatorias para los tiempos de on y off
  Ptr<ExponentialRandomVariable> tonExponencial = CreateObject<ExponentialRandomVariable> ();
  Ptr<ExponentialRandomVariable> toffExponencial = CreateObject<ExponentialRandomVariable> ();
  // Especificamos las medias de estas variables
  tonExponencial->SetAttribute("Mean", DoubleValue(ton.GetDouble()/1e9));
  toffExponencial->SetAttribute("Mean", DoubleValue(toff.GetDouble()/1e9));
  // Asociamos las variables aleatorias al cliente OnOff
  VoIP.SetAttribute("OnTime", PointerValue(tonExponencial));
  VoIP.SetAttribute("OffTime", PointerValue(toffExponencial));
  VoIP.SetAttribute("PacketSize", UintegerValue(sizePkt));
  VoIP.SetAttribute("DataRate", DataRateValue(dataRate));

  NodeContainer clientes;

  for (uint32_t i = 1; i < nCsma; i++)
    clientes.Add(csmaNodes.Get(i));

  //Instalamos la aplicación On/Off en todos y cada uno de los nodos de la red de área local.
  ApplicationContainer clientApps = VoIP.Install (clientes);
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (20.0));

  //Objeto observador para obtener los resultados de las simulaciones.
  Observador observador;

  for (uint32_t i = 0; i < clientApps.GetN(); i++)
    //Conectamos las trazas al observador para todos los clientes.
    clientApps.Get(i)->GetObject<OnOffApplication>()->TraceConnectWithoutContext ("Tx", MakeCallback(&Observador::PktGenerado, &observador));

  //Conectamos la traza del sumidero al observador.
  app.Get(0)->GetObject<PacketSink>()->TraceConnectWithoutContext ("Rx", MakeCallback(&Observador::PktRecibido, &observador));

  for (uint32_t j = 1; j < nCsma; j++)
  {
    //Aprovechamos para cambiar el numero maximo de reintentos de tx
    csmaDevices.Get(j)->GetObject<CsmaNetDevice>()->SetBackoffParams (Time ("1us"), 10, 1000, 10, 8);
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
simulacionWifi (uint32_t num_nodos, Time ton, Time toff, uint32_t sizePkt, 
                DataRate dataRate, double& retardo, double& porcentaje, double& tasa)
{
  
  // Nodos que pertenecen a la red WAN
  NodeContainer nodos;
  nodos.Create(num_nodos);

  // Nodos que pertenecen al enlace punto a punto
  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  // Como primer nodo añadimos el encaminador que proporciona acceso
  // al enlace punto a punto.
  nodos.Add (p2pNodes.Get (1));

  YansWifiChannelHelper canalWifi;
  canalWifi.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  canalWifi.AddPropagationLoss("ns3::LogDistancePropagationLossModel", "Exponent", DoubleValue(3.0));
  Ptr<YansWifiChannel> canal = canalWifi.Create();

  // Creamos el modelo de nivel PHY de las estaciones
  YansWifiPhyHelper medioWifi;
  // Establecemos el modelo de error
  medioWifi.SetErrorRateModel("ns3::NistErrorRateModel");
  // Le asociamos el canal
  medioWifi.SetChannel(canal);

  // Creamos el modelo de nivel MAC de las estaciones
  NqosWifiMacHelper MACwifi;
  // Seleccionamos modo AdHoc y sin soporte de QoS
  MACwifi.SetType("ns3::AdhocWifiMac", "QosSupported", BooleanValue(false));

  // Creamos el modelo para los dispositivos de red
  WifiHelper wifi;
  wifi.SetStandard(WIFI_PHY_STANDARD_80211a);

  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("OfdmRate9Mbps"), "ControlMode", StringValue("OfdmRate9Mbps"));

  // Y añadimos uno de estos dispositivos a cada uno de los nodos
  NetDeviceContainer dispositivosWifi = wifi.Install(medioWifi, MACwifi, nodos);

  // Instalamos el dispositivo en los nodos punto a punto
  PointToPointHelper pointToPoint;
  NetDeviceContainer p2pDevices;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue("2Mbps"));//cap enlace
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms")); //retardo prop
  p2pDevices = pointToPoint.Install (p2pNodes); 

  // Instalamos la pila TCP/IP en los nodos
  InternetStackHelper stack;
  stack.Install(nodos);
  stack.Install (p2pNodes.Get (0));


  // Asignamos direcciones a los dispositivos
  Ipv4AddressHelper address;
  address.SetBase("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfacesWiFi =
    address.Assign(dispositivosWifi);
  Ipv4InterfaceContainer p2pInterfaces;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  p2pInterfaces = address.Assign (p2pDevices);

  // Calculamos las rutas
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Creamos el modelo de movilidad
  MobilityHelper ayudanteMovilidad;
  Ptr < ListPositionAllocator > localizaciones =
    CreateObject < ListPositionAllocator > ();

    //Añadimos las posibles localizaciones para los nodos.
  for(uint32_t i = 0; i <= sqrt(num_nodos); i++)
    {
      for (uint32_t j = 0; j <= sqrt(num_nodos); j++)
        localizaciones->Add(Vector(i, j, 0));
    }
  
  ayudanteMovilidad.SetPositionAllocator(localizaciones);
  ayudanteMovilidad.SetMobilityModel
    ("ns3::ConstantPositionMobilityModel");

  ayudanteMovilidad.Install(nodos);

  uint16_t port = 9;

  PacketSinkHelper sink ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  ApplicationContainer serverApp = sink.Install (p2pNodes.Get (0));

  serverApp.Start(Seconds(1.0));
  serverApp.Stop(Seconds(23.0)); 


  OnOffHelper VoIP ("ns3::UdpSocketFactory", Address (InetSocketAddress (p2pInterfaces.GetAddress (0), port)));
  // Creamos las variables aleatorias para los tiempos de on y off
  Ptr<ExponentialRandomVariable> tonExponencial = CreateObject<ExponentialRandomVariable> ();
  Ptr<ExponentialRandomVariable> toffExponencial = CreateObject<ExponentialRandomVariable> ();
  // Especificamos las medias de estas variables
  tonExponencial->SetAttribute("Mean", DoubleValue(ton.GetDouble()/1e6));
  toffExponencial->SetAttribute("Mean", DoubleValue(toff.GetDouble()/1e6));
  // Asociamos las variables aleatorias al cliente OnOff
  VoIP.SetAttribute("OnTime", PointerValue(tonExponencial));
  VoIP.SetAttribute("OffTime", PointerValue(toffExponencial));
  VoIP.SetAttribute("PacketSize", UintegerValue(sizePkt));
  VoIP.SetAttribute("DataRate", DataRateValue(dataRate));

  NodeContainer clientes;

  for (uint32_t i = 1; i < num_nodos; i++)
    clientes.Add(nodos.Get(i));

  ApplicationContainer clientApps = VoIP.Install (clientes);
  clientApps.Start(Seconds(2.0));
  clientApps.Stop(Seconds(20.0));

  Observador observador;

  for (uint32_t i = 0; i < clientApps.GetN(); i++)
    clientApps.Get(i)->GetObject<OnOffApplication>()->TraceConnectWithoutContext ("Tx", MakeCallback(&Observador::PktGenerado, &observador));

  serverApp.Get(0)->GetObject<PacketSink>()->TraceConnectWithoutContext ("Rx", MakeCallback(&Observador::PktRecibido, &observador));
  
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