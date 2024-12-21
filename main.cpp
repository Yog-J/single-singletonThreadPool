#include <iostream>
#include "ThreadPool.h"

int add(int a,int b)
{
    return a + b;
}
int main() {
    ThreadPool threadPool(4);
    std::function<int(int,int)> f = add;
    int a = 1;
    int b = 2;
    for (int i = 0; i < 20; ++i) {
        threadPool.enqueue([i,f,a,b]{
            f(a,b);
            std::cout<<"task "<<i<<" is running by "<<std::this_thread::get_id()<<std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        });
    }
    return 0;
}
