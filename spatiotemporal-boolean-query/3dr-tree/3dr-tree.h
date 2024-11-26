#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>

// 定义3D点结构
struct Point3D {
    double x, y, z;
};

// 3DR树节点类
template <typename T>
class TreeNode {
public:
    TreeNode(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax, int maxObjects)
        : xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax), zMin(zMin), zMax(zMax), maxObjects(maxObjects) {}

    bool insert(const T& object, std::function<Point3D(const T&)> getCoordinates) {
        Point3D coords = getCoordinates(object);
        if (!contains(coords)) {
            return false;
        }

        if (objects.size() < maxObjects || isLeaf()) {
            objects.push_back(object);
            return true;
        }

        if (children.empty()) {
            subdivide();
        }

        for (auto& child : children) {
            if (child->insert(object, getCoordinates)) {
                return true;
            }
        }

        return false;
    }

    std::vector<T> range_query(double xMinQuery, double xMaxQuery, double yMinQuery, double yMaxQuery, double zMinQuery, double zMaxQuery, std::function<Point3D(const T&)> getCoordinates) {
        std::vector<T> results;
        if (!intersects(xMinQuery, xMaxQuery, yMinQuery, yMaxQuery, zMinQuery, zMaxQuery)) {
            return results;
        }

        for (const auto& object : objects) {
            Point3D coords = getCoordinates(object);
            if (coords.x >= xMinQuery && coords.x <= xMaxQuery &&
                coords.y >= yMinQuery && coords.y <= yMaxQuery &&
                coords.z >= zMinQuery && coords.z <= zMaxQuery) {
                results.push_back(object);
            }
        }

        if (!children.empty()) {
            for (const auto& child : children) {
                auto childResults = child->range_query(xMinQuery, xMaxQuery, yMinQuery, yMaxQuery, zMinQuery, zMaxQuery, getCoordinates);
                results.insert(results.end(), childResults.begin(), childResults.end());
            }
        }

        return results;
    }

private:
    double xMin, xMax, yMin, yMax, zMin, zMax;
    int maxObjects;
    std::vector<T> objects;
    std::vector<std::shared_ptr<TreeNode>> children;

    bool isLeaf() const {
        return children.empty();
    }

    bool contains(const Point3D& point) const {
        return point.x >= xMin && point.x <= xMax &&
               point.y >= yMin && point.y <= yMax &&
               point.z >= zMin && point.z <= zMax;
    }

    bool intersects(double xMinQuery, double xMaxQuery, double yMinQuery, double yMaxQuery, double zMinQuery, double zMaxQuery) const {
        return !(xMax < xMinQuery || xMin > xMaxQuery ||
                 yMax < yMinQuery || yMin > yMaxQuery ||
                 zMax < zMinQuery || zMin > zMaxQuery);
    }

    void subdivide() {
        double xMid = (xMin + xMax) / 2;
        double yMid = (yMin + yMax) / 2;
        double zMid = (zMin + zMax) / 2;

        children.push_back(std::make_shared<TreeNode>(xMin, xMid, yMin, yMid, zMin, zMid, maxObjects));
        children.push_back(std::make_shared<TreeNode>(xMid, xMax, yMin, yMid, zMin, zMid, maxObjects));
        children.push_back(std::make_shared<TreeNode>(xMin, xMid, yMid, yMax, zMin, zMid, maxObjects));
        children.push_back(std::make_shared<TreeNode>(xMid, xMax, yMid, yMax, zMin, zMid, maxObjects));
        children.push_back(std::make_shared<TreeNode>(xMin, xMid, yMin, yMid, zMid, zMax, maxObjects));
        children.push_back(std::make_shared<TreeNode>(xMid, xMax, yMin, yMid, zMid, zMax, maxObjects));
        children.push_back(std::make_shared<TreeNode>(xMin, xMid, yMid, yMax, zMid, zMax, maxObjects));
        children.push_back(std::make_shared<TreeNode>(xMid, xMax, yMid, yMax, zMid, zMax, maxObjects));
    }
};

// 示例使用
// struct MyData {
//     double x, y, z;
//     std::string name;
// };

// Point3D getMyDataCoordinates(const MyData& data) {
//     return {data.x, data.y, data.z};
// }

// int main() {
//     TreeNode<MyData> tree(0.0, 100.0, 0.0, 100.0, 0.0, 100.0, 4);

//     MyData data1 = {10.0, 10.0, 10.0, "Point1"};
//     MyData data2 = {20.0, 20.0, 20.0, "Point2"};
//     MyData data3 = {30.0, 30.0, 30.0, "Point3"};
//     MyData data4 = {40.0, 40.0, 40.0, "Point4"};
//     MyData data5 = {50.0, 50.0, 50.0, "Point5"};

//     tree.insert(data1, getMyDataCoordinates);
//     tree.insert(data2, getMyDataCoordinates);
//     tree.insert(data3, getMyDataCoordinates);
//     tree.insert(data4, getMyDataCoordinates);
//     tree.insert(data5, getMyDataCoordinates);

//     std::cout << "Inserted data points into 3DR tree." << std::endl;

//     // 执行区间查询
//     double xMinQuery = 15.0, xMaxQuery = 35.0;
//     double yMinQuery = 15.0, yMaxQuery = 35.0;
//     double zMinQuery = 15.0, zMaxQuery = 35.0;
//     auto results = tree.range_query(xMinQuery, xMaxQuery, yMinQuery, yMaxQuery, zMinQuery, zMaxQuery, getMyDataCoordinates);

//     std::cout << "Query results:" << std::endl;
//     for (const auto& result : results) {
//         std::cout << result.name << " (" << result.x << ", " << result.y << ", " << result.z << ")" << std::endl;
//     }

//     return 0;
// }
