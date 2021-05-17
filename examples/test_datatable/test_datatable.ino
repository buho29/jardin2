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
	//myTable.deserializeData(("teta"));
	//Serial.println(myTable.serializeString());
	//Serial.println( myList.serializeString());
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
maxsize :7 - 5
init 0 :: table 1
init 1 :: table 2
init 2 :: table 3
init 3 :: table 4
init 4 :: table 5
init 5 :: table 6
init 6 :: table 7
remove(id 4) True
end 0 :: table 1
end 1 :: table 2
end 2 :: table 3
end 3 :: table 4
end 5 :: table 6
end 6 :: table 7
init list 1
init list 2
init list 3
init list 4
init list 5
shift() True
end list 2
end list 3
end list 4
end list 5
[{"id":0,"name":"table 1","edad":68},{"id":1,"name":"table 2","edad":67},{"id":2,"name":"table 3","edad":66},{"id":3,"name":"table 4","edad":65},{"id":5,"name":"table 6","edad":63},{"id":6,"name":"table 7","edad":62}]
[{"id":1,"name":"list 2","edad":21},{"id":1,"name":"list 3","edad":22},{"id":1,"name":"list 4","edad":23},{"id":1,"name":"list 5","edad":24}]
*/