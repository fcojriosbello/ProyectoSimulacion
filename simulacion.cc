/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/object.h"
#include "ns3/global-value.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include <ns3/adhoc-aloha-noack-ideal-phy-helper.h>
#include <ns3/wifi-spectrum-value-helper.h>
#include <ns3/average.h>
#include <ns3/error-model.h>
#include <ns3/spectrum-helper.h>
#include <ns3/spectrum-channel.h>
#include "ns3/gnuplot.h"
#include "ObservadorCSMA.h"

#define T_STUDENT 2.2622	//GENERICA, habrá que cambiarla al final
#define SIMULACIONES 1	//HAbrá que modificarlo

#define CSMA 0
#define ALOHA 1
#define TOKEN_RING 2 //Se modificará el valor cuando tengamos más protocolos

//Simulación simple para el servicio VoIP usando CSMA
void
simulacionCSMA (uint32_t nCsma, Time ton, Time toff, uint32_t sizePkt, DataRate dataRate, double prob_error_pkt, double& retardo, double& porcentaje);
void
simulacionAloha (uint32_t nCsma, Time ton, Time toff, uint32_t sizePkt, DataRate dataRate, double prob_error_pkt, double& retardo, double& porcentaje);


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
	double porcentaje     = 0.0;
	double retardo        = 0.0;

	simulacionAloha (30, Time("0.150s"), Time("0.650s"), (uint32_t)40, DataRate("64kbps"), 1e-4,  retardo, porcentaje);


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
								double& porcentaje)
{
NS_LOG_FUNCTION(nCsma << ton << toff << sizePkt << dataRate << prob_error_pkt << retardo << porcentaje);

  // Creamos el modelo de error y le asociamos los parametros
  Ptr<RateErrorModel> modelo_error = CreateObject<RateErrorModel> ();

  modelo_error->SetRate(prob_error_pkt);
  modelo_error->SetUnit(RateErrorModel::ERROR_UNIT_BIT);

  // Nodos que pertenecen a la red de área local
  // Como primer nodo añadimos el sumidero.
  NodeContainer csmaNodes;
  csmaNodes.Create (nCsma);

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

  // Asignamos direcciones a cada una de las interfaces
  Ipv4AddressHelper address;
  Ipv4InterfaceContainer csmaInterfaces;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  csmaInterfaces = address.Assign (csmaDevices);

  // Calculamos las rutas del escenario.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  // Establecemos un sumidero para los paquetes en el puerto 9 del nodo
  //     aislado del enlace punto a punto
  uint16_t port = 9;
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  ApplicationContainer app = sink.Install (csmaNodes.Get (0));

  // Instalamos un cliente OnOff en uno de los equipos de la red de área local
  OnOffHelper VoIP ("ns3::UdpSocketFactory",
                        Address (InetSocketAddress (csmaInterfaces.GetAddress (0), port)));

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
  ObservadorCSMA observadorCSMA;

  for (uint32_t i = 0; i < clientApps.GetN(); i++)
    //Conectamos las trazas al observador para todos los clientes.
    clientApps.Get(i)->GetObject<OnOffApplication>()->TraceConnectWithoutContext ("Tx", MakeCallback(&ObservadorCSMA::PktGenerado, &observadorCSMA));

  //Conectamos la traza del sumidero al observador.
  app.Get(0)->GetObject<PacketSink>()->TraceConnectWithoutContext ("Rx", MakeCallback(&ObservadorCSMA::PktRecibido, &observadorCSMA));

  for (uint32_t j = 1; j < nCsma; j++)
  {
    //Aprovechamos para cambiar el numero maximo de reintentos de tx
    csmaDevices.Get(j)->GetObject<CsmaNetDevice>()->SetBackoffParams (Time ("1us"), 10, 1000, 10, 8);
  }
  observadorCSMA.SetTamPkt(sizePkt);

  // Lanzamos la simulación
  Simulator::Run ();
  Simulator::Destroy ();

  retardo = observadorCSMA.GetMediaTiempos()/1e3;
  porcentaje = observadorCSMA.GetPorcentajePktsPerdidos();

  NS_LOG_INFO("Retardo de transmisión medio: " <<  retardo << "ms");
  NS_LOG_INFO("Porcentaje de paquetes perdidos: " << porcentaje << "%");

}



//Simulación simple para el servicio VoIP usando Aloha
void
simulacionAloha (uint32_t nClientes,
                Time ton,
                Time toff,
                uint32_t sizePkt,
                DataRate dataRate,
                double prob_error_pkt,
                double& retardo,
                double& porcentaje)
{
NS_LOG_FUNCTION(nClientes << ton << toff << sizePkt << dataRate << prob_error_pkt << retardo << porcentaje);

  // Creamos el modelo de error y le asociamos los parametros
  Ptr<RateErrorModel> modelo_error = CreateObject<RateErrorModel> ();

  modelo_error->SetRate(prob_error_pkt);
  modelo_error->SetUnit(RateErrorModel::ERROR_UNIT_BIT);

  // Nodos que pertenecen a la red de área local
  // Como primer nodo añadimos el sumidero.
  NodeContainer alohaNodes;
  alohaNodes.Create (nClientes);

  NetDeviceContainer alohaDevices;

  AdhocAlohaNoackIdealPhyHelper aloha;

  SpectrumChannelHelper channelHelper = SpectrumChannelHelper::Default ();
  Ptr<SpectrumChannel> channel = channelHelper.Create ();

  WifiSpectrumValue5MhzFactory sf;
  double txPower = 0.1; // Watts
  uint32_t channelNumber = 1;
  Ptr<SpectrumValue> txPsd = sf.CreateTxPowerSpectralDensity (txPower, channelNumber);

  double k = 1.381e-23; //Boltzmann's constant
  double T = 290; // temperature in Kelvin
  double noisePsdValue = k * T; // watts per hertz
  Ptr<SpectrumValue> noisePsd = sf.CreateConstant (noisePsdValue);

  AdhocAlohaNoackIdealPhyHelper deviceHelper;
  deviceHelper.SetChannel (channel);
  deviceHelper.SetTxPowerSpectralDensity (txPsd);
  deviceHelper.SetNoisePowerSpectralDensity (noisePsd);
  deviceHelper.SetPhyAttribute ("Rate", DataRateValue (DataRate ("10Mbps")));

  alohaDevices = deviceHelper.Install (alohaNodes);

  // Instalamos la pila TCP/IP en todos los nodos
  InternetStackHelper stack;
  stack.Install (alohaNodes);

  // Asignamos direcciones a cada una de las interfaces
  Ipv4AddressHelper address;
  Ipv4InterfaceContainer alohaInterfaces;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  alohaInterfaces = address.Assign (alohaDevices);

  // Calculamos las rutas del escenario.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  // Establecemos un sumidero para los paquetes en el puerto 9 del nodo
  //     aislado del enlace punto a punto
  uint16_t port = 9;
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  ApplicationContainer app = sink.Install (alohaNodes.Get (0));

  // Instalamos un cliente OnOff en uno de los equipos de la red de área local
  OnOffHelper VoIP ("ns3::UdpSocketFactory",
                        Address (InetSocketAddress (alohaInterfaces.GetAddress (0), port)));

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

  for (uint32_t i = 1; i < nClientes; i++)
    clientes.Add(alohaNodes.Get(i));

  //Instalamos la aplicación On/Off en todos y cada uno de los nodos de la red de área local.
  ApplicationContainer clientApps = VoIP.Install (clientes);
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (20.0));

  //Objeto observador para obtener los resultados de las simulaciones.
  ObservadorCSMA observadorCSMA;

  for (uint32_t i = 0; i < clientApps.GetN(); i++)
    //Conectamos las trazas al observador para todos los clientes.
    clientApps.Get(i)->GetObject<OnOffApplication>()->TraceConnectWithoutContext ("Tx", MakeCallback(&ObservadorCSMA::PktGenerado, &observadorCSMA));

  //Conectamos la traza del sumidero al observador.
  app.Get(0)->GetObject<PacketSink>()->TraceConnectWithoutContext ("Rx", MakeCallback(&ObservadorCSMA::PktRecibido, &observadorCSMA));

  observadorCSMA.SetTamPkt(sizePkt);

  // Lanzamos la simulación
  Simulator::Run ();
  Simulator::Destroy ();

  retardo = observadorCSMA.GetMediaTiempos()/1e3;
  porcentaje = observadorCSMA.GetPorcentajePktsPerdidos();

  NS_LOG_INFO("Retardo de transmisión medio: " <<  retardo << "ms");
  NS_LOG_INFO("Porcentaje de paquetes perdidos: " << porcentaje << "%");

}
