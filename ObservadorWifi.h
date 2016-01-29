#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/object.h"
#include "ns3/global-value.h"
#include "ns3/packet.h"
#include "ns3/double.h"
#include "ns3/nstime.h"
#include "ns3/wifi-module.h"
#include "ns3/average.h"
#include <iterator>
#include <iostream>

using namespace ns3;

class ObservadorWifi
{

public:
  ObservadorWifi();
  void Tx(Ptr < const Packet > pkt);
  void Rx(Ptr < const Packet > pkt, const Address &);
  void PhyRx(Ptr < const Packet > pkt);
  void PhyTx(Ptr < const Packet > pkt);
  double GetPorcentaje();
  double GetRendimiento();

private:
  Ptr <Application> bulk;
  Ptr <Application> sink;
  Ptr <WifiPhy> cliente;
  Ptr <WifiPhy> servidor;

  Time tiempo_primer_paquete_enviado;
  Time tiempo_ultimo_paquete_recibido;

  long int recibidos;
  long int enviados;
  long int bytes_enviados;
  long int bytes_recibidos;

  bool primerPaquete;

  Ptr <NetDevice> cliente_device;
  Ptr <NetDevice> servidor_device;

};
