class iterator
{
public:
	iterator(TTreeNode* pNode, TTreeNode* pEndNode): m_pNode(pNode), m_pEndNode(pEndNode)
	{}
	iterator(const iterator& it): m_pNode(it.m_pNode), m_pEndNode(it.m_pEndNode)
	{}
	iterator& operator = (const iterator& it)
	{
		m_pNode = it.m_pNode;
		m_pEndNode = it.m_pEndNode;
		return *this;
	}

	~iterator(){}
	bool next(){
		if(!m_pNode)
			return false;
		assert(m_pNode!= m_pEndNode);
		m_pNode = m_pNode->m_pNext;
		return m_pNode != m_pEndNode;
	}
	bool back(){
		if(!m_pNode)
			return false;
		assert(m_pNode != m_pEndNode);
		m_pNode = m_pNode->m_pPrev;
		return m_pNode != m_pEndNode;
	}
	bool isNull() const
	{
		//assert(m_pNode != m_pEndNode);
		return m_pNode == m_pEndNode;
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
		return m_pNode->key_;
	}
	TypeVal& operator*()
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
	const TypeVal& value() const
	{
		return m_pNode->m_val;
	}
	TypeVal& value()
	{
		return m_pNode->m_val;
	}
	void update()
	{}
//protected:
	TTreeNode* m_pNode;
	TTreeNode* m_pEndNode;
	friend class RBMap;
};
