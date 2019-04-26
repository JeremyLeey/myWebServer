#pragma once

/* C++ Primer 13.1.6 阻止拷贝 
 * 新标准中可以将拷贝构造和拷贝赋值运算符定义成=delete来阻止拷贝; 
 * 而其继承类的拷贝控制函数也会被定义为删除的,从而阻止拷贝;
 */
class noncopyable {
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    noncopyable(const noncopyable&);
    const noncopyable& operator=(const noncopyable&);
};
