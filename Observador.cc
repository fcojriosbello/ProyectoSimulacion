/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/core-module.h>
#include <ns3/ethernet-header.h>
#include "Observador.h"


NS_LOG_COMPONENT_DEFINE ("Observador");


Observador::Observador ()
{
  NS_LOG_FUNCTION_NOARGS ();

  m_tamPkt = 0;
  m_numPeticionesTx = 0;
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



double
Observador::GetTasaMedia () {

  NS_LOG_FUNCTION_NOARGS ();

  double result;

  //Comprobamos que se haya realizado correctamente algun envio
  if (m_acTiempos.Count() > 0)
    //El tamaño de paquete es fijo.
    //Obtenemos la tasa media a nivel de aplicación dividiendo el tamaño de la carga útil
    //total enviada durante la simulación entre la duración total de la simulación.
    //result = (((double)(m_numPeticionesTx-(int)m_tiemposIniciales.size())/(double)m_numNodos)*(m_tamPkt*8))/(m_tiempoSimulado.GetDouble() * 1e-6);
    result = (double)(m_tamPkt*8)/(double)(this->GetMediaTiempos() * 1e-6);
  else 
    result = 0.0;

  return result;
}


//Función para indicar al objeto Observador el tamaño de la 
//carga útil de los pkts que serán enviados.
void
Observador::SetTamPkt (uint32_t tamPkt)
{
  NS_LOG_FUNCTION(tamPkt);
  m_tamPkt = tamPkt;
}

/*void
Observador::SetTiempoSimulado (Time tiempoSimulado)
{
  NS_LOG_FUNCTION(tiempoSimulado);
  m_tiempoSimulado = tiempoSimulado;
}

void
Observador::SetNumNodos (int numNodos)
{
  NS_LOG_FUNCTION(numNodos);
  m_numNodos = numNodos;
}*/