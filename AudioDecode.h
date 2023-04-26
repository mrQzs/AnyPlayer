
#ifndef AUDIODECODE_H
#define AUDIODECODE_H

class AudioDecode {
  // 禁止拷贝构造函数和赋值操作符
  AudioDecode(const AudioDecode &) = delete;
  AudioDecode &operator=(const AudioDecode &) = delete;

  // 获取单例实例的全局访问点
  static AudioDecode &getInstance();

 public:
  void test();

 private:
  // 将构造函数和析构函数设为私有，禁止外部创建和删除实例
  AudioDecode();
  ~AudioDecode();
};

#endif  // AUDIODECODE_H
