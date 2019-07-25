#ifndef MOJA_FLINT_MATRIXUBLAS_H_
#define MOJA_FLINT_MATRIXUBLAS_H_

#define BOOST_ALL_NO_LIB
#define BOOST_UBLAS_NDEBUG  // cuts some time off run
                            // (http://www.boost.org/doc/libs/1_49_0/libs/numeric/ublas/doc/index.htm)

// warning C4996: 'std::_Uninitialized_copy0': Function call with parameters that may be unsafe - this call relies on
// the caller to check that the passed values are correct.
#pragma warning(push)
#pragma warning(disable : 4996)
// Work-around needed for ublas when using boost >= 1.64
//#include <boost/serialization/array_wrapper.hpp>
#include <boost/math/tools/precision.hpp>
#include <boost/numeric/ublas/banded.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <boost/numeric/ublas/vector.hpp>
#pragma warning(pop)

namespace moja {
namespace flint {

namespace ublas = boost::numeric::ublas;
typedef ublas::compressed_matrix<double> moja_ublas_matrix;
typedef ublas::diagonal_matrix<double> moja_ublas_diagonal;
typedef ublas::identity_matrix<double> moja_ublas_identity;
typedef ublas::matrix<double> moja_ublas_fullmatrix;
typedef ublas::vector<double> moja_ublas_vector;

#define MOJA_UBLAS_MAT_PROD(a, b) ublas::prec_prod(a, b)
#define MOJA_UBLAS_MAKE_I(i, s) \
   { i = moja_ublas_identity(s); }

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_MATRIXUBLAS_H_
