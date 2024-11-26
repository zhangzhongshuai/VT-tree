//
// Created by shuai on 2022/4/27.
//

#ifndef QUADTRANS_SCAN_H
#define QUADTRANS_SCAN_H

template<typename T>
class Scan {
public:
    double dcmp(double x) const{
        if(fabs(x) < eps) return 0;
        else return x < 0 ? -1 : 1;
    }
    bool SegmentProperIntersection(const Point& a1, const Point& a2, const Point& b1, const Point& b2) const{
        double c1 = Cross(a2-a1,b1-a1), c2 = Cross(a2-a1,b2-a1),
                c3 = Cross(b2-b1,a1-b1), c4=Cross(b2-b1,a2-b1);
        return dcmp(c1)*dcmp(c2)<0 && dcmp(c3)*dcmp(c4)<0;
    }
    bool OnSegment(const Point& p, const Point& a1, const Point& a2) const{
        return dcmp(Cross(a1-p, a2-p)) == 0 && dcmp(Dot(a1-p, a2-p)) < 0;
    }
    // 点集凸包
    vector<Point> ConvexHull(vector<Point> q) const{
        // 预处理，删除重复点
        vector<Point> p=q;
        sort(p.begin(), p.end());
        p.erase(unique(p.begin(), p.end()), p.end());

        int n = p.size();
        int m = 0;
        vector<Point> ch(n+1);
        for(int i = 0; i < n; i++) {
            while(m > 1 && Cross(ch[m-1]-ch[m-2], p[i]-ch[m-2]) <= 0) m--;
            ch[m++] = p[i];
        }
        int k = m;
        for(int i = n-2; i >= 0; i--) {
            while(m > k && Cross(ch[m-1]-ch[m-2], p[i]-ch[m-2]) <= 0) m--;
            ch[m++] = p[i];
        }
        if(n > 1) m--;
        ch.resize(m);
        return ch;
    }
    //点是否在凸包内
    int IsPointInPolygon(const Point& p, const vector<Point>& poly) const{
        int wn = 0;
        int n = poly.size();
        for(int i=0; i<n; ++i) {
            const Point& p1 = poly[i];
            const Point& p2 = poly[(i+1)%n];
            if(p1 == p || p2 == p || OnSegment(p, p1, p2)) return 2;//在边界上
            int k = dcmp(Cross(p2-p1, p-p1));
            int d1 = dcmp(p1.y - p.y);
            int d2 = dcmp(p2.y - p.y);
            if(k > 0 && d1 <= 0 && d2 > 0) wn++;
            if(k < 0 && d2 <= 0 && d1 > 0) wn--;
        }
        return wn!=0;
    }


    std::vector<T> query_scan (vector<T>& nodes,vector<Point>& hull, int st, int ed) const{
        auto values = std::vector<T>();
        for (auto& node : nodes){
            int fid = node.box.frameIndex;
            Point p = Point(node.box.left,node.box.top);
            if(IsPointInPolygon(p,hull) && fid >=st && fid <= ed){
                values.push_back(node);
            }

        }
        return values;
    }

};


#endif //QUADTRANS_SCAN_H
