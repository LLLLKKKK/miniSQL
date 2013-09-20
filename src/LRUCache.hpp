
#ifndef LRUCACHE_HPP
#define LRUCACHE_HPP

#include <list>
#include <map>
#include <vector>
#include "Logger.hpp"

namespace miniSQL {

template<class K, class V, class GetSizeCallback>
class LRUCache {
public:
    LRUCache(int64_t cacheSize, GetSizeCallback getSizeCallback);
    ~LRUCache();

    LRUCache(const LRUCache<K, V, GetSizeCallback>&) = delete;
    LRUCache<K, V, GetSizeCallback> operator=(const LRUCache<K, V, GetSizeCallback>) = delete;
    
public:
    void invalidate(const std::vector<K>& keyList);
    bool invalidate(const K& key);

    bool put(const K& key, const V& value, bool pin = false);
    bool get(const K& key, V& value, bool pin = false);
    bool update(const K& key, const V& newValue);
    bool isInCache(const K& key);

    bool unpin(const K& key);

    int64_t getCacheSizeUsed() const { return _cacheSizeUsed; }
    int64_t getCacheSize() const { return _cacheSize; }
    uint64_t getKeyCount() const { return _accessQueue.getSize(); }
    
    
    GetSizeCallback& getGetSizeCallback()  { return _GetSizeCallback; }

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
    bool require(int64_t valSize);

private:
    struct CacheValue {
        V value;
        typename std::list<K>::iterator iter;
        bool pin;
    };

    GetSizeCallback _GetSizeCallback;

    int64_t _cacheSize;
    int64_t _cacheSizeUsed;

    std::map<K, CacheValue> _cacheMap;

    int64_t _totalQueryTimes;
    int64_t _hitQueryTimes;

    std::list<K> _accessQueue;

private:
    LoggerPtr _logger = Logger::getLogger("LRUCache");
};

template<class K, class V, class GetSizeCallback>
LRUCache<K, V, GetSizeCallback>::LRUCache(
        int64_t cacheSize, GetSizeCallback getSizeCallback)
    : _GetSizeCallback(getSizeCallback)
    , _cacheSize(cacheSize)
    , _cacheSizeUsed(0)
    , _totalQueryTimes(0)
    , _hitQueryTimes(0) { }

template<class K, class V, class GetSizeCallback>
bool LRUCache<K, V, GetSizeCallback>::require(int64_t requireSize) {
    if (_cacheSize < requireSize) {
        MINISQL_LOG_WARN("requireSize exceeds the memory limit, reuqireSize = %lld cacheSize=%lld", requireSize, _cacheSize);
        return false;
    }

    auto iter = _accessQueue.begin();
    while (requireSize + _cacheSizeUsed > _cacheSize && iter != _accessQueue.end()) {
        assert( ! _accessQueue.empty());
        K cacheKey = *iter;
        iter++;

        auto mapIter = _cacheMap.find(cacheKey);
        if (mapIter != _cacheMap.end() && ! mapIter->second.pin) {
            _cacheSizeUsed -= _GetSizeCallback(mapIter->second.value);
            _accessQueue.erase(mapIter->second.iter);
            _cacheMap.erase(mapIter);
        }
        else {
            MINISQL_LOG_ERROR("key %s not exists, get failed!", 
                        std::to_string(cacheKey).c_str());
        }
    }
    // well, all pined
    if (requireSize + _cacheSizeUsed > _cacheSize) {
        MINISQL_LOG_WARN("too many cache pined, require failed!");
        return false;
    }
    return true;
}


template<class K, class V, class GetSizeCallback>
bool LRUCache<K, V, GetSizeCallback>::put(const K& key, const V& value, bool pin) {
    int64_t valueSize = _GetSizeCallback(value);

    // if cacheSize is not enough now
    if (_cacheSize - valueSize < _cacheSizeUsed) {
        if ( ! require(valueSize)) {
            MINISQL_LOG_WARN("put failed, because of require valueSize=%s failed",
                        std::to_string(valueSize).c_str());
            return false;
        }
    }

    CacheValue newValue { value };
    auto res = _cacheMap.emplace(key, newValue);
    bool succ = res.second;
    auto mapIter = res.first;

    if (pin) {
        mapIter->second.pin = true;
    }

    // the new value is inserted, no equivalent element exists
    if (succ) {
        auto iter = _accessQueue.insert(_accessQueue.end(), key);
        mapIter->second.iter = iter;
        _cacheSizeUsed += valueSize;
    }
    // if it exists already, update the cacheSizeUsed
    else {
        int64_t oldSize = _GetSizeCallback(mapIter->second.value);
        _accessQueue.splice(_accessQueue.end(), _accessQueue, mapIter->second.iter);
        mapIter->second.value = value;
        _cacheSizeUsed += valueSize - oldSize;
    }

    return true;
}

template<class K, class V, class GetSizeCallback>
bool LRUCache<K, V, GetSizeCallback>::unpin(const K& key) {
    auto mapIter = _cacheMap.find(key);
    if (mapIter != _cacheMap.end()) {
        mapIter->second.pin = false;
        return true;
    }
    else {
        MINISQL_LOG_TRACE("key %s not exists, get failed!", 
                    std::to_string(key).c_str());
        return false;
    }    
}

template<class K, class V, class GetSizeCallback>
bool LRUCache<K, V, GetSizeCallback>::get(const K& key, V& val, bool pin) {
    _totalQueryTimes ++;

    auto mapIter = _cacheMap.find(key);
    if (mapIter != _cacheMap.end()) {
        _accessQueue.splice(_accessQueue.end(), _accessQueue, mapIter->second.iter);
        val = mapIter->second.value;
        if (pin) {
            mapIter->second.pin = true;
        }
        return true;
    }
    else {
        MINISQL_LOG_TRACE("key %s not exists, get failed!", 
                    std::to_string(key).c_str());
        return false;
    }
}

template<class K, class V, class GetSizeCallback>
bool LRUCache<K, V, GetSizeCallback>::isInCache(const K& key) {
    return (_cacheMap.find(key) != _cacheMap.end());
}

template<class K, class V, class GetSizeCallback>
bool LRUCache<K, V, GetSizeCallback>::update(const K& key, const V& newVal) {
    return put(key, newVal);
}

template<class K, class V, class GetSizeCallback>
bool LRUCache<K, V, GetSizeCallback>::invalidate(const K& key) {
    auto mapIter = _cacheMap.find(key);
    if (mapIter != _cacheMap.end()) {
        _cacheSizeUsed -= _GetSizeCallback(mapIter->second.value);
        _accessQueue.erase(mapIter->second.iter);
        _cacheMap.erase(mapIter);
        return true;
    }
    return false;
}

template<class K, class V, class GetSizeCallback>
void LRUCache<K, V, GetSizeCallback>::invalidate(const std::vector<K>& keyList) {
    for (auto& cache : keyList) {
        invalidate(cache);
    }
}

}

#endif
