#include <boost/test/unit_test.hpp>
#include "moja/flint/timing.h"
#include "moja/datetime.h"
#include "moja/flint/matrixeigen.h"

#include <memory>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>
#include <iomanip>      // std::setprecision

// warning C4996: 'std::_Copy_impl': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct.
#pragma warning( push )
#pragma warning( disable : 4996 )
#include "Eigen/Dense"
#include "Eigen/Sparse"
#pragma warning( pop )

using namespace moja;
using namespace moja::flint;

BOOST_AUTO_TEST_SUITE(MatrixTestsEigen);

typedef Eigen::SparseMatrix<double> EigenSparseMat; // declares a column-major sparse matrix type of double
typedef Eigen::DiagonalMatrix<double, Eigen::Dynamic, Eigen::Dynamic> EigenDiagonal;
typedef Eigen::MatrixXd EigenMat; // declares a dynamic matrix of type double
typedef Eigen::VectorXd EigenVec; // declares a dynamic vector of type double
typedef Eigen::MatrixXd moja_identity;

typedef Eigen::Triplet<double>				test_eigen_triplet;
typedef std::vector<moja_eigen_triplet>		test_eigen_tripletlist;


void eigen_transferPool(EigenMat& _matrix, int source, int sink, double proportion) {
	//Eigen::Triplet<double> t(source, sink, proportion);

	_matrix(source, sink) += proportion;
	_matrix(source, source) -= proportion;
}

void eigen_transferPool_Sparse(test_eigen_tripletlist& tripletList, int source, int sink, double value) {
	tripletList.push_back(moja_eigen_triplet(sink, source, value));
}

template <typename T>
inline T sum_kahan3e(const EigenMat& xs) {
	if (xs.size() == 0) return 0;
	T sumP(0);
	T sumN(0);
	T tP(0);
	T tN(0);
	T cP(0);
	T cN(0);
	T yP(0);
	T yN(0);
	for (size_t i = 0, size = xs.size(); i < size; i++) {
		if ((*(xs.data() + i)) > 0) {
			yP = (*(xs.data() + i)) - cP;
			tP = sumP + yP;
			cP = (tP - sumP) - yP;
			sumP = tP;
		}
		else {
			yN = (*(xs.data() + i)) - cN;
			tN = sumN + yN;
			cN = (tN - sumN) - yN;
			sumN = tN;
		}
	}
	return sumP + sumN;
}


// --------------------------------------------------------------------------------------------

#if 0

BOOST_AUTO_TEST_CASE(flint_Matrix_eigen_DoSomeMatrixStuff) {

	// Shows setIdentity blanks matrix in bounds of the 1's being inserted in diag

	Eigen::Matrix4i m1 = Eigen::Matrix4i::Zero();
	m1.block<3, 3>(1, 0).setIdentity();
	std::cout << "m1" << std::endl;
	std::cout << m1 << std::endl;

	Eigen::Matrix4i m2 = Eigen::Matrix4i::Zero();
	m2(0, 1) = 9;
	m2(0, 2) = 7;
	m2.block<3, 3>(1, 0).setIdentity();
	std::cout << "m2" << std::endl;
	std::cout << m2 << std::endl;

	Eigen::Matrix4i m3 = Eigen::Matrix4i::Zero();
	m3(0, 1) = 9;
	m3(0, 2) = 7;
	m3(1, 2) = 5;
	m3.setIdentity();
	std::cout << "m3" << std::endl;
	std::cout << m3 << std::endl;

	// Play with Vectors

	Eigen::Matrix4i m4 = Eigen::Matrix4i::Identity();

	Eigen::Vector2i v1 = Eigen::Vector2i::Zero();
	std::cout << "v1" << std::endl;
	std::cout << v1 << std::endl;

	Eigen::Vector4i v2 = Eigen::Vector4i::Identity();
	std::cout << "v2" << std::endl;
	std::cout << v2 << std::endl;

	Eigen::Vector4i r_v1 = m4 * v2;
	std::cout << "r_v1" << std::endl;
	std::cout << r_v1 << std::endl;

	//Eigen::Vector4i r_v2 = m4 * v1;
	//std::cout << "r_v2" << std::endl;
	//std::cout << r_v2 << std::endl;
}

#endif

// --------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(moja_Matrix_eigen_iterate_data) {
	Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> test = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(5, 5);

	auto now = time(0);
	sum_kahan3e<double>(test);
}

void simpleConstructAndCalc(int length) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();

	auto startConstruct = clock();

	moja_eigen_matrix _testMatrix;
	_testMatrix.resize(length, length);
	_testMatrix.setIdentity();
	//_testMatrix.setZero();
	moja_eigen_vector _testVector(length);
	_testVector.setRandom(length);

	auto finishConstruct = clock();

	//std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(40) << std::setfill(' ') << testName << ": Mat : " << _testMatrix << std::endl;
	//std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(40) << std::setfill(' ') << testName << ": Vec : " << _testVector << std::endl;

	auto startCalcs = clock();

	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 10000; j++) {
			moja_eigen_vector result = MOJA_EIGEN_MAT_PROD(_testMatrix, _testVector);
		}

	auto finishCalcs = clock();
	auto lengthCalcs = finishCalcs - startCalcs;
	auto lengthConstruct = finishConstruct - startConstruct;

	// CLOCKS_PER_SEC
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(40) << std::setfill(' ') << testName << " : For size (" << length << "): construction : " << lengthConstruct << std::endl;
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(40) << std::setfill(' ') << testName << " : For size (" << length << "): calculations : " << lengthCalcs << std::endl;
}

// --------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(flint_Matrix_eigen_SpeedChecks) {
	
	for (int i = 10; i <= 60; i+=10)
		simpleConstructAndCalc(i);
}


// --------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(moja_Matrix_eigen_001) {
	const int numVars = 6;
	EigenVec pools(numVars);
	pools.setZero();

	pools[0] = 0.02;
	pools[1] = 8.681;
	pools[2] = 0.05;
	pools[3] = 0;
	pools[4] = 20.96;
	pools[5] = 0;

	EigenMat _I(numVars, numVars);
	_I.setIdentity();

	EigenDiagonal diag = pools.asDiagonal();
	EigenMat forOutput = diag;

	EigenMat matrix(numVars, numVars);
	matrix.setIdentity();

	eigen_transferPool(matrix, 0, 5, 0.250852);
	eigen_transferPool(matrix, 1, 5, 0.012596);
	eigen_transferPool(matrix, 2, 5, 0);
	eigen_transferPool(matrix, 3, 5, 0.0190627);
	eigen_transferPool(matrix, 4, 5, 0.00126915);
	eigen_transferPool(matrix, 0, 2, 0.0397643);
	eigen_transferPool(matrix, 1, 2, 0.00199667);
	eigen_transferPool(matrix, 3, 2, 0.00302176);
	eigen_transferPool(matrix, 4, 3, 0);
	eigen_transferPool(matrix, 0, 4, 0.0413873);
	eigen_transferPool(matrix, 1, 4, 0.00207817);
	eigen_transferPool(matrix, 2, 4, 0);
	eigen_transferPool(matrix, 3, 4, 0.00314509);
	eigen_transferPool(matrix, 4, 4, 0.000410574);

	auto a = matrix - _I;
	auto b = diag * a;
	auto flux = b * 1; // timeScale

	pools = pools.transpose() * matrix; // timeScale

	BOOST_CHECK_CLOSE(pools[0], 0.013359927999999998, 0.000000000000001);
	BOOST_CHECK_CLOSE(pools[1], 8.5362804379599986, 0.000000000000001);
	BOOST_CHECK_CLOSE(pools[2], 0.068128378269999998, 0.000000000000001);
	BOOST_CHECK_CLOSE(pools[3], 0, 0.000000000000001);
	BOOST_CHECK_CLOSE(pools[4], 20.952266955769996, 0.000000000000001);
	BOOST_CHECK_CLOSE(pools[5], 0.14096429999999999, 0.000000000000001);
}

// --------------------------------------------------------------------------------------------
// NOTE: Eigen will NOT remove the 0.0 results from the final sparse matrix 

BOOST_AUTO_TEST_CASE(moja_Matrix_eigen_zero_transfer_test) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();

	const int numVars = 6;
	EigenVec pools(numVars);
	pools.setZero();

	pools[0] = 0.02;
	pools[1] = 8.681;
	pools[2] = 0.05;
	pools[3] = 0;
	pools[4] = 20.96;
	pools[5] = 0;

	EigenMat _I(numVars, numVars);
	_I.setIdentity();

	EigenDiagonal diag = pools.asDiagonal();
	test_eigen_tripletlist tripletList;
	EigenSparseMat matrix;
	matrix.resize(numVars, numVars);
	matrix.reserve(5);
	//matrix.setZero();

	//std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << ": matrix blank" << std::endl;
	//std::cout << matrix << std::endl;

	eigen_transferPool_Sparse(tripletList, 0, 5, 0.0);
	eigen_transferPool_Sparse(tripletList, 0, 4, 0.1);

	matrix.setFromTriplets(tripletList.begin(), tripletList.end());

	//std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << ": matrix set" << std::endl;
	//std::cout << matrix << std::endl;

	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << ": matrix iteration" << std::endl;
	for (int k = 0; k < matrix.outerSize(); ++k) {
		for (EigenSparseMat::InnerIterator flux(matrix, k); flux; ++flux) {
			auto srcIx = flux.col();
			auto dstIx = flux.row();
			auto val = flux.value();

			std::cout << "Src: " << srcIx << ", Dst: " << dstIx << ", Val: " << val << std::endl;
		}
	}

	EigenSparseMat fluxes = matrix * diag;

	//std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << ": fluxes" << std::endl;
	//std::cout << fluxes << std::endl;

	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << ": fluxes iteration" << std::endl;
	for (int k = 0; k < fluxes.outerSize(); ++k) {
		for (EigenSparseMat::InnerIterator flux(fluxes, k); flux; ++flux) {
			auto srcIx = flux.col();
			auto dstIx = flux.row();
			auto val = flux.value();

			std::cout << "Src: " << srcIx << ", Dst: " << dstIx << ", Val: " << val << std::endl;
		}
	}

}

BOOST_AUTO_TEST_SUITE_END();
