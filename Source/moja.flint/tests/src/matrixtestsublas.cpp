#include <boost/test/unit_test.hpp>

//warning C4996: 'std::_Uninitialized_copy0': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct.
#pragma warning( push )
#pragma warning( disable : 4996 )
//Work-around needed for ublas when using boost >= 1.64
//#include <boost/serialization/array_wrapper.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/banded.hpp>
#include <boost/numeric/ublas/io.hpp> 
#pragma warning( pop )

#include <boost/test/floating_point_comparison.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>
#include <iomanip>      // std::setprecision

BOOST_AUTO_TEST_SUITE(MatrixTestsuBlas);

namespace ublas = boost::numeric::ublas;
typedef ublas::vector<double> moja_vector;
typedef ublas::compressed_matrix<double> moja_matrix;
typedef ublas::diagonal_matrix<double> moja_diagonal;
typedef ublas::identity_matrix<double> moja_identity;
typedef moja_matrix::iterator1 it1_t;
typedef moja_matrix::iterator2 it2_t;

void transferPool(moja_matrix& _matrix, int source, int sink, double proportion) {
	_matrix(source, sink  ) += proportion;
	_matrix(source, source) -= proportion;
}

BOOST_AUTO_TEST_CASE(flint_Matrix_flux) {
	const int test_size = 6;

	moja_vector pools(test_size);
	pools[0] = 0.02;
	pools[1] = 8.681;
	pools[2] = 0.05;
	pools[3] = 0;
	pools[4] = 20.96;
	pools[5] = 0;

	moja_identity _I(test_size);
	moja_diagonal diag(test_size, pools.data());
	moja_matrix matrix(_I);

	transferPool(matrix, 0,5,0.250852    );
	transferPool(matrix, 1,5,0.012596	 );
	transferPool(matrix, 2,5,0			 );
	transferPool(matrix, 3,5,0.0190627	 );
	transferPool(matrix, 4,5,0.00126915	 );
	transferPool(matrix, 0,2,0.0397643	 );
	transferPool(matrix, 1,2,0.00199667	 );
	transferPool(matrix, 3,2,0.00302176	 );
	transferPool(matrix, 4,3,0			 );
	transferPool(matrix, 0,4,0.0413873	 );
	transferPool(matrix, 1,4,0.00207817	 );
	transferPool(matrix, 2,4,0			 );
	transferPool(matrix, 3,4,0.00314509	 );
	transferPool(matrix, 4,4,0.000410574 );

	// Flux
	//-0.00664007          0  0.000795285  0  0.000827746  0.00501704
	//	        0 - 0.144719    0.0173331  0    0.0180406    0.109346
	//	        0          0            0  0            0           0
	//	        0          0            0  0            0           0
	//	        0          0            0  0  - 0.0266013   0.0266013
	//	        0          0            0  0            0           0

	auto a = matrix - _I;
	auto b = ublas::prec_prod(diag, a);
	auto flux = b * 1; // timeScale

	pools = ublas::prec_prod(pools, matrix) * 1; // timeScale

	BOOST_CHECK_CLOSE(pools[0], 0.013359927999999998, 0.000000000000001);
	BOOST_CHECK_CLOSE(pools[1], 8.5362804379599986	, 0.000000000000001);
	BOOST_CHECK_CLOSE(pools[2], 0.068128378269999998, 0.000000000000001);
	BOOST_CHECK_CLOSE(pools[3], 0					, 0.000000000000001);
	BOOST_CHECK_CLOSE(pools[4], 20.952266955769996	, 0.000000000000001);
	BOOST_CHECK_CLOSE(pools[5], 0.14096429999999999	, 0.000000000000001);
}

// --------------------------------------------------------------------------------------------
// NOTE: UBLAS will remove the 0.0 results from the final sparse matrix 

BOOST_AUTO_TEST_CASE(moja_Matrix_ublas_zero_transfer_test) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();

	const int test_size = 6;
	moja_vector pools(test_size);

	pools[0] = 0.02;
	pools[1] = 8.681;
	pools[2] = 0.05;
	pools[3] = 0;
	pools[4] = 20.96;
	pools[5] = 0;

	moja_diagonal diag(test_size, pools.data());

	moja_matrix matrix(test_size, test_size, test_size);

	//std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << ": matrix blank" << std::endl;
	//std::cout << matrix << std::endl;

	matrix(0, 5) = 0.0;
	matrix(0, 4) = 0.1;

	//std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << ": matrix set" << std::endl;
	//std::cout << matrix << std::endl;

	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << ": matrix iteration" << std::endl;
	for (it1_t it1 = matrix.begin1(); it1 != matrix.end1(); ++it1) {
		for (it2_t it2 = it1.begin(); it2 != it1.end(); ++it2) {
			//std::cout << "(" << it2.index1() << "," << it2.index2() << ") = ";
			//std::cout << *it2 << std::endl;
			auto srcIx = it2.index1();
			auto dstIx = it2.index2();
			auto val = *it2;
			std::cout << "Src: " << srcIx << ", Dst: " << dstIx << ", Val: " << val << std::endl;
		}
	}

	moja_matrix fluxes = ublas::prec_prod(diag, matrix);

	//std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << ": fluxes" << std::endl;
	//std::cout << fluxes << std::endl;

	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << ": fluxes iteration" << std::endl;
	for (it1_t it1 = fluxes.begin1(); it1 != fluxes.end1(); ++it1) {
		for (it2_t it2 = it1.begin(); it2 != it1.end(); ++it2) {
			//std::cout << "(" << it2.index1() << "," << it2.index2() << ") = ";
			//std::cout << *it2 << std::endl;
			auto srcIx = it2.index1();
			auto dstIx = it2.index2();
			auto val = *it2;
			std::cout << "Src: " << srcIx << ", Dst: " << dstIx << ", Val: " << val << std::endl;
		}
	}

}

BOOST_AUTO_TEST_SUITE_END();
