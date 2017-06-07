#pragma once
#include "../BTreePlus/BaseNodeCompressor.h"
#include "../BTreePlus/BaseDIffEncoder.h"
#include "../BTreePlus/BaseEncoder.h"
#include "../BTreePlus/EmptyEncoder.h"
#include "../../Utils/compress/NumLen/SignedNumLenEncoder.h"
#include "../../Utils/compress/NumLen/UnsignedNumLenEncoder.h"


namespace embDB
{

	typedef UnsignedNumLenEncoder<uint64, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 64> TUnsignedNumLenEncoderU64;
	typedef UnsignedNumLenEncoder<uint32, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 32> TUnsignedNumLenEncoderU32;
	typedef UnsignedNumLenEncoder<uint16, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 16> TUnsignedNumLenEncoderU16;
	typedef UnsignedNumLenEncoder<byte, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 8> TUnsignedNumLenEncoderU8;

	typedef UnsignedNumLenEncoder<int64, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 64> TUnsignedNumLenEncoder64;
	typedef UnsignedNumLenEncoder<int32, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 32> TUnsignedNumLenEncoder32;
	typedef UnsignedNumLenEncoder<int16, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 16> TUnsignedNumLenEncoder16;
	typedef UnsignedNumLenEncoder<int8, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 8> TUnsignedNumLenEncoder8;

 
	typedef SignedNumLenEncoder<int64, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 64> SignedNumLenEncoder64;
	typedef SignedNumLenEncoder<int32, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 32> SignedNumLenEncoder32;
	typedef SignedNumLenEncoder<int16, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 16> SignedNumLenEncoder16;
	typedef SignedNumLenEncoder<int8, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 8> SignedNumLenEncoder8;


	typedef SignedNumLenEncoder<uint64, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 64> SignedNumLenEncoderU64;
	typedef SignedNumLenEncoder<uint32, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 32> SignedNumLenEncoderU32;
	typedef SignedNumLenEncoder<uint16, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 16> SignedNumLenEncoderU16;
	typedef SignedNumLenEncoder<byte, CommonLib::TACEncoder64, CommonLib::TACDecoder64, CompressorParamsBaseImp, 8> SignedNumLenEncoderU8;


	typedef TBaseValueDiffEncoder<int64, TUnsignedNumLenEncoder64> TUnsignedDiffEncoder64;

	typedef TBaseValueDiffEncoder<int64, SignedNumLenEncoder64> TSignedDiffEncoder64;
	typedef TBaseValueDiffEncoder<uint64, SignedNumLenEncoderU64> TSignedDiffEncoderU64;

	typedef TBaseValueDiffEncoder<int32, SignedNumLenEncoder32> TSignedDiffEncoder32;
	typedef TBaseValueDiffEncoder<uint32, SignedNumLenEncoderU32> TSignedDiffEncoderU32;

	typedef TBaseValueDiffEncoder<int16, SignedNumLenEncoder16> TSignedDiffEncoder16;
	typedef TBaseValueDiffEncoder<uint16, SignedNumLenEncoderU16> TSignedDiffEncoderU16;

	typedef TBaseValueDiffEncoder<int8, SignedNumLenEncoder8> TSignedDiffEncoder8;
	typedef TBaseValueDiffEncoder<byte, SignedNumLenEncoderU8> TSignedDiffEncoderU8;
	
	typedef TEmptyValueEncoder<double, CompressorParamsBaseImp> TDoubleEncoder;
	typedef TEmptyValueEncoder<float, CompressorParamsBaseImp> TFloatEncoder;

	typedef TBaseNodeCompressor<int64, int64, IDBTransaction, TUnsignedDiffEncoder64, TSignedDiffEncoder64> TInnerNodeLinkDiffComp;


	typedef TBaseNodeCompressor<int64, int64, IDBTransaction, TUnsignedDiffEncoder64, TSignedDiffEncoder64> TLeafNodeLinkDiffComp64;
	typedef TBaseNodeCompressor<int64, uint64, IDBTransaction, TUnsignedDiffEncoder64, TSignedDiffEncoderU64> TLeafNodeLinkDiffCompU64;

	typedef TBaseNodeCompressor<int64, int32, IDBTransaction, TUnsignedDiffEncoder64, TSignedDiffEncoder32> TLeafNodeLinkDiffComp32;
	typedef TBaseNodeCompressor<int64, uint32, IDBTransaction, TUnsignedDiffEncoder64, TSignedDiffEncoderU32> TLeafNodeLinkDiffCompU32;

	typedef TBaseNodeCompressor<int64, int16, IDBTransaction, TUnsignedDiffEncoder64, TSignedDiffEncoder16> TLeafNodeLinkDiffComp16;
	typedef TBaseNodeCompressor<int64, uint16, IDBTransaction, TUnsignedDiffEncoder64, TSignedDiffEncoderU16> TLeafNodeLinkDiffCompU16;

	typedef TBaseNodeCompressor<int64, int8, IDBTransaction, TUnsignedDiffEncoder64, TSignedDiffEncoder8> TLeafNodeLinkDiffComp8;
	typedef TBaseNodeCompressor<int64, byte, IDBTransaction, TUnsignedDiffEncoder64, TSignedDiffEncoderU8> TLeafNodeLinkDiffCompU8;


	typedef TBaseNodeCompressor<int64, double, IDBTransaction, TUnsignedDiffEncoder64, TDoubleEncoder> TLeafNodeLinkDiffCompDouble;
	typedef TBaseNodeCompressor<int64, float, IDBTransaction, TUnsignedDiffEncoder64, TFloatEncoder> TLeafNodeLinkDiffCompFloat;
}