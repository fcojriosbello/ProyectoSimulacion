/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/packet.h>
#include <ns3/average.h>
#include <ns3/ethernet-header.h>
#include "ns3/double.h"
#include "ns3/applications-module.h"
#include <ns3/header.h>
#include "ns3/nstime.h"
#include <iterator>
#include "ns3/csma-net-device.h"
#include <iostream>

using namespace ns3;

class Observador
{
public:
  Observador  (ApplicationContainer clientes, Ptr<Application> sumidero);
  void TiempoEnvio (Ptr<const Packet> paquete);
  void TiempoRecepcion (Ptr<const Packet> paquete, const Address &);
  void PaqueteRechazado(Ptr<const Packet> paquete);
  double GetRetardo();
  double GetListaVacia();
  double GetPorcentajePaquetesRechazados();
  double GetPaquetesCorrectos();

private:

 BooleanValue listaVacia;

 //Ptr<Application> aplicacion_OnOff;
 Ptr<Application> aplicacion_sumidero;
 ApplicationContainer m_clientes;

 Average<double> acumulador_intervalo;

 uint32_t totalPaquetes;
 uint32_t paqueteRechazado;

 std::map<uint64_t, Time> map_lista;
};
