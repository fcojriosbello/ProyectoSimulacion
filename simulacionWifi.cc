#include "ns3/object.h"
#include "ns3/global-value.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include <ns3/error-model.h>
#include "ns3/point-to-point-module.h"

#include "simulacionWifi.h"
#include "Observador.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("simulacionWifi");

void
simulacionWifi (uint32_t nWifi, uint32_t nodosSede2, Time ton, Time toff, uint32_t sizePkt, DataRate dataRate, 
    std::string wifi_dataRate, double p2p_prob_error_bit, std::string p2p_dataRate, std::string p2p_delay, 
    double& retardo, double& porcentaje, double& jitter)
{
  NS_LOG_FUNCTION (nWifi << nodosSede2 << ton << toff << sizePkt << dataRate << wifi_dataRate << p2p_prob_error_bit
         << p2p_dataRate << p2p_delay << retardo << porcentaje << jitter);
  
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
  wifiNodes2.Create(nodosSede2+2);


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
      StringValue(wifi_dataRate), "ControlMode", StringValue(wifi_dataRate));

  // Añadimos el netDevice Wifi a cada nodo de la sede 1
  NetDeviceContainer wifiDevices1 = wifi.Install(wifiPhy1, wifiMac, wifiNodes1);

  // Añadimos el netDevice Wifi a cada nodo de la sede 2
  NetDeviceContainer wifiDevices2 = wifi.Install(wifiPhy2, wifiMac, wifiNodes2);

  // Creamos el modelo de error para el enlace p2p
  Ptr<RateErrorModel> modelo_error = CreateObject<RateErrorModel> ();
  modelo_error->SetRate(p2p_prob_error_bit);
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
  ApplicationContainer appSink2 = sink.Install (wifiNodes2.Get (nodosSede2 + 1)); 

  // Instalamos un cliente OnOff en los equipos de la sede 1.
  OnOffHelper VoIP1 ("ns3::UdpSocketFactory",
          Address (InetSocketAddress (interfacesWifi2.GetAddress (nodosSede2 + 1), port)));

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

  for (uint32_t i = 1; i <= nodosSede2; i++)
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
  jitter = observador.GetJitter()/1e3;


  NS_LOG_INFO("Retardo de transmisión medio: " <<  retardo << "ms");
  NS_LOG_INFO("Porcentaje de paquetes perdidos: " << porcentaje << "%");
  NS_LOG_INFO("Jitter medio: " << jitter << "ms");

}