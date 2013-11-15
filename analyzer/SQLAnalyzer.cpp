
#include <cassert>
#include "SQLAnalyzer.h"

namespace miniSQL {

SQLAnalyzer::SQLAnalyzer(CatelogManager *catelogManager) : 
    _catelogManager(catelogManager) {

}

bool SQLAnalyzer::validateCreate(ParseNodePtr node, TableInfo& tableInfo) {
    auto typeToken = node->children.front()->token;
    if (typeToken == TABLE) {
        auto tablename = static_cast<IdentifierNode*>(node->children[1].get())->id;
        if (_catelogManager->getCatelog(tablename, tableInfo)) {
            MINISQL_LOG_ERROR("Table [%s] has existed!", tablename.c_str());
            return false;
        }
        tableInfo.name = tablename;
        uint32_t offsetNow = 0;
        for (size_t i = 1; i < node->children.size(); i++) {
            auto identifierNode = static_cast<IdentifierNode*>(node->children[i].get());
            auto typeNode = identifierNode->children.front();
            FieldInfo fieldInfo;
            fieldInfo.offset = offsetNow;
            switch (typeNode->token) {
            case INTEGER:
                fieldInfo.type.baseType = IntType;
                fieldInfo.type.length = 1;
                offsetNow += fieldInfo.type.length * sizeof(int);
                break;
            case FLOAT:
                fieldInfo.type.baseType = FloatType;
                fieldInfo.type.length = 1;
                offsetNow += fieldInfo.type.length * sizeof(float);
                break;
            case CHAR:
                fieldInfo.type.baseType = CharType;
                fieldInfo.type.length = 
                    static_cast<IntNode*>(typeNode->children.front().get())->i;
                offsetNow += fieldInfo.type.length * sizeof(char);
                break;
            default:
                assert(false);
            }
            tableInfo.recordInfo.fieldInfoMap[identifierNode->id] = fieldInfo;
        }
        tableInfo.recordInfo.size = offsetNow;
    }
    else if (typeToken == INDEX) {
        auto tablename = static_cast<IdentifierNode*>(node->children[1].get())->id;
        auto indexname = static_cast<IdentifierNode*>(node->children[2].get())->id;
        auto columnname = static_cast<IdentifierNode*>(node->children[3].get())->id;
        if (!_catelogManager->getCatelog(tablename, tableInfo)) {
            MINISQL_LOG_ERROR("Table [%s] does not exist!", tablename.c_str());
            return false;
        }
        if (tableInfo.indexMap.find(indexname) != tableInfo.indexMap.end()) {
            MINISQL_LOG_ERROR("Index [%s] has existed on table [%s] does not exist!", 
                    indexname.c_str(), tablename.c_str());
            return false;
        }
        if (tableInfo.recordInfo.fieldInfoMap.find(columnname) == 
            tableInfo.recordInfo.fieldInfoMap.end()) {
            MINISQL_LOG_ERROR("Field [%s] does not exist on table [%s]!", 
                    columnname.c_str(), tablename.c_str());
            return false;
        } 
    }
    else {
        assert(false);
        return false;
    }
    return true;
}

bool SQLAnalyzer::validateDrop(ParseNodePtr node) {
    auto typeToken = node->children.front()->token;
    if (typeToken == TABLE) {
        auto tablename = static_cast<IdentifierNode*>(node->children[1].get())->id;
        TableInfo tableInfo;
        if (!_catelogManager->getCatelog(tablename, tableInfo)) {
            MINISQL_LOG_ERROR("Table [%s] does not exist!", tablename.c_str());
            return false;
        }
    }
    else if (typeToken == INDEX) {
        auto indexname = static_cast<IdentifierNode*>(node->children[1].get())->id;
        if (_catelogManager->getIndex(indexname)) {
            MINISQL_LOG_ERROR("Index [%s] does not exist!", indexname.c_str());
            return false;
        }
    }
    else {
        assert(false);
        return false;
    }
    return true;
}

bool validateInsert(ParseNodePtr node, Record& record) {
    return true;
    
}

bool validateDelete(ParseNodePtr node) {
    return true;
}

bool validateSelect(ParseNodePtr node) {
    return true;
}

AnalyzerError SQLAnalyzer::getLastError() const {
    return _lastError;
}

}
