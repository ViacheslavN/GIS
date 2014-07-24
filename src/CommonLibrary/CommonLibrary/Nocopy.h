#ifndef __NOCOPY_H__
#define __NOCOPY_H__

class Nocopy
{
	const Nocopy & operator=(const Nocopy & );
	Nocopy(const Nocopy & );
public:
	Nocopy()
	{}
	virtual ~Nocopy()
	{}
};

#endif //__NOCOPY_H__