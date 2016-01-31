using namespace ns3;

#ifndef SIMULACIONCSMA_H
#define SIMULACIONCSMA_H

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

//Simulación simple para el servicio VoIP usando CSMA
void
simulacionCSMA (uint32_t nCsma, Time ton, Time toff, uint32_t sizePkt, DataRate dataRate, 
    double csma_prob_error_bit, std::string csma_dataRate, std::string csma_delay, double p2p_prob_error_bit, 
    std::string p2p_dataRate, std::string p2p_delay, double& retardo, double& porcentaje, double& tasa);

#endif