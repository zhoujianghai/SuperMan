#include "MyContactListener.h"


MyContactListener::MyContactListener()
{

}


MyContactListener::~MyContactListener()
{

}

void MyContactListener::BeginContact(b2Contact* contact)
{
	MyContact myContact = {contact->GetFixtureA(), contact->GetFixtureB()};
	_contacts.push_back(myContact);
}

void MyContactListener::EndContact(b2Contact* contact)
{
	MyContact myContact = {contact->GetFixtureA(), contact->GetFixtureB()};
	std::vector<MyContact>::iterator it = std::find(_contacts.begin(), _contacts.end(), myContact);
	if(it != _contacts.end()) {
		_contacts.erase(it);
	}
}
