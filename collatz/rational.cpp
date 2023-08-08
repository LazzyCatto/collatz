#include "rational.h"

void Rational::Simplify() {
	if (denominator_ == 0) {
		throw RationalDivisionByZero{};
	}

	int64_t other;
	if (numerator_ >= 0) {
		other = static_cast<int64_t>(numerator_);
	} else {
		other = static_cast<int64_t>(-numerator_);
	}
	int64_t gcd = denominator_;
	while (other != 0) {
		gcd %= other;
		std::swap(gcd, other);
	}

	numerator_ /= gcd;
	denominator_ /= gcd;
}

Rational::Rational(int64_t numerator, int64_t denominator) : numerator_(numerator), denominator_(denominator) {
	Simplify();
}

Rational::Rational(long double value, size_t aproximation_presidion) {
	int64_t p_previous = 1;
	int64_t p_current = static_cast<int64_t>(std::floorl(value));
	int64_t q_previous = 0;
	int64_t q_current = 1;

	if (value - p_current < 0.0000001L) {
		numerator_ = p_current;
		denominator_ = q_current;

		Simplify();
		return;
	}

	value = 1 / (value - p_current);
	for (size_t i = 0; i < aproximation_presidion; ++i) {
		int64_t whole_part = static_cast<int64_t>(std::floorl(value));
		
		int64_t p_new = p_current * whole_part + p_previous;
		int64_t q_new = q_current * whole_part + q_previous;

		p_previous = p_current;
		p_current = p_new;

		q_previous = q_current;
		q_current = q_new;

		if (value - p_current < 0.0000001L) {
			break;
		}
		value = 1 / (value - whole_part);
	}

	numerator_ = p_current;
	denominator_ = q_current;

	Simplify();
}

int64_t Rational::GetNumerator() const {
	return numerator_;
}

int64_t Rational::GetDenominator() const {
	return denominator_;
}

void Rational::SetNumerator(int64_t numerator) {
	numerator_ = numerator;
	Simplify();
}

void Rational::SetDenominator(int64_t denominator) {
	denominator_ = denominator;
	Simplify();
}

Rational Rational::GetInverce() const {
	if (numerator_ >= 0) {
		return { static_cast<int64_t>(denominator_), static_cast<int64_t>(numerator_) };
	}
	return { -static_cast<int64_t>(denominator_), static_cast<int64_t>(-numerator_) };
}

Rational::operator long double() const {
	return static_cast<long double>(numerator_) / static_cast<long double>(denominator_);
}

Rational Rational::operator+() const {
	return { +numerator_, denominator_ };
}

Rational Rational::operator-() const {
	return { -numerator_, denominator_ };
}

Rational& Rational::operator++() {
	numerator_ += denominator_;
	return *this;
}

Rational Rational::operator++(int) {
	Rational reserve{ numerator_, denominator_ };
	numerator_ += denominator_;
	return reserve;
}

Rational& Rational::operator--() {
	numerator_ -= denominator_;
	return *this;
}

Rational Rational::operator--(int) {
	Rational reserve{ numerator_, denominator_ };
	numerator_ -= denominator_;
	return reserve;
}

Rational& Rational::operator+=(const Rational& rhs) {
	numerator_ = numerator_ * rhs.denominator_ + rhs.numerator_ * denominator_;
	denominator_ *= rhs.denominator_;
	Simplify();
	return *this;
}

Rational& Rational::operator-=(const Rational& rhs) {
	return *this += -rhs;
}

Rational& Rational::operator*=(const Rational& rhs) {
	numerator_ *= rhs.numerator_;
	denominator_ *= rhs.denominator_;
	Simplify();
	return *this;
}

Rational& Rational::operator/=(const Rational& rhs) {
	return *this *= rhs.GetInverce();
}

Rational operator+(Rational lhs, const Rational& rhs) {
	return lhs += rhs;
}

Rational operator-(Rational lhs, const Rational& rhs) {
	return lhs -= rhs;
}

Rational operator*(Rational lhs, const Rational& rhs) {
	return lhs *= rhs;
}

Rational operator/(Rational lhs, const Rational& rhs) {
	return lhs /= rhs;
}

bool operator==(const Rational& lhs, const Rational& rhs) {
	return (lhs.GetNumerator() == rhs.GetNumerator() && lhs.GetDenominator() == rhs.GetDenominator());
}

bool operator!=(const Rational& lhs, const Rational& rhs) {
	return !(lhs == rhs);
}

bool operator>(const Rational& lhs, const Rational& rhs) {
	return (lhs - rhs).GetNumerator() > 0;
}

bool operator<(const Rational& lhs, const Rational& rhs) {
	return rhs > lhs;
}

bool operator>=(const Rational& lhs, const Rational& rhs) {
	return !(lhs < rhs);
}

bool operator<=(const Rational& lhs, const Rational& rhs) {
	return !(lhs > rhs);
}

std::ostream& operator<<(std::ostream& out, const Rational& rhs) {
	if (rhs.GetDenominator() == 1) {
		return out << rhs.GetNumerator();
	}
	return out << rhs.GetNumerator() << '/' << rhs.GetDenominator();
}

std::istream& operator>>(std::istream& in, Rational& rhs) {
	int64_t numerator;
	int64_t denominator = 1;
	in >> numerator;
	if (in.peek() == '/') {
		char temp;
		in >> temp >> denominator;
	}

	rhs.SetNumerator(numerator);
	rhs.SetDenominator(denominator);
	return in;
}