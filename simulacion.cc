/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/object.h"
#include "ns3/global-value.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include <ns3/average.h>
#include "ns3/gnuplot.h"
#include "ObservadorCSMA.h"

#define NUM_SIMULACIONES 10
#define TSTUDENT10 2.2622
#define DNI0 5
#define DNI1 1


void simulacionCSMA_FTP (uint32_t nCsma, Time retardoProp, DataRate capacidad, 
        uint32_t tamPaquete);





using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FTP-CSMA");


int
main (int argc, char *argv[])
{
	GlobalValue::Bind("ChecksumEnabled", BooleanValue(true));
	Time::SetResolution (Time::NS);

simulacionCSMA_FTP ((uint32_t) 50, Time("6560ns"), DataRate("10Mbps"), (uint32_t)40);


/*
    //Variable para el calculo del intervalo de confianza.
    double z = 0.0;

	uint32_t nCsma = 10-DNI0/2;
    Time retardoProp = Time("6560ns");
    DataRate capacidad = DataRate("100Mbps");
    uint32_t tamPaquete = 500+100*DNI1;
    Time intervalo = Time("1s");

	CommandLine cmd;
	cmd.AddValue ("nCsma", "Número de nodos de la red local", nCsma);
    cmd.AddValue ("retardoProp", "Retardo de propagación en el bus", retardoProp );
    cmd.AddValue ("capacidad", "Capacidad del bus", capacidad );
    cmd.AddValue ("tamPaquete", "Tamaño de las SDU de aplicación", tamPaquete);
    cmd.AddValue ("intervalo", "Tiempo entre dos paquetes consecutivos enviados por el mismo cliente", intervalo);

	cmd.Parse (argc,argv);

    NS_LOG_INFO ("Se han configurado los siguientes argumentos de entrada:" << std::endl <<
                 "     -nCsma:           " << (unsigned int)nCsma  << std::endl <<
                 "     -retardoProp:     " << retardoProp.GetDouble()/1e3 << "us" << std::endl <<
                 "     -capacidad:       " << capacidad.GetBitRate()/1e6 << "Mbps" << std::endl <<
                 "     -tamPaquete:      " << tamPaquete << "B" << std::endl <<
                 "     -intervalo:       " << intervalo.GetDouble()/1e9 << "s" << std::endl);


    // -----------Antes de simular, preparamos las graficas-------
    Gnuplot grafIntentos;
    grafIntentos.SetTitle ("Número medio de intentos para tx en función del número máximo de reintentos admisibles");
    grafIntentos.SetLegend ("Número máximo de reintentos admisibles", 
        "Numero medio de intentos para transmitir de los clientes");
    Gnuplot2dDataset datosIntentos("intentos");
    datosIntentos.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    datosIntentos.SetErrorBars(Gnuplot2dDataset::Y);

    Gnuplot grafTiempo;
    grafTiempo.SetTitle ("Tiempo medio hasta recepción del eco en función del número máximo de reintentos admisibles");
    grafTiempo.SetLegend ("Número máximo de reintentos admisibles", 
        "Tiempo medio hasta recepción del eco (ms)");
    Gnuplot2dDataset datosTiempo("tiempo de eco");
    datosTiempo.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    datosTiempo.SetErrorBars(Gnuplot2dDataset::Y);

    Gnuplot grafPorcentaje;
    grafPorcentaje.SetTitle ("Porcentaje de paquetes transmitidos en función del número máximo de reintentos admisibles");
    grafPorcentaje.SetLegend ("Número máximo de reintentos admisibles", 
        "Porcentaje de paquetes transmitidos correctamente (%)");
    Gnuplot2dDataset datosPorcentaje("% pkts enviados");
    datosPorcentaje.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    datosPorcentaje.SetErrorBars(Gnuplot2dDataset::Y);
    // ---------------------------------------------------------------------------------------------- 

    ResultSimulaciones result;

    for (int maxReintentos = 1; maxReintentos <= 15; maxReintentos++)
    {

        NS_LOG_INFO("-----------------Comenzamos el bloque de simulaciones para maxReintentos = " << maxReintentos << "-----------------");
        result=simulacion(nCsma, retardoProp, capacidad, tamPaquete, intervalo, maxReintentos);


        //Si los resultados son correctos, los introducimos en la grafica
        if(result.medias.intentos != -1 and result.varianzas.intentos != -1)
        {
            //Calculamos el intervalo de confianza
            z = TSTUDENT10*sqrt(result.varianzas.intentos/(NUM_SIMULACIONES));
            // Añadimos los datos para representar en la grafica
            datosIntentos.Add(maxReintentos, result.medias.intentos, z);
        }

        if(result.medias.tiempo != -1 and result.varianzas.tiempo != -1)   
        {
            //Calculamos el intervalo de confianza
            z = TSTUDENT10*sqrt(result.varianzas.tiempo/(NUM_SIMULACIONES));
            // Añadimos los datos para representar en la grafica
            datosTiempo.Add(maxReintentos, result.medias.tiempo, z);
        }
    
        if(result.medias.porcentaje != -1 and result.varianzas.porcentaje != -1)
        {
            //Calculamos el intervalo de confianza
            z = TSTUDENT10*sqrt(result.varianzas.porcentaje/(NUM_SIMULACIONES));
            // Añadimos los datos para representar en la grafica
            datosPorcentaje.Add(maxReintentos, result.medias.porcentaje, z);
        }
    }

    //--------Introducimos los resultados en las graficas-----------------------------------
    std::ofstream ficheroIntentos("practica05-01.plt");
    grafIntentos.AddDataset(datosIntentos);
    grafIntentos.GenerateOutput(ficheroIntentos);
    ficheroIntentos << "pause -1" << std::endl;
    ficheroIntentos.close();

    std::ofstream ficheroTiempo("practica05-02.plt");
    grafTiempo.AddDataset(datosTiempo);
    grafTiempo.GenerateOutput(ficheroTiempo);
    ficheroTiempo << "pause -1" << std::endl;
    ficheroTiempo.close();

    std::ofstream ficheroPorcentaje("practica05-03.plt");
    grafPorcentaje.AddDataset(datosPorcentaje);
    grafPorcentaje.GenerateOutput(ficheroPorcentaje);
    ficheroPorcentaje << "pause -1" << std::endl;
    ficheroPorcentaje.close();
    //-------------------------------------------------  

    return 0;*/
}


//Simulación simple para el servicio FTP usando CSMA
void
simulacionCSMA_FTP (uint32_t nCsma, Time retardoProp, DataRate capacidad, uint32_t tamPaquete)
{
    NS_LOG_FUNCTION(nCsma << retardoProp << capacidad << tamPaquete );

    NodeContainer csmaNodes;
    csmaNodes.Create (nCsma);

    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", DataRateValue (capacidad));
    csma.SetChannelAttribute ("Delay", TimeValue (retardoProp));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install (csmaNodes);
    // Instalamos la pila TCP/IP en todos los nodos
    InternetStackHelper stack;
    stack.Install (csmaNodes);
    // Y les asignamos direcciones
    Ipv4AddressHelper address;
    address.SetBase ("10.0.0.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces = address.Assign (csmaDevices);            

    // Cálculo de rutas
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    /////////// Instalación de las aplicaciones
    // Servidor
    uint16_t port = 9;
    PacketSinkHelper sink ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
    sink.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
    ApplicationContainer sinkApp = sink.Install (csmaNodes.Get (0));

    sinkApp.Start (Seconds (1.0));
    sinkApp.Stop (Seconds (10.0));

    // Clientes
    AddressValue remoteAddress (InetSocketAddress (csmaInterfaces.GetAddress(0,0), port));
    //Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (tcp_adu_size));
    BulkSendHelper ftp ("ns3::TcpSocketFactory", Address());
    ftp.SetAttribute ("Remote", remoteAddress);
    ftp.SetAttribute ("MaxBytes", UintegerValue (0));

    //Contenedor de los nodos clientes
    NodeContainer clientes;
    for (uint32_t i = 1; i < nCsma; i++)
        clientes.Add(csmaNodes.Get(i));

    ApplicationContainer sourceApp = ftp.Install (clientes);
    sourceApp.Start (Seconds (2.0));
    sourceApp.Stop (Seconds (9.0));

    // Suscribimos un objeto Observador a las trazas de cada nodo.
    //Nodo 0 es el sumidero
    ObservadorCSMA observadorCSMA;

    for (uint32_t j = 1; j < nCsma; j++)
    {
        observadorCSMA.SetNodo((int)j);
        observadorCSMA.SetTamPkt(tamPaquete);
        //csmaDevices.Get(j)->TraceConnectWithoutContext ("MacTxBackoff", MakeCallback(&ObservadorCSMA::EnvioRetrasado, &observadorCSMA[j]));
        csmaDevices.Get(j)->TraceConnectWithoutContext ("PhyTxDrop", MakeCallback(&ObservadorCSMA::EnvioDescartado, &observadorCSMA));
        //csmaDevices.Get(j)->TraceConnectWithoutContext ("PhyTxEnd", MakeCallback(&ObservadorCSMA::EnvioTerminado, &observadorCSMA[j]));
        //csmaDevices.Get(j)->TraceConnectWithoutContext ("MacTx", MakeCallback(&ObservadorCSMA::OrdenEnvio, &observadorCSMA[j]));
        sourceApp.Get(j-1)->GetObject<BulkSendApplication>()->TraceConnectWithoutContext ("Tx", MakeCallback(&ObservadorCSMA::PktGenerado, &observadorCSMA));
            
        //Aprovechamos para cambiar el numero maximo de reintentos de tx 
        //csmaDevices.Get(j)->GetObject<CsmaNetDevice>()->SetBackoffParams (Time ("1us"), 10, 1000, 10, maxReintentos);
    }  

    observadorCSMA.SetNodo((int)0);
    observadorCSMA.SetTamPkt(tamPaquete);
    sinkApp.Get(0)->GetObject<PacketSink>()->TraceConnectWithoutContext ("Rx", MakeCallback(&ObservadorCSMA::PktRecibido, &observadorCSMA));   

    Simulator::Stop(Seconds(10.0));
    Simulator::Run ();
    Simulator::Destroy ();

    NS_LOG_ERROR(observadorCSMA.GetMediaTiempos());
    //NS_LOG_ERROR(observadorCSMA[157].GetMediaIntentos());
    //NS_LOG_ERROR(observadorCSMA[157].GetPorcentajePktsRetrasados ());
}