#include "stdafx.h"
#include "../../EmbDB/CompressDoubleDiff.h"
#include "../../EmbDB/CompressDouble.h"
#include <math.h>
#include <cmath>
#include <stdio.h>

struct SDoubleCast 
{
	bool m_bSign;
	uint32 m_nExponent;
	uint64 m_nFraction;
};


union doublebits {
	double d;
	struct {
		unsigned long long mantissa1:52;
		unsigned long long exponent:11;
		unsigned long long negative:1;
		
	};
};

#define MAX 100
//#define eps 1e-9
#define eps 0
int64 p[MAX], q[MAX], a[MAX], len;
void getFraq1(double x)
{

	printf("val: %.15f\n", x);
	double dIn = x;
	//https://shreevatsa.wordpress.com/2011/01/10/not-all-best-rational-approximations-are-the-convergents-of-the-continued-fraction/
	int i;
	//The first two convergents are 0/1 and 1/0
	p[0] = 0; q[0] = 1;
	p[1] = 1; q[1] = 0;
	//The rest of the convergents (and continued fraction)
	for(i=2; i<MAX; ++i) {
		//a[i] = lrint(floor(x));
		a[i] = int64(floor(x));
		p[i] = a[i]*p[i-1] + p[i-2];
		q[i] = a[i]*q[i-1] + q[i-2];

		double dValue = (double)p[i]/(double)q[i];
		printf("%I64d:  %I64d/%I64d  %.15f err: %.15f\n", a[i], p[i], q[i], (double)p[i]/(double)q[i], dIn - dValue);
		len = i;
		if(fabs(dIn - dValue) == 0.)
			return;

		if(fabs(x-a[i])<eps || (q[i] < 0)|| (p[i] < 0)) 
			return;
		x = 1.0/(x - a[i]);
	}
}



uint32 getScale(double dVal, uint32 nMaxScale = 15)
{
	uint64 nPow = 1;
	dVal = fabs(dVal);
  

	for (uint32 i = 0; i < nMaxScale; ++i)
	{	
		uint64 nVal = uint64(dVal * nPow);
		double dBack = (double)nVal/nPow;
		if(fabs(dVal - dBack) == 0.)
			return i;
		nPow *= 10;
	}
	return nMaxScale;
}



uint32 getScale2(double dVal)
{
 
	dVal = fabs(dVal);
	double dM = dVal;
	uint32 nResult = 0;

	while( ( dM + dVal ) > dVal )
	{
		dM = dM / 10;
		++nResult;
	}

	return nResult;
}


void getFraq(double val, int maxD,uint32& n, uint32& d)
{
  assert(val<1.0);


  embDB::double_cast dVal1;
  dVal1.val = val;

  int64 mant = dVal1.parts.mantisa;
  uint32 *pmnt = (uint32 *)&mant;

  int64 x = 0, y = 0;
  uint32 *pxx = (uint32 *)&x;
  uint32 *pyy = (uint32 *)&y;
  uint32 BIT = (1 << 20);
  uint32 MASK = BIT - 1;

  uint32 *px = (uint32 *)&val;
  pxx[0] = px[0];
  pxx[1] = (px[1]&MASK)|BIT;

  double unit = 1.0;
  uint32 *py = (uint32 *)&unit;
  pyy[0] = py[0];
  pyy[1] = (py[1]&MASK)|BIT;

  int pow = (py[1] >> 20) - (px[1] >> 20);

  x >>= pow;
  if(x == y)
  {
    n = d = 1;
    return;
  }

  int64 p0 = 0; 
  int64 p1 = 1; 
  int64 q0 = 1; 
  int64 q1 = 0;
  int64 p2 = p1; 
  int64 q2 = q1; 
  assert(y >= x);
  while(true)
  {
    if(x == 0)
	{
      d = (int)(p1);
      n = int(q1);
      return;
    }
    int64 A = y/x;
    int64 tmpx = x;
    x = y - A*x;
    y = tmpx;

    p2=A*p1 + p0;
    q2=A*q1 + q0;
    if(p2 >= maxD)
	{
      A = (maxD - p0)/p1;
      d = (int)(A*p1 + p0);
      n = int(A*q1 + q0);
      return;
    }
    p0 = p1;
    p1 = p2;
    q0 = q1;
    q1 = q2;
  }
  assert(0);
}


void TestDoubleCompress()
{

	embDB::double_cast dVal, dVal1;

	dVal.val = sqrt(2.);//45654.17775000000;
	dVal.val = 1532e-3;
	uint32 n4, d1;
	
	CommonLib::CString str1;
	str1.format(L"%e", dVal.val);

	uint32 nScale = getScale(dVal.val, 1000);
	//getFraq(dVal.val,(int)pow(10.0,5) - 1,  n4, d1);
	getFraq1(dVal.val);

	uint32 nScale2 =getScale2(dVal.val);

    int i, n; long cp, cq;
	for(i=2; i<len; ++i) 
	{
		//Test n = a[i+1]/2 separately. Enough to test when a[i+1] is even, actually.
		n = a[i+1]/2; 
		cp = n*p[i]+p[i-1]; 
		cq = n*q[i]+q[i-1];

		double dd = (double)cp/(double)cq;
		dd++;
	}
	CommonLib::CString str;
	str.format("%f", dVal.val);
	uint64 nMat = dVal.parts.mantisa;
	uint64 nExp = dVal.parts.exponent;

	uint64 nDiv = (((uint64)2) << 51);
	dVal.parts.exponent = 2;

	uint32 nBits = embDB::TFindMostSigBit::FMSB(nMat);

	uint64 nMat1 = nMat/nDiv;

	uint64 n1 = nDiv/nMat;
	uint64 n2 = nDiv%nMat;


	uint32 nBits1 = embDB::TFindMostSigBit::FMSB(n1);
	uint32 nBits2 = embDB::TFindMostSigBit::FMSB(n2);


	double dVal2 = (1 + (double)nMat/nDiv) * pow((double)2, (double)nExp - 1023);

	
	
	embDB::TDoubleDiffCompreessor doubleCompressorDiff;
	embDB::TDoubleCompreessor doubleCompressor;

	embDB::TBPVector<double> vec;
	double dd = 1;
	for (int32 i = 1; i < 100000; ++i)
	{
		vec.push_back(dd);
		doubleCompressorDiff.AddSymbol(vec.size(), vec.size() - 1, dd, vec);

		uint32 nSize = doubleCompressorDiff.GetComressSize();
		if(nSize > 8192)
		{

			int dd = 0;
			dd++;
			nSize = doubleCompressorDiff.GetComressSize();
			break;
		}

		dd += i *10.3254;


	}


	dd = 1;
	for (int32 i = 1; i < 100000; ++i)
	{
		vec.push_back(dd);
		doubleCompressor.AddSymbol(vec.size(), vec.size() - 1, dd, vec);

		uint32 nSize = doubleCompressor.GetComressSize();
		if(nSize > 8192)
		{
			int dd = 0;
			dd++;
			break;
		}

		dd += i *10.3254;

		
	}

}