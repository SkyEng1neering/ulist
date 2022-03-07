/*
 * Copyright 2021 Alexey Vasilenko
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef ULIST_H_
#define ULIST_H_

#include "dalloc.h"

#define ULIST_VERSION               "1.0.0"

#define ulist_debug                 printf

template <typename T>
struct ulist_element {
    ulist_element<T>* _next;
    T _value;

    ulist_element(){
        _next = nullptr;
    }

    ulist_element(T value){
        _next = nullptr;
        _value = value;
    }
};

template <typename T>
class ulist {
private:
    heap_t* _heap_ptr;
    uint32_t _size;
    ulist_element<T>* _first_ptr;
    ulist_element<T>* _last_ptr;

public:
    ulist();
#ifdef USE_SINGLE_HEAP_MEMORY
#else
    ulist(heap_t *heap_ptr);
#endif
    ulist(const ulist &other);

    ulist<T>& operator=(const ulist &other);

    bool empty();
    uint32_t size();
    void clear();
    bool push_back(T value);
    bool pop_back();
    bool push_front(T value);
    bool pop_front();
    T& at(uint32_t i);
};

#ifdef USE_SINGLE_HEAP_MEMORY
template <typename T>
ulist<T>::ulist(){
    _heap_ptr = &default_heap;
    _size = 0;
    _first_ptr = nullptr;
    _last_ptr = nullptr;
}
#else
template <typename T>
ulist<T>::ulist(){
    _heap_ptr = nullptr;
    _size = 0;
    _first_ptr = nullptr;
    _last_ptr = nullptr;
}

template <typename T>
ulist<T>::ulist(heap_t *heap_ptr){
    _heap_ptr = heap_ptr;
    _size = 0;
    _first_ptr = nullptr;
    _last_ptr = nullptr;
}
#endif

template <typename T>
uint32_t ulist<T>::size(){
    return _size;
}

template <typename T>
bool ulist<T>::push_back(T value){
    ulist_element<T>** list_element_ptr = nullptr;

    if(size() == 0){
        list_element_ptr = &_first_ptr;
    } else {
        list_element_ptr = &_last_ptr->_next;
    }

    /* Allocate memory for new element */
    dalloc(_heap_ptr, sizeof(ulist_element<T>), (void**)list_element_ptr);
    if(*list_element_ptr == NULL){
        return false;
    }

    /* Memory allocated, construct here new element */
    new (*list_element_ptr) ulist_element<T>(value);

    _last_ptr = *list_element_ptr;

    if(add_ptr_to_validate_list(_heap_ptr, (void **)list_element_ptr) != true){
        return false;
    }

    _size++;

    return true;
}

template <typename T>
T& ulist<T>::at(uint32_t i){
    if(i > size() - 1){
        ulist_debug("Index %lu is out of range\n", i);
        return ulist_element<T>();
    }

    ulist_element<T>* _value_ptr = _first_ptr;
    uint32_t index = 0;
    while(index != i){
        _value_ptr = _value_ptr->_next;
        index++;
    }
    return _value_ptr->_value;
}

#endif /* ULIST_H_ */
