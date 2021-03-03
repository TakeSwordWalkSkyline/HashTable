#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StringHash.h"

StringHash::StringHash() {
    m_hashTable = NULL;
    m_hashTableSize = 0;
    m_cursor = NULL;
    m_cursorPos = 0;
    m_wordArrPool = NULL;
    m_wordArr = NULL;
    m_wordNum = 0;
    m_maxKeyPoolSize = 0;

    m_valueArrPool = NULL;
    m_curValuePoolPos = 0;
    m_valueArr = NULL;
    m_valueNum = 0;
    m_valueAllocNum = 0;
    m_maxValuePoolSize = 0;
    
    m_hashPool = NULL;
    m_hashPoolSize = 0;
    m_hashNum = 0;
    m_curPoolPos = 0;
    m_wordAllocNum = 0;
}

StringHash::~StringHash() {
    if(m_hashPool != NULL)
        free(m_hashPool);
    if(m_wordArr != NULL)
        free(m_wordArr);
    if(m_wordArrPool != NULL)
        free(m_wordArrPool);
    if(m_valueArr != NULL)
        free(m_valueArr);
    if(m_valueArrPool != NULL)
        free(m_valueArrPool);
    m_hashTable = NULL;
    m_cursor = NULL;
}

int StringHash::initHashTable(size_t size, size_t key_elem_size, size_t value_elem_size) {
    if(m_hashTableSize > 0)
        return 0;
    if(size == 0){
        return -1;
    }
    if (key_elem_size == 0 || value_elem_size == 0) {
        return -1;
    }
    m_hashPoolSize = size;
    m_hashTableSize = size / 20;
    if (m_hashTableSize == 0) {
        m_hashTableSize = size * 2;
    }
    m_hashPoolSize += m_hashTableSize;
    m_hashPool = (HashTableT)malloc(sizeof(HashTableSt) * m_hashPoolSize);
    if(m_hashPool == NULL){
        m_hashPoolSize = 0;
        m_hashTableSize = 0;
        return -1;
    }
    bzero(m_hashPool,m_hashPoolSize);
    m_hashTable = m_hashPool;
    m_hashNum = m_hashTableSize;

    m_maxKeyPoolSize = size * key_elem_size;
    m_wordArrPool = (char*)malloc(sizeof(char) * m_maxKeyPoolSize);
    if(m_wordArrPool == NULL){
        return -1;
    }
    bzero(m_wordArrPool,sizeof(char) * m_maxKeyPoolSize);

    m_wordArr = (char**)malloc(sizeof(char*) * size);
    if(m_wordArr == NULL){
        return -1;
    }
    bzero(m_wordArr,sizeof(char*) * size);

    m_wordAllocNum = size;
    m_wordNum = 0;
    m_curPoolPos = 0;

    m_maxValuePoolSize = size * value_elem_size;
    m_valueArrPool = (char*)malloc(sizeof(char) * m_maxValuePoolSize);
    if(m_valueArrPool == NULL){
        return -1;
    }
    bzero(m_valueArrPool,sizeof(char) * m_maxValuePoolSize);

    m_valueArr = (char**)malloc(sizeof(char*) * size);
    if(m_valueArr == NULL){
        return -1;
    }
    bzero(m_valueArr,sizeof(char*) * size);
    m_valueAllocNum = size;
    m_curValuePoolPos = 0;
    m_valueNum = 0;

    return 0;
}

char *StringHash::addValue(const char *value) {
    if(value == NULL)
        return NULL;
    if(strlen(value) + (size_t)m_curValuePoolPos >= (size_t)m_maxValuePoolSize){
        return NULL;
    }
    int id = m_valueNum ++;
    m_valueArr[id] = m_valueArrPool + m_curValuePoolPos;
    strcpy(m_valueArr[id],value);
    m_curValuePoolPos += strlen(value);
    m_curValuePoolPos += 1;
    return m_valueArr[id];
}

int StringHash::addKey(const char *key) {
    if(key == NULL)
        return -1;
    if(strlen(key) + (size_t)m_curPoolPos >= (size_t)m_maxKeyPoolSize){
        return -1;
    }
    HashTableT hash = m_hashTable + getHashCode(key);
    if (hash) {
        hash = hash->next;
    } else {
        return -1;
    }
    for(; hash != NULL; hash = hash->next) {
        if(hash->key == NULL)
            break;
        if(strcmp(hash->key,key) == 0){
            if(hash->id >= 0)
                return hash->id;
        }
    }
    int id = m_wordNum++;
    m_wordArr[id] = m_wordArrPool + m_curPoolPos;
    strcpy(m_wordArr[id],key);
    m_curPoolPos += strlen(key);
    m_curPoolPos += 1;
    return id;
}

void StringHash::resetCursor(){
    m_cursor = NULL;
    m_cursorPos = 0;
}

char *StringHash::getNext(const char *key, int input_type, int& type) {
    if(m_cursor == NULL) {
        if(m_cursorPos != 0)
            return NULL;
        m_cursor = m_hashTable + getHashCode(key) ;
        if (m_cursor == NULL) {
            return NULL;
        } else {
            m_cursor = m_cursor->next;
        }
    }
    type = 0;
    m_cursorPos ++ ;
    for(; m_cursor != NULL; m_cursor = m_cursor->next) {
        if (m_cursor->key == NULL)
            return NULL;
        if (input_type) {
            if (strcmp(m_cursor->key,key) == 0 && (m_cursor->type & input_type)) {
                char *value = m_cursor->value;
                type = m_cursor->type;
                m_cursor = m_cursor->next;
                return value;
            }
        } else {
            if (strcmp(m_cursor->key,key) == 0) {
                char *value = m_cursor->value;
                type = m_cursor->type;
                m_cursor = m_cursor->next;
                return value;
            }
        }
    }
    return NULL;
}

char *StringHash::findString(const char *key, int input_type) {
    HashTableT hash_beg = m_hashTable + getHashCode(key);
    if (!hash_beg) {
        return NULL;
    }
    for (HashTableT hash = hash_beg->next; hash != NULL; hash = hash->next) {
        if (hash->key == NULL)
            return NULL;
        if (input_type) {
            if ((strcmp(hash->key,key) == 0) && (hash->type & input_type))
                return(hash->value);
        } else {
            if (strcmp(hash->key,key) == 0)
                return(hash->value);
        }
    }
    return NULL;
}

int StringHash::getID(const char *key, int input_type, float* weight) {
    HashTableT hash_beg = m_hashTable + getHashCode(key);
    if (!hash_beg) {
        return -1;
    }
    if(weight != NULL)
        *weight = 1.f;
    for (HashTableT hash = hash_beg->next; hash != NULL; hash = hash->next) {
        if (hash->key == NULL)
            return -1;
        if (input_type) {
            if (strcmp(hash->key,key) == 0 && (hash->type & input_type))
                return hash->id;
        } else {
            if (strcmp(hash->key,key) == 0)
                return(hash->id);
        }
    }
    return -1;
}

const char *StringHash::getWord(int id) {
    if(id >= m_wordNum || id < 0)
        return NULL;
    return m_wordArr[id];
}

int StringHash::addString(const char *key,const char *value,int type) {
    if (m_hashNum >= m_hashPoolSize) {
        return -1;
    }
    HashTableT hash,newhash;
    int id = getHashCode(key),hid;
    hash = m_hashTable + id;
    hid = addKey(key);
    if(hid == -1) {
        return -1;
    }
    if(hash->key == NULL) {
        hash->key = NULL;
        hash->value = NULL;
        hash->id = -1;
        hash->type = 0;
    }
    HashTableT searchhash = NULL;
    bool is_new_value = true;
    for (searchhash = hash; searchhash != NULL; searchhash = searchhash->next) {
        if (searchhash->value != NULL) {
            if (strcmp(searchhash->key,key) == 0
                && strcmp(searchhash->value, value) == 0) {
                is_new_value = false;
                break;
            }
        }
    }
    if (is_new_value) {
        newhash = m_hashPool + m_hashNum++;
        newhash->key = m_wordArr[hid];
        newhash->value = addValue(value);
        newhash->next = hash->next;
        newhash->id = hid;
        newhash->type |= type;
        hash->next = newhash;
    } else {
        searchhash->type |= type;
    }
    return id;
}

unsigned int StringHash::getHashCode(const char *key) {
    register unsigned int hash;
    hash = 0;
    while (*key != '\0') {
        hash = hash * 101  +  *key++;
    }
    return (hash % m_hashTableSize);
}
