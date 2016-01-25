/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/object.h"
#include "ns3/global-value.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include <ns3/average.h>
#include "ns3/gnuplot.h"
#include "Observador.h"

#define NUM_SIMULACIONES 10
#define TSTUDENT10 2.2622
#define DNI0 5
#define DNI1 1

//Estructura de datos para almacenar
//los resultados medios de una unica simulacion
typedef struct
  {
    double intentos;
    double tiempo;
    double porcentaje;
  } ResultSimulacion;

//Estructura de datos para almacenar los
//resultados meddios y las varianzas de 
//varias simulaciones.
//Notese la diferencia con la estructura anterior
//ya que esta la usaremos para guardar el resultado
//de varias simulaciones y la anterior solo para una.
typedef struct 
  {
    ResultSimulacion medias;
    ResultSimulacion varianzas;
  } ResultSimulaciones;

//Con esta funcion realizaremos NUM_SIMULACIONES simulaciones para cada punto de las graficas
//Devuelve una estructura donde se alamacenan las medias y varianzas necesarias
//para representar el punto de las graficas.Devuelve -1 en la variable correspondiente
// si hay algun error (si no se ha contado ningun evento).
ResultSimulaciones simulacion (uint32_t nCsma, Time retardoProp, DataRate capacidad, 
        uint32_t tamPaquete, Time intervalo, int maxReintentos);

//Funcion que obtiene los resultados de una simulacion a partir de los objetos
//Observador. Devuelve -1 en la variable correspondiente si hay algun error 
//(si no se ha contado ningun evento).
ResultSimulacion ResultadosSimulacion (Observador *observador, uint32_t nCsma);



using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("practica05");


int
main (int argc, char *argv[])
{
	GlobalValue::Bind("ChecksumEnabled", BooleanValue(true));
	Time::SetResolution (Time::NS);

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

    return 0;
}




//Funcion que obtiene los resultados de una simulacion a partir de los objetos
//Observador. Devuelve -1 en la variable correspondiente si hay algun error 
//(si no se ha contado ningun evento).
ResultSimulacion
ResultadosSimulacion (Observador *observador, uint32_t nCsma)
{
    NS_LOG_FUNCTION(observador << nCsma);

    ResultSimulacion resultados = {-1, -1, -1};

    //Objeto para guardar el numero de intentos medios de cada nodo.
    Average<double> acIntentos;
    //Objeto para guardar el tiempos medios (ns) de eco de cada nodo.
    Average<double> acTiempos;
    //Objeto para guardar los porcentajes de paquetes perdidos de cada nodo.
    Average<double> acPorcentajes;

    //Obtenemos los resultados de la simulacion
    //para el conjunto de nodos clientes menos el de indice 0.
    for (uint32_t j = 3; j < nCsma-1; j++)
    {
        if (observador[j].GetMediaIntentos() != -1)
        {
            acIntentos.Update(observador[j].GetMediaIntentos());
            NS_LOG_LOGIC ("Número medio de intentos necesarios para transmitir efectivamente un paquete en el nodo "
                            << observador[j].GetNodo() << ": " << observador[j].GetMediaIntentos());
        }
        else
            NS_LOG_LOGIC ("No se ha transmitido con exito ningun paquete en el nodo "
                << observador[j].GetNodo());

        if (observador[j].GetMediaTiempos() != -1) 
        {
            //Actualizamos el acumulador de tiempos de eco.
            acTiempos.Update(observador[j].GetMediaTiempos());
            NS_LOG_LOGIC ("Tiempo medio de ECO en el nodo "
                        << observador[j].GetNodo() << ": " << observador[j].GetMediaTiempos()/1e6 << " ms");
        }
        else
            NS_LOG_LOGIC ("No se ha completado con exito ninguna peticion ECO en el nodo "
                << observador[j].GetNodo());

        if (observador[j].GetPorcentajePktsPerdidos() != -1)
        {
            acPorcentajes.Update(observador[j].GetPorcentajePktsPerdidos());
            NS_LOG_LOGIC ("Porcentaje de paquetes enviados correctamente en el nodo "
                << observador[j].GetNodo() << ": " << 100-observador[j].GetPorcentajePktsPerdidos() << "%");
        }
        else
            NS_LOG_LOGIC ("No se ha podido obtener el porcentaje de paquetes enviados correctamente en el nodo " 
                << observador[j].GetNodo());
    }

    if(acIntentos.Count() > 0)
    {
        NS_LOG_INFO ("Numero medio de intentos necesarios para transmitir un paquete en el conjunto de clientes: "
             << acIntentos.Mean()); 
        resultados.intentos = acIntentos.Mean();
    }
    else
        NS_LOG_INFO ("No se ha transmitido ningun paquete correctamente.");

    if(acIntentos.Count() > 0)   
    {
        NS_LOG_INFO ("Tiempo medio de ECO en el conjunto de clientes: "
             << acTiempos.Mean()/1e6 << " ms");
        resultados.tiempo = acTiempos.Mean()/1e6;
    }
    else
        NS_LOG_INFO ("No se ha completado con exito ninguna peticion ECO.");

    if(acPorcentajes.Count() > 0)
    {
        NS_LOG_INFO ("Porcentaje de paquetes enviados correctamente por el conjunto de clientes :"
        << 100 - acPorcentajes.Mean() << "%");
        resultados.porcentaje = 100 - acPorcentajes.Mean();
    }
    else
        NS_LOG_INFO ("No se ha podido obtener el porcentaje de paquetes enviados correctamente por el conjunto de clientes.");

    return resultados;
}



//Con esta funcion realizaremos NUM_SIMULACIONES simulaciones simples para cada punto de las graficas
//Devuelve una estructura donde se alamacenan las medias y varianzas necesarias
//para representar el punto de las graficas.Devuelve -1 en la variable correspondiente
// si hay algun error (si no se ha contado ningun evento).
ResultSimulaciones
simulacion (uint32_t nCsma, Time retardoProp, DataRate capacidad, 
    uint32_t tamPaquete, Time intervalo, int maxReintentos)
{
    NS_LOG_FUNCTION(nCsma << retardoProp << capacidad << tamPaquete << intervalo << maxReintentos);

    //Objeto para guardar el numero de intentos medios de cada simulacion.
    Average<double> acIntentos;
    //Objeto para guardar el tiempos medios (ns) de eco de cada simulacion.
    Average<double> acTiempos;
    //Objeto para guardar los porcentajes de paquetes perdidos de cada simulacion.
    Average<double> acPorcentajes;

    ResultSimulaciones resultados = {{-1,-1,-1},{-1,-1,-1}};
    
    //Realizamos NUM_SIMULACIONES simulaciones con los parametros indicados.
    for (int i = 0; i < NUM_SIMULACIONES; i++)
    {

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
        address.SetBase ("10.1.2.0", "255.255.255.0");
        Ipv4InterfaceContainer csmaInterfaces = address.Assign (csmaDevices);


        // Suscribimos un objeto Observador a las trazas de cada nodo.
        Observador observador[nCsma];

        for (uint32_t j = 0; j < nCsma; j++)
        {
            observador[j].SetNodo((int)j);
            csmaDevices.Get(j)->TraceConnectWithoutContext ("MacTxBackoff", MakeCallback(&Observador::EnvioRetrasado, &observador[j]));
            csmaDevices.Get(j)->TraceConnectWithoutContext ("PhyTxDrop", MakeCallback(&Observador::EnvioDescartado, &observador[j]));
            csmaDevices.Get(j)->TraceConnectWithoutContext ("PhyTxEnd", MakeCallback(&Observador::EnvioTerminado, &observador[j]));
            csmaDevices.Get(j)->TraceConnectWithoutContext ("MacTx", MakeCallback(&Observador::OrdenEnvio, &observador[j]));
            csmaDevices.Get(j)->TraceConnectWithoutContext ("MacRx", MakeCallback(&Observador::OrdenPktDisponible, &observador[j]));
            
            //Aprovechamos para cambiar el numero maximo de reintentos de tx 
            csmaDevices.Get(j)->GetObject<CsmaNetDevice>()->SetBackoffParams (Time ("1us"), 10, 1000, 10, maxReintentos);
        }              

        // Cálculo de rutas
        Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

        /////////// Instalación de las aplicaciones
        // Servidor
        UdpEchoServerHelper echoServer (9);
        ApplicationContainer serverApp = echoServer.Install (csmaNodes.Get (nCsma - 1));
        serverApp.Start (Seconds (1.0));
        serverApp.Stop (Seconds (100.0));
        // Clientes
        UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma - 1), 9);
        echoClient.SetAttribute ("MaxPackets", UintegerValue (10000));
        echoClient.SetAttribute ("Interval", TimeValue (intervalo));
        echoClient.SetAttribute ("PacketSize", UintegerValue (tamPaquete));

        NodeContainer clientes;

        for (uint32_t i = 0; i < nCsma - 1; i++)
        {
            clientes.Add (csmaNodes.Get (i));
        }
        ApplicationContainer clientApps = echoClient.Install (clientes);
        clientApps.Start (Seconds (2.0));
        clientApps.Stop (Seconds (100.0));


        Simulator::Run ();
        Simulator::Destroy ();

        //Obtenemos los resultados de la simulacion iesima
        resultados.medias = ResultadosSimulacion(observador, nCsma);

        //Si los resultados son correctos, los acumulamos
        if(resultados.medias.intentos != -1)
            acIntentos.Update(resultados.medias.intentos);

        if(resultados.medias.tiempo != -1)   
            acTiempos.Update(resultados.medias.tiempo);
    
        if(resultados.medias.porcentaje != -1)
            acPorcentajes.Update(resultados.medias.porcentaje);
    }

    //Igualamos los resultados a -1 para el control de errores.
    resultados.medias.intentos = -1;
    resultados.medias.tiempo = -1;
    resultados.medias.porcentaje = -1;

    if (acIntentos.Count() > 0)
    {
        resultados.medias.intentos = acIntentos.Mean();
        resultados.varianzas.intentos = acIntentos.Var();
    }

    if (acTiempos.Count() > 0)
    {
        resultados.medias.tiempo = acTiempos.Mean();
        resultados.varianzas.tiempo = acTiempos.Var();
    }

    if (acPorcentajes.Count() > 0)
    {
        resultados.medias.porcentaje = acPorcentajes.Mean();
        resultados.varianzas.porcentaje = acPorcentajes.Var();
    }

    return resultados;
}
