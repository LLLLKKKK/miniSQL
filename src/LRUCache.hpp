
#ifndef LRUCACHE_HPP
#define LRUCACHE_HPP

#include <list>
#include <map>
#include "logger.hpp"


template<class K, class V, class GetSizeCallBack>
class LRUCache {
public:
    LRUCache(int64_t cacheSize, GetSizeCallBack getSizeCallBack = GetSizeCallBack());
    ~LRUCache();

    DISALLOW_COPY_AND_ASSGIN(LRUCache);
    
public:
    void invalidate(const std::vector<K>& keyList);
    bool invalidate(const K& key);

    bool put(const K& key, const V& value);
    bool get(const K& key, V& value);
    bool update(const K& key, const V& newValue);
    bool isInCache(const K& key);

    bool getAndPin(const K& key, V& value);
    bool unpin(const K& key);

    int64_t getCacheSizeUsed() const { return _cacheSizeUsed; }
    int64_t getCacheSize() const { return _cacheSize; }
    uint64_t getKeyCount() const { return _accessQueue.getSize(); }
    
    
    GetSizeCallBack& getGetSizeCallBack()  { return _getSizeCallBack; }

    int64_t getTotalQueryTimes() { return _totalQueryTimes; }
    int64_t getHitQueryTimes() { return _hitQueryTimes; }
    float getHitRatio() const {
        if (_totalQueryTimes != 0) {
            return (float)_hitQueryTimes / _totalQueryTimes;
        }
        else {
            return 0.0f;
        }
    }

    void resetHitStatistics() {
        _totalQueryTimes = 0;
        _hitQueryTimes = 0;
    }

private:
    bool replacement(int64_t valSize);

private:
    GetSizeCallBack _getSizeCallBack;

    int64_t _cacheSize;
    int64_t _cacheSizeUsed;

    std::map<K, CacheValue<K, V> > _cacheMap;

    int64_t _totalQueryTimes;
    int64_t _hitQueryTimes;

    std::list<K> _accessQueue;

private:
    struct CacheValue {
        V value;
        std::list<K>::iterator iter;
        bool pin;

        CacheValue() : iter(nullptr), pin(false) { }
        CacheValue(const V& v, std::list<K>::iterator iterator = nullptr)
            : value(v), iter(iterator), pin(false) { }
    };

private:
    LOG_DECLARE();
};

SETUP_LOG(LRUCache);

template<class K, class V, class GetSizeCallBack>
LruCache<K, V, GetSizeCallBack, UpdateCallBack>::LruCache(
        int64_t cacheSize, GetSizeCallBack getSizeCallBack)
    : _getSizeCallBack(getSizeCallBack)
    , _cacheSize(cacheSize)
    , _cacheSizeUsed(0)
    , _totalQueryTimes(0)
    , _hitQueryTimes(0) { }

template<class K, class V, class GetSizeCallBack>
bool LruCache<K, V, GetSizeCallBack, UpdateCallBack>::require(int64_t requireSize) {
    if (_cacheSize < requireSize) {
        MINISQL_LOG(WARN, "requireSize exceeds the memory limit, "
                    "reuqireSize = %lld cacheSize=%lld", requireSize, _cacheSize);
        return false;
    }

    auto iter = _accessQueue.begin();
    while (requireSize + _cacheSizeUsed > _cacheSize && iter != _accessQueue.end()) {
        assert( ! _accessQueue.empty());
        K cacheKey = *iter;
        iter++;

        auto mapIter = _cacheMap.find(cacheKey);
        if (mapIter != _cachemap.end() && ! mapIter->second.pin) {
            _cacheSizeUsed -= _getSizeCallBack(mapIter->second.value);
            _accessQueue.erase(mapIter->second.iter);
            _cacheMap.erase(mapIter);
        }
        else {
            MINISQL_LOG(ERROR, "key %s not exists, get failed!", 
                        std::string::to_string(val).c_str());        
        }
    }
    // well, all pined
    if (requireSize + _cacheSizeUsed > _cacheSize) {
        MINISQL_LOG(WARN, "too many cache pined, require failed!");
        return false;
    }
    return true;
}


template<class K, class V, class GetSizeCallBack>
bool LruCache<K, V, GetSizeCallBack>::put(const K& key, const V& value) {
    int64_t valueSize = _getSizeCallBack(value);

    // if cacheSize is not enough now
    if (_cacheSize - valueSize < _cacheSizeUsed) {
        if ( ! require(valueSize)) {
            MINISQL_LOG(WARN, "put failed, because of require valueSize=%lld failed",
                        valueSize);
            return false;
        }
    }

    CacheValue<K, V> newValue(value);
    auto res = _cacheMap.emplace(key, newValue);
    bool succ = res->second;
    auto mapIter = res->first;

    // the new value is inserted, no equivalent element exists
    if (succ) {
        auto iter = _accessQueue.insert(_accessQueue.end(), K);
        mapIter->iter = iter;
        _cacheSizeUsed += valueSize;
    }
    // if it exists already, update the cacheSizeUsed
    else {
        int64_t oldSize = GetSizeCallBack(mapIter->value);
        _accessQueue.splice(_accessQueue.end(), _accessQueue, mapIter->iter);
        mapIter->value = value;
        _cacheSizeUsed += valueSize - oldSize;
    }

    return true;
}

template<class K, class V, class GetSizeCallBack>
bool LruCache<K, V, GetSizeCallBack>::getAndPin(const K& key, V& value) {
    _totalQueryTimes ++;

    auto mapIter = _cacheMap.find(key);
    if (mapIter != _cacheMap.end()) {
        _accessQueue.splice(_accessQueue.end(), _accessQueue, mapIter->second.iter);
        mapIter->second.pin = true;
        value = mapIter->value;
        return true;
    }
    else {
        MINISQL_LOG(TRACE, "key %s not exists, get failed!", 
                    std::string::to_string(val).c_str());
        return false;
    }
}

template<class K, class V, class GetSizeCallBack>
bool LruCache<K, V, GetSizeCallBack>::unpin(const K& key) {
    auto mapIter = _cacheMap.find(key);
    if (mapIter != _cacheMap.end()) {
        mapIter->second.pin = false;
        return true;
    }
    else {
        MINISQL_LOG(TRACE, "key %s not exists, get failed!", 
                    std::string::to_string(val).c_str());
        return false;
    }    
}

template<class K, class V, class GetSizeCallBack>
bool LruCache<K, V, GetSizeCallBack>::get(const K& key, V& val) {
    _totalQueryTimes ++;

    auto mapIter = _cacheMap.find(key);
    if (mapIter != _cacheMap.end()) {
        _accessQueue.splice(_accessQueue.end(), _accessQueue, mapIter->iter);
        val = mapIter->value;
        return true;
    }
    else {
        MINISQL_LOG(TRACE, "key %s not exists, get failed!", 
                    std::string::to_string(val).c_str());
        return false;
    }
}

template<class K, class V, class GetSizeCallBack>
bool LruCache<K, V, GetSizeCallBack>::isInCache(const K& key) {
    return (_cacheMap.find(key) != _cacheMap.end());
}

template<class K, class V, class GetSizeCallBack>
bool LruCache<K, V, GetSizeCallBack>::update(const K& key, const V& newVal) {
    return put(key, newVal);
}

template<class K, class V, class GetSizeCallBack>
bool LruCache<K, V, GetSizeCallBack>::invalidate(const K& key) {
    auto mapIter = _cacheMap.find(key);
    if (mapIter != _cacheMap.end()) {
        _cacheSizeUsed -= _getSizeCallBack(mapIter->value);
        _accessQueue.erase(mapIter->iter);
        _cacheMap.erase(mapIter);
        return true;
    }
    return false;
}

template<class K, class V, class GetSizeCallBack>
void LruCache<K, V, GetSizeCallBack>::invalidate(const std::vector<K>& keyList) {
    for (auto iter = keyList.begin(); iter != keyList.end(); ++iter) {
        invalidate(*iter);
    }
}
