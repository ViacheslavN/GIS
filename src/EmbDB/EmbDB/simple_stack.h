#ifndef _EMBEDDED_DATABASE_TREES_SIMPLE_STACK_
#define _EMBEDDED_DATABASE_TREES_SIMPLE_STACK_

#include "Commonlibrary/general.h"
#include "Commonlibrary/alloc_t.h"
namespace embDB
{

template <class Type>
class TSimpleStack
{
  public:
    TSimpleStack(CommonLib::alloc_t* alloc = NULL, size_t capacity = 0):alloc_(alloc), capacity_(capacity), size_(0), data_(0)
    {
      if(!alloc_)
		  alloc_ = &m_alloc;
      if(capacity_)
      {
        data_ = (Type*)alloc_->alloc(capacity_ * sizeof(Type));
      }
    }
    ~TSimpleStack()
    {
      if(data_)
      {
        alloc_->free(data_);
      }
    }

    void push(const Type & val)
    {

      if(size_ >= capacity_)
      {
         allocate();
      }
      data_[size_] = val;
      size_++;
    }
    Type top()
    {
      if(size_)
      {
        size_--;
        return data_[size_];
      }
      assert(false);
	  return Type();
    }
    void allocate()
    {
      if(!size_)
      {
       capacity_ = 2;
      }
      else
      {
        capacity_ = 2* size_;
      }

      Type* tmp = (Type*)alloc_->alloc(capacity_ * sizeof(Type));

      if(data_)
      {
        memcpy(tmp, data_, size_* sizeof(Type));
        alloc_->free(data_);
        data_ = tmp;
      }
      else
      {
        data_ = tmp;
      }
    }
	void set_size(size_t nsize)
	{
		assert(capacity_ > size_);
		size_ = nsize;
	}
    size_t size() const { return size_; }
	Type& operator [](size_t nIndex)
	{
		return data_[nIndex];
	}
	const Type& operator [](size_t nIndex) const
	{
		return data_[nIndex];
	}
	size_t capacity() const { return capacity_; }
	bool empty(){return size_ == 0;}
	void reserve(size_t nSize)
	{
		if(capacity_ > nSize)
			return;

		capacity_ = nSize;
		Type* tmp = (Type*)alloc_->alloc(capacity_ * sizeof(Type));

		if(data_)
		{
			memcpy(tmp, data_, size_* sizeof(Type));
			alloc_->free(data_);
			data_ = tmp;
		}
		else
		{
			data_ = tmp;
		}

	}
  private:
    Type*  data_;
    CommonLib::alloc_t* alloc_;
    size_t capacity_;
    size_t size_;
	CommonLib::simple_alloc_t  m_alloc;

};
}

#endif