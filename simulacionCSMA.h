#ifndef SIMULACIONCSMA_H
#define SIMULACIONCSMA_H


using namespace ns3;

#define NODOS_SEDE2 30  //Número de nodos en la sede 2. Será fijo ya que sólo
                        //nos interesa medir en un sentido (problema simétrico).

//Simulación simple para el servicio VoIP usando CSMA
void
simulacionCSMA (uint32_t nCsma, Time ton, Time toff, uint32_t sizePkt, DataRate dataRate, 
    double csma_prob_error_bit, std::string csma_dataRate, std::string csma_delay, double p2p_prob_error_bit, 
    std::string p2p_dataRate, std::string p2p_delay, double& retardo, double& porcentaje, double& tasa);

#endif