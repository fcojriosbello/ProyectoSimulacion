#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/gnuplot.h"
#include "ObservadorWifi.h"

using namespace ns3;

#define T_STUDENT 2.2622
#define SIMULACIONES 1
#define MAXWIFI 18
#define NWIFI 100
#define PUNTOSDIST 25
#define OFFSET 5


NS_LOG_COMPONENT_DEFINE("wifi");

double
CalculaZ(double varianza)
{
    double z = T_STUDENT*sqrt(varianza/(SIMULACIONES));
    return z;
}

int main(int argc, char *argv[])
{
  GlobalValue::Bind("ChecksumEnabled", BooleanValue(true));
  Time::SetResolution(Time::US);

//  uint32_t duracion = 1;
  Time ton =Time("0.150s");
  Time toff =  Time("0.650s");
  uint32_t sizePkt = 40;
  DataRate dataRate = DataRate("64kbps");
//  Average <double>acumulador_rendimiento;
  Average <double>acu_porcentaje;

  std::string modulaciones = "OfdmRate9Mbps";

  //configuracion del plot de la grafica del porcentaje medio de perdidas
  Gnuplot plotPorcentaje;
	plotPorcentaje.SetTitle("Porcentaje de paquetes erróneos");
	plotPorcentaje.SetLegend( "Número de nodos", "Porcentaje de Paquetes erróneos (%)");

  Gnuplot2dDataset datosPorcentaje;
  datosPorcentaje.SetStyle(Gnuplot2dDataset::LINES_POINTS);
  datosPorcentaje.SetErrorBars(Gnuplot2dDataset::Y);
  datosPorcentaje.SetTitle("Wifi");
      for (uint32_t num_nodos =10; num_nodos < NWIFI+1; num_nodos+=5)
        {
          acu_porcentaje.Reset();
          for (uint32_t n_sim = 0; n_sim < SIMULACIONES; n_sim++)
            {
              NodeContainer nodos;
              nodos.Create(num_nodos);

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

              // Instalamos la pila TCP/IP en los nodos
              InternetStackHelper stack;
              stack.Install(nodos);

              // Asignamos direcciones a los dispositivos
              Ipv4AddressHelper address;
              address.SetBase("192.168.1.0", "255.255.255.0");
              Ipv4InterfaceContainer interfacesWiFi =
                address.Assign(dispositivosWifi);

              // Calculamos las rutas
              Ipv4GlobalRoutingHelper::PopulateRoutingTables();

              // Creamos el modelo de movilidad
              MobilityHelper ayudanteMovilidad;
              Ptr < ListPositionAllocator > localizaciones =
                CreateObject < ListPositionAllocator > ();

              localizaciones->Add(Vector(0, 0, 0));
              uint32_t dist = 2 + OFFSET;
              localizaciones->Add(Vector(dist, 0, 0));
              ayudanteMovilidad.SetPositionAllocator(localizaciones);
              ayudanteMovilidad.SetMobilityModel
                ("ns3::ConstantPositionMobilityModel");

              ayudanteMovilidad.Install(nodos);

              uint16_t port = 9;

              PacketSinkHelper sink ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
              ApplicationContainer serverApp = sink.Install (nodos.Get (0));

              serverApp.Start(Seconds(1.0));
              serverApp.Stop(Seconds(23.0)); //dejamos que acabe 1 segundo más tarde que e
              OnOffHelper VoIP ("ns3::UdpSocketFactory", Address (InetSocketAddress (interfacesWiFi.GetAddress (0), port)));
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

              for (uint32_t i = 1; i < num_nodos; i++)
              clientes.Add(nodos.Get(i));

              ApplicationContainer clientApps = VoIP.Install (clientes);
              clientApps.Start(Seconds(2.0));
              clientApps.Stop(Seconds(20.0));

              ObservadorWifi observadorWifi;

              for (uint32_t i = 0; i < clientApps.GetN(); i++)
              clientApps.Get(i)->GetObject<OnOffApplication>()->TraceConnectWithoutContext ("Tx", MakeCallback(&ObservadorWifi::Tx, &observadorWifi));

              serverApp.Get(0)->GetObject<PacketSink>()->TraceConnectWithoutContext ("Rx", MakeCallback(&ObservadorWifi::Rx, &observadorWifi));
              for(uint32_t i = 1; i < num_nodos; i++)
              dispositivosWifi.Get(i)->GetObject<WifiNetDevice>()->GetPhy()->TraceConnectWithoutContext("PhyTxBegin", MakeCallback(&ObservadorWifi::PhyTx, &observadorWifi));
              dispositivosWifi.Get(0)->GetObject<WifiNetDevice>()->GetPhy()->TraceConnectWithoutContext("PhyRxBegin", MakeCallback(&ObservadorWifi::PhyTx, &observadorWifi));

              // Lanzamos la simulación
              NS_LOG_INFO("Aquí3");

              Simulator::Run();
              // Eliminamos el simulador
              Simulator::Destroy();

              NS_LOG_INFO("Simulación: " << n_sim << " | Nodos: " << num_nodos << " | " << observadorWifi.GetPorcentaje() << "%");

              acu_porcentaje.Update(observadorWifi.GetPorcentaje());
            }

          if(acu_porcentaje.Count() > 0)
          datosPorcentaje.Add(num_nodos, acu_porcentaje.Mean(), CalculaZ(acu_porcentaje.Var()));
      }

      plotPorcentaje.AddDataset(datosPorcentaje);

  std::ofstream file1("wifi-1.plt");
  plotPorcentaje.GenerateOutput(file1);
  file1 << "pause -1" << std::endl;
  file1.close();

}
