
#ifndef LRUCACHE_HPP
#define LRUCACHE_HPP

#include <list>
#include <map>

template <class K, class V, class GetSizeCallback>
class LRUCache {
    
public:
    LRUCache(uint64_t cacheSize);
    
    bool get(const K& key, V& value);
    bool put(const K& key, const V& value);
    bool getAndPin(const K& key, V& value);
    bool pin(const K& key);
    bool unpin(const K& key);
    bool update(const K& key, const V& value);
    bool invalidate(const K& key);

private:
    struct CacheValue {
        V value;
        std::list<K>::iterator iter;
        CacheValue(V value, std::list<K>::iterator iter) {
            this.value = value;
            this.iter = iter;
        }
    };
    std::list<K> _accessQueue;
    std::map<K, CacheValue> _cacheMap;

    uint64_t _cacheSize;
    uint64_t _cacheUsed;
};

#endif

template<class K, class V>
LRUCache::LRUCache(uint64_t cacheSize) 
    : _cacheSize(cacheSize), _cacheUsed(0) {
}

template<class K, class V>
bool LRUCache::get(K key, V value) {
    auto mapIter =  _cacheMap.find(key);
    if (mapIter == _cacheMap.end()) {
        return false;
    }
    auto listIter = mapIter->second.iter;
    _accessQueue.splice(_accessQueue.end(), _accessQueue, listIter);

    value = mapIter->second.value;
    return true;
}

tempate<class K, class V>
bool LRUCache::put(K key, V value) {
    auto mapIter =  _cacheMap.find(key);
    if (mapIter != _cacheMap.end()) {
        return update(key, value);
    }

    auto size = value.getSizeCallback();
    if (_cacheSize - size < _cacheUsed) {
        replace(_cacheSize - size);
    }

    auto it = _accessQueue.insert(_accessQueue.end(), key);
    _cacheMap.emplace(key, CacheValue(key, it));
    _cacheUsed += size

    return true;
}

tempate<class K, class V>
void LRUCache::update(K key, V value) {
    auto mapIter =  _cacheMap.find(key);
    if (mapIter == _cacheMap.end()) {
        return false;
    }

    auto size = value.getSizeCallback();
    auto prevSize = mapIter->second.value.getSizeCallback();
    if (_cacheSize - size < _cacheUsed - prevSize) {
        replace(_cacheSize - size);
    }

    auto listIter = mapIter->second.iter;
    _accessQueue.splice(_accessQueue.end(), _accessQueue, listIter);

    mapIter->second.value = value;
    return true;
}

template<class K, class V>
void LRUCache::replace(uint64_t reuqiredSize) {

}


template<class K, class V>
void LRUCache::invalidate(K key) {
    auto mapIter =  _cacheMap.find(key);
    if (mapIter == _cacheMap.end()) {
        return false;
    }
    
    _accessQueue.earse(mapIter->second.iter);
    _cacheMap.erase(mapIter);

    return true;
}
