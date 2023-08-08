#pragma once

#include <cstdint>
#include <stdexcept>
#include <iostream>

class RationalDivisionByZero : std::logic_error {
public:
	RationalDivisionByZero() : std::logic_error("RationalDivisionByZero") {}
};

class Rational {
private:
	int64_t numerator_;
	int64_t denominator_;

	void Simplify();

public:
	Rational(int64_t numerator = 0, int64_t denominator = 1);
	Rational(long double value, size_t aproximation_presidion);

	int64_t GetNumerator() const;
	int64_t GetDenominator() const;

	void SetNumerator(int64_t numerator);
	void SetDenominator(int64_t denominator);

	Rational GetInverce() const;

	operator long double() const ;

	Rational operator+() const;
	Rational operator-() const;

	Rational& operator++();
	Rational operator++(int);
	Rational& operator--();
	Rational operator--(int);

	Rational& operator+=(const Rational& rhs);
	Rational& operator-=(const Rational& rhs);
	Rational& operator*=(const Rational& rhs);
	Rational& operator/=(const Rational& rhs);

	friend Rational operator+(Rational lhs, const Rational& rhs);
	friend Rational operator-(Rational lhs, const Rational& rhs);
	friend Rational operator*(Rational lhs, const Rational& rhs);
	friend Rational operator/(Rational lhs, const Rational& rhs);

	friend bool operator==(const Rational& lhs, const Rational& rhs);
	friend bool operator!=(const Rational& lhs, const Rational& rhs);

	friend bool operator>(const Rational& lhs, const Rational& rhs);
	friend bool operator<(const Rational& lhs, const Rational& rhs);
	friend bool operator>=(const Rational& lhs, const Rational& rhs);
	friend bool operator<=(const Rational& lhs, const Rational& rhs);
};

std::ostream& operator<<(std::ostream& out, const Rational& rhs);
std::istream& operator>>(std::istream& in, Rational& rhs);