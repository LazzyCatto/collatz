#pragma once

#include <vector>
#include <array>
#include <iostream>
#include <stdexcept>

#include "rational.h"

#ifndef M
#define M 1000000
#endif

#ifndef THRESHOLD
#define THRESHOLD 0
#endif

class InvalidOperation : std::logic_error {
public:
	InvalidOperation() : std::logic_error("InvalidOperation") {}
};

enum class EquationType {
	LESS = 0,
	LESS_OR_EQUAL = 1,
	EQUAL = 2,
	GREATER_OR_EQUAL = 3,
	GREATER = 4
};

template <class T>
struct Equation {
private:
	std::vector<T> coefitients_;
	T result_;
	EquationType type_;

	void FlipType() {
		type_ = static_cast<EquationType>(4 - static_cast<int>(type_));
	}

public:
	Equation() = default;

	Equation(const std::vector<T>& coefitients, const T& result = 0, EquationType type = EquationType::EQUAL) :
		coefitients_(coefitients),
		result_(result),
		type_(type) {}

	std::vector<T>& GetCoefitients() {
		return coefitients_;
	}

	std::vector<T> GetCoefitients() const {
		return coefitients_;
	}

	T& GetResult() {
		return result_;
	}

	T GetResult() const {
		return result_;
	}

	EquationType& GetType() {
		return type_;
	}

	EquationType GetType() const {
		return type_;
	}

	size_t VariableCount() const {
		return coefitients_.size();
	}

	bool IsSameCotegory(const Equation& other) {
		return (static_cast<int>(other.type_) <= 2 && static_cast<int>(type_) <= 2) ||
			(static_cast<int>(other.type_) >= 2 && static_cast<int>(type_) >= 2);
	}

	bool IsDifferentCotegory(const Equation& other) {
		return (static_cast<int>(other.type_) >= 2 && static_cast<int>(type_) <= 2) ||
			(static_cast<int>(other.type_) <= 2 && static_cast<int>(type_) >= 2);
	}

	bool IsEquality() const {
		return type_ == EquationType::EQUAL;
	}

	bool IsInequality() const {
		return type_ != EquationType::EQUAL;
	}

	bool IsStictInequality() const {
		return type_ == EquationType::LESS || type_ == EquationType::GREATER;
	}

	bool IsLessInequality() const {
		return type_ == EquationType::LESS || type_ == EquationType::LESS_OR_EQUAL;
	}

	bool IsGreaterInequality() const {
		return type_ == EquationType::GREATER || type_ == EquationType::GREATER_OR_EQUAL;
	}

	void Clear() {
		for (auto& coefitient : coefitients_) {
			coefitient = 0;
		}
		result_ = 0;
	}

	Equation operator+() const {
		Equation temp;
		temp.coefitients_.reserve(coefitients_.size());
		for (auto coefitient : coefitients_) {
			temp.coefitients_.push_back(+coefitient);
		}
		temp.result_ = +result_;
		temp.type_ = type_;
		return temp;
	}

	Equation operator-() const {
		Equation temp;
		temp.coefitients_.reserve(coefitients_.size());
		for (auto coefitient : coefitients_) {
			temp.coefitients_.push_back(-coefitient);
		}
		temp.result_ = -result_;
		temp.type_ = type_;
		temp.FlipType();
		return temp;
	}

	Equation& operator+=(const Equation& rhs) {
		if (!IsSameCotegory(rhs)) {
			throw InvalidOperation{};
		}
		size_t min_size = coefitients_.size();
		if (min_size > rhs.coefitients_.size()) {
			min_size = rhs.coefitients_.size();
		}
		for (size_t i = 0; i < min_size; ++i) {
			coefitients_[i] += rhs.coefitients_[i];
		}
		result_ += rhs.result_;
		return *this;
	}

	Equation& operator-=(const Equation& rhs) {
		return *this += -rhs;
	}

	Equation& operator*=(T rhs) {
		for (auto& coefitient : coefitients_) {
			coefitient *= rhs;
		}
		result_ *= rhs;
		if (rhs < THRESHOLD) {
			FlipType();
		}
		return *this;
	}

	Equation& operator/=(T rhs) {
		for (auto& coefitient : coefitients_) {
			coefitient /= rhs;
		}
		result_ /= rhs;
		if (rhs < THRESHOLD) {
			FlipType();
		}
		return *this;
	}
};

template <class T>
Equation<T> operator+(Equation<T> lhs, const Equation<T>& rhs) {
	return lhs += rhs;
}

template <class T>
Equation<T> operator-(Equation<T> lhs, const Equation<T>& rhs) {
	return lhs -= rhs;
}

template <class T>
Equation<T> operator*(Equation<T> lhs, const T& rhs) {
	return lhs *= rhs;
}

template <class T>
Equation<T> operator*(const T& lhs, Equation<T> rhs) {
	return rhs *= lhs;
}

template <class T>
Equation<T> operator/(Equation<T> lhs, const T& rhs) {
	return lhs /= rhs;
}

template <class T>
std::ostream& operator<<(std::ostream& out, const Equation<T>& rhs) {
	for (size_t i = 0; i < rhs.VariableCount(); ++i) {
		out << "(" << rhs.GetCoefitients()[i] << ")*x" << i;
		if (i != rhs.VariableCount() - 1) {
			out << " + ";
		}
	}
	switch (rhs.GetType()) {
	case EquationType::LESS:
		out << " < ";
		break;
	case EquationType::LESS_OR_EQUAL:
		out << " <= ";
		break;
	case EquationType::EQUAL:
		out << " = ";
		break;
	case EquationType::GREATER_OR_EQUAL:
		out << " >= ";
		break;
	case EquationType::GREATER:
		out << " > ";
		break;
	default:
		break;
	}
	return out << rhs.GetResult();
}

class SystemUnbounded : std::logic_error {
public:
	SystemUnbounded() : std::logic_error("SystemUnbounded") {}
};

template <class T>
class Solver {
private:
	std::vector<Equation<T> > system_;
	Equation<T> max_equation_;
	std::vector<T> basis_;
	size_t variable_count;
	size_t pseudo_variable_count;

public:
	Solver(const std::vector<Equation<T> >& equations, const Equation<T>& max_equation) : 
		system_(equations),
		max_equation_(max_equation),
		basis_(equations.size(), 0),
		variable_count(max_equation.VariableCount()),
		pseudo_variable_count(0)
	{
		for (auto equation : equations) {
			if (variable_count < equation.VariableCount()) {
				variable_count = equation.VariableCount();
			}
			if (equation.IsGreaterInequality()) {
				++pseudo_variable_count;
			}
			++pseudo_variable_count;
		}

		max_equation_.GetCoefitients().resize(variable_count + pseudo_variable_count);

		for (size_t i = 0, j = 0; i < system_.size(); ++i) {
			system_[i].GetCoefitients().resize(variable_count + pseudo_variable_count, 0);
			if (system_[i].IsGreaterInequality()) {
				system_[i].GetCoefitients()[variable_count + j++] = -1;
			}
			if (!system_[i].IsLessInequality()) {
				max_equation_.GetCoefitients()[variable_count + j] = -M;
				basis_[i] = -M;
			}
			system_[i].GetCoefitients()[variable_count + j++] = 1;
			system_[i].GetType() = EquationType::EQUAL;
		}
	}

	T GetMaxim() {
		Equation<T> contribution = max_equation_;

		while (true) {
			// PIVOT COL SELECTION
			size_t pivot_col = 0;
			for (size_t col = 0; col < contribution.VariableCount(); ++col) {
				if (contribution.GetCoefitients()[col] >
					contribution.GetCoefitients()[pivot_col]) {
					pivot_col = col;
				}
			}
			T value = contribution.GetCoefitients()[pivot_col];
			if (contribution.GetCoefitients()[pivot_col] <= THRESHOLD) {
				return -contribution.GetResult();
			}

			// PIVOT ROW SELECTION
			size_t pivot_row = 0;
			bool find_minimum = false;
			for (size_t row = 0; row < system_.size(); ++row) {
				if (system_[row].GetCoefitients()[pivot_col] <= THRESHOLD) {
					continue;
				}
				if (!find_minimum ||
					system_[row].GetResult() / system_[row].GetCoefitients()[pivot_col] <
					system_[pivot_row].GetResult() / system_[pivot_row].GetCoefitients()[pivot_col] || (
						system_[row].GetResult() <= THRESHOLD &&
						system_[row].GetCoefitients()[pivot_col] > system_[pivot_row].GetCoefitients()[pivot_col])) {
					pivot_row = row;
					find_minimum = true;
				}
			}
			if (!find_minimum) {

				std::cout << "\n\nERROR\n\nbasis: { ";
				for (auto var : basis_) {
					std::cout << var << " ";
				}
				std::cout << "}\ncontibution :\n" << contribution << std::endl;
				std::cout << "max_equation :\n" << max_equation_ << "\nsystem:\n";
				Log();
				std::cout << "pivot_col = " << pivot_col << std::endl;
				std::cout << "pivot_row = " << pivot_row << std::endl;

				throw SystemUnbounded{};
			}

			// PIVOT ROTATION
			basis_[pivot_row] = max_equation_.GetCoefitients()[pivot_col];
			system_[pivot_row] /= system_[pivot_row].GetCoefitients()[pivot_col];
			for (size_t row = 0; row < system_.size(); ++row) {
				if (row == pivot_row) {
					continue;
				}
				system_[row] -= system_[pivot_row] * system_[row].GetCoefitients()[pivot_col];
			}
			
			contribution -= system_[pivot_row] * contribution.GetCoefitients()[pivot_col];
			if (contribution.GetResult() < -THRESHOLD) {
				return 1;
			}

#ifdef DEBUG
			std::cout << "basis: { ";
			for (auto var : basis_) {
				std::cout << var << " ";
			}
			std::cout << "}\ncontibution :\n" << contribution << std::endl;
			std::cout << "max_equation :\n" << max_equation_ << "\nsystem:\n";
			Log();
			std::cout << "pivot_col = " << pivot_col << std::endl;
			std::cout << "pivot_row = " << pivot_row << std::endl;
#endif // DEBUG
		}
	}
	void Log() {
		for (auto equation : system_) {
			std::cout << equation << std::endl;
		}
	}
};