#include "functional_system.h"

#ifndef MU
#define MU 0
#endif

#ifndef ALPHA
#define ALPHA std::log2(3)
#endif


FunctionalEquation::FunctionalEquation(size_t m, size_t k) : current_m(m), current_k(k) {
	if (m % 3 != 2) {
		throw IncorectEquation{};
	}

	size_t pow = static_cast<size_t>(std::pow(3, k));

	if (m % 9 == 2) {         // Q^2_k
		corresponding_equation_[{ (4 * m) % pow, k }] = 2;
		corresponding_equation_[{ ((4 * m  - 2) / 3) % (pow / 3), k - 1 }] = 2 - ALPHA;
	} else if (m % 9 == 5) {  // Q^5_k
		corresponding_equation_[{ (4 * m) % pow, k }] = 2;
	} else {                  // Q^8_k
		corresponding_equation_[{ (4 * m) % pow, k }] = 2;
		corresponding_equation_[{ ((2 * m - 1) / 3) % (pow / 3), k - 1 }] = 1 - ALPHA;
	}
}

long double FunctionalEquation::GetAlpha(size_t m, size_t k) const {
	return corresponding_equation_.at({ m, k });
}

void FunctionalEquation::SetAlpha(size_t m, size_t k, long double alpha) {
	if (m % 3 != 2) {
		throw InvalidOperation{};
	}
	corresponding_equation_[{ m, k }] = alpha;
}

void FunctionalEquation::MuTruncation() {
	for (auto& [key, alpha] : corresponding_equation_) {
		if (alpha < 0) {
			alpha = MU;
		}
	}
}

Equation<long double> FunctionalEquation::Generate(long double lambda) const {
	size_t current_pow = static_cast<size_t>(std::pow(3, current_k));
	size_t current_index = ((current_pow / 3) - 1) / 2 + (current_m - 2) / 3;

	std::vector<long double> coefitients = std::vector<long double>((current_pow - 1) / 2);
	coefitients[current_index] = 1;

	for (auto [key, alpha] : corresponding_equation_) {
		auto [m, k] = key;
		size_t pow = static_cast<size_t>(std::pow(3, k));
		size_t index = ((pow / 3) - 1) / 2 + (m - 2) / 3;
		coefitients[index] = -std::pow(lambda, -alpha);
	}

	return { coefitients, 0, EquationType::LESS_OR_EQUAL };
}

FunctionalSystem::FunctionalSystem(size_t k) {
	size_t pow = static_cast<size_t>(std::pow(3, k));
	functional_system_.reserve(pow / 3);
	for (size_t m = 2; m < pow; m += 3) {
		functional_system_.emplace_back(m, k);
	}
	variable_count_ = (pow - 1) / 2;
}

std::vector<Equation<long double> > FunctionalSystem::Generate(long double lambda) const {
	std::vector<Equation<long double> > generated_system;
	generated_system.reserve(functional_system_.size());
	for (auto functional_equation : functional_system_) {
		functional_equation.MuTruncation();
		generated_system.push_back(functional_equation.Generate(lambda));
	}

	generated_system.emplace_back(std::vector<long double>{ 1 }, 1, EquationType::LESS_OR_EQUAL);

	for (size_t pow = 3; pow < 2 * variable_count_ + 1; pow *= 3) {
		for (size_t n = 2; n < pow; n += 3) {
			for (size_t l = 0; l < 3; ++l) {
				size_t current_index = ((pow / 3) - 1) / 2 + (n - 2) / 3;
				size_t next_index = (pow - 1) / 2 + (n + pow * l - 2) / 3;
				std::vector<long double> coefitients = std::vector<long double>(variable_count_);
				coefitients[current_index] = 1;
				coefitients[next_index] = -1;
				generated_system.emplace_back(coefitients, 0, EquationType::LESS_OR_EQUAL);
			}
		}
	}

	return generated_system;
}