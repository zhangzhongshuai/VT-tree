#ifndef __CSETREE__
#define __CSETREE__
#include <functional>
#include <cstring>
#include <vector>
#include <cmath>
#include <iostream>
namespace cse{
    const int TIME_THRESHOLD = 1000;
    const int K = 5;
    int MODIFY_TIMES; //当前修改次数
    template<typename T>
    struct StartTimeNode{
        int size = 0; //子树大小
        int n = 0; //当前节点键值个数
        int key[K];
        std::vector<T>* value[K];
        StartTimeNode* father;
        StartTimeNode* son[K + 1];
        StartTimeNode() : n(0), size(0), father(nullptr){
        	for(int i = 0; i <= K; ++i) son[i] = nullptr;
            for(int i = 0; i < K; ++i){
                value[i] = new(std::vector<T>);
            }
        }
        ~StartTimeNode(){
            for(int i = 0; i < K; ++i){
                delete value[i];
            }
            for(int i = 0; i <= n; ++i){
                if(son[i] != nullptr){
                    delete son[i];
                }
            }
        }
    };
    template<typename T>
    struct StartTimeTree{
        #define StartTimeNode StartTimeNode<T>
        private:
        int lastModifyTime;
        bool isCompressed;
        const std::function<int(T)> getStartTime;
        struct StartTimeNode* root;
        int minEt, maxEt;
        std::vector<T> s; //压缩后节点

        bool isLeaf(StartTimeNode* a){
            return a->son[0] == nullptr;
        }
        int getPos(StartTimeNode* p){
            if(p -> fa == nullptr) return -1;
            for(int i = 0; i <= p -> fa -> n; ++i){
                if(p -> fa -> son[i] == p) return i;
            }
            return -1;
        }
        void update(StartTimeNode* p){
            p -> size = 0;
            for(int i = 0; i < p -> n; ++i){
                p -> size += p -> value[i] -> size();
            }
            for(int i = 0; i <= p -> n; ++i){
                if(p -> son[i] != nullptr) p -> size += p -> son[i] -> size;
            }
            return;
        }
        void insertNonFullNode(StartTimeNode* p, int st, std::vector<T>* val, StartTimeNode* other){
            int pos = 0;
            while(pos < p -> n && p -> key[pos] < st) ++pos;
            p -> n++;
            // if(p -> n - 1 > pos) delete p -> value[p -> n - 1];
            for(int i = p -> n - 1; i > pos; --i){
                p -> key[i] = p -> key[i - 1];
//                delete p -> value[i];
                p -> value[i] = p -> value[i - 1];
                p -> son[i + 1] = p -> son[i];
            }
            p -> key[pos] = st;
            // std::cerr << p << " " << pos << std::endl;
//            delete p -> value[pos];
            p -> value[pos] = val;
            p -> son[pos + 1] = other;
            other -> father = p;
            // std::cerr << "fa" << other << " " << p << std::endl;
            update(p);
            return;
        }
        void split(StartTimeNode* p){
            if(p -> n < K) return;
            if(p == root){
                root = p -> father = new(StartTimeNode);
                p -> father -> son[0] = p;
            }
            int mid = (K >> 1);
            StartTimeNode* other = new StartTimeNode;
            for(int i = mid + 1; i < p -> n; ++i){
                other -> key[other -> n] = p -> key[i];
//                delete other -> value[other -> n];
                other -> value[other -> n] = p -> value[i];
                other -> son[other -> n] = p -> son[i];
                if(other -> son[other -> n] != nullptr) other -> son[other -> n] -> father = other;
                ++other -> n;
            }
            other -> son[other -> n] = p -> son[p -> n];
            if(other -> son[other -> n] != nullptr) other -> son[other -> n] -> father = other;
            // other -> father = p -> father;
            update(other);
            p -> n = mid;
            update(p);
            insertNonFullNode(p -> father, p -> key[mid], p -> value[mid], other);
            return;
        }
        void addToTree(StartTimeNode* p, const T & val){
            int st = getStartTime(val);
            // std::cerr << "val:" << st <<std::endl;
            if(p == nullptr){
                p = new StartTimeNode;
                p -> n = 1;
                p -> key[0] = st;
                p -> value[0] -> push_back(val);
                if(root == nullptr) root = p;
            }
            else if(isLeaf(p)){ //叶子节点，直接插入
                int pos = 0;
                while(pos < p -> n && p -> key[pos] < st) ++pos; //找到插入节点
                if(p -> key[pos] == st){ //如果 key 匹配直接 push
                    // std::cerr<< st;
                    p -> value[pos] -> push_back(val);
                }
                else{
                    ++p -> n;
                    for(int i = p -> n - 1; i > pos; --i){ //否则之后的节点向后移
                        p -> key[i] = p -> key[i - 1];
//                        delete p -> value[i];
                        p -> value[i] = p -> value[i - 1];
                    }
                    //加入新节点
                    p -> key[pos] = st;
//                    delete p -> value[pos];
                    p -> value[pos] = new std::vector<T>;
                    p -> value[pos] -> push_back(val);
                }
            }
            else{
                int pos = 0;
                while(pos < p -> n && p -> key[pos] < st) ++pos; //寻找试图插入位置
                if(pos < p -> n && p -> key[pos] == st){ //若 key 存在直接插入
                    p -> value[pos] -> push_back(val);
                }
                else{
                    // std::cerr << "->" << p << " " << pos << std::endl;
                    addToTree(p -> son[pos], val); //否则递归插入
                }
            }
            if(p -> n >= K) split(p); //分裂
            update(p);
        }
        void transToVector(StartTimeNode* p, std::vector<T> &ans){
            if(p == nullptr) return;
            if(isLeaf(p)){
                for(int i = 0; i < p -> n; ++i){
                    for(auto a : *(p -> value[i])){
                        ans.push_back(a);
                    }
                }
                return;
            }
            else{
                if(p -> son[0] != nullptr) transToVector(p -> son[0], ans);
                for(int i = 0; i < p -> n; ++i){
                    for(auto a : *(p -> value[i])){
                        ans.push_back(a);
                    }
                    if(p -> son[i + 1] != nullptr) transToVector(p -> son[i + 1], ans);
                }
                return;
            }
        }
        // 找出小于时间 t 的元素
        void queryTree(StartTimeNode* p, int t, std::vector<T> &ans){
            if(p == nullptr) return;
            if(isLeaf(p)){
                for(int i = 0; i < p -> n && p -> key[i] <= t; ++i){
                    for(auto a : *(p -> value[i])){
                        ans.push_back(a);
                        // std::cerr << "()" << ans.size() << std::endl;
                    }
                }
                return;
            }
            else{
                if(p -> son[0] != nullptr) queryTree(p -> son[0], t, ans);
                for(int i = 0; i < p -> n && p -> key[i] <= t; ++i){
                    for(auto a : *(p -> value[i])){
                        ans.push_back(a);
                    }
                    if(p -> son[i + 1] != nullptr) queryTree(p -> son[i + 1], t, ans);
                }
                return;
            }
        }
        public:
        StartTimeTree(auto fun, int minEt = 0, int maxEt = 0x3f3f3f3f) : getStartTime(fun), lastModifyTime(MODIFY_TIMES), isCompressed(false), minEt(minEt), maxEt(maxEt), root(nullptr){
        }
        void setMinEt(int t){
            minEt = t;
        }
        void setMaxEt(int t){
            maxEt = t;
        }
        int getMinEt(){
            return minEt;
        }
        int getMaxEt(){
            return maxEt;
        }
        int getSize(){
            return root -> size;
        }
        void insert(T val){
            if(!isCompressed){
                // std::cerr << "vv:" << getStartTime(val) << std::endl;
                addToTree(root, val);
            }
            else{

            }
            lastModifyTime = ++MODIFY_TIMES;
            return;
        }
        void query(int t, std::vector<T> &ans){
            if(!isCompressed){
                queryTree(root, t, ans);
                return;
            }
            else{

            }
        }
        #undef StartTimeNode
    };
    template<typename T>
    struct EndTimeNode{
        int size; //子树大小
        int n; //当前节点键值个数
        EndTimeNode* father;
        int key[K];
        StartTimeTree<T>* value[K];
        EndTimeNode* son[K + 1];
        EndTimeNode() : n(0), size(0), father(nullptr){
            memset(son, 0, sizeof(son));
            for(int i = 0; i <= K; ++i){
            	son[i] = nullptr;
			}
			for(int i = 0; i < K; ++i){
				value[i] = nullptr;
			}
        }
        ~EndTimeNode(){
            for(int i = 0; i < K; ++i){
                delete value[i];
            }
            for(int i = 0; i <= K; ++i){
                if(son[i] != nullptr){
                    delete son[i];
                }
            }
        }
    };
    template<typename T>
    class CSEtree{
        #define CSENode EndTimeNode<T>
        private:
        const std::function<int(T)> getStartTime, getEndTime;
        int maxEt;
        CSENode* root;
        bool isLeaf(CSENode* a){
            return a -> son[0] == nullptr;
        }
        int getPos(CSENode* p){
            if(p -> fa == nullptr) return -1;
            for(int i = 0; i <= p -> fa -> n; ++i){
                if(p -> fa -> son[i] == p) return i;
            }
            return -1;
        }
        void update(CSENode* p){
            p -> size = p -> n;
            for(int i = 0; i <= p -> n; ++i){
                if(p -> son[i] != nullptr) p -> size += p -> son[i] -> size;
            }
            return;
        }
        void insertNonFullNode(CSENode* p, int et, StartTimeTree<T>* val, CSENode* other){
            int pos = 0;
            while(pos < p -> n && p -> key[pos] < et) ++pos;
            p -> n++;
            // std::cerr << pos << " ";
            for(int i = p -> n - 1; i > pos; --i){
                p -> key[i] = p -> key[i - 1];
                p -> value[i] = p -> value[i - 1];
                p -> son[i + 1] = p -> son[i];
            }
            p -> key[pos] = et;
            p -> value[pos] = val;
            p -> son[pos + 1] = other;
            other -> father = p;
            update(p);
            return;
        }
        void split(CSENode* p){
            if(p -> n < K) return;
            if(p == root){
                root = p -> father = new(CSENode);
                p -> father -> son[0] = p;
            }
            int mid = (K >> 1);
            CSENode* other = new CSENode;
            for(int i = mid + 1; i < p -> n; ++i){
                other -> key[other -> n] = p -> key[i];
                other -> value[other -> n] = p -> value[i];
                other -> son[other -> n] = p -> son[i];
                if(other -> son[other -> n] != nullptr) other -> son[other -> n] -> father = other;
                ++other -> n;
            }
            other -> son[other -> n] = p -> son[p -> n];
            if(other -> son[other -> n] != nullptr) other -> son[other -> n] -> father = other;
            update(other);
            p -> n = mid;
            update(p);
            insertNonFullNode(p -> father, p -> key[mid], p -> value[mid], other);
            // std::cerr << mid << " ";
            return;
        }
        void add(CSENode* p, const T & val){
            int et = getEndTime(val);
            // std::cerr << et << " ";
            if(et > maxEt) maxEt = et;
            if(p == nullptr){
                p = new CSENode;
                p -> n = 1;
                p -> key[0] = 0x3f3f3f3f;
                p -> value[0] = new StartTimeTree<T>(getStartTime);
                p -> value[0] -> insert(val);
                if(root == nullptr) root = p;
            }
            else if(isLeaf(p)){ //叶子节点，直接插入
                int pos = 0;
                while(pos < p -> n && p -> key[pos] < et) ++pos; //找到插入节点
                // std::cerr << pos << " ";
                if(pos == p -> n){
                    //比右端点还大，新开辟节点
                    // std::cerr << "new node";
                    ++p -> n;

                    p -> value[pos] = new StartTimeTree<T>(getStartTime, p -> key[pos - 1] + 1);
                    p -> value[pos] -> insert(val);
                    p -> key[pos] = 0x3f3f3f3f;
                        // std::cerr << "yes";
                }
                else if(p -> value[pos] -> getMinEt() <= et){ //如果 key 匹配直接 push
                    // std::cerr << p << " ";
                    if(getStartTime(val) == 402){
                        // std::cerr << " now";
                    }
                    p -> value[pos] -> insert(val);
                        // std::cerr << p -> value[pos] -> getSize() << std::endl;
                    if(p -> key[pos] == 0x3f3f3f3f && p -> value[pos] -> getSize() >= TIME_THRESHOLD){
                        p -> value[pos] -> setMaxEt(maxEt);
                        p -> key[pos] = maxEt;
                        // std::cerr << "yes";
                    }
                }
            }
            else{
                int pos = 0;
                while(pos < p -> n && p -> key[pos] < et) ++pos; //寻找试图插入位置

                if(pos < p -> n && p -> value[pos] -> getMinEt() <= et){ //如果 key 匹配直接 push
                    p -> value[pos] -> insert(val);
                }
                else{
                    add(p -> son[pos], val); //否则递归插入
                // std::cerr << "3!";
                }
            }
            if(p -> n >= K) split(p); //判断是否需要分裂
            update(p);
            return;
        }
        // 找出时间内的元素
        void queryTree(CSENode* p, int st, int et, std::vector<T> &ans){
            if(p == nullptr) return;
            if(isLeaf(p)){
                // std::cout << p -> key[0] << std::endl;
                for(int i = p -> n - 1; i >= 0 && p -> key[i] >= st; --i){
                    p -> value[i] -> query(et, ans);
                }
                return;
            }
            else{
                if(p -> son[0] != nullptr) queryTree(p -> son[0], st, et, ans);
                for(int i = 0; i < p -> n && p -> key[i] >= st; ++i){
                    p -> value[i] -> query(et, ans);
                    if(p -> son[i + 1] != nullptr) queryTree(p -> son[i + 1], st, et, ans);
                }
                return;
            }
        }
        public:
        CSEtree(auto fun1, auto fun2) : getStartTime(fun1), getEndTime(fun2), root(nullptr), maxEt(0){
        }
        void insert(T val){
//            std::cerr<<getStartTime(val) << " " <<getEndTime(val) << std::endl;
            add(root, val);
//            std::cerr << MODIFY_TIMES << " ";
            // std::cerr << "[!" << getStartTime(val) << " " << getEndTime(val) << std::endl;
        }
        void query(int st, int et, std::vector<T> &ans){
            std::vector<T> tmp;
            queryTree(root, st, et, tmp);
            for(auto x : tmp){
                // std::cerr << "[}" << getStartTime(x) << " " << getEndTime(x) << std::endl;
                if(getEndTime(x) < st || getStartTime(x) > et) continue;
                else ans.push_back(x);
            }
        }
        int size(){
            if(root == nullptr) return 0;
            else return root -> size;
        }
        #undef CSEnode
    };
};

#endif
