template <class TypeKey>
struct RBSimpleMultiSetNode
{
	RBMultiSetNode(const TypeKey& key): 
	left_(0)
	,right_(0)
	,parent_(0)
	,prev_(0)
	,next_(0)
	,color_(RED)
	,key_(key)
	,nCnt_(0)
	{}
	RBSimpleMultiSetNode* left_;
	RBSimpleMultiSetNode* right_;
	RBSimpleMultiSetNode* parent_;
	RBSimpleMultiSetNode* prev_;
	RBSimpleMultiSetNode* next_;
	COLOR  color_;
	TypeKey key_;
	int64 nCnt_;

};
