#include <vector>
#include <utility>      // std::pair
#include <iostream>
#include <string>
#include <tuple>
#include <typeinfo>
#include <algorithm> 

using namespace std;
typedef tuple<string,string,int> contact;
typedef vector< pair< string,vector<contact> > > list;

// Functions

//query lists
pair<int, vector<string> > query_lists(list allContacts) 
{
	vector<string> names;
	for(list::iterator it = allContacts.begin(); it !=allContacts.end(); it++)
	{
		names.push_back(it->first);
	}
	return make_pair(allContacts.size(), names);
	 
}

// print vector string only
void printVector(vector<string> names)
{
	for(vector<string>::iterator it = names.begin(); it !=names.end(); it++)
	{
		cout << *it << endl;
	}
}

void printVectorTuple(vector<contact> names)
{
	for(vector<contact>::iterator it = names.begin(); it !=names.end(); it++)
	{
		cout << "(" << get<0>(*it) << ", " << get<1>(*it) << ", " << get<2>(*it) << ")" << endl;
	}
}
// register contact-list-name
int registerContactListName(list& contactList, string name)
{
	vector<contact> empty;
	for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
	{
		if(name == it->first)
		{
			cout << "FAILURE" << endl;
			return 0;
		}
	}
	contactList.push_back(make_pair(name, empty));
	cout << "SUCCESS"<< endl;
	return 1;


}

// leave contact-list-name contact name

int leave(list& contactList, string contactListName, string hostName)
{
	
	for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
	{
		if(contactListName == it->first)
		{
			if(it->second.size() == 0)
			{
				cout << "Failure" << endl;
				return 0;
			}
			for(int i =0; i <= it->second.size(); i++)
			{
				if(hostName == get<0>(*(it->second.begin()+i)))
				{
					it->second.erase(it->second.begin()+i);
					cout << "SUCCESS" << endl;
					return 1;
				}
			}
			cout << "Failure" << endl;
			return 0;
		}
	}
	cout << "Failure" << endl;
	return 0;

}

// im contact-list-name contact -name
pair<int, vector<contact> > im(list contactList, string contactListName, string hostName)
{
	int size;
	vector <contact> hostContacts;
	for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
	{
		if(contactListName == it->first)
		{	
			size = it->second.size();
			for(int i =0; i <= it->second.size(); i++)
			{
				if(hostName == get<0>(*(it->second.begin()+i)))
				{
					hostContacts.push_back(*(it->second.begin()+i));
					it->second.erase(it->second.begin()+i);
					hostContacts.insert(hostContacts.end(), it->second.begin(), it->second.end());
					break;
				}
			}

		}
	}

	return make_pair(size, hostContacts);


}
// join contact-list-name contact-name Ip address port
int join(list& contactList, string listName, string hostName, string ip, int port)
{
	for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
	{
		if(listName == it->first)
		{
			for(vector<contact>::iterator it2 = it->second.begin(); it2 !=it->second.end(); it2++)
			{
				if(hostName == get<0>(*it2))
				{
					cout << "FAILURE" << endl;
					return 0;
				}
			}
			it->second.push_back(make_tuple(hostName,ip,port));
			cout << "SUCCESS" << endl;
			return 1;
		}
		
	}
	cout << "FAILURE" << endl;
	return 0;

} 
int main()
{
	list contactList;
	vector<contact> bobs;

	/* ----------- Testing ---------------- */
	// creating contact lists
	contactList.push_back(make_pair("bob", bobs));
	contactList.push_back(make_pair("alice", bobs));
	
	// create pair to Return Query List
	pair<int, vector<string> > toReturn = query_lists(contactList);
	
	cout << toReturn.first << endl;
	printVector(toReturn.second);

	//register contact-list-name 
	registerContactListName(contactList,"newTest");
	registerContactListName(contactList,"bob");

	// testing join function
	join(contactList, "bob", "kevin", "10", 15);
	join(contactList, "bob", "k", "101", 115);
	join(contactList, "bob", "evin", "109", 1445);
	join(contactList, "bob", "vin", "180", 1885);
	

	auto toReturn2 = im(contactList, "bob", "kevin");
	cout << toReturn2.first << endl;
	printVectorTuple(toReturn2.second);
	//cout << 'Hello World!\n';
	return 0;
}
