#include "avdeque.h"

template <class T>
Deque<T>::Deque(int _max_elements)
    :max_elements(_max_elements)
{
    cur_elements = 0;
    pthread_mutex_init(&deque_mutex, NULL);
    pthread_cond_init(&deque_cond, NULL);
    pthread_cond_signal(&deque_cond); //保证可以先使用一次
}

template <class T>
Deque<T>::~Deque()
{
    pthread_mutex_destroy(&deque_mutex);
    pthread_cond_destroy(&deque_cond);
}

template <class T>
bool Deque<T>::is_empty()
{
    return _deque.empty();
}

template <class T>
bool Deque<T>::is_full()
{
    return (cur_elements==max_elements);
}

template <class T>
int Deque<T>::get_cur_elements()
{
    return cur_elements;
}

template <class T>
int Deque<T>::push_data(T &data)
{
    if (is_full())
    {
        std::cout << "avdeque is full !" << std::endl;
        return -1;
    }
    pthread_mutex_lock(&deque_mutex);
    pthread_cond_wait(&deque_cond, &deque_mutex);
    _deque.push_back(data);
    pthread_cond_signal(&deque_cond);
    pthread_mutex_unlock(&deque_mutex);

    return 0;
}

template <class T>
T& Deque<T>::pop_data(T &data)
{
    if (is_empty())
    {
        std::cout << "avdeque is empty !" << std::endl;
        return data;
    }
    pthread_mutex_lock(&deque_mutex);
    pthread_cond_wait(&deque_cond, &deque_mutex);
    data = _deque.front();
    _deque.pop_front();
    pthread_cond_signal(&deque_cond);
    pthread_mutex_unlock(&deque_mutex);

    return data;
}
