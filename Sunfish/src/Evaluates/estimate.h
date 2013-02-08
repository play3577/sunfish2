/*
 * estimate.h
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#ifndef ESTIMATE_H_
#define ESTIMATE_H_

#include "param.h"

namespace Evaluates {
	class Estimate {
	private:
		Value value;
		Value error;

	public:
		Estimate() {
		}

		Estimate(Value value, Value error) :
			value(value), error(error){
		}

		void setValue(Value value) {
			this->value = value;
		}

		void setError(Value error) {
			this->error = error;
		}

		Value getValue() const {
			return value;
		}

		Value getError() const {
			return error;
		}

		Estimate operator-() {
			return Estimate(-value, error);
		}
	};
}

#endif // ESTIMATE_H_
