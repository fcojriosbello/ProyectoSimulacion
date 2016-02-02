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
  double    GetJitter ();

private:
  int m_numPeticionesTx;
  double m_jitter;

  Time m_retardo;         //Tiempo transcurrido hasta la recepción del pkt.
  Time m_retardoPrevio;   //Retardo calculado en el paquete previo.

  Average<double> m_acTiempos; //Acumulador de retardos en micro segundos.
  Average<double> m_acJitter;  //Acumulador de jitter en micro segundos.
  
  std::map<uint64_t, Time> m_tiemposIniciales;    //Estructura tipo map para almacenar los 
                                                  //tiempos de envio de cada paquete.
  std::map<uint64_t, Time>::iterator m_iterador;    //Objeto para manejar la estructura anterior.

};
