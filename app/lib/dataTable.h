
#ifndef _DATATABLE_h
#define _DATATABLE_h

#include <map>
#include <list>
#include <iterator>

#include <ArduinoJson.h>

///////////////////////////////////////
// TODO hacer doc
///////////////////////////////////////

struct Item {
	int32_t id = -1;
	virtual void serializeItem(JsonObject &obj,bool extra) = 0;
	virtual void deserializeItem(JsonObject &obj) = 0;
};

class Iserializable {
public:
	virtual bool deserializeData(const String & json) = 0;
	virtual String serializeString() = 0;
	virtual void serializeData(JsonArray &obj, bool extra) = 0;
};

template <int N,class T>
class BaseData:public Iserializable {
protected:
	T items[N];
public:
	const int maxSize = N;

	virtual T* getEmpty()
	{
		for (int i = 0; i < N; i++)
		{
			T * item = &items[i];
			if (item->id < 0) return item;
		}
		return nullptr;
	};

	T* create(JsonObject &obj) 
	{
		T* t = getEmpty();

		if (t) {
			t->deserializeItem(obj);
			return t;
		}
		return nullptr;
	};

	virtual void clear() {
		for (int i = 0; i < N; i++)
		{
			T* item = &items[i];
			item->id = -1;
		}
	}

	//Iserializable
	bool deserializeData(const String & json) {
		bool result = false;
		DynamicJsonDocument doc(20000);
		// Parse
		deserializeJson(doc, json);
		// Loop through all the elements of the array
		for (JsonObject item : doc.as<JsonArray>()) {
			push(create(item));
			result = true;
		}

		return result;
	};
	String serializeString() {
		DynamicJsonDocument doc(20000);
		JsonArray root = doc.to<JsonArray>();
		String str;

		serializeData(root);

		serializeJson(root, str);//Pretty
		return str;
	};

	virtual void serializeData(JsonArray &obj, bool extra=false) = 0;
	virtual T* push(T* item) = 0;
};

// map base
template <int N, class T, class K>
class MapBaseData :public BaseData<N,T > {
protected:
	std::map<K, T*> mapItems;
public:
	typename std::map<K, T*>::iterator begin() {
		return mapItems.begin();
	};
	typename std::map<K, T*>::iterator end() {
		return mapItems.end();;
	};

	T* operator[](K key) { return mapItems[key]; };
	uint32_t size() { return mapItems.size(); };

	bool has(K key) { 
		return this->mapItems.find(key) != this->mapItems.end(); 
	};
	virtual void clear() { 
		BaseData<N, T >::clear();
		mapItems.clear(); 
	};
	virtual bool remove(K key) {
		if (this->mapItems.find(key) != this->mapItems.end()) {
			T* item = this->mapItems[key];
			if (this->mapItems.erase(key)) {
				//lo marcamos para reutilizacion
				item->id = -1;
				return true;
			}
		}

		return false;
	};
	bool remove(T* item) { return remove(item->id); };
	void printItems() {
		for (size_t i = 0; i < N; i++)
		{
			Serial.printf("%d item id: %d", i, this->items[i].id);
			if (has(i)) {
				Serial.printf(" has map id: %d\n", this->mapItems[i]->id);
			}
			else Serial.printf(" not map \n");
		}
	};

	void serializeData(JsonArray &obj, bool extra = false) {
		for (auto elem : this->mapItems)
		{
			T* item = elem.second;

			JsonObject o = obj.createNestedObject();
			item->serializeItem(o, extra);
		};
	};

	virtual T* push(T* item) = 0;
};

// Table
template < int N,class T>
class DataTable :public MapBaseData<N,T, int> {
protected:
	int getUniqueId(int id = -1) {
		if (id >= 0) return id;
		for (int i = 0; i < N; i++)
		{
			if (this->mapItems.find(i) == this->mapItems.end())
				return i;
		}
		return -1;
	};
public:
	T* push(T * item) {
		if (item) {
			int id = getUniqueId(item->id);
			if (id >= 0) {
				item->id = id;
				this->mapItems.insert(std::make_pair(id, item));
				return item;
			}
		}
		return nullptr;
	};
};

// List
template <int N,class T>
class DataList :public BaseData<N,T> {
protected:
	std::list<T *> listItems;
public:
	typename std::list<T*>::iterator begin() {
		return listItems.begin();
	};
	typename std::list<T*>::iterator end() {
		return listItems.end();
	};

	uint32_t size() { return  listItems.size(); };
	T* push(T* item) {
		if (item) {
			item->id = 1;
			listItems.push_back(item);
			return item;
		}
		return nullptr;
	};
	bool has(T* item) {
		auto it = std::find(listItems.begin(), listItems.end(), item);
		if (it != listItems.end()) return true;
		return false;
	};
	bool remove(T*item) {
		auto it = std::find(listItems.begin(), listItems.end(), item);

		if (it != listItems.end()) {

			listItems.erase(it);

			//lo marcamos para reutilizacion
			item->id = -1;

			return true;
		}

		return false;
	};
	void clear() {
		BaseData<N, T >::clear();
		listItems.clear();
	};
	bool shift() {
		if (size() <= 0) return false;

		//lo marcamos para reutilizacion
		listItems.front()->id = -1;
		listItems.pop_front();
		return true;
	};
	bool pop() {
		if (size() <= 0) return false;
		//lo marcamos para reutilizacion
		listItems.back()->id = -1;
		listItems.pop_back();
		return true;
	};
	T* last() {
		return listItems.back();
	};
	T* first() {
		return listItems.front();
	};

	void printItems() {
		for (size_t i = 0; i < N; i++)
		{
			Serial.printf("%d item id: %d", i, this->items[i].id);
			if (has(&this->items[i])) {
				Serial.printf(" has listItems\n");
			}
			else Serial.printf(" not listItems \n");
		}
	};
	void serializeData(JsonArray &root, bool extra = false) {
		for (T* item : listItems)
		{
			JsonObject o = root.createNestedObject();
			item->serializeItem(o, extra);
		}
	};
};

#endif

