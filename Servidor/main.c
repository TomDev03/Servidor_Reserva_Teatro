#include <stdio.h>
#include <stdlib.h>
#include "server.h"

/* Swaps position of strings in array (char**) */
void swap(const void** a, const void** b) {
	const void* temp = *a;
	*a = *b;
	*b = temp;
}

char* toUpper(char temp[]) {
	char name[100];
	strcpy(name, temp);
	int lenName = strlen(name);
	if (lenName > 0)
	{
		// Convert to upper case

		for (unsigned int i = 0; i < lenName; i++) {
			name[i] = toupper((unsigned char)name[i]);
		}
	}

	return name;
}

/*
R = earth radius
acos(sin(lat(city1))-sin(lat(city2))+cos(lat(city1))-cos(lat(city2))-cos(delta(long)))*R
*/

long double toRadians(const long double degree)
{
	// cmath library in C++
	// defines the constant
	// M_PI as the value of
	// pi accurate to 1e-30
	long double one_deg = (M_PI) / 180;
	return (one_deg * degree);
}

long double distance(long double lat1, long double long1,
	long double lat2, long double long2)
{
	// Convert the latitudes
	// and longitudes
	// from degree to radians.
	lat1 = toRadians(lat1);
	long1 = toRadians(long1);
	lat2 = toRadians(lat2);
	long2 = toRadians(long2);

	// Haversine Formula
	long double dlong = long2 - long1;
	long double dlat = lat2 - lat1;

	long double ans = pow(sin(dlat / 2), 2) +
		cos(lat1) * cos(lat2) *
		pow(sin(dlong / 2), 2);

	ans = 2 * asin(sqrt(ans));

	// Radius of Earth in
	// Kilometers, R = 6371
	// Use R = 3956 for miles
	long double R = 6371;

	// Calculate the result
	ans = ans * R;

	return ans;
}

int main()
{
	// Initialise winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	printf("\nInitialising Winsock...\n");
	int wsResult = WSAStartup(ver, &wsData);
	if (wsResult != 0) {
		fprintf(stderr, "\nWinsock setup fail! Error Code : %d\n", WSAGetLastError());
		return 1;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		fprintf(stderr, "\nSocket creationg fail! Error Code : %d\n", WSAGetLastError());
		return 1;
	}
	
	MutexCidades = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

	if (MutexCidades == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}


	MutexTeatros = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

	if (MutexTeatros == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}

	cidades = InitCidades();

	qsort(cidades, tamCidades, sizeof(Cidade*), compareCidades);
	
	printf("Socket created.\n");

	// Bind the socket (ip address and port)
	struct sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(DS_TEST_PORT);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (struct sockaddr*)&hint, sizeof(hint));

	// Setup the socket for listening
	listen(listening, SOMAXCONN);

	// Wait for connection
	struct sockaddr_in client;
	int clientSize = sizeof(client);

	// Main program loop
	SOCKET clientSocket;

	unsigned int numClients = 0;
	Client* c;

	while ((clientSocket = accept(listening, (struct sockaddr*)&client, &clientSize))) {
		// Create a new thread for the accepted client (also pass the accepted client socket).
		unsigned threadID;
		c = calloc(1, sizeof(Client));
		if (c != NULL)
		{
			c->socket = clientSocket;
			c->teatroVisitados = NULL;
			c->tamTeatrosVisitados = 0;
			c->hThread = (HANDLE)_beginthreadex(NULL, 0, &ClientSession, (Client*)c, 0, &threadID);
			c->threadID = threadID;
			c = NULL;
		}
		else {
			printf("Error to connect to client\n");
			free(c);
		}
	}

	CloseHandle(MutexCidades);
	CloseHandle(MutexTeatros);

	// Close listening socket
	closesocket(listening);

	//Cleanup winsock
	WSACleanup();
}