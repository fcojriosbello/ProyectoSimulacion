using namespace ns3;

#ifndef SIMULACIONWIFI_H
#define SIMULACIONWIFI_H

#include "ns3/object.h"
#include "ns3/global-value.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include <ns3/average.h>
#include <ns3/error-model.h>
#include "ns3/point-to-point-module.h"
#include "Observador.h"

#define NODOS_SEDE2 30  //Número de nodos en la sede 2. Será fijo ya que sólo
                        //nos interesa medir en un sentido (problema simétrico).


//Simulación simple para el servicio VoIP usando WIFI
void
simulacionWifi (uint32_t nWifi, Time ton, Time toff, uint32_t sizePkt, DataRate dataRate, std::string wifi_dataRate, 
    double p2p_prob_error_bit, std::string p2p_dataRate, std::string p2p_delay, double& retardo, 
    double& porcentaje, double& tasa);

#endif