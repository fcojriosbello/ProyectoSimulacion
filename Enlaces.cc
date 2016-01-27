/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/core-module.h>
#include <ns3/callback.h>
#include <ns3/packet.h>
#include "Enlaces.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Enlace");

EnlaceClientes::EnlaceClientes(Ptr<NetDevice> disp,
                              uint32_t       tamPqt,
                              uint32_t id_transmisor
                              uint32_t num_clientes)
{
  NS_LOG_FUNCTION (disp << tamPqt);

  //Variables:
  m_disp      = disp;
  m_tamPqt    = tamPqt;
  m_id        = id_transmisor;
  m_clientes  = num_clientes;

}



void 
EnlaceClientes::TokenRecibido(Ptr<NetDevice> receptor, 
                  Ptr<const Packet> recibido,
                  uint16_t protocolo,
                  const Address &desde,
                  const Address &hacia,
                  NetDevice::PacketType tipoPaquete);
{
  NS_LOG_FUNCTION (receptor << recibido->GetSize () <<
                   std::hex << protocolo <<
                   desde << hacia << tipoPaquete);
  
  //Variables para obtener el número de secuencia y el tipo de pkt de la cabecera
  uint8_t token_id;
  
  //Variabla para hacer una copia del pkt recibido y su cabecera:
  Ptr<Packet> copia;
  CabEnlace header;
  
  //Hacemos la copia del pkt:
  copia=recibido->Copy ();
  //Le quitamos la cabecera:
  copia->RemoveHeader (header);
  
  //Obtenemos el numero de secuencia y el tipo del pkt:
  tipo = header.GetTokenID();
                
  //Una vez extraidos los datos de la cabecera compruebo el tipo de pkt:
  
  if (token_id==m_id) //Pkt de datos
    //Aqui iria llamar a la otra interfaz para enviar el pkt al servidor.

  else //ACK
    NS_LOG_ERROR("Token recibido erroneo");
}


void 
EnlaceClientes::EnviaToken()
{

  NS_LOG_FUNCTION_NOARGS();
  //Creamos el pkt a enviar:
  Ptr<Packet> m_paquete = Create<Packet> (m_tamPqt);
    
  //Creamos la cabecera (el tipo del pkt es 0 al ser de datos):
  CabEnlace header;
  header.SetData((m_id +1)%num_clientes); //Esto debería hacerse circular para que acaba
  m_paquete->AddHeader(header);


  // Envío el paquete
  m_node->GetDevice(0)->Send(m_paquete, m_disp->GetAddress(), 0x0800);

  NS_LOG_INFO ("   Transmitido token de " << m_paquete->GetSize () <<
               " octetos en nodo " << m_node->GetId() <<
               " con " << (unsigned int) m_tx <<
               " en " << Simulator::Now());
}




EnlaceClienteServidor::EnlaceClienteServidor(Ptr<NetDevice> disp,
                              uint32_t       tamPqt)
{
  NS_LOG_FUNCTION (disp << tamPqt);

  //Variables:
  m_disp      = disp;
  m_tamPqt    = tamPqt;

}


void 
EnlaceClienteServidor::EnviaPkt()
{

  NS_LOG_FUNCTION_NOARGS();
  //Creamos el pkt a enviar:
  Ptr<Packet> m_paquete = Create<Packet> (m_tamPqt);

// Envío el paquete
  m_node->GetDevice(0)->Send(m_paquete, m_disp->GetAddress(), 0x0800);

  NS_LOG_INFO ("   Transmitido paquete de " << m_paquete->GetSize () <<
               " octetos en nodo " << m_node->GetId() <<
               " con " << (unsigned int) m_tx <<
               " en " << Simulator::Now());
}


