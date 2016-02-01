#ifndef SIMULACIONWIFI_H
#define SIMULACIONWIFI_H


using namespace ns3;

#define NODOS_SEDE2 30  //Número de nodos en la sede 2. Será fijo ya que sólo
                        //nos interesa medir en un sentido (problema simétrico).

//Simulación simple para el servicio VoIP usando WIFI
void
simulacionWifi (uint32_t nWifi, Time ton, Time toff, uint32_t sizePkt, DataRate dataRate, std::string wifi_dataRate, 
    double p2p_prob_error_bit, std::string p2p_dataRate, std::string p2p_delay, double& retardo, 
    double& porcentaje, double& jitter);

#endif