/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/core-module.h>
#include <ns3/ethernet-header.h>
#include "Observador.h"


NS_LOG_COMPONENT_DEFINE ("Observador");


Observador::Observador ()
{
  NS_LOG_FUNCTION_NOARGS ();

  m_nodo = 0;
  m_tamPkt = 0;
  m_numPeticionesTx = 0;
  m_numPktsPerdidos = 0;
  m_retardo = Time(0);
  m_acTiempos.Reset();
}


//Funcion que maneja la traza Tx
void
Observador::PktGenerado (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION (paquete);

  NS_LOG_DEBUG ("Se ha generado un nuevo paquete y va a ser enviado.");

  //Almacenamos el tiempo inicial.
  m_tiemposIniciales[paquete->GetUid()] = Simulator::Now();
  m_numPeticionesTx++;
}


//Funcion que maneja la traza Rx
void
Observador::PktRecibido (Ptr<const Packet> paquete, const Address &)
{
  NS_LOG_FUNCTION (paquete);

  //Buscamos en la estructura de timepos iniciales el correspondiente a este paquete.
  m_iterador = m_tiemposIniciales.find (paquete->GetUid());

  if (m_iterador != m_tiemposIniciales.end())
  {
    //Si encontramos el paquete en la estructura...

    //Calculamos el tiempo transcurrido.
    m_retardo = Simulator::Now() - m_tiemposIniciales[paquete->GetUid()];

    //Actualizamos el acumulador.
    m_acTiempos.Update(m_retardo.GetDouble());
    NS_LOG_DEBUG ("Se ha recibido un paquete en el sumidero en  " << m_retardo.GetDouble()/1e3 << "ms.");

    //Eliminamos el paquete de la estructura
    m_tiemposIniciales.erase(m_iterador);
  }
  else
    NS_LOG_WARN("No se ha encontrado el paquete recibido en la estructura de tiempos iniciales."); 
}



//Devuelve el tiempo medio de ECO del nodo asociado al
//objeto Observador. Se devuelve en ns.
double
Observador::GetMediaTiempos ()
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
Observador::GetPorcentajePktsPerdidos()
{
  NS_LOG_FUNCTION_NOARGS ();

  double result = -1;

  if (m_numPeticionesTx > 0)
    result = 100*(double)m_tiemposIniciales.size() / (double)m_numPeticionesTx;

  return result;
}


DataRate
Observador::GetTasaMedia () {
  NS_LOG_FUNCTION_NOARGS ();

  DataRate result;

  //Comprobamos que se haya realizado correctamente algun envio
  if (m_acTiempos.Count() > 0)
    //El tamaño de paquete es fijo.
    //Obtenemos la tasa media a nivel de aplicación dividiendo el tamaño de la carga útil
    //entre el tiempo medio de recepción de un pkt a nivel de aplicación.
    result = DataRate((uint64_t)(m_tamPkt*8)/(uint64_t)(this->GetMediaTiempos() * 1e-6));
  else 
    result = DataRate("0bps");

  return result;
}

//Funcion para guardar en la variable m_nodo el identificador
//del nodo al que esta asociado el Observador. Esto se usara 
//en las trazas.
void
Observador::SetNodo (int nodo) 
{ 
  NS_LOG_FUNCTION(nodo);
  m_nodo = nodo; 
}


//Funcion para obtener de la variable m_nodo el identificador
//del nodo al que esta asociado el Observador. Esto se usara 
//en las trazas.
int     
Observador::GetNodo () { return m_nodo; }

//Función para indicar al objeto Observador el tamaño de la 
//carga útil de los pkts que serán enviados.
void
Observador::SetTamPkt (uint32_t tamPkt)
{
  NS_LOG_FUNCTION(tamPkt);
  m_tamPkt = tamPkt;
}