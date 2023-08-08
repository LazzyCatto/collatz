#pragma once

#include "linear_solver.h"
#include "rational.h"
#include <map>

class IncorectEquation : std::logic_error {
public:
	IncorectEquation() : std::logic_error("IncorectEquation") {}
};

struct FunctionalEquation {
private:
	size_t current_m;
	size_t current_k;
	std::map<std::pair<size_t, size_t>, long double> corresponding_equation_;
public:
	FunctionalEquation() = default;
	FunctionalEquation(size_t m, size_t k);

	long double GetAlpha(size_t m, size_t k) const;
	void SetAlpha(size_t m, size_t k, long double alpha);

	void MuTruncation();

	Equation<long double> Generate(long double lambda) const;
};

struct FunctionalSystem {
private:
	std::vector<FunctionalEquation> functional_system_;
	size_t variable_count_;
public:
	FunctionalSystem() = default;
	FunctionalSystem(size_t k);

	FunctionalEquation GetEquation(size_t m, size_t k) const;
	void SetEquation(size_t m, size_t k, FunctionalEquation alpha);

	std::vector<Equation<long double> > Generate(long double lambda) const;
};