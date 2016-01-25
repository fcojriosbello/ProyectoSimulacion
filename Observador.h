/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/packet.h>
#include <ns3/average.h>

using namespace ns3;


class Observador
{
public:
  Observador  ();
  void 		EnvioRetrasado (Ptr<const Packet> paquete);
  void 		EnvioDescartado (Ptr<const Packet> paquete);
  void 		EnvioTerminado (Ptr<const Packet> paquete);
  void 		OrdenEnvio (Ptr<const Packet> paquete);
  void		OrdenPktDisponible (Ptr<const Packet> paquete);
  double 	GetMediaIntentos ();
  double	GetMediaTiempos ();
  double 	GetPorcentajePktsPerdidos ();
  void    Reset ();
  void 		SetNodo (int nodo);
  int 		GetNodo ();
private:
	int m_numIntentos;
	int m_nodo;
	int m_numPeticionesTx;
	int m_numPktsPerdidos;
	int64_t m_tiempoInicial; //En nano segundos
	Average<int> m_acIntentos;
	Average<int64_t> m_acTiempos; //En nano segundos

};
