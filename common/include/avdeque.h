#ifndef DEQUE_H
#define DEQUE_H
#include <deque>
#include <pthread.h>
#include <iostream>

template <class T>
class Deque
{
public:
    explicit Deque(int _max_elements = 0);
    ~Deque();
    bool is_empty();
    bool is_full();
    int get_cur_elements();
    int push_data(T &data);
    T& pop_data(T &data);
private:
    //使用互斥锁和条件变量控制同步问题
    pthread_mutex_t deque_mutex;
    pthread_cond_t deque_cond;

    const int max_elements = 0;
    int cur_elements = 0;
    std::deque<T> _deque;
};

#endif // DEQUE_H
