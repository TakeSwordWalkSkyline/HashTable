#ifndef __STRING_HASH_INC_
#define __STRING_HASH_INC_
#include <stddef.h>

typedef struct HashTableSt {
    char *key;
    char *value;
    struct HashTableSt* next;
    int id;
    int type;
    HashTableSt(): key(NULL), value(NULL), next(NULL), id(-1), type(0) {}
    ~HashTableSt() {}
}*HashTableT;

class LexiconAdapter {
public:
    LexiconAdapter(){}
    virtual ~LexiconAdapter(){}
    virtual int getID(const char *word, int input_type, float *weight) = 0;
    virtual const char *getWord(int id) = 0;
};

class StringHash : public LexiconAdapter {
public:
    StringHash();
    ~StringHash();

    int initHashTable(size_t size, size_t key_elem_size, size_t value_elem_size);
    char *findString(const char *key, int input_type);

    int addString(const char *key,const char *value,int type = 0);

    const char *getWord(int id);

    void resetCursor();
    char *getNext(const char *key, int input_type, int &type);

    int getID(const char *key, int input_type, float *weight = NULL);

private:
    unsigned int getHashCode(const char *string);
    char *addValue(const char *value);
    int addKey(const char *key);
private:
    HashTableT m_hashPool;

    HashTableT m_hashTable;
    int m_hashTableSize;

    int m_hashPoolSize;
    int m_hashNum;

    HashTableT m_cursor;

    int m_cursorPos;

    char *m_wordArrPool;
    int m_curPoolPos;

    char **m_wordArr;
    int m_wordNum;
    int m_wordAllocNum;
    int m_maxKeyPoolSize;

    char *m_valueArrPool;
    int m_curValuePoolPos;

    char **m_valueArr;
    int m_valueNum;
    int m_valueAllocNum;
    int m_maxValuePoolSize;
};

#endif
