#ifndef _EMBEDDED_DATABASE_TRANSACTIONS_SIMPLE_SELECT_OP_CURSOR_H_
#define _EMBEDDED_DATABASE_TRANSACTIONS_SIMPLE_SELECT_OP_CURSOR_H_
#include "../../../../embDBInternal.h"
#include "CommonLibrary/PodVector.h"
namespace embDB
{



	class SimpleSelectOpCursor : public ICursor
	{
	public:
		SimpleSelectOpCursor(IDBTransaction* pTran, ITable* pTable,  IFieldSet *pFileds, const wchar_t *pszField, const CommonLib::CVariant& var, OpType opType);
		virtual ~SimpleSelectOpCursor();

		bool Init();

		virtual IFieldSetPtr GetFieldSet() const;
		virtual IFieldsPtr   GetSourceFields() const;
		virtual bool         IsFieldSelected(int index) const;
		virtual bool NextRow(IRowPtr* pRow = NULL);
		virtual bool  value(CommonLib::CVariant* pValue, int32 nNum);
		virtual int64 GetRowID() const;
	private:
		void SetNext(int64 nOID, bool bNext);

		struct IValueValidator
		{
			IValueValidator()
			
			{}
			virtual ~IValueValidator(){}

			virtual bool IsConditions(const CommonLib::CVariant& left, const CommonLib::CVariant& right) = 0;
			virtual OpType GetType() const= 0;
			
		};

		struct ValueValidatorEQ : public IValueValidator
		{
			ValueValidatorEQ()

			{}
			virtual ~ValueValidatorEQ(){}

			virtual bool IsConditions(const CommonLib::CVariant& left, const CommonLib::CVariant& right)
			{
				return left == right;
			}
			virtual OpType GetType() const
			{
				return OpEqual;
			}
		};

		struct ValueValidatorLess : public IValueValidator
		{
			ValueValidatorLess()

			{}
			virtual ~ValueValidatorLess(){}

			virtual bool IsConditions(const CommonLib::CVariant& left, const CommonLib::CVariant& right)
			{
				return left < right;
			}
			virtual OpType GetType() const
			{
				return OpLess;
			}
		};


		struct ValueValidatorLessEQ : public IValueValidator
		{
			ValueValidatorLessEQ()

			{}
			virtual ~ValueValidatorLessEQ(){}

			virtual bool IsConditions(const CommonLib::CVariant& left, const CommonLib::CVariant& right)
			{
				return left <= right;
			}
			virtual OpType GetType() const
			{
				return OpLessOrEqual;
			}
		};


		struct ValueValidatorGreater : public IValueValidator
		{
			ValueValidatorGreater()

			{}
			virtual ~ValueValidatorGreater(){}

			virtual bool IsConditions(const CommonLib::CVariant& left, const CommonLib::CVariant& right)
			{
				return left > right;
			}
			virtual OpType GetType() const
			{
				return OpGreater;
			}
		};

		struct ValueValidatorGreaterEQ: public IValueValidator
		{
			ValueValidatorGreaterEQ()

			{}
			virtual ~ValueValidatorGreaterEQ(){}

			virtual bool IsConditions(const CommonLib::CVariant& left, const CommonLib::CVariant& right)
			{
				return left >= right;
			}
			virtual OpType GetType() const
			{
				return OpGreaterOrEqual;
			}
		};

		struct ISearhHolder
		{
			public:
				ISearhHolder(const CommonLib::CVariant& value, OpType opType) :m_value(value), m_optType(opType),
					m_nCacheCount(1000000), m_pValueValidator(NULL), m_bEnd(false), m_nCurrRowID(0)
				{
					switch(opType)
					{
						case OpEqual:
							m_pValueValidator = new ValueValidatorEQ();
							break;
						case OpLess:
							m_pValueValidator = new ValueValidatorLess();
							break;
						case OpLessOrEqual:
							m_pValueValidator = new ValueValidatorLessEQ();
							break;
						case OpGreater:
							m_pValueValidator = new ValueValidatorGreater();
							break;
						case OpGreaterOrEqual:
							m_pValueValidator = new ValueValidatorGreaterEQ();
							break;
					}

					assert(m_pValueValidator != NULL);
				}
				virtual ~ISearhHolder()
				{
					if(m_pValueValidator)
					{
						delete m_pValueValidator;
						m_pValueValidator = NULL;
					}
				}
				virtual void reset() = 0;
				virtual  int64 nextRowID()
				{
					if(m_bEnd)
						return -1;

					if(m_vecROWIDs.size() == m_nCurrRowID)
					{
						FillChache();
						if(m_vecROWIDs.empty())
						{
							m_bEnd = true;
							return -1;
						}
						m_nCurrRowID = 0;
					}

					int64 nRowID = m_vecROWIDs[m_nCurrRowID];
					m_nCurrRowID++;
					return nRowID;
				}

			 
		protected:
			virtual void	FillChache() = 0;
		protected:
			CommonLib::CVariant m_value;
			OpType				m_optType;
			typedef CommonLib::TPodVector<int64> TVecOids;
			TVecOids m_vecROWIDs;
			uint32 m_nCacheCount;
			IValueValidator *m_pValueValidator;
			uint32 m_nCurrRowID;
			bool m_bEnd;

		};


		struct IndexSearchHolder : public ISearhHolder
		{
			public:
				IndexSearchHolder(const CommonLib::CVariant& value, OpType opType, IndexFiled* pIndex);
				virtual ~IndexSearchHolder();
				virtual void reset();
			private:
				virtual void FillChache();
			private:
				IndexFiledPtr m_pIndex;
				IIndexIteratorPtr m_pIndexIterator;
				bool m_bEnd;
				bool m_bIterEnd;

		};

		struct FieldSearchHolder : public ISearhHolder
		{
		public:
			FieldSearchHolder(const CommonLib::CVariant& value, OpType opType, IValueField* pIndex);
			virtual ~FieldSearchHolder();
			virtual void reset();
		private:
			virtual void FillChache();
		private:
			IValueFieldPtr m_pField;
			IFieldIteratorPtr m_pFieldIterator;
			bool m_bEnd;
	
		};


		CommonLib::CString m_sField;
		OpType m_nOpType;
		CommonLib::CVariant m_value;

		IRowPtr m_pCacheRow;
		IDBTransactionPtr m_pTran;
		IDBTablePtr		  m_pTable;
		IFieldSetPtr	  m_pFieldSet;
		IFieldsPtr		  m_pFields;

		ISearhHolder	*m_pSearhHolder;
		int32 m_nIterIndex;

		struct SField
		{
			IFieldIteratorPtr m_pFieldIterator;
			IValueFieldPtr	  m_pValueField;
		};


		typedef std::vector<SField> TVecValueField;
		TVecValueField m_vecFields;
		bool m_bAllNext;
		bool m_bEnd;
		int64 m_nPrevROWID;
		int64 m_nCurrROWID;
	};
}


#endif