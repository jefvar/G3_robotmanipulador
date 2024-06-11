#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <vector>
extern WiFiServer _server; // servidor (n� de puerto definido en MyServer.cpp)
struct MyClient { // estructura para cada cliente conectado
	WiFiClient cli; // cliente conectado
	String name; // nombre identificativo ("UNKNOWN" si no ha recibido)
	String recv; // Ultimo mensaje recibido del cliente
};
extern std::vector<struct MyClient> _conn; // lista de clientes activos
// Comprueba si alg�n cliente nuevo se ha intentado conectar. En caso afirmativo,
// env�a el mensaje "<<<NAME>>>\n" para que se identifique, y devuelve true
bool CheckNewConn();
// Comprueba si alg�n cliente ha enviado un mensaje. En caso afirmativo, devuelve el
// �ndice del cliente en la tabla _conn, y se puede obtener el contenido en el campo
// recv de dicha tabla. Si el mensaje es de identificaci�n, asocia el nombre al
// cliente. Si no hay mensaje nuevo, o el mensaje era de identificaci�n, devuelve -1.
int CheckReceive();
// Env�a mensaje a un cliente a partir de su �ndice en la tabla (ojo, el �ndice puede
// cambiar al eliminarse elementos de la tabla, usar solamente tras CheckReceive)
int SendTo(int j, const char* txt);
// Env�a mensaje a un cliente a partir de su nombre identificativo
int SendTo(const char* name, const char* txt);
// Env�a mensaje a todos los clientes cuyo nombre identificativo empiece por name
// (si name es "", env�a a todos)
int SendToAllStartingWith(const char* name, const char* txt);
