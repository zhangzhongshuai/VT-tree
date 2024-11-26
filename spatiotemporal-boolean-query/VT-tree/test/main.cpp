#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

// 定义矩形类
template <typename T>
class Rectangle {
public:
    T data;
    std::function<double(const T&)> getMinX;
    std::function<double(const T&)> getMinY;
    std::function<double(const T&)> getMaxX;
    std::function<double(const T&)> getMaxY;

    // 默认构造函数
    Rectangle() {}

    Rectangle(T data,
              std::function<double(const T&)> getMinX,
              std::function<double(const T&)> getMinY,
              std::function<double(const T&)> getMaxX,
              std::function<double(const T&)> getMaxY)
            : data(data), getMinX(getMinX), getMinY(getMinY), getMaxX(getMaxX), getMaxY(getMaxY) {}

    double minX() const { return getMinX(data); }
    double minY() const { return getMinY(data); }
    double maxX() const { return getMaxX(data); }
    double maxY() const { return getMaxY(data); }

    bool overlaps(const Rectangle<T>& other) const {
        return !(minX() > other.maxX() || maxX() < other.minX() ||
                 minY() > other.maxY() || maxY() < other.minY());
    }
};

// 定义R树节点类
template <typename T>
class RTreeNode {
public:
    std::vector<Rectangle<T>> rectangles;
    std::vector<RTreeNode*> children;
    bool isLeaf;

    RTreeNode(bool isLeaf = true) : isLeaf(isLeaf) {}

    ~RTreeNode() {
        for (RTreeNode* child : children) {
            delete child;
        }
    }
};

// 定义R树类
template <typename T>
class RTree {
public:
    RTreeNode<T>* root;
    int maxEntries;
    int minEntries;

    RTree(int maxEntries = 4, int minEntries = 2)
            : root(new RTreeNode<T>(true)), maxEntries(maxEntries), minEntries(minEntries) {}

    ~RTree() {
        delete root;
    }

    void insert(const Rectangle<T>& rect) {
        RTreeNode<T>* node = chooseLeaf(root, rect);
        node->rectangles.push_back(rect);
        if (node->rectangles.size() > maxEntries) {
            split(node);
        }
    }

    std::vector<Rectangle<T>> search(const Rectangle<T>& query) {
        std::vector<Rectangle<T>> result;
        searchRecursive(root, query, result);
        return result;
    }

private:
    RTreeNode<T>* chooseLeaf(RTreeNode<T>* node, const Rectangle<T>& rect) {
        if (node->isLeaf) {
            return node;
        }

        // 简单选择第一个子节点，可以改进
        return chooseLeaf(node->children[0], rect);
    }

    void split(RTreeNode<T>* node) {
        RTreeNode<T>* newNode = new RTreeNode<T>(node->isLeaf);
        int half = node->rectangles.size() / 2;
        for (int i = half; i < node->rectangles.size(); ++i) {
            newNode->rectangles.push_back(node->rectangles[i]);
        }
        node->rectangles.resize(half);

        if (node == root) {
            RTreeNode<T>* newRoot = new RTreeNode<T>(false);
            newRoot->children.push_back(node);
            newRoot->children.push_back(newNode);
            root = newRoot;
        } else {
            // 处理非根节点的分裂
            // 简化处理，实际中需要将newNode插入父节点，并可能继续分裂父节点
        }
    }

    void searchRecursive(RTreeNode<T>* node, const Rectangle<T>& query, std::vector<Rectangle<T>>& result) {
        for (const auto& rect : node->rectangles) {
            if (rect.overlaps(query)) {
                if (node->isLeaf) {
                    result.push_back(rect);
                } else {
                    for (RTreeNode<T>* child : node->children) {
                        searchRecursive(child, query, result);
                    }
                }
            }
        }
    }
};

// 示例使用
int main() {
    auto getMinX = [](const std::pair<std::pair<double, double>, std::pair<double, double>>& p) { return p.first.first; };
    auto getMinY = [](const std::pair<std::pair<double, double>, std::pair<double, double>>& p) { return p.first.second; };
    auto getMaxX = [](const std::pair<std::pair<double, double>, std::pair<double, double>>& p) { return p.second.first; };
    auto getMaxY = [](const std::pair<std::pair<double, double>, std::pair<double, double>>& p) { return p.second.second; };

    RTree<std::pair<std::pair<double, double>, std::pair<double, double>>> rtree;
    rtree.insert(Rectangle<std::pair<std::pair<double, double>, std::pair<double, double>>>({{1.0, 2.0}, {2.0, 3.0}}, getMinX, getMinY, getMaxX, getMaxY));
    rtree.insert(Rectangle<std::pair<std::pair<double, double>, std::pair<double, double>>>({{3.0, 4.0}, {4.0, 5.0}}, getMinX, getMinY, getMaxX, getMaxY));
    rtree.insert(Rectangle<std::pair<std::pair<double, double>, std::pair<double, double>>>({{5.0, 6.0}, {6.0, 7.0}}, getMinX, getMinY, getMaxX, getMaxY));

    Rectangle<std::pair<std::pair<double, double>, std::pair<double, double>>> query({{2.0, 2.0}, {5.0, 5.0}}, getMinX, getMinY, getMaxX, getMaxY);
    std::vector<Rectangle<std::pair<std::pair<double, double>, std::pair<double, double>>>> results = rtree.search(query);

    std::cout << "Query results:" << std::endl;
    for (const auto& rect : results) {
        std::cout << "Rectangle: ((" << rect.minX() << ", " << rect.minY() << "), (" << rect.maxX() << ", " << rect.maxY() << "))" << std::endl;
    }

    return 0;
}
