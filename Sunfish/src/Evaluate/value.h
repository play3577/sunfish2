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
	template <class T, int max, int min>
	class TempValue {
	private:
		T value;

	public:
		//typedef decltype(*this) X; // C++11
		typedef TempValue<T, max, min> X;
		static const T MAX = (T)max;
		static const T MIN = (T)min;

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

		X& operator=(const X& v) {
			value = v.value;
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

		X operator+() const {
			return X(value);
		}

		X operator-() const {
			return X(-value);
		}

		X operator+(const X& v) const {
			return X(value + v.value);
		}

		X operator-(const X& v) const {
			return X(value - v.value);
		}

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
	};

	typedef TempValue<int, INT_MAX, INT_MIN> Value;
	typedef TempValue<short, SHRT_MAX, SHRT_MIN> ValueS;
	typedef TempValue<float, INT_MAX, INT_MIN> ValueF;
}

#endif // VALUE_H_
