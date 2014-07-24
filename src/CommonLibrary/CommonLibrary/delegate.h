#ifndef _LIB_COMMON_DELEGATE_H
#define _LIB_COMMON_DELEGATE_H

//MD
#include <list>

namespace CommonLib
{

	class delegate_t
	{
	public:
		virtual void invoke() = 0;
		virtual bool is_equal(const delegate_t& d) = 0;
		virtual ~delegate_t() { }
	public:
		void operator()()
		{
			invoke();
		}
	};

	template <typename T>
	class delegate1_t
	{
	public:
		virtual void invoke(T arg) = 0;
		virtual bool is_equal(const delegate1_t& d) = 0;
		virtual ~delegate1_t() { }
	public:
		void operator()(T arg)
		{
			invoke(arg);
		}
	};

	template <typename T1, typename T2>
	class delegate2_t
	{
	public:
		virtual void invoke(T1 arg1, T2 arg2) = 0;
		virtual bool is_equal(const delegate2_t& d) = 0;
		virtual ~delegate2_t() { }
	public:
		void operator()(T1 arg1, T2 arg2)
		{
			invoke(arg1, arg2);
		}
	};

	template <typename T1, typename T2, typename T3>
	class delegate3_t
	{
	public:
		virtual void invoke(T1 arg1, T2 arg2, T3 arg3) = 0;
		virtual bool is_equal(const delegate3_t& d) = 0;
		virtual ~delegate3_t() { }
	public:
		void operator()(T1 arg1, T2 arg2, T3 arg3)
		{
			invoke(arg1, arg2, arg3);
		}
	};

	template <typename T1, typename T2, typename T3, typename T4>
	class delegate4_t
	{
	public:
		virtual void invoke(T1 arg1, T2 arg2, T3 arg3, T4 arg4) = 0;
		virtual bool is_equal(const delegate4_t& d) = 0;
		virtual ~delegate4_t() { }
	public:
		void operator()(T1 arg1, T2 arg2, T3 arg3, T4 arg4)
		{
			invoke(arg1, arg2, arg3, arg4);
		}
	};

	template<typename T>
	class delegateimpl_t : public delegate_t
	{
	public:
		delegateimpl_t(T* obj, void (T::*method)())
			: obj_(obj),
			method_(method)
		{
		}
	public:
		virtual void invoke()
		{
			(obj_->*method_)();
		}
		bool is_equal(const delegate_t& dg)
		{
			const delegateimpl_t& d = static_cast<const delegateimpl_t&>(dg);
			return (obj_ == d.obj_ && method_ == d.method_);
		}
	private:
		T* obj_;
		void (T::*method_)();
	};

	template<typename T, typename T1>
	class delegateimpl1_t : public delegate1_t<T1>
	{
	public:
		delegateimpl1_t(T* obj, void (T::*method)(T1 arg))
			: obj_(obj),
			method_(method)
		{
		}
	public:
		virtual void invoke(T1 arg)
		{
			(obj_->*method_)(arg);
		}
		//bool is_equal(const delegate1_t& dg)//MD
		bool is_equal(const delegate1_t<T1>& dg)
		{
			const delegateimpl1_t& d = static_cast<const delegateimpl1_t&>(dg);
			return (obj_ == d.obj_ && method_ == d.method_);
		}
	private:
		T* obj_;
		void (T::*method_)(T1 arg);
	};

	template<typename T, typename T1, typename T2>
	class delegateimpl2_t : public delegate2_t<T1, T2>
	{
	public:
		delegateimpl2_t(T* obj, void (T::*method)(T1 arg1, T2 arg2))
			: obj_(obj),
			method_(method)
		{
		}
	public:
		virtual void invoke(T1 arg1, T2 arg2)
		{
			(obj_->*method_)(arg1, arg2);
		}
		//bool is_equal(const delegate2_t& dg)//MD
		bool is_equal(const delegate2_t<T1, T2>& dg)
		{
			const delegateimpl2_t& d = static_cast<const delegateimpl2_t&>(dg);
			return (obj_ == d.obj_ && method_ == d.method_);
		}
	private:
		T* obj_;
		void (T::*method_)(T1 arg1, T2 arg2);
	};

	template<typename T, typename T1, typename T2, typename T3>
	class delegateimpl3_t : public delegate3_t<T1, T2, T3>
	{
	public:
		delegateimpl3_t(T* obj, void (T::*method)(T1 arg1, T2 arg2, T3 arg3))
			: obj_(obj),
			method_(method)
		{
		}
	public:
		virtual void invoke(T1 arg1, T2 arg2, T3 arg3)
		{
			(obj_->*method_)(arg1, arg2, arg3);
		}

		//MD added <T1, T2, T3>
		bool is_equal(const delegate3_t<T1, T2, T3>& dg)
		{
			const delegateimpl3_t& d = static_cast<const delegateimpl3_t&>(dg);
			return (obj_ == d.obj_ && method_ == d.method_);
		}
	private:
		T* obj_;
		void (T::*method_)(T1 arg1, T2 arg2, T3 arg3);
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4>
	class delegateimpl4_t : public delegate4_t<T1, T2, T3, T4>
	{
	public:
		delegateimpl4_t(T* obj, void (T::*method)(T1 arg1, T2 arg2, T3 arg3, T4 arg4))
			: obj_(obj),
			method_(method)
		{
		}
	public:
		virtual void invoke(T1 arg1, T2 arg2, T3 arg3, T4 arg4)
		{
			(obj_->*method_)(arg1, arg2, arg3, arg4);
		}
		//MD added <T1, T2, T3, T4> and change code (d was instead dg)
		bool is_equal(const delegate4_t<T1, T2, T3, T4>& dg)
		{
			const delegateimpl4_t& d = static_cast<const delegateimpl4_t&>(dg);
			return (obj_ == d.obj_ && method_ == d.method_);
		}
	private:
		T* obj_;
		void (T::*method_)(T1 arg1, T2 arg2, T3 arg3, T4 arg4);
	};

	template<typename T>
	delegate_t* Delegate(T* obj, void (T::*method)())
	{
		return new delegateimpl_t<T>(obj, method);
	}

	template<typename T, typename T1>
	delegate1_t<T1>* Delegate(T* obj, void (T::*method)(T1 arg))
	{
		return new delegateimpl1_t<T, T1>(obj, method);
	}

	template<typename T, typename T1, typename T2>
	delegate2_t<T1, T2>* Delegate(T* obj, void (T::*method)(T1 arg1, T2 arg2))
	{
		return new delegateimpl2_t<T, T1, T2>(obj, method);
	}

	template<typename T, typename T1, typename T2, typename T3>
	delegate3_t<T1, T2, T3>* Delegate(T* obj, void (T::*method)(T1 arg1, T2 arg2, T3 arg3))
	{
		return new delegateimpl3_t<T, T1, T2, T3>(obj, method);
	}

	template<typename T, typename T1, typename T2, typename T3, typename T4>
	delegate4_t<T1, T2, T3, T4>* Delegate(T* obj, void (T::*method)(T1 arg1, T2 arg2, T3 arg3, T4 arg4))
	{
		return new delegateimpl4_t<T, T1, T2, T3, T4>(obj, method);
	}

	template<typename T>
	delegateimpl_t<T> Function(T* obj, void (T::*method)())
	{
		return delegateimpl_t<T>(obj, method);
	}

	template<typename T, typename T1>
	delegateimpl1_t<T, T1> Function(T* obj, void (T::*method)(T1 arg))
	{
		return delegateimpl1_t<T, T1>(obj, method);
	}

	template<typename T, typename T1, typename T2>
	delegateimpl2_t<T, T1, T2> Function(T* obj, void (T::*method)(T1 arg1, T2 arg2))
	{
		return delegateimpl2_t<T, T1, T2>(obj, method);
	}

	template<typename T, typename T1, typename T2, typename T3>
	delegateimpl3_t<T, T1, T2, T3> Function(T* obj, void (T::*method)(T1 arg1, T2 arg2, T3 arg3))
	{
		return delegateimpl3_t<T, T1, T2, T3>(obj, method);
	}

	template<typename T, typename T1, typename T2, typename T3, typename T4>
	delegateimpl4_t<T, T1, T2, T3, T4> Function(T* obj, void (T::*method)(T1 arg1, T2 arg2, T3 arg3, T4 arg4))
	{
		return delegateimpl4_t<T, T1, T2, T3, T4>(obj, method);
	}

	template <typename T>
	class EventBase
	{
		typedef std::list<T*> itemlist_t;
	public:
		~EventBase()
		{
			if(items_.size() == 0)
				return;
			//MD added typename (for compability with GCC)
			for(typename itemlist_t::iterator it = items_.begin(), iend = items_.end(); it != iend; it++)
				delete (*it);
		}
		EventBase& operator += (T* d)
		{
			items_.push_back(d);
			return *this;
		}
		EventBase& operator -= (T* d)
		{
			if(items_.size() == 0)
			{
				delete d;
				return *this;
			}

			//MD added typename (for compability with GCC)
			for(typename itemlist_t::iterator it = items_.begin(), iend = items_.end(); it != iend; it++)
			{
				if((*it)->is_equal(*d))
				{
					delete (*it);
					items_.erase(it);
					break;
				}
			}
			delete d;
			return *this;
		}
		operator bool () const
		{
			return items_.size() > 0;
		}
		void fire()
		{
			if(items_.size() == 0)
				return;

			//MD added typename (for compability with GCC)
			for(typename itemlist_t::iterator it = items_.begin(), iend = items_.end(); it != iend; it++)
				(*it)->invoke();//(*(*it))();
		}
		template<typename T1>
		void fire(T1 arg)
		{
			if(items_.size() == 0)
				return;

			//MD added typename (for compability with GCC)
			for(typename itemlist_t::iterator it = items_.begin(), iend = items_.end(); it != iend; it++)
				(*it)->invoke(arg);// (*(*it))();
		}
		template<typename T1, typename T2>
		void fire(T1 arg1, T2 arg2)
		{
			if(items_.size() == 0)
				return;

			//MD added typename (for compability with GCC)
			for(typename itemlist_t::iterator it = items_.begin(), iend = items_.end(); it != iend; it++)
				(*it)->invoke(arg1, arg2);//(*(*it))(arg1, arg2);
		}
		template<typename T1, typename T2, typename T3>
		void fire(T1 arg1, T2 arg2, T3 arg3)
		{
			if(items_.size() == 0)
				return;

			//MD added typename (for compability with GCC)
			for(typename itemlist_t::iterator it = items_.begin(), iend = items_.end(); it != iend; it++)
				(*it)->invoke(arg1, arg2, arg3);//((*it))(arg1, arg2, arg3);
		}
		template<typename T1, typename T2, typename T3, typename T4>
		void fire(T1 arg1, T2 arg2, T3 arg3, T4 arg4)
		{
			if(items_.size() == 0)
				return;

			//MD added typename (for compability with GCC)
			for(typename itemlist_t::iterator it = items_.begin(), iend = items_.end(); it != iend; it++)
				(*it)->invoke(arg1, arg2, arg3, arg4);//((*it))(arg1, arg2, arg3, arg4);
		}
		void operator()()
		{
			fire();
		}
		template<typename T1>
		void operator()(T1 arg)
		{
			fire(arg);
		}
		template<typename T1, typename T2>
		void operator()(T1 arg1, T2 arg2)
		{
			fire(arg1, arg2);
		}
		template<typename T1, typename T2, typename T3>
		void operator()(T1 arg1, T2 arg2, T3 arg3)
		{
			fire(arg1, arg2, arg3);
		}
		template<typename T1, typename T2, typename T3, typename T4>
		void operator()(T1 arg1, T2 arg2, T3 arg3, T4 arg4)
		{
			fire(arg1, arg2, arg3, arg4);
		}
	private:
		itemlist_t items_;
	};

	//class Event : public EventBase<delegate_t>
	//{
	//};
	//
	//template <typename T>
	//class Event1 : public EventBase<delegate1_t<T> >
	//{
	//};
	//
	//template <typename T1, typename T2>
	//class Event2 : public EventBase<delegate2_t<T1, T2> >
	//{
	//};
	//
	//template <typename T1, typename T2, typename T3>
	//class Event3 : public EventBase<delegate3_t<T1, T2, T3> >
	//{
	//};
	//
	//template <typename T1, typename T2, typename T3, typename T4>
	//class Event4 : public EventBase<delegate4_t<T1, T2, T3, T4> >
	//{
	//};

	template<typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void>
	class Event : public EventBase<delegate4_t<T1, T2, T3, T4> >
	{
	};

	template<typename T1, typename T2, typename T3>
	class Event<T1, T2, T3, void> : public EventBase<delegate3_t<T1, T2, T3> >
	{
	};

	template<typename T1, typename T2>
	class Event<T1, T2, void, void> : public EventBase<delegate2_t<T1, T2> >
	{
	};

	template<typename T1>
	class Event<T1, void, void, void> : public EventBase<delegate1_t<T1> >
	{
	};

	template<>
	class Event<void, void, void, void> : public EventBase<delegate_t >
	{
	};

}


#endif