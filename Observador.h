#include <ns3/packet.h>
#include <ns3/average.h>


using namespace ns3;


class Observador
{
public:
  Observador  ();
  void      EnvioDescartado (Ptr<const Packet> paquete);
  void      PktGenerado (Ptr<const Packet> paquete);
  void      PktRecibido (Ptr<const Packet> paquete, const Address &);
  double    GetMediaTiempos ();
  double    GetPorcentajePktsPerdidos ();
  double    GetTasaMedia();
  void      SetTamPkt (uint32_t tamPkt);

private:
  int m_numPeticionesTx;
  int m_numPktsRetrasados;
  uint32_t m_tamPkt;
  Time m_retardo;         //Tiempo transcurrido hasta la recepción del pkt.

  Average<int64_t> m_acTiempos; //En micro segundos
  
  std::map<uint64_t, Time> m_tiemposIniciales;    //Estructura tipo map para almacenar los 
                                                  //tiempos de envio de cada paquete.
  std::map<uint64_t, Time>::iterator m_iterador;    //Objeto para manejar la estructura anterior.

};
