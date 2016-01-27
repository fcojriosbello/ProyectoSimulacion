/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

using namespace ns3;

#include "ns3/node.h"
#include "ns3/net-device.h"
#include "ns3/application.h"
#include "CabEnlace.h"


/* Clase que servirá para simular los enlaces entre los clientes en 
una red con topología de anillo */
class EnlaceClientes : public Application
{
public:

  // TRANSMISOR Y/O RECEPTOR DE TOKEN: Constructor de la clase. Necesita como parámetros:
  // - el puntero al dispositivo de red con el que debe comunicarse (para enviar o recibir el token).
  // - el tamaño de paquete. 
  //Inicializa las variables privadas.
  EnlaceClientes(Ptr<NetDevice>, uint32_t tamPqt, uint32_t id_transmisor, uint32_t num_clientes);
                   
  // Función para el procesamiento del Token recibido
  // Comprueba si el Token es el adecuado (tiene mi id). Si lo es,
  //envia un paquete al servidor por su otra interfaz..
  void TokenRecibido(Ptr<NetDevice> receptor, Ptr<const Packet> recibido,
                       uint16_t protocolo, const Address &desde, const Address &hacia,
                       NetDevice::PacketType tipoPaquete);
                    
  
  // Función que envía un Token a la siguiente estación.
  void EnviaToken();


private:
  // Método de inicialización de la aplicación.
  // Se llama sólo una vez al inicio.
  // En nuestro caso sirve para instalar el Callback que va a procesar
  // los asentimientos recibidos.
  void DoInitialize()
  {
    // Solicitamos que nos entreguen (mediante la llamada a ACKRecibido)
    // cualquier paquete que llegue al nodo.
    m_node->RegisterProtocolHandler (ns3::MakeCallback(&Enlace::TokenRecibido,
                                                       this),
                                     0x0000, 0, false);
    Application::DoInitialize();
  };

  // Método que se llama en el instante de comienzo de la aplicación.
  void StartApplication()
  {
     //Enviamos todos los paquetes que nos permita la ventana:
      //ventanaTx.Vacia();
      //EnviaDatos();
  }

  // Método que se llama en el instante de final de la aplicación.
  void StopApplication()
  {
    Simulator::Stop ();
  }

  // Dispositivo de red con el que hay que comunicarse (enviar o recibir Token).
  Ptr<NetDevice> m_disp;
  // Tamaño del paquete
  uint32_t       m_tamPqt;
  // Token a enviar
  Ptr<Packet>    m_paquete;
  //Número de clientes:
  uint32_t m_clientes;
};

/* Clase que servirá para simular los enlaces entre los clientes en 
una red con topología de anillo */
class EnlaceClienteServidor : public Application
{
public:

  // TRANSMISOR DE PKTS AL SERIDOR: Constructor de la clase. Necesita como parámetros:
  // - el puntero al servidor con el que debe comunicarse.
  // - el tamaño de paquete. 
  //Inicializa las variables privadas.
  EnlaceClienteServidor(Ptr<NetDevice>, uint32_t tamPqt);
                    
 // Función que envía un paquete al servidor.
  void EnviaPkt();


private:
  // Método de inicialización de la aplicación.
  // Se llama sólo una vez al inicio.
  // En nuestro caso sirve para instalar el Callback que va a procesar
  // los asentimientos recibidos.
  void DoInitialize()
  {
    // Solicitamos que nos entreguen (mediante la llamada a ACKRecibido)
    // cualquier paquete que llegue al nodo.
    //m_node->RegisterProtocolHandler (ns3::MakeCallback(&Enlace::TokenRecibido,
                                                       this),
                                     0x0000, 0, false);
    //Application::DoInitialize();
  };

  // Método que se llama en el instante de comienzo de la aplicación.
  void StartApplication()
  {
     //Enviamos todos los paquetes que nos permita la ventana:
      //ventanaTx.Vacia();
      //EnviaDatos();
  }

  // Método que se llama en el instante de final de la aplicación.
  void StopApplication()
  {
    Simulator::Stop ();
  }

  // Servidor con  el que hay que comunicarse (solo enviar).
  Ptr<NetDevice> m_disp;
  // Tamaño del paquete
  uint32_t       m_tamPqt;
  // Número de secuencia de los paquetes a transmitir
  uint8_t        m_tx;
  // Token a enviar
  Ptr<Packet>    m_paquete;
  // Identificador del nodo Tx:
  uint32_t m_id;
};

