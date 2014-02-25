#ifndef SUPERMAN_MYCONTACTLISTENER_H_
#define SUPERMAN_MYCONTACTLISTENER_H_

#include "Box2D/Box2D.h"
#include <vector>

struct MyContact {
	b2Fixture *fixtureA;
	b2Fixture *fixtureB;
	
	bool operator==(const MyContact &other) const{
		return (fixtureA == other.fixtureA) && (fixtureB == other.fixtureB);
	}
};

class MyContactListener : public b2ContactListener {
public:
	MyContactListener();
	~MyContactListener();

	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact);
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);    
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

	std::vector<MyContact> _contacts;
};

#endif