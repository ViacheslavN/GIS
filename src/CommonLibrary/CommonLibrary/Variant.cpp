#include "stdafx.h"
#include "Variant.h"
namespace CommonLib
{
	struct baseStruct{
		virtual baseStruct *getPrev(){return NULL;}
	};

	template<int>  struct int2type : public baseStruct{
		int2type(){}
	};




#define _curVal_ (__LINE__-_start_type_list_)

	template <typename T> 
	void VarDestructor(void *ptr){
		T *tptr=(T *)ptr; 
		tptr->~T();
	}
	template <typename T> 
	void VarConstructor(void *ptr,const void *src){ 
		new(ptr) T(*(T *)src); 
	} 
	template <typename T> 
	void AssignVariant(void *dst, int idx, const void *src){ 
		VarDestructor<T>(dst); 
		var_constructors_[idx](dst,  src); 
	} 
	template <typename T> 
	void CopyVariant(void *dst,const void *src){ 
		*(T *)dst = *(T *)src; \
	} 

	template <typename T> 
	static void AcceptVariant(const void *ptr,IVisitor &visitor)
	{ 
		visitor.Visit(*(T *)ptr); 
	} 






#define DECLARE_TYPE(Type) \
	template<>  \
	struct int2type< _curVal_ > : public baseStruct{ \
	public: \
	int2type<_curVal_>(){ \
	var_constructors_[_curVal_] = VarConstructor<Type>; \
	var_destructors_[_curVal_] = VarDestructor<Type>; \
	var_compare_[_curVal_] = int2type< _curVal_ >::CompareVariant;\
	var_copy_[_curVal_]= CopyVariant<Type>; \
	var_assign_[_curVal_]=AssignVariant<Type>; \
	var_accept_[_curVal_]=AcceptVariant<Type>; \
	} \
	virtual baseStruct *getPrev(){\
	int nTmp = _curVal_;\
	if(nTmp > 0) \
	return new(this) int2type< _curVal_ - 1>(); \
	return NULL; \
	} \
	static int CompareVariant(const void *pDst, const void *pSrc){ \
	if(*(Type *)pDst == *(Type *)pSrc) \
	return 0; \
	if(*(Type *)pDst < *(Type *)pSrc) \
	return -1; \
	return 1; \
	} \
	};
#define DECLARE_SIMPLE_TYPE(TT) DECLARE_TYPE(TT) 


#define DECLARE_FUNC_TABLES
#define CREATE_FUNC_TABLES
#define START_TYPE_LIST()
#define FINISH_TYPE_LIST()

#include "VartTypeList.h"

#undef START_TYPE_LIST
#undef FINISH_TYPE_LIST
#undef CREATE_FUNC_TABLES
#undef DECLARE_FUNC_TABLES
#undef DECLARE_SIMPLE_TYPE
#undef DECLARE_TYPE
#undef _curVal_ 


	struct tablesFuncCreator{
		tablesFuncCreator(){
			int2type<TYPE_COUNT-1> val;
			baseStruct *pval=&val;
			while(pval)
				pval=pval->getPrev(); 
		}
	};
	tablesFuncCreator tabsFuncCreator_;
}
