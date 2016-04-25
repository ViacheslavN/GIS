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
    TSimpleStack(CommonLib::alloc_t* alloc = NULL, uint32 capacity = 0):alloc_(alloc), capacity_(capacity), size_(0), data_(0)
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


	TSimpleStack(const TSimpleStack& stack)
	{
		if(stack.alloc_ != &stack.m_alloc)
			alloc_ = stack.alloc_;
		else
			alloc_ = &m_alloc;

		size_ = stack.size_;
		capacity_ = stack.capacity_;
		data_ = 0;
		if(capacity_)
		{
			data_ = (Type*)alloc_->alloc(capacity_ * sizeof(Type));
			memcpy(data_, stack.data_, capacity_ * sizeof(Type));
		}
	 
	}

	const TSimpleStack& operator = (const TSimpleStack& stack)
	{
		if(stack.alloc_ != &stack.m_alloc)
			alloc_ = stack.alloc_;
		else
			alloc_ = &m_alloc;

		size_ = stack.size_;
		capacity_ = stack.capacity_;
		data_ = 0;
		if(capacity_)
		{
			data_ = (Type*)alloc_->alloc(capacity_ * sizeof(Type));
			memcpy(data_, stack.data_, capacity_ * sizeof(Type));
		}

		return *this;
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
	void set_size(uint32 nsize)
	{
		assert(capacity_ > size_);
		size_ = nsize;
	}
    uint32 size() const { return size_; }
	Type& operator [](uint32 nIndex)
	{
		return data_[nIndex];
	}
	const Type& operator [](uint32 nIndex) const
	{
		return data_[nIndex];
	}
	uint32 capacity() const { return capacity_; }
	bool empty(){return size_ == 0;}
	void reserve(uint32 nSize)
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
    uint32 capacity_;
    uint32 size_;
	CommonLib::simple_alloc_t  m_alloc;

};
}

#endif