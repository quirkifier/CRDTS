#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
using namespace std;

class ORSet
{
private:
    string nodeId;
    int counter;
    set<pair<string, string>> added;
    set<pair<string, string>> removed;

    void processIncomingTag(const string& tag)
    {
        size_t colonPos = tag.find(":");
        if (colonPos != string::npos)
        {
            string id = tag.substr(0, colonPos);
            int num = stoi(tag.substr(colonPos + 1));
            if (id == nodeId)
            {
                counter = max(counter, num);
            }
        }
    }

public:
    ORSet(string id = "") 
    {
        nodeId = id;
        counter = 0;
    }

    void add(string element)
    {
        counter++;
        string tag = nodeId + ":" + to_string(counter);
        added.insert({ element, tag });
    }

    void remove(string element) 
    {
        for (auto item : added) 
        {
            if (item.first == element) 
            {
                removed.insert(item);
            }
        }
    }

    set<string> query() const 
    {
        set<string> result;

        for (auto item : added)
        {
            if (removed.find(item) == removed.end())
            {
                result.insert(item.first);
            }
        }

        return result;
    }

    void merge(const ORSet& other) 
    {
        for (const auto& item : other.added) 
        {
            added.insert(item);
            processIncomingTag(item.second);
        }

        for (const auto& item : other.removed)
        {
            removed.insert(item);
        }
    }

    bool equals(const ORSet& other) const 
    {
        return added == other.added && removed == other.removed;
    }

    string serialize() const
    {
        stringstream ss;

        ss << nodeId << "\n";
        ss << counter << "\n";

        ss << added.size() << "\n";
        for (const auto& item : added) 
        {
            ss << item.first << "|" << item.second << "\n";
        }

        ss << removed.size() << "\n";
        for (const auto& item : removed) 
        {
            ss << item.first << "|" << item.second << "\n";
        }

        return ss.str();
    }

    static ORSet deserialize(string data) 
    {
        stringstream ss(data);

        string nodeId;
        string line;
        int counter;

        if (!getline(ss, nodeId)) return ORSet("");
        if (!getline(ss, line)) return ORSet(nodeId);
        counter = stoi(line);

        ORSet obj(nodeId);
        obj.counter = counter;

        int addedSize;
        if (getline(ss, line)) {
            addedSize = stoi(line);
            for (int i = 0; i < addedSize; i++) 
            {
                if (!getline(ss, line)) break;
                int sep = line.find("|");
                string element = line.substr(0, sep);
                string tag = line.substr(sep + 1);
                obj.added.insert({ element, tag });
                obj.processIncomingTag(tag);
            }
        }

        int removedSize;
        if (getline(ss, line)) {
            removedSize = stoi(line);
            for (int i = 0; i < removedSize; i++) 
            {
                if (!getline(ss, line)) break;
                int sep = line.find("|");
                string element = line.substr(0, sep);
                string tag = line.substr(sep + 1);
                obj.removed.insert({ element, tag });
            }
        }

        return obj;
    }

    void print() const 
    {
        set<string> result = query();

        cout << "{ ";
        for (auto element : result) 
        {
            cout << element << " ";
        }
        cout << "}" << endl;
    }

    void debugPrint() const 
    {
        cout << "\nAdded:\n";
        for (auto item : added)
        {
            cout << "(" << item.first << ", " << item.second << ")" << endl;
        }

        cout << "Removed:\n";
        for (auto item : removed) 
        {
            cout << "(" << item.first << ", " << item.second << ")" << endl;
        }
    }
};
