START_TYPE_LIST()
DECLARE_TYPE(CNullVariant)
DECLARE_SIMPLE_TYPE(bool)
DECLARE_SIMPLE_TYPE(byte)
DECLARE_SIMPLE_TYPE(int8)
DECLARE_SIMPLE_TYPE(int16)
DECLARE_SIMPLE_TYPE(uint16)
DECLARE_SIMPLE_TYPE(int32)
DECLARE_SIMPLE_TYPE(uint32)
DECLARE_SIMPLE_TYPE(int64)
DECLARE_SIMPLE_TYPE(uint64)
DECLARE_SIMPLE_TYPE(float)
DECLARE_SIMPLE_TYPE(double)
DECLARE_TYPE(TPoint2D16)
DECLARE_TYPE(TPoint2D32)
DECLARE_TYPE(TPoint2D64)
DECLARE_TYPE(TPoint2Du16)
DECLARE_TYPE(TPoint2Du32)
DECLARE_TYPE(TPoint2Du64)
DECLARE_TYPE(TRect2D16)
DECLARE_TYPE(TRect2D32)
DECLARE_TYPE(TRect2D64)
DECLARE_TYPE(TRect2Du16)
DECLARE_TYPE(TRect2Du32)
DECLARE_TYPE(TRect2Du64)
DECLARE_TYPE(CString)
DECLARE_TYPE(IRefObjectPtr)
DECLARE_TYPE(CBlob)
DECLARE_TYPE(CGeoShape)
FINISH_TYPE_LIST()

#ifndef TYPE_COUNT
#define TYPE_COUNT _type_list_count_
#define MAX_GV_SIZE maxSize<TYPE_COUNT-1>::mxSize
#endif


#ifdef DECLARE_FUNC_TABLES
#ifndef CREATE_FUNC_TABLES

extern constructorVariant		var_constructors_[TYPE_COUNT];
extern destructorVariant		var_destructors_[TYPE_COUNT];
extern compareVariantFunc		var_compare_[TYPE_COUNT];
extern copyVariantFunc			var_copy_[TYPE_COUNT];
extern assignVariantFunc		var_assign_[TYPE_COUNT];
extern acceptVariantFunc		var_accept_[TYPE_COUNT];

#else

constructorVariant		var_constructors_[TYPE_COUNT];
destructorVariant		var_destructors_[TYPE_COUNT];
compareVariantFunc		var_compare_[TYPE_COUNT];
copyVariantFunc			var_copy_[TYPE_COUNT];
assignVariantFunc		var_assign_[TYPE_COUNT];
acceptVariantFunc		var_accept_[TYPE_COUNT];

#endif
#endif

