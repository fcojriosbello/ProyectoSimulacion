/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/packet.h>
#include <ns3/average.h>
#include <ns3/data-rate.h>


using namespace ns3;


class ObservadorCSMA
{
public:
  ObservadorCSMA  ();
  void      EnvioRetrasado (Ptr<const Packet> paquete);
  void      EnvioDescartado (Ptr<const Packet> paquete);
  void      EnvioTerminado (Ptr<const Packet> paquete);
  void      OrdenEnvio (Ptr<const Packet> paquete);
  //void		  OrdenPktDisponible (Ptr<const Packet> paquete);
  void      PktGenerado (Ptr<const Packet> paquete);
  void      PktRecibido (Ptr<const Packet> paquete, const Address &);
  double 	  GetMediaIntentos ();
  double    GetMediaTiempos ();
  //double 	  GetPorcentajePktsPerdidos ();
  double    GetPorcentajePktsRetrasados ();
  DataRate  GetTasaMedia();
  //void      Reset ();
  void      SetNodo (int nodo);
  int       GetNodo ();
  void      SetTamPkt (uint32_t tamPkt);

private:
	int m_numIntentos;
	int m_nodo;
	int m_numPeticionesTx;
	//int m_numPktsPerdidos;
  int m_numPktsRetrasados;
  uint32_t m_tamPkt;
	//int64_t m_tiempoInicial; //En nano segundos
  bool m_pktRetrasado; //Flag que indica que el pkt ha sido retrasado antes de enviarlo.
  Time m_retardo;         //Tiempo transcurrido hasta la recepción del pkt.

	Average<int> m_acIntentos;
	Average<int64_t> m_acTiempos; //En nano segundos
  
  std::map<uint64_t, Time> m_tiemposIniciales;    //Estructura tipo map para almacenar los 
                                                  //tiempos de envio de cada paquete.
  std::map<uint64_t, Time>::iterator m_iterador;    //Objeto para manejar la estructura anterior.

};
