/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/gnuplot.h"
#include "Observador.h"

#define SIMULACIONES 10
#define T_STUDENT 2.2622
#define DNI_0 1
#define DNI_1 3

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0

/*
    Parámetros de la red de área local
      -número de equipos en el bus
      -capacidad del bus
      -retardo de propagación
    Parámetros del enlace punto a punto
      -capacidad del enlace
      -retardo de propagación
*/

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Practica06");

void simulacion ( double ton,double toff,
                  uint32_t sizePkt,
                  uint32_t nCsma,
                  uint32_t tamCola,
                  std::string dataRate,
                  double& porcentaje,
                  double& retardo)
{
  nCsma = nCsma == 0 ? 1 : nCsma;

  // Nodos que pertenecen al enlace punto a punto
  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  // Nodos que pertenecen a la red de área local
  // Como primer nodo añadimos el encaminador que proporciona acceso
  //      al enlace punto a punto.
  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);

  // Instalamos el dispositivo en los nodos punto a punto
  PointToPointHelper pointToPoint;
  NetDeviceContainer p2pDevices;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("2Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
  p2pDevices = pointToPoint.Install (p2pNodes);

  // Instalamos el dispositivo de red en los nodos de la LAN
  CsmaHelper csma;
  NetDeviceContainer csmaDevices;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));
  csmaDevices = csma.Install (csmaNodes);

  // Instalamos la pila TCP/IP en todos los nodos
  InternetStackHelper stack;
  stack.Install (p2pNodes.Get (0));
  stack.Install (csmaNodes);

  // Asignamos direcciones a cada una de las interfaces
  // Utilizamos dos rangos de direcciones diferentes:
  //    - un rango para los dos nodos del enlace
  //      punto a punto
  //    - un rango para los nodos de la red de área local.
  Ipv4AddressHelper address;
  Ipv4InterfaceContainer p2pInterfaces;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  p2pInterfaces = address.Assign (p2pDevices);
  Ipv4InterfaceContainer csmaInterfaces;
  address.SetBase ("10.1.2.0", "255.255.255.0");
  csmaInterfaces = address.Assign (csmaDevices);

  // Calculamos las rutas del escenario. Con este comando, los
  //     nodos de la red de área local definen que para acceder
  //     al nodo del otro extremo del enlace punto a punto deben
  //     utilizar el primer nodo como ruta por defecto.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  // Establecemos un sumidero para los paquetes en el puerto 9 del nodo
  //     aislado del enlace punto a punto
  uint16_t port = 9;
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  ApplicationContainer app = sink.Install (p2pNodes.Get (0));

  // Instalamos un cliente OnOff en uno de los equipos de la red de área local
  OnOffHelper clientes ("ns3::UdpSocketFactory",
                        Address (InetSocketAddress (p2pInterfaces.GetAddress (0), port)));

    // Creamos las variables aleatorias para el tiempo de actividad y el tiempo de silencio
    Ptr<ExponentialRandomVariable> tonExp = CreateObject<ExponentialRandomVariable> ();
    Ptr<ExponentialRandomVariable> toffExp = CreateObject<ExponentialRandomVariable> ();
    // Asignamos los valores medios de las variables aleatorias
    tonExp->SetAttribute("Mean", DoubleValue(ton));
    toffExp->SetAttribute("Mean", DoubleValue(toff));
    // Asignamos los atributos del cliente On/Off con el fin de sustituir los valores por defecto
    clientes.SetAttribute("OnTime", PointerValue(tonExp));
    clientes.SetAttribute("OffTime", PointerValue(toffExp));
    clientes.SetAttribute("PacketSize", UintegerValue(sizePkt));
    clientes.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
    //Instalamos la aplicación en todos los nodos de la red Local
    ApplicationContainer clientApps = clientes.Install (csmaNodes);

  // Creación de los observadores
  Observador * observador = new Observador(clientApps, app.Get(0));

  // Con las siguientes lineas modificamos el tamaño de la cola para provocar la pérdida de paquetes
  Ptr<Queue> cola = p2pDevices.Get(1)->GetObject<PointToPointNetDevice>()->GetQueue();
  cola->SetAttribute("MaxPackets", UintegerValue(tamCola));
  // Esta traza nos permite contabilizar los paquetes perdidos
  cola->TraceConnectWithoutContext("Drop", MakeCallback(&Observador::PaqueteRechazado, observador));

  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  // Lanzamos la simulación
  Simulator::Run ();
  Simulator::Destroy ();

  porcentaje = (double)(100 - (double)observador->GetPorcentajePaquetesRechazados());
  retardo = observador->GetRetardo()/1000;

  observador->GetListaVacia();
}

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

  double ton            = 0.150;  //Media del tiempo de actividad por defecto
  double ton_for        = 0.0;
  double toff           = 0.650;  //Media del tiempo de inactividad por defecto
  uint32_t sizePkt      = 40 - DNI_1/2; //Tamaño del paquete
  uint32_t nCsma        = 150 + 5*DNI_0;
  uint32_t tamCola      = 1;  //Tamaño máximo de la cola
  std::string dataRate  = "64kb/s";
  double porcentaje     = 0.0;
  double retardo        = 0.0;
  Average<double> acu_porcentaje;
  Average<double> acu_retardo;

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("ton", "Tiempo de actividad", ton);
  cmd.AddValue ("toff", "Tiempo de silencio", toff);
  cmd.AddValue ("sizePkt", "Tamaño del paquete", sizePkt);
  cmd.AddValue ("dataRate", "Tasa de bit en el estado activo", dataRate);
  cmd.Parse (argc,argv);

  Gnuplot plotPorcentaje;
  plotPorcentaje.SetTitle("Porcentaje de paquetes correctamente transmitidos en función del tiempo de permanencia en On");
  plotPorcentaje.SetLegend( "Tiempo medio de permanencia en estado On (s)", "Porcentaje de Paquetes Tx correctamente (%)");


  Gnuplot plotRetardo;
  plotRetardo.SetTitle("Retardo medio en función del tiempo de permanencia en On");
  plotRetardo.SetLegend( "Tiempo medio de permanencia en estado ON (s)", "Retardo medio (ms)");

for( uint16_t tamCola_for = tamCola; tamCola_for < (tamCola + 5); tamCola_for++)
{
  std::stringstream sstm;
  sstm << "Tamaño de cola: " << tamCola_for;
  std::string titleTam = sstm.str();

  NS_LOG_DEBUG("Tamaño de la cola " << tamCola_for);


// Preparamos las distintas curvas
  Gnuplot2dDataset datosPorcentaje;
  datosPorcentaje.SetStyle(Gnuplot2dDataset::LINES_POINTS);
  datosPorcentaje.SetErrorBars(Gnuplot2dDataset::Y);
  datosPorcentaje.SetTitle(titleTam);


  Gnuplot2dDataset datosRetardo;
  datosRetardo.SetStyle(Gnuplot2dDataset::LINES_POINTS);
  datosRetardo.SetErrorBars(Gnuplot2dDataset::Y);
  datosRetardo.SetTitle(titleTam);
  for( ton_for = ton; ton_for <= (ton + 0.20); ton_for += 0.050)
  {
    NS_LOG_DEBUG("TOn " << ton_for);
    for(uint32_t numSimulaciones=0; numSimulaciones < SIMULACIONES; numSimulaciones++)
    {
      NS_LOG_DEBUG("Número de simulación " << numSimulaciones);

      simulacion ( ton_for, toff, sizePkt, nCsma, tamCola_for, dataRate, porcentaje, retardo);
      acu_porcentaje.Update(porcentaje);
      acu_retardo.Update(retardo);
    }
    if(acu_porcentaje.Count() > 0)
      datosPorcentaje.Add( ton_for, acu_porcentaje.Mean(), CalculaZ(acu_porcentaje.Var()));
      acu_porcentaje.Reset();
    if(acu_retardo.Count() > 0)
      datosRetardo.Add( ton_for, acu_retardo.Mean(), CalculaZ(acu_retardo.Var()));
      acu_retardo.Reset();
  }
  plotPorcentaje.AddDataset(datosPorcentaje);
  plotRetardo.AddDataset(datosRetardo);
}


  std::ofstream fichero1("practica06-1.plt");
  plotPorcentaje.GenerateOutput(fichero1);
  fichero1 << "pause -1" << std::endl;
  fichero1.close();

  std::ofstream fichero2("practica06-2.plt");
  plotRetardo.GenerateOutput(fichero2);
  fichero2 << "pause -1" << std::endl;
  fichero2.close();

  NS_LOG_INFO("La media del retardo es: " << retardo << " ms");
  NS_LOG_INFO("El porcentaje de paquetes correctos es: " << porcentaje );

  return 0;
}
