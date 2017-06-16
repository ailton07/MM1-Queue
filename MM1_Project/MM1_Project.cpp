// MM1_Project.cpp : Defines the entry point for the console application.
//
#include "MM1_Project.h"

#include "stdio.h"
#include "stdlib.h"

#include "Original\TimedQueue.h"
#include "Original\ServerList.h"
#include "Original\mt.h"

#include <string>
#include <chrono>
// Originais
#include <iostream>
#include <cmath>

using namespace std;

MersenneTwister mt;



void Log(const char * str) {
    //cout << str << endl;
}

double ExponentialRandom(double seed, double lambda) {
    return -log(seed) / lambda;
}


double valorTeoricoDaMedia(double lambda, double mi) {
	double ro = lambda / mi;
	double media = ro * (1 / mi);
	media = media / (1 - ro);

	return media;
}

TimedQueue ExecMM1_Queue(TimedQueue queue, int exponent, float interArrivalMean, float serviceRateMean, int servers, bool listQueue) {

	Log("starting");

	ServerList server(servers);

	// we need to generate a random number to make the MersenneTwister be different everytime it starts.
	// Read more about it on MersenneTwister documentation.
	unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();

	mt.init_genrand(seed1);

	int sampleSize = pow(10, exponent);

	double interArrivalTime;
	double serviceTime;

	string s = "creating random interval arrival times for a sampling of " + to_string(sampleSize) + " elements.";

	Log(s.c_str());

	//double arrivalTime = 0;
	// obtem o ultimo arrivaltime pra utilizar como base
	double arrivalTime = queue[queue.Size() - 1].arrivalTime;

	// create queue with all elements
	for (int i = 0; i < sampleSize; ++i) {
		// random seeds will be used to generate exponential random number - which means M from "M"/M/1
		double seed = mt.genrand_real2();
		interArrivalTime = ExponentialRandom(seed, interArrivalMean);

		seed = mt.genrand_real2();
		serviceTime = ExponentialRandom(seed, serviceRateMean); // here is the 2nd M - M/"M"/1

																// the sum of inter arrival times will be giving the real arriving time of each element.
																// ex.: if first interval is 1 - arriving[0] = 1.
																// if next interval = 0.5 -> arriving[1] = 1+0.5 = 1.5.
																// if next = .75 arr[2] = 1 + 0.5 + .75 = 2.25....
		arrivalTime += interArrivalTime;
		queue.AddElement(i, arrivalTime, serviceTime);
	}

	Log("Running queue on servers.");

	// after we add all process to the queue then we can run the queue on a list of server - in this case 1 (M/M/"1")
	server.RunQueue(&queue);

	Log("Calculating queue times.");

	// after all process pass in a server we need to get their waiting average. It will be calculate here:
	queue.ProcessQueue();

	// this is not necessary and you should never set it to true unless you want to wait forever for 10^7.
	// Writing in console is so expensive compared to just math only.
	if (listQueue) {
		Log("Listing queue:");
		queue.ListQueue();
	}

	// the we return the processed waiting average time.
	return queue;
}

TimedQueue ExecMM1_Queue(int exponent, float interArrivalMean, float serviceRateMean, int servers, bool listQueue) {

	Log("starting");

	TimedQueue queue;
	ServerList server(servers);

	// we need to generate a random number to make the MersenneTwister be different everytime it starts.
	// Read more about it on MersenneTwister documentation.
	unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();

	mt.init_genrand(seed1);

	int sampleSize = pow(10, exponent);

	double interArrivalTime;
	double serviceTime;

	string s = "creating random interval arrival times for a sampling of " + to_string(sampleSize) + " elements.";

	Log(s.c_str());

	double arrivalTime = 0;

	// create queue with all elements
	for (int i = 0; i < sampleSize; ++i) {
		// random seeds will be used to generate exponential random number - which means M from "M"/M/1
		double seed = mt.genrand_real2();
		interArrivalTime = ExponentialRandom(seed, interArrivalMean);

		seed = mt.genrand_real2();
		serviceTime = ExponentialRandom(seed, serviceRateMean); // here is the 2nd M - M/"M"/1

																// the sum of inter arrival times will be giving the real arriving time of each element.
																// ex.: if first interval is 1 - arriving[0] = 1.
																// if next interval = 0.5 -> arriving[1] = 1+0.5 = 1.5.
																// if next = .75 arr[2] = 1 + 0.5 + .75 = 2.25....
		arrivalTime += interArrivalTime;
		queue.AddElement(i, arrivalTime, serviceTime);
	}

	Log("Running queue on servers.");

	// after we add all process to the queue then we can run the queue on a list of server - in this case 1 (M/M/"1")
	server.RunQueue(&queue);

	Log("Calculating queue times.");

	// after all process pass in a server we need to get their waiting average. It will be calculate here:
	queue.ProcessQueue();

	// this is not necessary and you should never set it to true unless you want to wait forever for 10^7.
	// Writing in console is so expensive compared to just math only.
	if (listQueue) {
		Log("Listing queue:");
		queue.ListQueue();
	}

	// the we return the processed waiting average time.
	return queue;
}

void questao03() {
	TimedQueue queue;
	bool continuarExecucao = true;

	// taxa de entrada
	double lambda = 9;
	// taxa de servico
	double mi = 10;

	// Inicia os calculos para o caso inicial de uma fila de 10^3
	queue = ExecMM1_Queue(3, lambda, mi, 1, false);
	// Media da espera
	double waitingAverage = queue.WaitingAverage();
	// Intervalo de confianca
	long double variacaoIC = queue.IntervaloConfianca();

	printf("Media de Espera: %f\n", waitingAverage);
	printf("VariacaoIC: %f\n", variacaoIC);
	// Aplicacao do intervalo de confianca na media
	printf("IC-: %f;\n", waitingAverage - variacaoIC);
	printf("IC+: %f;\n", waitingAverage + variacaoIC);

	// Obtem o H, ou 2d
	double h = 2 * variacaoIC;
	// Obtem gama
	double gama = h / waitingAverage;

	// Checa condicao de parada
	if (gama <= 0.05) {
		continuarExecucao = false;
		printf("\n\nCondicao de parada\n");
	}
	// Caso a condicao de parada nao seja alcancada
	// Repete os passos anteriores
	while (continuarExecucao) {
		// Adiciona 100 elementos na queue e reprocessa
		printf("\nAdicionando 100 elementos na queue e reprocessando\n");
		queue = ExecMM1_Queue(queue, 2, 90, 100, 1, false);

		waitingAverage = queue.WaitingAverage();
		variacaoIC = queue.IntervaloConfianca();

		printf("Media de Espera: %f\n", waitingAverage);
		printf("VariacaoIC: %f\n", variacaoIC);

		printf("IC-: %f;\n", waitingAverage - variacaoIC);
		printf("IC+: %f;\n", waitingAverage + variacaoIC);

		h = 2 * variacaoIC;
		gama = h / waitingAverage;
		if (gama <= 0.05) {
			continuarExecucao = false;
			printf("\n\nCondicao de parada\n");
			break;
		}

	}

	// obtem o valor teorico esperado da media
	printf("Valor teorico da media: %f\n", valorTeoricoDaMedia(lambda, mi));

	/*printf("Desvio padrao: %f\n", queue.DesvioPadrao());
	for (int i = 0; i < queue.Size(); i++) {
		printf("%f, ", queue[i].waitTime);
	}*/
}


int main()
{
	
	questao03();

	system("pause");
	return 0;
}