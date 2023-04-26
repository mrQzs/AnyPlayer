
#ifndef VIDEODECODE_H
#define VIDEODECODE_H

class VideoDecode {
 public:
  // 禁止拷贝构造函数和赋值操作符
  VideoDecode(const VideoDecode &) = delete;
  VideoDecode &operator=(const VideoDecode &) = delete;

  // 获取单例实例的全局访问点
  static VideoDecode &getInstance();

 public:
  void test();

 private:
  // 将构造函数和析构函数设为私有，禁止外部创建和删除实例
  VideoDecode();
  ~VideoDecode();
};

#endif  // VIDEODECODE_H
