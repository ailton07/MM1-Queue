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

TimedQueue questao03();
TimedQueue questao04(TimedQueue queue);

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

	ServerList server(servers);

	// we need to generate a random number to make the MersenneTwister be different everytime it starts.
	// Read more about it on MersenneTwister documentation.
	unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();

	mt.init_genrand(seed1);

	int sampleSize = pow(10, exponent);

	double interArrivalTime;
	double serviceTime;


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

	// after we add all process to the queue then we can run the queue on a list of server - in this case 1 (M/M/"1")
	server.RunQueue(&queue);

	// after all process pass in a server we need to get their waiting average. It will be calculate here:
	queue.ProcessQueue();

	// the we return the processed waiting average time.
	return queue;
}

TimedQueue ExecMM1_Queue(int exponent, float interArrivalMean, float serviceRateMean, int servers, bool listQueue) {


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

	// after we add all process to the queue then we can run the queue on a list of server - in this case 1 (M/M/"1")
	server.RunQueue(&queue);

	// after all process pass in a server we need to get their waiting average. It will be calculate here:
	queue.ProcessQueue();

	// this is not necessary and you should never set it to true unless you want to wait forever for 10^7.
	// Writing in console is so expensive compared to just math only.
	if (listQueue) {
		// Log("Listing queue:");
		queue.ListQueue();
	}

	// the we return the processed waiting average time.
	return queue;
}

void printQueue(TimedQueue queue) {
	for (int i = 0; i < queue.Size(); i++) {
		printf("%f, ", queue[i].waitTime);
	}
	printf("\n\n");
}


int main()
{
	printf("************************\n");
	printf("Execucao da Questao 03\n");
	// Execucao da Questao 03
	TimedQueue queue  = questao03();
	// Execucao da Questao 04
	printf("\n\n************************\n");
	printf("Execucao da Questao 04\n");
	questao04(queue);

	system("pause");
	return 0;
}


TimedQueue obterInformacoesQueue(TimedQueue queue, int j) {
	queue.RemoveElements(j);

	// Recalcula Media
	queue.ProcessQueue();
	// Media da espera
	double waitingAverage = queue.WaitingAverage();
	// Intervalo de confianca
	long double variacaoIC = queue.IntervaloConfianca();
	
	printf("\nobterInformacoesQueue:\n");
	printf("Media de Espera: %f\n", waitingAverage);
	printf("VariacaoIC: %f\n", variacaoIC);
	// Aplicacao do intervalo de confianca na media
	printf("IC-: %f;\n", waitingAverage - variacaoIC);
	printf("IC+: %f;\n", waitingAverage + variacaoIC);
	printf("Numero de elementos da queue final: %f\n", queue.Size());

	return queue;
}

TimedQueue questao04DoJobForK(TimedQueue queue, int k) {
	int n = 0;
	int j = 0;  // posicao ate encontrar k passagens pela media
	int contadorTransicoes = 0;
	double media = 0.0;
	double referencia = 0;

	while (contadorTransicoes < k) {
		vector<double> observacoes;
		n = n + (queue.Size() / 100) + 1;
		// observacoes.clear();
		contadorTransicoes = 0;

		// Pega N primeiro elementos
		for (int i = 0; i < n; i++) {
			observacoes.push_back(queue[i].waitTime);
			media = media + queue[i].waitTime;
			// printf("%f, ", queue[i].waitTime);
		}
		media = media / n;

		referencia = observacoes[0];

		for (int i = 1; i < n; i++) {
			if ((observacoes[i] > media
				&& referencia < media) ||
				(observacoes[i] < media
					&& referencia > media)) {
				contadorTransicoes++;
				j = i;

				if (contadorTransicoes == k)
					break;
			}
			referencia = observacoes[i];
		}
	}
	// Faz prints e retorna a queue
	return obterInformacoesQueue(queue, j);
}

TimedQueue questao04(TimedQueue queue) {
	TimedQueue auxQueue;
	// K = 5
	auxQueue = queue;
	questao04DoJobForK(auxQueue, 5);

	// K = 7
	auxQueue = queue;
	questao04DoJobForK(auxQueue, 7);

	// K = 15
	auxQueue = queue;
	questao04DoJobForK(auxQueue, 15);

	// K = 20
	auxQueue = queue;
	auxQueue = questao04DoJobForK(auxQueue, 20);

	return auxQueue;
}

TimedQueue questao03() {
	TimedQueue queue;
	bool continuarExecucao = true;

	// taxa de entrada
	double lambda = 9;
	// taxa de servico
	double mi = 10;

	int queueSize;

	// Inicia os calculos para o caso inicial de uma fila de 10^4 ou 10^5
	// Evitar usar valores inferiores
	queue = ExecMM1_Queue(5, lambda, mi, 1, false);
	queueSize = queue.Size();
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
		printf("\nCondicao de parada\n");
	}
	// Caso a condicao de parada nao seja alcancada
	// Repete os passos anteriores
	while (continuarExecucao) {
		// Adiciona 100 elementos na queue e reprocessa
		printf("\nAdicionando 100 elementos na queue e reprocessando\n");
		queue = ExecMM1_Queue(queue, 2, 90, 100, 1, false);
		queueSize = queue.Size();

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

	printf("Numero de elementos da queue final: %d\n", queueSize);

	// obtem o valor teorico esperado da media
	printf("Valor teorico da media: %f\n", valorTeoricoDaMedia(lambda, mi));

	//printf("Desvio padrao: %f\n", queue.DesvioPadrao());
	//printQueue(queue);

	return queue;
}