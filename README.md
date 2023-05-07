# AnyPlayer

* 0.2版本任务列表

  - [x] Packet引入对象池
  - [x] frame引入内存池
  - [x] 使用无锁队列
  - [ ] 降低整体多线程占用
  - [ ] 降低所有队列空间占用
  - [ ] 使用vld检测内存泄漏
  - [ ] 使用*Dr*Memory检测内存泄漏
  - [ ] 适配多通道的平面音频数据

  

 * 0.1版本任务列表
- [x] 成功播放视频
- [x] 实现音视频精准同步
- [x] 实现多线程解码
- [x] 多线程安全退出
- [x] 实现线程安全且支持异步操作的Logger

 * 架构设计
 <img width="717" alt="Snipaste_2023-04-09_02-49-26" src="https://user-images.githubusercontent.com/37052865/230738031-b5d04141-4726-44eb-afd2-321b54849b84.png">

 * 设计模式参考
 ![image](https://user-images.githubusercontent.com/37052865/230738482-0a9312b0-beb0-44d5-802f-fcfbdb5e41a7.png)

