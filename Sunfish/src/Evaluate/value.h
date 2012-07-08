/*
 * value.h
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#ifndef VALUE_H_
#define VALUE_H_

#include <climits>
#include <cfloat>

namespace Evaluate {
	template <class T, int vmax, int vmin>
	class TempValue {
	private:
		T value;

	public:
		//typedef decltype(*this) X; // C++11
		typedef TempValue<T, vmax, vmin> X;
		static const T MAX = (T)vmax;
		static const T MIN = (T)vmin;

		TempValue() {
		}

		TempValue(T value) {
			this->value = value;
		}

		operator T() const {
			return value;
		}

		X& operator=(const T& value) {
			this->value = value;
			return *this;
		}

		X& operator+=(T v) {
			value += v;
			return *this;
		}

		X& operator-=(T v) {
			value -= v;
			return *this;
		}

		X& operator+=(const X& v) {
			value += v.value;
			return *this;
		}

		X& operator-=(const X& v) {
			value -= v.value;
			return *this;
		}

		X& operator*=(const X& v) {
			value *= v.value;
			return *this;
		}

		X& operator/=(const X& v) {
			value /= v.value;
			return *this;
		}

		X operator+() const {
			return X(value);
		}

		X operator-() const {
			return X(-value);
		}

		X operator+(T v) const {
			return X(value + v);
		}

		X operator-(T v) const {
			return X(value - v);
		}

		X operator+(const X& v) const {
			return X(value + v.value);
		}

		X operator-(const X& v) const {
			return X(value - v.value);
		}

		/*
		X operator*(T v) const {
			return X(value * v);
		}

		X operator/(T v) const {
			return X(value / v);
		}
		*/

		/*
		X operator*(const X& v) const {
			return X(value * v.value);
		}

		X operator/(const X& v) const {
			return X(value / v.value);
		}
		*/

		bool operator==(const X& v) const {
			return value == v.value;
		}

		bool operator!=(const X& v) const {
			return value != v.value;
		}

		bool operator<(const X& v) const {
			return value < v.value;
		}

		bool operator>(const X& v) const {
			return value > v.value;
		}

		bool operator<=(const X& v) const {
			return value <= v.value;
		}

		bool operator>=(const X& v) const {
			return value >= v.value;
		}

		static X max(X a, X b) {
			return a >= b ? a : b;
		}

		static X min(X a, X b) {
			return a < b ? a : b;
		}
	};

	// INT_MINは-1倍できない。
	typedef TempValue<int, INT_MAX, -INT_MAX> Value;
	typedef TempValue<short, SHRT_MAX, -SHRT_MAX> ValueS;
	typedef TempValue<float, INT_MAX, -INT_MAX> ValueF;
	typedef TempValue<double, INT_MAX, -INT_MAX> ValueD;
}

#endif // VALUE_H_
