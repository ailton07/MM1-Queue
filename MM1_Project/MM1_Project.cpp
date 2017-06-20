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
void questao05(TimedQueue queue);

double student_dist(int ndf, double p);
double normal_dist(double p);

void obterInformacoesQueue(TimedQueue queue);

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
	queue = questao04(queue);


	printf("\n\n************************\n");
	printf("Execucao da Questao 05\n");
	questao05(queue);
	// obterInformacoesQueue(queue);

	system("pause");
	return 0;
}

// *******************************
// Desenvolvimento para Questao 5

bool testeVonNeuman(vector<double> mediasBlocos) {
	double B = 0.05; // Precisao de 5% ?
	vector<int> R;
	int contadorR = 0;

	double RVN = 0.0;

	for (int i = 0; i < mediasBlocos.size(); i++) {
		for (int j = 0; j < mediasBlocos.size(); j++) {
			if (i == j) {
				continue;
			}
			
			if (mediasBlocos[j] <= mediasBlocos[i]) {
				contadorR++;
			}
		}
		R.push_back(contadorR);
		contadorR = 0;
	}
	// Media R
	double mediaR = 0.0;
	for (int i = 0; i < R.size(); i++) {
		mediaR = mediaR + R[i];
	}
	mediaR = mediaR / R.size();

	// Numerador
	double numerador = 0.0;
	for (int i = 0; i < R.size() - 1; i++) {
		numerador = numerador + pow((R[i] - R[i + 1]), 2);
	}
	// Denominador
	double denominador = 0.0;
	for (int i = 0; i < R.size(); i++) {
		denominador = denominador + pow((R[i] - mediaR), 2);
	}

	// Se RVN = 2, amostras independentes
	// https://repositorio.ufrn.br/jspui/bitstream/123456789/20190/1/PatriciaVianaDeLima_DISSERT.pdf
	// RVN
	RVN = numerador / denominador;
	if (RVN > 1.5)
		return true;

	return false;
}

double desvioPadrao(vector<double> mediasBlocos, double waitAvg) {
	double desvio = 0.0;
	double aux;

	if (waitAvg > 0.0) {

		for (int i = 0; i < mediasBlocos.size(); ++i) {
			aux = mediasBlocos[i];
			desvio = desvio + pow(aux - waitAvg, 2);
		}
		desvio = desvio / (mediasBlocos.size() - 1);
		desvio = sqrt(desvio);
	}

	return desvio;
}

long double intervaloConfianca(vector<double> element, double waitAvg, double desvio) {
	double lvlConfianca = 0.95;
	long double variacao = 0.0;

	if (waitAvg > 0.0) {

		// obtendo T[n-1; alpha]
		double alpha = 1 - lvlConfianca;
		double student = student_dist(element.size() - 1, alpha);

		if ((alpha > 0)
			&& (student > 0)
			&& (element.size() > 0)) {
			variacao = (desvio * student) / sqrt(element.size());
		}

	}
	return variacao;
}

bool batchMeans(TimedQueue queue, int N, int M, int B) {
	double media = 0.0;

	vector<double> mediasBlocos;

	for (int i = 0; i < B; i++) {
		for (int j = 0; j < M; j++) {
			media = media + queue[i*M + j].waitTime;
		}
		media = media / M;
		mediasBlocos.push_back(media);
		// printf("media: %f\n", media);
		media = 0;
	}

	double mediaBlocos = 0.0;
	double desvio = 0.0;
	long double variacaoIC = 0.0;

	if (testeVonNeuman(mediasBlocos)) {
		// Calcula a media dos blocos
		for (int i = 0; i < mediasBlocos.size(); i++) {
			mediaBlocos = mediaBlocos + mediasBlocos[i];
		}
		mediaBlocos = mediaBlocos / mediasBlocos.size();

		// Calcula do Desvio Padrao
		desvio = desvioPadrao(mediasBlocos, mediaBlocos);

		variacaoIC = intervaloConfianca(mediasBlocos, mediaBlocos, desvio);

		// Obtem o H, ou 2d
		long double h = 2 * variacaoIC;
		// Obtem gama
		long double gama = h / mediaBlocos;

		// Checa condicao de parada
		printf("\nGama: %f\n", gama);
		if (gama <= 0.05) {
			printf("Media Global: %f\n", mediaBlocos);
			printf("IC-: %f;\n", mediaBlocos - variacaoIC);
			printf("IC+: %f;\n", mediaBlocos + variacaoIC);
			printf("\nCondicao de parada\n");
			return true;
		}
	}

	return false;
}

void questao05(TimedQueue queue) {
	// Utilizando 50 %  da queue
	int N = queue.Size() / 1;
	// Valor arbitrario de 20% de N
	int M = N / 10;
	// N = B * M
	// B vai ser igual a 5 nesse caso
	int B = N / M;

	for (int i = 16; i > 0; i--) {
		N = queue.Size() / 1;
		M = N / (i );
		B = N / M;
		printf("i = %d\n", i);
		if (batchMeans(queue, N, M, B)) {
			break;
		}
	}
	


}

// *******************************
// Desenvolvimento para Questao 4

void obterInformacoesQueue(TimedQueue queue) {
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
}

TimedQueue podaQueueEObtemInformacoes(TimedQueue queue, int j) {
	// Remove os J primeiros elementos
	queue.RemoveElements(j);

	// Recalcula Media
	queue.ProcessQueue();

	obterInformacoesQueue(queue);

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
	return podaQueueEObtemInformacoes(queue, j);
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

// *******************************
// Desenvolvimento para Questao 3

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
	queue = ExecMM1_Queue(6, lambda, mi, 1, false);
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
	long double h = 2 * variacaoIC;
	// Obtem gama
	long double gama = h / waitingAverage;

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