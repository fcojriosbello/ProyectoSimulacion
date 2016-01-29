#include "ObservadorWifi.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("ObservadorWifi");

ObservadorWifi::ObservadorWifi()
{
  NS_LOG_FUNCTION("Constructor del Observador");

  bytes_enviados = 0;
  bytes_recibidos = 0;
  enviados = 0;
  recibidos = 0;
}

void ObservadorWifi::Tx(Ptr <const Packet> pkt)
{
  NS_LOG_INFO("Paquete transmitido a nivel de aplicacion");

  bytes_enviados += pkt->GetSize();
  if (primerPaquete == false)
    {
      primerPaquete = true;
      tiempo_primer_paquete_enviado = Simulator::Now();
      NS_LOG_DEBUG("Se ha fijado el tiempo del primer paquete transmitido");
    }
}

void ObservadorWifi::PhyTx(Ptr <const Packet> pkt)
{
  NS_LOG_INFO("Paquete transmitido a nivel fisico");
  enviados++;

}
void ObservadorWifi::Rx(Ptr <const Packet> pkt, const Address &)
{
  NS_LOG_INFO("Paquete recibido a nivel de aplicacion");
  bytes_recibidos += pkt->GetSize();
  tiempo_ultimo_paquete_recibido = Simulator::Now();

}

void ObservadorWifi::PhyRx(Ptr <const Packet> pkt)
{
  NS_LOG_INFO("Paquete recibido a nivel fisico");
  recibidos++;
}

double ObservadorWifi::GetPorcentaje()
{
  if (enviados != 0 && this->GetRendimiento() != 0)
    {
      return (double) (1 - (double) recibidos / (double) enviados) * 100;
    }
  else
    return 0;
}

double ObservadorWifi::GetRendimiento()
{
  Time tiempo_transcurrido = tiempo_ultimo_paquete_recibido -
    tiempo_primer_paquete_enviado;
  if (tiempo_transcurrido != 0)
    {
      return (double) bytes_recibidos /
        (double) tiempo_transcurrido.GetSeconds();
    }
  else
    return 0;
}
