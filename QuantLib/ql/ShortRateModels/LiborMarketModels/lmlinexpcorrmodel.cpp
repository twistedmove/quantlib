/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Klaus Spanderen

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/Math/pseudosqrt.hpp>
#include <ql/ShortRateModels/LiborMarketModels/lmlinexpcorrmodel.hpp>

namespace QuantLib {

    LmLinearExponentialCorrelationModel::LmLinearExponentialCorrelationModel(
        Size size, Real rho, Real beta)
   : LmCorrelationModel(size, 2),
     corrMatrix_(size, size),
     pseudoSqrt_(size, size) {
        arguments_[0] = ConstantParameter(rho, BoundaryConstraint(-1.0, 1.0));
        arguments_[1] = ConstantParameter(beta, PositiveConstraint());
        generateArguments();
    }

    Disposable<Matrix> LmLinearExponentialCorrelationModel::correlation(
                                                   Time, const Array&) const {
        Matrix tmp(corrMatrix_);
        return tmp;
    }

    Real LmLinearExponentialCorrelationModel::correlation(
                                   Size i, Size j, Time, const Array&) const {
        return corrMatrix_[i][j];
    }

    bool LmLinearExponentialCorrelationModel::isTimeIndependent() const {
        return true;
    }

    Disposable<Matrix> LmLinearExponentialCorrelationModel::pseudoSqrt(
                                                   Time, const Array&) const {
        Matrix tmp(pseudoSqrt_);
        return tmp;
    }

    void LmLinearExponentialCorrelationModel::generateArguments() {
        const Real rho = arguments_[0](0.0);
        const Real beta= arguments_[1](0.0);

        for (Size i=0; i<size_; ++i) {
            for (Size j=i; j<size_; ++j) {
                corrMatrix_[i][j] = corrMatrix_[j][i]
                    = rho + (1-rho)*std::exp(-beta*std::fabs(Real(i)-Real(j)));
            }
        }

        pseudoSqrt_ = QuantLib::pseudoSqrt(corrMatrix_,
                                           SalvagingAlgorithm::Spectral);
    }

}

