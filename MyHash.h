// MyHash.h

// Skeleton for the MyHash class template.  You must implement the first seven
// member functions; we have implemented the eighth.
template<typename KeyType, typename ValueType>
class MyHash
{
public:
    MyHash(double maxLoadFactor = 0.5);
    ~MyHash();
    void reset();
    void associate(const KeyType& key, const ValueType& value);
    int getNumItems() const;
    double getLoadFactor() const;

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyHash*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    MyHash(const MyHash&) = delete;
    MyHash& operator=(const MyHash&) = delete;

private:
    struct Bucket
    {
        Bucket(const KeyType& key, const ValueType& value, Bucket* next)
        {
            this->key = key;
            this->val = value;
            this->next = next;
        }
        KeyType key;
        ValueType val;
        Bucket* next;
    };
    
    double m_maxLoadFactor;
    double m_currLoadFactor;
    int m_items;
    int m_capacity;
    Bucket** m_map;
    
    unsigned int getBucketNumber(const KeyType& key) const;
    void transferMap();
};

template<typename KeyType, typename ValueType>
MyHash<KeyType, ValueType>::MyHash(double maxLoadFactor)
{
    if (maxLoadFactor <= 0) // Handle different corner cases
    {
        m_maxLoadFactor = 0.5;
    }
    else if (maxLoadFactor > 2)
    {
        m_maxLoadFactor = 2.0;
    }
    else
    {
        m_maxLoadFactor = maxLoadFactor;
    }
    m_currLoadFactor = 0;
    m_items = 0;
    m_capacity = 100;
    m_map = new Bucket*[m_capacity];
    for(int i = 0; i < m_capacity; i++) // Set all buckets to nullptr to indicate emptiness
    {
        m_map[i] = nullptr;
    }
}

template<typename KeyType, typename ValueType>
MyHash<KeyType, ValueType>::~MyHash()
{
    for (int i = 0; i < m_capacity; i++) // Deallocate all buckets
    {
        Bucket* p = m_map[i];
        while(p != nullptr ) // Deallocate "linked lists"
        {
            Bucket* toDelete = p;
            p = p->next;
            delete toDelete;
        }
    }
    delete [] m_map; // Deallocate array
}

template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::reset()
{
    for (int i = 0; i < m_capacity; i++) // Basically same as destructor + constructor
    {
        Bucket* p = m_map[i];
        while(p != nullptr)
        {
            Bucket* toDelete = p;
            p = p->next;
            delete toDelete;
        }
    }
    delete [] m_map;
    m_items = 0;
    m_currLoadFactor = 0;
    m_capacity = 100;
    m_map = new Bucket*[m_capacity];
    for(int i = 0; i < m_capacity; i++)
    {
        m_map[i] = nullptr;
    }
}

template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    ValueType* prevIdx = find(key);
    if (prevIdx != nullptr) // We only update the previous value if the key already exists
    {
        *prevIdx = value;
        return;
    }
    m_items++;
    m_currLoadFactor = (1.0 * getNumItems() ) / m_capacity;
    if (m_currLoadFactor > m_maxLoadFactor )
    {
        // resize
        m_capacity *= 2; // Changing it here makes for more concise code later
        transferMap();
    }
    int idx = getBucketNumber(key); // No matter what, we create an accurate hashed idx for our key
    Bucket* newBucket = new Bucket(key, value, m_map[idx]); // Add it to the front of the "linked list"
    m_map[idx] = newBucket;
}

template<typename KeyType, typename ValueType>
int MyHash<KeyType, ValueType>::getNumItems() const
{
    return m_items;
}

template<typename KeyType, typename ValueType>
double MyHash<KeyType, ValueType>::getLoadFactor() const
{
    return m_currLoadFactor;
}

template<typename KeyType, typename ValueType>
const ValueType* MyHash<KeyType, ValueType>::find(const KeyType& key) const
{
    int idx = getBucketNumber(key);
    Bucket* p = m_map[idx];
    while (p != nullptr) // Iterate through the linked list until we find our target
    {
        if(p->key == key)
        {
            return &(p->val);
        }
        p = p->next;
    }
    return nullptr; // This means it wasn't found
}

template<typename KeyType, typename ValueType>
unsigned int MyHash<KeyType, ValueType>::getBucketNumber(const KeyType& key) const
{
    unsigned int hash(const KeyType& k);
    return hash(key) % m_capacity; // Mod by m_capacity to make sure it fits in our array
}

template<typename KeyType, typename ValueType>
void MyHash<KeyType, ValueType>::transferMap()
{
    Bucket** newMap = new Bucket*[m_capacity]; // Allocate new block of 2x memory
    for (int i = 0; i < m_capacity; i++)
    {
        newMap[i] = nullptr; // Must set to nullptr, so that future buckets will "terminate"
    }
    for(int i = 0; i < m_capacity / 2; i++) // Iterate through old hash
    {
        Bucket* p = m_map[i];
        while (p != nullptr)
        {
            Bucket* temp = p->next; // Must store in temp, since we continue to iterate w/ p
            int newIdx = getBucketNumber(p->key); // Recalculate index
            p->next = newMap[newIdx]; // Add to front of new linked list
            newMap[newIdx] = p;
            p = temp;
        }
    }
    delete [] m_map; // Deallocate old block of memory
    m_map = newMap; // Switch to new array
}


