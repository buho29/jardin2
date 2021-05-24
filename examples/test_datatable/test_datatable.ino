#include "lib/dataTable.h"
#include <AUnit.h>
using aunit::TestRunner;

struct MyItem : public Item
{
	uint8_t edad;
	char name[20] = "";

	void set(int id, uint8_t edad, const char * name)
	{
		this->id = id; this->edad = edad;
		strcpy(this->name, name);
	};
	void serializeItem(JsonObject &obj, bool extra) 
	{
		obj["id"] = this->id;
		obj["edad"] = this->edad;
		obj["name"] = this->name;
	};
	void deserializeItem(JsonObject &obj) {
		set(
			obj["id"].as<int>(),
			obj["edad"].as<int>(),
			obj["name"].as<char*>()
		);
	};
};

DataTable<7,MyItem> myTable;
DataList<5,MyItem> myList;

test(DataTable) {

	assertEqual(myTable.maxSize,7);
	assertEqual(myList.maxSize,5);

	//fill table
	int i = myTable.size();
	while (myTable.size() < myTable.maxSize)
	{
		String t = "table " + (String)++i;

		MyItem* item = myTable.getEmpty();
		assertTrue(item);
		if (item) {
			item->set(-1, 69 - i, t.c_str());
			myTable.push(item);
		}
	}
	//fill list
	i = myList.size();
	while (myList.size() < myList.maxSize)
	{
		String t = "list " + (String)++i;

		MyItem* item = myList.getEmpty();
		assertTrue(item);
		if (item) {
			item->set(-1, 19 + i, t.c_str());
			myList.push(item);
		}
	}

	//Table loop

	assertTrue(myTable.remove(4));
	assertFalse(myTable.has(4));
	assertFalse(myTable.has(666));
	assertTrue(myTable.has(0));

	MyItem* item = myList.first();
	assertTrue(myList.shift());
	assertFalse(myList.has(item));

	Serial.println("myTable");
	for (auto pair : myTable)
		Serial.printf("\tid %d %s \n", pair.first, pair.second->name);

	Serial.println("myList");
	for (MyItem* item : myList)
		Serial.printf("\t %s\n", item->name);

	//print json
	Serial.println(myTable.serializeString());
	Serial.println( myList.serializeString());

	myTable.clear();
	assertEqual((int)myTable.size(), 0);
	myList.clear();
	assertEqual((int)myList.size(), 0);

	String json = "[{\"id\":1,\"edad\":21,\"name\":\"list 2\"},{\"id\":1,\"edad\":22,\"name\":\"list 3\"}]";
	assertTrue(myTable.deserializeData(json));
	assertEqual((int)myTable.size(), 2);
	assertFalse(myTable.deserializeData(String("teta")));

}

void setup() 
{
	delay(1000);
	Serial.begin(115200);
}


void loop()
{
	TestRunner::run();
}

/* output:
05:00:49:985 -> TestRunner started on 1 test(s).
myTable
	id 0 table 1
	id 1 table 2
	id 2 table 3
	id 3 table 4
	id 5 table 6
	id 6 table 7
myList
	 list 2
	 list 3
	 list 4
	 list 5
[{"id":0,"edad":68,"name":"table 1"},{"id":1,"edad":67,"name":"table 2"},{"id":2,"edad":66,"name":"table 3"},{"id":3,"edad":65,"name":"table 4"},{"id":5,"edad":63,"name":"table 6"},{"id":6,"edad":62,"name":"table 7"}]
[{"id":1,"edad":21,"name":"list 2"},{"id":1,"edad":22,"name":"list 3"},{"id":1,"edad":23,"name":"list 4"},{"id":1,"edad":24,"name":"list 5"}]
Test DataTable passed.
TestRunner duration: 0.038 seconds.
TestRunner summary: 1 passed, 0 failed, 0 skipped, 0 timed out, out of 1 test(s).
*/