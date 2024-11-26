#include "Point.h"
#include "Quadtree.h"
#include <bits/stdc++.h>
 
template <typename T>  
class BPlusTreeNode {  
public:  
    bool isLeaf;  
    std::vector<int> keys;  
    std::vector<std::shared_ptr<BPlusTreeNode<T>>> children;  
    std::vector<T> values;
    std::shared_ptr<BPlusTreeNode<T>> next; 

    BPlusTreeNode(bool leaf) : isLeaf(leaf), next(nullptr) {}  

    void insertNonFull(int key, const T& value, int t);  
    void splitChild(int index, std::shared_ptr<BPlusTreeNode<T>> y, int t);  
};  

template <typename T>  
class BPlusTree {  
private:  
    std::shared_ptr<BPlusTreeNode<T>> root;  
    int t; // Minimum degree  

public:  
    BPlusTree(int degree) : t(degree) {  
        root = std::make_shared<BPlusTreeNode<T>>(true);  
    }  

    void insert(int key, const T& value);  
    void rangeQuery(int lower, int upper, std::vector<T>& res);  
    void rangeQueryHelper(std::shared_ptr<BPlusTreeNode<T>> node, int lower, int upper, std::vector<T>& res);  
    void rangeQuery_my(int lower, int upper, std::vector<T>& res);
};  

template <typename T>  
void BPlusTreeNode<T>::insertNonFull(int key, const T& value, int t) {  
    int i = keys.size() - 1;  

    if (isLeaf) {  
        while (i >= 0 && keys[i] > key) {  
            i--;  
        }  
        keys.insert(keys.begin() + i + 1, key);  
        values.insert(values.begin() + i + 1, value);  
    } else {  
        while (i >= 0 && keys[i] > key) {  
            i--;  
        }  
        i++;  

        if (children[i]->keys.size() == 2 * t - 1) {  
            splitChild(i, children[i], t);  
            if (keys[i] < key) {  
                i++;  
            }  
        }  
        children[i]->insertNonFull(key, value, t);  
    }  
}  

template <typename T>  
void BPlusTreeNode<T>::splitChild(int index, std::shared_ptr<BPlusTreeNode<T>> y, int t) {  
    auto z = std::make_shared<BPlusTreeNode<T>>(y->isLeaf);  

    for (int j = 0; j < t - 1; j++) {  
        z->keys.push_back(y->keys[j + t]);  
    }  
    if (y->isLeaf) {  
        for (int j = 0; j < t - 1; j++) {  
            z->values.push_back(y->values[j + t]);  
        }  
        z->next = y->next; 
        y->next = z; 
    } else {  
        for (int j = 0; j < t; j++) {  
            z->children.push_back(y->children[j + t]);  
        }  
    }  

    y->keys.resize(t - 1);  
    if (y->isLeaf) {  
        y->values.resize(t - 1);  
    } else {  
        y->children.resize(t);  
    }  

    children.insert(children.begin() + index + 1, z);  
    keys.insert(keys.begin() + index, y->keys[t - 1]);  
}  

template <typename T>  
void BPlusTree<T>::insert(int key, const T& value) {  
    if (root->keys.size() == 2 * t - 1) {  
        auto newRoot = std::make_shared<BPlusTreeNode<T>>(false);  
        newRoot->children.push_back(root);  
        newRoot->splitChild(0, root, t);  
        int i = 0;  
        if (newRoot->keys[0] < key) {  
            i++;  
        }  
        newRoot->children[i]->insertNonFull(key, value, t);  
        root = newRoot;  
    } else {  
        root->insertNonFull(key, value, t);  
    }  
}  

template <typename T>  
void BPlusTree<T>::rangeQuery(int lower, int upper, std::vector<T>& res) {  
    rangeQueryHelper(root, lower, upper, res);  
}  

template <typename T>  
void BPlusTree<T>::rangeQueryHelper(std::shared_ptr<BPlusTreeNode<T>> node, int lower, int upper, std::vector<T>& res) {  
    int i = 0;  
    // if(res.size()) return;
    while (i < node->keys.size() && node->keys[i] < lower) {  
        i++;  
    }  

    if (node->isLeaf) {  
        for (; i < node->keys.size() && node->keys[i] <= upper; i++) {  
            res.push_back(node->values[i]);  
            // return;
        }  
    } else {  
        for (; i < node->keys.size() && node->keys[i] <= upper; i++) {  
            rangeQueryHelper(node->children[i], lower, upper, res);  
        }  
        if (i < node->children.size()) {  
            rangeQueryHelper(node->children[i], lower, upper, res);  
        }  
    }  
}    

template <typename T>  
void BPlusTree<T>::rangeQuery_my(int lower, int upper, std::vector<T>& res) {  
    std::shared_ptr<BPlusTreeNode<T>> currentNode = root;  
    while (!currentNode->isLeaf) {  
        int i = 0;  
        while (i < currentNode->keys.size() && currentNode->keys[i] < lower) {  
            i++;  
        }  
        currentNode = currentNode->children[i];  
    }  


    while (currentNode != nullptr) {  
        for (size_t i = 0; i < currentNode->keys.size(); ++i) {  
            if (currentNode->keys[i] >= lower && currentNode->keys[i] <= upper) {  
                //res.push_back(currentNode->values[i]);  
            }  

            if (currentNode->keys[i] > upper) {  
                return;  
            }  
        }  
        
        currentNode = currentNode->next;  
    }  
}
 
const int N = 360078*2 + 31000; 
int M = 10;
// int cnt2 = 0;
#define FLEXIBLE
template<typename T> 
class our_method{
public:
    int siz = 0;
    std::vector<BPlusTree<T>*> t;
    // std::vector<set<T>*> tt;

    our_method():t(N){}
    int lowbit(int x)
    {
        return x & (-x);
    }
    void add(int st, int se, const T& s)
    {
        // cout << "-------" << cnt2 << endl;
        // cnt2 += se - st + 1;
        //cout << st << ' ' << se << "\n";
        ++siz;
        for(int i = st;i < N;i += lowbit(i))
        {
            if(t[i] == nullptr)
            {
                #ifdef FLEXIBLE
                    t[i] = new BPlusTree<T>(max(M, __lg(lowbit(i)) * M));
                #else
                    t[i] = new BPlusTree<T>(M);
                #endif
                t[i]->insert(se, s);
            }
            else
            {
                t[i]->insert(se, s);
            }
        }
    }
    void query(int st, int se, auto& res)
    {
        if(se < st) return;
        if(!siz) return;
        for(int i = se; i; i -= lowbit(i))
        {
            if(t[i] != nullptr) 
                t[i]->rangeQuery(st, 1e9, res);
        }
        //std::cout << res.size() << std::endl;
    }
    int size() const {
        return siz;
    }
};

