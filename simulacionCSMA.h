#ifndef SIMULACIONCSMA_H
#define SIMULACIONCSMA_H


using namespace ns3;

#define NUM_REINTENTOS 8

//Simulación simple para el servicio VoIP usando CSMA
void
simulacionCSMA (uint32_t nCsma, uint32_t nodosSede2, Time ton, Time toff, uint32_t sizePkt, DataRate dataRate, 
    double csma_prob_error_bit, std::string csma_dataRate, std::string csma_delay, double p2p_prob_error_bit, 
    std::string p2p_dataRate, std::string p2p_delay, double& retardo, double& porcentaje, double& jitter);

#endif