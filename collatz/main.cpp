//#define DEBUG
#define THRESHOLD 0.000001L
#define PRESIDION 20
#include "linear_solver.h"

#include "functional_system.h"
#include <cmath>
#include <stdio.h>
#include <windows.h>

//#include "rational.h"

bool L(long double lambda, const FunctionalSystem& j_system) {
	Solver<long double> solver { j_system.Generate(lambda), {{1}} };

	long double maximum = solver.GetMaxim();
	return maximum > THRESHOLD;
}

void ProgressBar(int percent, int width) {
	if (percent == 100) {
		printf("\x1b[32m");
	}
	else {
		printf("\x1b[31m");
	}
	printf("\r[");
	for (int i = 0; i < percent * width / 100; ++i) {
		printf("#");
	}
	for (int i = percent * width / 100; i < width; ++i) {
		printf(" ");
	}
	std::cout << "] " << percent << "%";
	std::cout.flush();
	printf("\x1b[0m");
}

std::pair<long double, long double> N(const FunctionalSystem& j_system) {
	long double min_lambda = 1;
	long double max_lambda = 2;
	for (size_t i = 0; i < PRESIDION; ++i) {
		ProgressBar(i * 100 / PRESIDION, 20);
		long double lambda = (min_lambda + max_lambda) / 2;
		if (L(lambda, j_system)) {
			min_lambda = lambda;
		} else {
			max_lambda = lambda;
		}
	}
	ProgressBar(100, 20);
	printf("\nDone!\n\n");
	return { min_lambda, max_lambda };
}

void Evaluate() {
	std::cout << std::fixed;
	while (true) {
		int k;
		std::cout << "enter value of k : ";
		std::cin >> k;

		clock_t start, end;

		start = clock();

		auto [min_lambda, max_lambda] = N(FunctionalSystem(k));
		long double min_gamma = std::log2(min_lambda);
		long double max_gamma = std::log2(max_lambda);

		end = clock();

		long double duration = static_cast<long double>(end - start) / CLOCKS_PER_SEC;

		std::cout <<
			"lambda : " << min_lambda << "-" << max_lambda <<
			"\ngamma : " << min_gamma << "-" << max_gamma <<
			"\nevaluation time : " << duration<< "\n\n\n";
		std::cout.flush();
	}
}

int main() {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD consoleMode;
	GetConsoleMode(console, &consoleMode);
	consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(console, consoleMode);

	Evaluate();
}