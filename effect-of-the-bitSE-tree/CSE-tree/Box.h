//
// Created by zcc on 2021/6/22.
//

#ifndef QUADTRANS_BOX_H
#define QUADTRANS_BOX_H
#pragma once

#include "Vector2.h"
#include <string>
class Box {
public:
  double left;
  double top;
  double width; // Must be positive
  double height; // Must be positive
  int track_id;
  int frameIndex;
  //    edit by wmz
  //在四叉树中使用，表示四叉树的节点编码
  std::string code;
public:

  Box(double Left = 0, double Top = 0, double Width = 0, double Height = 0) {
    left = Left, top = Top, width = Width, height = Height, track_id = 0, frameIndex = 0;
  }

  Box(const Vector2& position, const Vector2& size) :
    left(position.x), top(position.y), width(size.x), height(size.y), track_id(0), frameIndex(0)
  {

  }
  Box(double Left, double Top, double Width, double Height, int Track_id, int FrameIndex) :
    left(Left), top(Top), width(Width), height(Height), track_id(Track_id), frameIndex(FrameIndex) {

  }
  Box(const Vector2& position, const Vector2& size, int Track_id, int FrameIndex) :
    left(position.x), top(position.y), width(size.x), height(size.y), track_id(Track_id), frameIndex(FrameIndex) {

  }
  double getRight() const {
    return this->left + this->width;
  }
  double getBottom() const {
    return this->top + this->height;
  }
  Vector2 getTopLeft() const {
    return Vector2(left, top);
  }
  Vector2 getCenter() const {
    return Vector2(left + width / 2, top + height / 2);
  }
  Vector2 getSize() const {
    return Vector2(width, height);
  }
  bool contains(const Box& box) const
  {
    return left <= box.left && box.getRight() <= getRight() &&
      top <= box.top && box.getBottom() <= getBottom();
  }
  bool intersects(const Box& box) const
  {
    return !(left > box.getRight() || getRight() < box.left ||
      top > box.getBottom() || getBottom() < box.top);
  }
  bool baohan(const Box& box) const {
    return (box.getRight() <= getRight() && box.left >= left && box.top <= getBottom() && box.getBottom() >= top);
  }
  double getArea() const {
    return width * height;
  }

};
#endif //QUADTRANS_BOX_H
