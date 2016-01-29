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
  NS_LOG_INFO("Paquete transmitido a nivel fisico " << enviados);
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
  NS_LOG_ERROR("Paquete recibido a nivel fisico"  << recibidos);
  recibidos++;
}

double ObservadorWifi::GetPorcentaje()
{
  NS_LOG_ERROR("Recibidos " << recibidos << "Enviados " << enviados);
  if (enviados != 0)
    {
      return (double) (1 - (double) recibidos / (double) enviados) * 100;
    }
  else
    return 0;
}
