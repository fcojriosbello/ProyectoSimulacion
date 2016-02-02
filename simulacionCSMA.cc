#include "ns3/object.h"
#include "ns3/global-value.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include <ns3/error-model.h>
#include "ns3/point-to-point-module.h"
#include <ns3/data-rate.h>

#include "simulacionCSMA.h"
#include "Observador.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("simulacionCSMA");

//Simulación simple para el servicio VoIP usando CSMA
void
simulacionCSMA (uint32_t nCsma,
                uint32_t nodosSede2,
                Time ton,
                Time toff,
                uint32_t sizePkt,
                DataRate dataRate,
                double csma_prob_error_bit,
                std::string csma_dataRate,
                std::string csma_delay,                
                double p2p_prob_error_bit,
                std::string p2p_dataRate,
                std::string p2p_delay,
                double& retardo,
                double& porcentaje,
                double& jitter)
{
NS_LOG_FUNCTION(nCsma << nodosSede2 << ton << toff << sizePkt << dataRate << csma_prob_error_bit
  << p2p_prob_error_bit << p2p_dataRate << p2p_delay << retardo << porcentaje << jitter);

  // Creamos los modelos de errores y le asociamos los parámetros
  Ptr<RateErrorModel> modelo_error1 = CreateObject<RateErrorModel> ();
  Ptr<RateErrorModel> modelo_error2 = CreateObject<RateErrorModel> ();
  Ptr<RateErrorModel> modelo_error3 = CreateObject<RateErrorModel> ();

  modelo_error1->SetRate(csma_prob_error_bit);
  modelo_error1->SetUnit(RateErrorModel::ERROR_UNIT_BIT);

  modelo_error2->SetRate(csma_prob_error_bit);
  modelo_error2->SetUnit(RateErrorModel::ERROR_UNIT_BIT);

  modelo_error3->SetRate(p2p_prob_error_bit);
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
  csmaNodes2.Create (nodosSede2 + 2);

  // Nodos que pertenecen al enlace punto a punto.
  // Nodo cero de cada red csma.
  NodeContainer p2pNodes;
  p2pNodes.Add (csmaNodes1.Get(0));
  p2pNodes.Add (csmaNodes2.Get(0));

  // Instalamos el dispositivo de red en los nodos de la LAN
  CsmaHelper csma;
  NetDeviceContainer csmaDevices1;
  NetDeviceContainer csmaDevices2;
  csma.SetChannelAttribute ("DataRate", StringValue (csma_dataRate));
  csma.SetChannelAttribute ("Delay", TimeValue (Time (csma_delay)));
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
  for (uint32_t k = 0; k < nodosSede2+2; k++ )
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
  ApplicationContainer appSink2 = sink.Install (csmaNodes2.Get (nodosSede2 + 1)); 

  // Instalamos un cliente OnOff en los equipos de la sede 1.
  OnOffHelper VoIP1 ("ns3::UdpSocketFactory",
          Address (InetSocketAddress (csmaInterfaces2.GetAddress (nodosSede2 + 1), port)));

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

  for (uint32_t i = 1; i <= nodosSede2; i++)
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
    csmaDevices1.Get(j)->GetObject<CsmaNetDevice>()->SetBackoffParams (Time ("1us"), 10, 1000, 10, NUM_REINTENTOS);
  }

    for (uint32_t j = 1; j <= nodosSede2; j++)
  {
    //Aprovechamos para cambiar el numero maximo de reintentos de tx.
    csmaDevices2.Get(j)->GetObject<CsmaNetDevice>()->SetBackoffParams (Time ("1us"), 10, 1000, 10, NUM_REINTENTOS);
  }

  // Lanzamos la simulación
  Simulator::Run ();
  Simulator::Destroy ();

  retardo = observador.GetMediaTiempos()/1e3;
  porcentaje = observador.GetPorcentajePktsPerdidos();
  jitter = observador.GetJitter()/1e3;


  NS_LOG_INFO("Retardo de transmisión medio: " <<  retardo << "ms");
  NS_LOG_INFO("Porcentaje de paquetes perdidos: " << porcentaje << "%");
  NS_LOG_INFO("Jitter medio: " << jitter << "ms");

}