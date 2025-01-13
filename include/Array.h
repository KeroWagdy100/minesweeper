// Array.h
#pragma once
#include <algorithm>

template <class T, size_t size>
class Array
{
public:
    Array(): len(0)
    {}

    length() const 
    {
        return len;
    }

    T& operator[](size_t index)
    {
        if (validIndex(index))
            return data[index];
        return data[0];
    }

    // pushed value to its sorted position
    bool push(T val)
    {
        if (!validIndex(len))
            return false;
        size_t i = 0;
        for (; i < len; i++)
            if (data[i] > val)
                break;
        data[++i] = val;
        len++;    
    }

    bool remove(T val)
    {
        std::find()
        std::sort(data, data + len);
        

    }



private:
    bool validIndex(size_t index)
    {
        return (index >= 0 && index <= size)
    }
private:

    T data[size];
    size_t len;
};
