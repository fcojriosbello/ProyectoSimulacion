#ifndef SIMULACIONWIFI_H
#define SIMULACIONWIFI_H


using namespace ns3;

//Simulación simple para el servicio VoIP usando WIFI
void
simulacionWifi (uint32_t nWifi, uint32_t nodosSede2, Time ton, Time toff, uint32_t sizePkt, DataRate dataRate, std::string wifi_dataRate, 
    double p2p_prob_error_bit, std::string p2p_dataRate, std::string p2p_delay, double& retardo, 
    double& porcentaje, double& jitter);

#endif