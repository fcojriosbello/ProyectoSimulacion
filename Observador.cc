/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/core-module.h>
#include "ns3/point-to-point-module.h"
#include "ns3/onoff-application.h"
#include "ns3/packet-sink.h"
#include <ns3/average.h>
#include "Observador.h"

NS_LOG_COMPONENT_DEFINE ("Observador");

Observador::Observador (ApplicationContainer clientes, Ptr<Application> sumidero)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_FUNCTION ("En-----> Constructor de Observador");

 // Inicializamos las variables y reseteamos los acumuladores
 totalPaquetes      = 0;
 paqueteRechazado   = 0;
 listaVacia         = false;
 m_clientes         = clientes;
 aplicacion_sumidero = sumidero->GetObject<PacketSink>();
 acumulador_intervalo.Reset();

 /* Obtenemos un iterador que nos permitirá recorrer las distintas aplicaciones
    para subscribirlas a la traza. Con este iterador podemos obtener las aplicaciones On/Off con el método
    GetObject */
 ApplicationContainer::Iterator i;
 for (i = m_clientes.Begin(); i != m_clientes.End(); ++i)
  {
    Ptr<Application> aplicacion = *i;
    // Obtenemos las aplicaciones on/off
    Ptr<OnOffApplication> aplicacion_onoff = aplicacion->GetObject<OnOffApplication>();
    // Las subscribimos a la traza Tx
    aplicacion_onoff->TraceConnectWithoutContext("Tx", MakeCallback(&Observador::TiempoEnvio, this));
  }
  //Subscribimos a la traza Rx la aplicación Sumidero
  aplicacion_sumidero->GetObject<PacketSink>()->TraceConnectWithoutContext("Rx", MakeCallback(&Observador::TiempoRecepcion, this));
}

// Método encargado de traza Tx
void Observador::TiempoEnvio(Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION ("En-----> TiempoEnvio de Observador");

  // Obtenemos el identificador del paquete
  uint64_t identificador = paquete->GetUid();
  map_lista[identificador] = Simulator::Now();
  NS_LOG_INFO("Nuevo paquete creado y enviado");
  totalPaquetes++;
  listaVacia=false;
}

// Método encargado de traza Rx
void Observador::TiempoRecepcion(Ptr<const Packet> paquete, const Address &)
{
  NS_LOG_FUNCTION ("En-----> TiempoRecepcion de Observador");
  // Obtenemos el identificador del paquete
  uint64_t identificador = paquete->GetUid();
  // Creamos el iterador que nos permitirá buscar el paquete en la lista
  std::map <uint64_t, Time>::iterator iterador = map_lista.find(identificador);

  if (iterador != map_lista.end())
  {
    Time retardo = Simulator::Now() - map_lista[identificador];
    acumulador_intervalo.Update(retardo.GetDouble());
    NS_LOG_DEBUG("Paquete recibido con un retardo de " << retardo.GetDouble()/1000 << "ms");
    map_lista.erase(iterador);
  }
  else
    NS_LOG_DEBUG ("El paquete con identificador " << identificador << " no se encuentra en la estructura.");
}

double Observador::GetRetardo()
{
  NS_LOG_FUNCTION ("En-----> GetRetardo de Observador");
  return acumulador_intervalo.Mean();
}

double Observador::GetPaquetesCorrectos()
{
  NS_LOG_FUNCTION ("En-----> GetPaquetesCorrectos de Observador");
  return totalPaquetes;
}

double Observador::GetListaVacia()
{
  NS_LOG_FUNCTION ("En-----> GetListaVacia de Observador");

  if(map_lista.size() == 0){
    NS_LOG_WARN("La lista está vacía");
    listaVacia == true;
  }
  else{
    NS_LOG_WARN("La lista no está vacía");
    listaVacia == false;
  }

  NS_LOG_DEBUG("Tamaño de la lista " << (unsigned int) map_lista.size());

  return listaVacia;
}

void Observador::PaqueteRechazado(Ptr<const Packet> paquete)
{
NS_LOG_FUNCTION("Se ha tirado un paquete");
paqueteRechazado++;

// Obtenemos el identificador del paquete
uint64_t identificador = paquete->GetUid();
// Creamos el iterador que nos permitirá buscar el paquete en la lista
std::map <uint64_t, Time>::iterator iterador = map_lista.find(identificador);

if (iterador != map_lista.end())
  map_lista.erase(iterador);
}

double Observador::GetPorcentajePaquetesRechazados()
{
  return ((double)paqueteRechazado/(double)totalPaquetes)*100;
}
