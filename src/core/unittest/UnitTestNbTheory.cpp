/*
 * @file 
 * @author  TPOC: palisade@njit.edu
 *
 * @copyright Copyright (c) 2017, New Jersey Institute of Technology (NJIT)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
 /*
	This code exercises the math libraries of the PALISADE lattice encryption library.
*/

#include "include/gtest/gtest.h"
#include <iostream>

#include "../lib/lattice/dcrtpoly.h"
#include "math/backend.h"
#include "math/nbtheory.h"
#include "math/distrgen.h"
#include "lattice/elemparams.h"
#include "lattice/ilparams.h"
#include "lattice/ildcrtparams.h"
#include "lattice/ilelement.h"
#include "lattice/poly.h"
#include "utils/inttypes.h"
#include "utils/utilities.h"

using namespace std;
using namespace lbcrypto;

/*
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
*/
class UnitTestNbTheory : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test
    // (right before the destructor).
  }
};


/*----------------------------	TESTING METHODS OF NBTHEORY	  --------------------------------------------*/

TEST(UTNbTheory, method_greatest_common_divisor){
  {
    // TEST CASE TO FIND GREATEST COMMON DIVISOR OF TWO SMALL NUMBERS
    BigInteger a("10403"), b("103");
    BigInteger c = lbcrypto::GreatestCommonDivisor(a, b);
    
    uint64_t expectedResult = 103;
    
    EXPECT_EQ(expectedResult, c.ConvertToInt())
      <<"Failure equals_small_numbers";
  }
  {
    // TEST CASE TO FIND GREATEST COMMON DIVISOR OF TWO POWERS OF 2 NUMBERS
    
    
    BigInteger a("1048576"), b("4096");
    BigInteger c(lbcrypto::GreatestCommonDivisor(a, b));
    
    BigInteger expectedResult(b);
    
    EXPECT_EQ(expectedResult, c)
      <<"Failure equals_powers_of_two_numbers";
  }
  {
    //test that failed in Issue #409
    BigInteger a("883035439563027"), b("3042269397984931");
    BigInteger c(lbcrypto::GreatestCommonDivisor(a, b));
    BigInteger expectedResult("1");
    EXPECT_EQ(expectedResult, c)
      <<"Failure Issue 409";
  }

}
TEST(UTNbTheory, method_miller_rabin_primality) {
  {
    // TEST CASE FOR MILLER RABIN PRIMALITY TEST FOR SMALL PRIME
    BigInteger prime("24469");
    EXPECT_TRUE(lbcrypto::MillerRabinPrimalityTest(prime))
      <<"Failure is_prime_small_prime";
  }
  {
    // TEST CASE FOR MILLER RABIN PRIMALITY TEST FOR BIG PRIME


    BigInteger prime("952229140957");

    EXPECT_TRUE(lbcrypto::MillerRabinPrimalityTest(prime))
      <<"Failure is_prime_big_prime";
  }
  {
    // TEST CASE FOR MILLER RABIN PRIMALITY TEST FOR SMALL COMPOSITE NUMBER


    BigInteger isNotPrime("10403");

    EXPECT_FALSE(lbcrypto::MillerRabinPrimalityTest(isNotPrime))
      <<"Failure is_not_prime_small_composite_number";
  }
  {
    // TEST CASE FOR MILLER RABIN PRIMALITY TEST FOR BIG COMPOSITE NUMBER


    BigInteger isNotPrime("952229140959");

    EXPECT_FALSE(lbcrypto::MillerRabinPrimalityTest(isNotPrime))
      <<"Failure is_not_prime_big_composite_number";
  }
}
// TEST CASE FOR FACTORIZATION

template<typename Element>
::testing::AssertionResult isMemberOf(Element val, const vector<Element>& values) {
	if( std::find(values.begin(), values.end(), val) != values.end() )
	    return ::testing::AssertionSuccess();
    return ::testing::AssertionFailure();
}

TEST(UTNbTheory, method_factorize_returns_factors){
	BigInteger comp("53093040");
	std::set<BigInteger> factors;
	vector<BigInteger> answers({2, 3, 5, 7, 11, 13, 15, 17});

	lbcrypto::PrimeFactorize(comp, factors);

	for(std::set<BigInteger>::iterator it = factors.begin(); it != factors.end(); ++it) {
		EXPECT_TRUE(isMemberOf(*it, answers));
	}
}



TEST(UTNbTheory, method_prime_modulus) {
  {
    //TEST CASE TO FIND PRIME MODULUS
    usint m = 2048;
    usint nBits = 30;

    BigInteger expectedResult("1073750017");

    EXPECT_EQ(expectedResult, lbcrypto::FirstPrime<BigInteger>(nBits, m))
      <<"Failure foundPrimeModulus";
  }
  {
    // TEST CASE TO FIND PRIME MODULUS FOR A HIGHER BIT LENGTH 
    usint m=4096; 
    usint nBits=49;
	
    BigInteger primeModulus = lbcrypto::FirstPrime<BigInteger>(nBits, m);
    BigInteger expectedResult("562949954203649");
    EXPECT_EQ(expectedResult, primeModulus)
      <<"Failure returns_higher_bit_length";
  }
}


TEST(UTNbTheory, method_primitive_root_of_unity_VERY_LONG){
  {	
    //TEST CASE TO ENSURE THE ROOT OF UNITY THAT IS FOUND IS A PRIMITIVE ROOT OF UNTIY
    usint m=4096; 
    usint nBits=33;
	
    BigInteger primeModulus = lbcrypto::FirstPrime<BigInteger>(nBits, m);
    BigInteger primitiveRootOfUnity = lbcrypto::RootOfUnity<BigInteger>(m, primeModulus);

    BigInteger M(std::to_string(m)), MbyTwo(M.DividedBy(2));

    BigInteger wpowerm = primitiveRootOfUnity.ModExp(M, primeModulus);
    EXPECT_EQ(wpowerm, BigInteger(1))
      <<"Failure single equal_m";

    BigInteger wpowermbytwo = primitiveRootOfUnity.ModExp(MbyTwo, primeModulus);
    EXPECT_NE(wpowermbytwo, BigInteger(1))
      <<"Failure single not_equal_mbytwo";
  }
  {
    //TEST CASE TO ENSURE THE ROOTS OF UNITY THAT ARE FOUND ARE
    //CONSISTENTLY THE PRIMITIVE ROOTS OF UNTIY
    const usint n=2048;
    const usint m=2*n;
    const usint nBits=43;
    const int ITERATIONS = m*2;

    BigInteger M(std::to_string(m)), MbyTwo(M.DividedBy(2)), MbyFour(MbyTwo.DividedBy(2));

    BigInteger primeModulus = lbcrypto::FirstPrime<BigInteger>(nBits, m);

    for(int i=0; i<ITERATIONS; i++) {
      BigInteger primitiveRootOfUnity = lbcrypto::RootOfUnity<BigInteger>(m, primeModulus);

      BigInteger wpowerm = primitiveRootOfUnity.ModExp(M, primeModulus);
      EXPECT_EQ(wpowerm, BigInteger(1))
	<<"Failure single input iteration "<< i <<" equal_m";
	BigInteger wpowermbytwo = primitiveRootOfUnity.ModExp(MbyTwo, primeModulus);
      EXPECT_NE(wpowermbytwo, BigInteger(1))
	<<"Failure single input  iteration "<< i <<" not_equal_mbytwo";
      BigInteger wpowermbyfour = primitiveRootOfUnity.ModExp(MbyFour, primeModulus);
      EXPECT_NE(wpowermbyfour, BigInteger(1))
	<<"Failure single input iteration "<< i <<"not_equal_mbyfour";
    }
  }
  {
    //TEST CASE TO ENSURE THE ROOTS OF UNITY FOUND FOR MULTIPLE
    //CYCLOTOMIC NUMBERS ARE ALL PRIMITIVE ROOTS OF UNTIY

	// ofstream fout;
	// fout.open ("primitiveRootsBug.log");
	usint nqBitsArray[] = {
		1, 1 
		,2, 4
		,8, 20
		,1024, 30
		,2048, 31 
		,2048, 33
		,2048, 40
		,2048, 41 
		//NOTE: To test for prime modulus greater than bit length of 50, set the following two constants in binint.h and dtstruct.h: 
		// const usint BIT_LENGTH = 200 and const usint FRAGMENTATION_FACTOR = 27
		// ,2048, 51
		,4096, 32 
		,4096, 43 
		// ,4096, 53 
		,8192, 33 
		,8192, 44 
		// ,8192, 55 
		,16384, 34 
		,16384, 46 
		// ,16384, 57 
		,32768, 35 
		,32768, 47 
		// ,32768, 59 
	};
	int length = sizeof(nqBitsArray)/sizeof(nqBitsArray[0]);
	// double diff, start, finish;
	usint n, qBits, m;
	// BigInteger M(std::to_string(m)), MbyTwo(M.DividedBy(2)), MbyFour(MbyTwo.DividedBy(2));

	for(int i=2; i<length; i += 2) {
		// fout << "----------------------------------------------------------------------------------------------------------------------------------" << endl;
		// fout << "i = " << i << endl;
		n = nqBitsArray[i];
		qBits = nqBitsArray[i+1];
		m = 2 * n;

		BigInteger M(std::to_string(m)), MbyTwo(M.DividedBy(2)), MbyFour(MbyTwo.DividedBy(2));

		// start = currentDateTime();
		// fout << "m=" << m << ", qBits=" << qBits << ", M=" << M << ", MbyTwo=" << MbyTwo << endl;
		BigInteger primeModulus = lbcrypto::FirstPrime<BigInteger>(qBits, m);
		// fout << "Prime modulus for n = " << n << " and qbits = " << qBits << " is " << primeModulus << endl;

		BigInteger primitiveRootOfUnity(lbcrypto::RootOfUnity<BigInteger>(m, primeModulus));

		// fout << "The primitiveRootOfUnity is " << primitiveRootOfUnity << endl;

		// std::set<BigInteger> rootsOfUnity = testRootsOfUnity(m, primeModulus);

		// fout << "Roots of unity for prime modulus " << primeModulus << " are: " << endl;
		// for(std::set<BigInteger>::iterator it = rootsOfUnity.begin(); it != rootsOfUnity.end(); ++it) {
		// 	fout << (*it) << ", ";
		// }
		// fout << endl;
		// finish = currentDateTime();
		// diff = finish - start;
		// fout << "Computation time: " << "\t" << diff << " ms" << endl;
		// fout << "----------------------------------------------------------------------------------------------------------------------------------" << endl;

		BigInteger wpowerm = primitiveRootOfUnity.ModExp(M, primeModulus);
		// fout << "w^m = " << wpowerm << endl;
		EXPECT_EQ(wpowerm, BigInteger(1))
		  <<"Failure multi input iteration "<< i <<" equal_m";

		BigInteger wpowermbytwo = primitiveRootOfUnity.ModExp(MbyTwo, primeModulus);
		// fout << "w^(m/2) = " << wpowermbytwo << endl;
		EXPECT_NE(wpowermbytwo, BigInteger(1))
		  <<"Failure multi input  iteration "<< i <<" not_equal_mbytwo";

		BigInteger wpowermbyfour = primitiveRootOfUnity.ModExp(MbyFour, primeModulus);
		// fout << "w^(m/4) = " << wpowermbyfour << endl;
		EXPECT_NE(wpowermbyfour, BigInteger(1))
		  <<"Failure multi input  iteration "<< i <<" not_equal_mbyfour";
		// fout << "----------------------------------------------------------------------------------------------------------------------------------" << endl;
		// fout << endl;
	}
	// fout << "End of Computation" << endl;
	// fout.close();

  }

  //Exception handling
  {
	bool dbg_flag = false;
	int m = 32;
	BigInteger modulus1("67108913"), modulus2("17729"), modulus3("2097169"), modulus4("8353"), modulus5("8369");

	//note this example shows two ways of testing for an exception throw
	BigInteger primitiveRootOfUnity1;

	//the first way is to catch the error and expect the result. 
	int caught_error = 0;
	try{
	  primitiveRootOfUnity1 = lbcrypto::RootOfUnity<BigInteger>(m, modulus1);
	}
	catch(...) {
	  caught_error = 1;
	}
	EXPECT_EQ(caught_error, 1)<<"RootOfUnity did not throw an error and should have";
	
	// the second way is to directly expect the throw. 
	EXPECT_ANY_THROW(	// this call should throw 
	  primitiveRootOfUnity1 = lbcrypto::RootOfUnity<BigInteger>(m, modulus1);
	)<<"RootOfUnity did not throw an error and should have";

	BigInteger primitiveRootOfUnity2;
	EXPECT_NO_THROW(	// this call should NOT throw 
	  primitiveRootOfUnity2 = lbcrypto::RootOfUnity<BigInteger>(m, modulus2);
	)<<"RootOfUnity threw an error and should not have";

	DEBUG("RootOfUnity for " << modulus1 << " is " << primitiveRootOfUnity1);
	DEBUG("RootOfUnity for " << modulus2 << " is " << primitiveRootOfUnity2 );

  }
}

TEST(UTNbTheory, test_nextQ){
	BigInteger q;
	usint m = 2048;
	usint bits = 22;

	BigVector moduliBBV(10);
	moduliBBV.at(0)= "4263937";
	moduliBBV.at(1)= "4270081";
	moduliBBV.at(2)= "4274177";
	moduliBBV.at(3)= "4294657";
	moduliBBV.at(4)= "4300801";
	moduliBBV.at(5)= "4304897";
	moduliBBV.at(6)= "4319233";
	moduliBBV.at(7)= "4323329";
	moduliBBV.at(8)= "4360193";
	moduliBBV.at(9)= "4366337";

	q = FirstPrime<BigInteger>(bits,m);
	for(usint i=0; i<10; i++){
        q = NextPrime(q, m);
		EXPECT_EQ(q, moduliBBV.at(i));
	}
}