class iterator
{
public:
	//typedef TNode TTreeNode;
	//typedef TListEl TListNode;
	//TKey, class TVal

	iterator(TTreeNode *pNode, TListNode *pListNode) : m_pNode(pNode), m_pListNode(pListNode)
	{}
	iterator(const iterator& it) : m_pNode(it.m_pNode) , m_pListNode(it.m_pListNode)
	{}
	iterator& operator = (const iterator& it){
		m_pNode = it.m_pNode;
		m_pListNode = it.m_pListNode;
		return *this;
	}
	bool next(){
		m_pListNode = m_pListNode->m_pNext;
		if(m_pListNode)
			return true;
		m_pNode = m_pNode->m_pNext;
		if(!m_pNode)
			return false;
		m_pListNode = m_pNode->m_pFirst;
		return m_pListNode == NULL;
	}
	bool back(){
		m_pListNode = m_pListNode->m_pPrev;
		if(m_pListNode)
			return true;
		m_pNode = m_pNode->m_pPrev;
		if(!m_pNode)
			return false;
		m_pListNode = m_pNode->m_pLast;
		return m_pListNode == NULL;
	}
	bool IsNull(){
		return m_pListNode == NULL;
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


	const TypeVal& operator*() const
	{	// return designated value
		return m_pListNode->val_;
	}
	TypeVal& operator*()
	{	// return designated value
		return m_pListNode->val_;
	}

	bool operator == (const iterator& it){
		return m_pListNode == it.m_pListNode;
	}
	bool operator != (const iterator& it){
		return m_pListNode != it.m_pListNode;
	}
	const TypeKey& getKey() const{
		return m_pListNode->key_;
	}
	TypeKey& getKey(){
		return m_pListNode->key_;
	}
	
	const TypeVal& getVal() const{
		return m_pListNode->val_;
	}
	TypeVal& getVal(){
		return m_pListNode->val_;
	}

protected:
	TTreeNode *m_pNode;
	TListNode *m_pListNode;
	friend class RBMultiMap;

};