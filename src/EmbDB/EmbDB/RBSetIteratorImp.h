class iterator
{
	public:
	 	iterator(TTreeNode* pNode, TTreeNode* pNodeEnd): m_pNode(pNode), m_pNodeEnd(pNodeEnd)
		{}
		iterator(const iterator& it): m_pNode(it.m_pNode), m_pNodeEnd(it.m_pNodeEnd)
		{}
		iterator& operator = (const iterator& it){
			m_pNode = it.m_pNode;
			m_pNodeEnd = it.m_pNodeEnd;
			return *this;
		}

		~iterator(){}
		bool next(){
			//if(!m_pNode)
			//	return false;
			m_pNode = m_pNode->m_pNext;
			return m_pNode != m_pNodeEnd;
		}
		bool back(){
			// if(!m_pNode)
			//	return false;
			m_pNode = m_pNode->m_pPrev;
			return m_pNode != m_pNodeEnd;
		}
		bool isNull() const{
			return  m_pNode == m_pNodeEnd;
		}
		iterator& operator ++ (){
			next();
 			return *this;
		}
		iterator& operator -- (){
			back();
 			return *this;
		}
		iterator operator++(int){
			iterator itTmp = *this;
			next();
			return (itTmp);
		}
		iterator operator--(int){
			iterator itTmp = *this;
			back();
			return (itTmp);
		}

		
		const TypeKey& operator*() const
		{	// return designated value
			return m_pNode->m_key;
		}
		TypeKey& operator*()
		{	// return designated value
			return m_pNode->m_key;
		}

		bool operator == (const iterator& it){
			return m_pNode == it.m_pNode;
		}
		bool operator != (const iterator& it){
			return m_pNode != it.m_pNode;
		}
		const TypeKey& key() const{
			 return m_pNode->m_key;
		}
		TypeKey& key(){
			 return m_pNode->m_key;
		}
	//protected:
		TTreeNode* m_pNode;
		TTreeNode* m_pNodeEnd;
		friend class RBSet;
	};
