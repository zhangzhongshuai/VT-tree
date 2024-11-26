#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
int TEST_TIME = 0;
class Rectangle{
public:
    int minX, minY, maxX, maxY;
public:
    Rectangle(int minX, int minY, int maxX, int maxY)
            : minX(minX), minY(minY), maxX(maxX), maxY(maxY){

    }
    Rectangle() = default;
    friend Rectangle getMBR(std::vector<Rectangle> & a){
        if(a.size() == 0) throw -1;
        Rectangle ans = a[0];
        for(auto x : a){
            ans.minX = std::min(ans.minX, x.minX);
            ans.maxX = std::max(ans.maxX, x.maxX);
            ans.minY = std::min(ans.minY, x.minY);
            ans.maxY = std::max(ans.maxY, x.maxY);
        }
        return ans;
    }
    friend Rectangle merge(const Rectangle & a, const Rectangle & b){
        return Rectangle(std::min(a.minX, b.minX), std::min(a.minY, b.minY),
                         std::max(a.maxX, b.maxX), std::max(a.maxY, b.maxY));
    }
    int getArea(){
        return (maxX - minX) * (maxY - minY);
    }
    bool isOverlap(const Rectangle & other){
        return !(maxX < other.minX || minX > other.maxX || maxY < other.minY || minY > other.maxY);
    }
};

const int MAXSIZE = 4;

template<typename T>
class RtreeNode{
public:
    Rectangle MBR;
    std::vector<RtreeNode<T>*> son;
    std::vector<T> data;
    int last_time;
    bool isLeaf(){
        return son.size() == 0;
    }
    RtreeNode<T>* father;
    RtreeNode() : father(nullptr), last_time(-1){

    }
    void update(){
        std::vector<Rectangle> tmp;
        if(isLeaf()){
            for(auto x : data){
                tmp.push_back(Rectangle(x));
            }
        }
        else{
            for(RtreeNode<T>* x : son){
                tmp.push_back(x->MBR);
            }
        }
        MBR = getMBR(tmp);
        return;
    }
};

template<typename T>
class Rtree{
#define Node RtreeNode<T>
public:
    RtreeNode<T>* root;
    Rtree() : root(nullptr){
    }
    Rtree(const Rtree & other) : root(other.root){
    }
    Node* clone(Node* p){
        Node* node = new Node(*p);
//		std::cout << "OK";
        for(Node* x : node->son){
            x->father = node;
        }
        return node;
    }
    Rtree* clone(){
        return new Rtree(*this);
    }
    Node* insert(Node* p, const T & a){
//		std::cout << p << " ";
        if(p == nullptr){
            p = new Node;
        }
        else p = clone(p);
//		std::cout << p << " ";
        if(p->isLeaf()){
            p->data.push_back(a);
            p->update();
            if(p->data.size() > MAXSIZE){
                if(p->father == nullptr){
                    //it is root
                    Node* newnode = new Node;
                    root = newnode;
                    newnode->MBR = p->MBR;
                    Node* other = new Node;
                    int mid = MAXSIZE / 2;
                    other->father = newnode;
                    for(int i = mid; i < p->data.size(); ++i){
                        other->data.push_back(p->data[i]);
                    }
                    other->update();
                    p->father = newnode;
                    p->data.erase(p->data.begin() + mid, p->data.end());
                    p->update();
                    newnode->son.push_back(p);
                    newnode->son.push_back(other);
                    newnode->update();
                    return root;
                }
                else{
                    Node* other = new Node;
                    int mid = MAXSIZE / 2;
                    other->father = p->father;
                    for(int i = mid; i < p->data.size(); ++i){
                        other->data.push_back(p->data[i]);
                    }
                    other->update();
                    p->data.erase(p->data.begin() + mid, p->data.end());
                    p->update();
                    p->father->son.push_back(other);
                    p->father->update();
                }
            }
//			std::cout << p << " ";
            return p;
        }
        else{
            int pos = 0;
            int mindelta = 0x3f3f3f3f;
            for(int i = 0; i < p->son.size(); ++i){
                if(merge(p->son[i]->MBR, Rectangle(a)).getArea() - p->son[i]->MBR.getArea() < mindelta){
                    mindelta = merge(p->son[i]->MBR, Rectangle(a)).getArea() - p->son[i]->MBR.getArea();
                    pos = i;
                }
            }
            p->update();
//			std::cout << p->son[pos] << " ";
            Node* tmp = insert(p->son[pos], a);
//			std::cout << tmp << ' ';
            p->son[pos] = tmp;
            // p->son[pos] = insert(p->son[pos], a);
//			std::cout << p->son[pos] << " " << p->son.size() << ' ';
            if(p->son.size() > MAXSIZE){
                if(p->father == nullptr){
                    //it is root
                    Node* newnode = new Node;
                    root = newnode;
                    newnode->MBR = p->MBR;
                    Node* other = new Node;
                    int mid = MAXSIZE / 2;
                    other->father = newnode;
                    for(int i = mid; i < p->son.size(); ++i){
                        other->son.push_back(p->son[i]);
                    }
                    other->update();
                    p->father = newnode;
                    p->son.erase(p->son.begin() + mid, p->son.end());
                    p->update();
                    newnode->son.push_back(p);
                    newnode->son.push_back(other);
                    newnode->update();
                    return root;
                }
                else{
                    Node* other = new Node;
                    int mid = MAXSIZE / 2;
                    other->father = p->father;
                    p->father->son.push_back(other);
                    for(int i = mid; i < p->son.size(); ++i){
                        other->son.push_back(p->son[i]);
                    }
                    other->update();
                    p->son.erase(p->son.begin() + mid, p->son.end());
                    p->update();
                    p->father->update();
                }
            }
//			std::cout << p << " ";
            return p;
        }
    }

    Rtree<T>* insert(T a){
        Rtree* tr = clone();
        tr->root = tr->insert(tr->root, a);
        return tr;
    }
    void query(Node* p, const Rectangle & a, std::vector<T> & ans){
        if(p == nullptr) return;
	if(p->last_time == TEST_TIME) return;
	p->last_time = TEST_TIME;
        if(p->isLeaf()){
            for(T x : p->data){
                if(Rectangle(x).isOverlap(a)){
                    ans.push_back(x);
//					std::cout << Rectangle(x).minX;
                }
            }
        }
        else{
            for(Node* x : p->son){
                if(x->MBR.isOverlap(a)){
                    query(x, a, ans);
                }
            }
        }
        return;
    }
    void query(const Rectangle & a, std::vector<T> & ans){
        query(root, a, ans);
        return;
    }

#undef Node
};
//int main(){
//    Rtree<Rectangle> *tree = new Rtree<Rectangle>;
//    for(int i = 0; i < 100; ++i){
//        tree = tree->insert(Rectangle(i, i, 100, 100));
//    }
//    std::vector<Rectangle> ans;
//    tree->query(Rectangle(0, 0, 50, 50), ans);
//    std::cout << ans.size() << std::endl;
//    return 0;
//}
