//
// Created by wmz on 2023/12/4.
//

#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>


template <typename KeyType, typename ValueType>
class InvertedIndex {
public:
    void add(const KeyType& key, const ValueType& value) {
        index[key].push_back(value);
    }
    void add(const KeyType& key, ValueType&& value) {
        index[key].push_back(value);
    }
    const std::vector<ValueType>& get(const KeyType& key) const {
        return index.at(key);
    }

    bool contains(const KeyType& key) const {
        return index.find(key) != index.end();
    }

    std::vector<ValueType> query(KeyType& key)
    {
        return index[key];
    }

    void query(std::vector<KeyType>& keylist,std::vector<ValueType>& valuelist)
    {
        for(auto key: keylist)
        {
            valuelist.insert(valuelist.end(),index[key].begin(),index[key].end());
        }
    }

    int size()
    {
        return index.size();
    }
//    test
    void print()
    {
        for(auto item: index)
        {
            std::cout<<item.first<<":"<<std::endl;
            for(auto item1: item.second)
            {
                item1.print();
            }
            std::cout<<std::endl;
        }
    }
private:
    std::unordered_map<KeyType, std::vector<ValueType>> index;
};


#endif
