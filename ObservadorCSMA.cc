/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/core-module.h>
#include <ns3/ethernet-header.h>
#include "ObservadorCSMA.h"


NS_LOG_COMPONENT_DEFINE ("ObservadorCSMA");


ObservadorCSMA::ObservadorCSMA ()
{
  NS_LOG_FUNCTION_NOARGS ();

  m_nodo = 0;
  m_pktRetrasado = false;
  m_numPktsRetrasados = 0;
  Reset();
}

//Funcion que maneja la traza MacTxBackoff
void
ObservadorCSMA::EnvioRetrasado (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION (paquete);


  // Obtengo una copia del paquete
  Ptr<Packet> copia = paquete->Copy();

  EthernetHeader header;
  // Quitamos la cabecera del paquete y la guardamos en header.
  copia->RemoveHeader (header);

  // Comprobamos que sea un paquete ip (contiene el paquete eco).
  if (header.GetLengthType() == 0x0800)
  {
    NS_LOG_DEBUG ("NODO " << m_nodo <<"-> Se ha retrasado el envio de un paquete debido al proceso de backoff.");
    m_numIntentos ++;
    m_pktRetrasado = true;
    NS_LOG_DEBUG ("Numero de intentos: " << m_numIntentos);
  }
}

//Funcion que maneja la traza PhyTxDrop
void
ObservadorCSMA::EnvioDescartado (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION (paquete);

  // Obtengo una copia del paquete
  Ptr<Packet> copia = paquete->Copy();

  EthernetHeader header;
  // Quitamos la cabecera del paquete y la guardamos en header.
  copia->RemoveHeader (header);

  // Comprobamos que sea un paquete ip (contiene el paquete eco).
  if (header.GetLengthType() == 0x0800)
  {
    NS_LOG_DEBUG("NODO " << m_nodo <<"-> Se ha descartado el envio de un paquete porque ha llegado al maximo numero de intentos.");
    m_numIntentos = 0;
    m_pktRetrasado = false;
    //Aumentamos el numero de paquetes perdidos
    m_numPktsPerdidos++;
  }
}

//Funcion que maneja la traza PhyTxEnd
void
ObservadorCSMA::EnvioTerminado (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION (paquete);


  // Obtengo una copia del paquete
  Ptr<Packet> copia = paquete->Copy();

  EthernetHeader header;
  // Quitamos la cabecera del paquete y la guardamos en header.
  copia->RemoveHeader (header);

  // Comprobamos que sea un paquete ip (contiene el paquete eco).
  if (header.GetLengthType() == 0x0800)
  {
    m_numIntentos ++;
    NS_LOG_DEBUG ("NODO " << m_nodo <<"-> Se ha terminado el proceso de envio de un paquete en el nodo.");
    NS_LOG_DEBUG ("Numero de intentos: " << m_numIntentos);
    m_acIntentos.Update(m_numIntentos);
    m_numIntentos = 0;

    if (m_pktRetrasado == true)
    {
      m_numPktsRetrasados++;
      m_pktRetrasado = false;
    }
  }
}

//Funcion que maneja la traza MacTx
void 
ObservadorCSMA::OrdenEnvio (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION(paquete);

  // Obtengo una copia del paquete
  Ptr<Packet> copia = paquete->Copy();

  EthernetHeader header;
  // Quitamos la cabecera del paquete y la guardamos en header.
  copia->RemoveHeader (header);

  // Comprobamos que sea un paquete ip (contiene el paquete eco).
  if (header.GetLengthType() == 0x0800)
  {
    NS_LOG_DEBUG("NODO " << m_nodo <<"-> Se ha recibido la orden de envio de un paquete ECO.");
    m_tiempoInicial = Simulator::Now().GetNanoSeconds();

    //Aumentamos el numero de peticiones de transmision
    m_numPeticionesTx++;
  }

}

//Funcion que maneja la traza MacRx
void
ObservadorCSMA::OrdenPktDisponible (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION(paquete);

  int64_t transcurrido = 0.0;

  // Obtengo una copia del paquete
  Ptr<Packet> copia = paquete->Copy();

  EthernetHeader header;
  // Quitamos la cabecera del paquete y la guardamos en header.
  copia->RemoveHeader (header);

  // Comprobamos que sea un paquete ip (contiene el paquete eco).
  if (header.GetLengthType() == 0x0800)
  {
    NS_LOG_DEBUG("NODO " << m_nodo <<"-> Dispone de un paquete ECO para su entrega al nivel de aplicaion.");

    transcurrido = Simulator::Now().GetNanoSeconds() - m_tiempoInicial;
    m_acTiempos.Update(transcurrido);

    NS_LOG_DEBUG("Tiempo de ECO: " << (double)transcurrido/1e6 << " ms");
  }
}

//Devuelve el número medio de intentos necesarios para 
//transmitir efectivamente un paquete del nodo asociado 
//al objeto ObservadorCSMA.
double 
ObservadorCSMA::GetMediaIntentos ()
{
  NS_LOG_FUNCTION_NOARGS ();

  double result = -1;

  //Comprobamos que se haya realizado correctamente algun envio
  if (m_acIntentos.Count() > 0)
    result = m_acIntentos.Mean();

  return result;

}

//Devuelve el porcentaje de pkts retrasados antes de ser enviados.
double
ObservadorCSMA::GetPorcentajePktsRetrasados ()
{
  return (double)m_numPktsRetrasados/(double)m_numPeticionesTx;
}

//Devuelve el tiempo medio de ECO del nodo asociado al
//objeto ObservadorCSMA. Se devuelve en ns.
double
ObservadorCSMA::GetMediaTiempos ()
{
  NS_LOG_FUNCTION_NOARGS ();

  double result = -1;

  //Comprobamos que se haya realizado correctamente algun envio
  if (m_acTiempos.Count() > 0)
    result = m_acTiempos.Mean();

  return result;
}

//Devuelve el porcentaje de paquetes perdidos al llegar al 
//numero maximo de intentos configurado.
double  
ObservadorCSMA::GetPorcentajePktsPerdidos()
{
  NS_LOG_FUNCTION_NOARGS ();

  double result = -1;

  if (m_numPeticionesTx > 0)
    result = 100*(double)m_numPktsPerdidos / (double)m_numPeticionesTx;

  return result;
}


//Funcion para resetear variables y objetos Average, pero
//no resetea la variable m_nodo ya que el ObservadorCSMA
//seguira asociado al mismo nodo.
void
ObservadorCSMA::Reset()
{
  m_numIntentos = 0;
  m_numPeticionesTx = 0;
  m_numPktsPerdidos = 0;
  m_tiempoInicial = 0;
  m_acIntentos.Reset();
  m_acTiempos.Reset();
}

//Funcion para guardar en la variable m_nodo el identificador
//del nodo al que esta asociado el ObservadorCSMA. Esto se usara 
//en las trazas.
void
ObservadorCSMA::SetNodo (int nodo) 
{ 
  NS_LOG_FUNCTION(nodo);
  m_nodo = nodo; 
}


//Funcion para obtener de la variable m_nodo el identificador
//del nodo al que esta asociado el ObservadorCSMA. Esto se usara 
//en las trazas.
int     
ObservadorCSMA::GetNodo () { return m_nodo; }