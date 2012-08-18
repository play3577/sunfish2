/*
 * estimate.h
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#ifndef ESTIMATE_H_
#define ESTIMATE_H_

#include "param.h"

namespace Evaluate {
	template <class T>
	class Estimate {
	private:
		T value;
		T error;

	public:
		Estimate() {
		}

		Estimate(T value, T error) :
			value(value), error(error){
		}

		void setValue(T value) {
			this->value = value;
		}

		void setError(T error) {
			this->error = error;
		}

		T getValue() const {
			return value;
		}

		T getError() const {
			return error;
		}

		Estimate operator-() {
			return Estimate(-value, error);
		}
	};
}

#endif // ESTIMATE_H_
