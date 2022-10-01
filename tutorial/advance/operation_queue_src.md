<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-09-20 13:11:39
 * @Description: 
-->
# 源码剖析操作队列内存运行机制

下面代码是 AvZ 内部操作队列运行逻辑的提炼，具体解释参考视频: [https://www.bilibili.com/video/BV1uG411G7bi](https://www.bilibili.com/video/BV1uG411G7bi)

```C++

/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-09-18 12:48:16
 * @Description:
 */
#include <cstdio>
#include <functional>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

void Sleep(int cs)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(cs * 10));
}

using Func = std::function<void()>;

int insert_time = 0;
std::map<int, std::vector<Func>> queue; // 操作队列
int current_time = 0;

void SetTime(int time);

void Delay(int time)
{
    insert_time += time;
}

void SetDelayTime(int time)
{
    insert_time = current_time + time;
}

void SetNowTime()
{
    insert_time = current_time;
}

void WaitUntil(int time)
{
    while (current_time != time) {
        Sleep(1);
    }
    SetTime(time);
}

void SetTime(int time)
{
    insert_time = time;
}

void InsertOperation(Func func)
{
    // 把操作插入到操作队列相应的时间点中
    if (insert_time == current_time) {
        func();
        return;
    }
    queue[insert_time].push_back(func);
}

void __Run()
{
    while (true) {
        Sleep(1);
        ++current_time;
        while (!queue.empty()) {
            auto iter = queue.begin();
            if (iter->first < current_time) {
                printf("时间出问题了 ");
            }

            if (iter->first <= current_time) {
                for (auto&& operation : iter->second) {
                    printf("当前时间: %d, 插入时间 : %d, 操作 : ", current_time, iter->first);
                    operation();
                }
                queue.erase(iter);
            } else {
                break;
            }
        }
    }
}

void Script()
{
    SetTime(100);
    InsertOperation([=]() {
        printf("hello\n");
    });

    WaitUntil(200);
    InsertOperation([=]() {
        printf("hello\n");
    });

    SetTime(150);
    InsertOperation([=]() {
        printf("hello\n");
    });
}

int main()
{
    system("chcp 65001");
    std::thread game_loop(__Run);
    std::thread script(Script);
    game_loop.join();
    script.join();
    return 0;

    // //       key    value
    // std::map<int, std::string> m;
    // m[1] = "hello";
    // m[3] = "world";
    // m[200] = "hi";
    // m[4] = "we";
    // return 0;
}


```


[上一篇 属性转换](./transform_attribute.md)

[目录](../catalogue.md)

[下一篇 运行调试](./debug.md)
