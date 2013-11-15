
#include <cassert>
#include "SQLAnalyzer.h"

namespace miniSQL {

SQLAnalyzer::SQLAnalyzer(CatelogManager *catelogManager) : 
    _catelogManager(catelogManager) {

}

bool SQLAnalyzer::validateCreateTable(ParseNodePtr node, TableInfo& tableInfo) {
    auto typeToken = node->children.front()->token;
    assert(typeToken == TABLE);
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

    return true;
}

bool SQLAnalyzer::validateCreateIndex(ParseNodePtr node, IndexInfo& indexInfo) {
    auto tablename = static_cast<IdentifierNode*>(node->children[1].get())->id;
    auto indexname = static_cast<IdentifierNode*>(node->children[2].get())->id;
    auto columnname = static_cast<IdentifierNode*>(node->children[3].get())->id;
    TableInfo tableInfo;
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
    indexInfo.tablename = tablename;
    indexInfo.indexname = indexname;
    indexInfo.columnname = columnname;

    return true;
}

bool SQLAnalyzer::validateDropTable(ParseNodePtr node, std::string& tablename) {
    auto typeToken = node->children.front()->token;
    assert(typeToken == TABLE);
    auto tname = static_cast<IdentifierNode*>(node->children[1].get())->id;
    TableInfo tableInfo;
    if (!_catelogManager->getCatelog(tname, tableInfo)) {
        MINISQL_LOG_ERROR("Table [%s] does not exist!", tname.c_str());
        return false;
    }
    return true;
}


bool SQLAnalyzer::validateDropIndex(ParseNodePtr node, std::string& indexname) {
    auto typeToken = node->children.front()->token;
    assert(typeToken == INDEX);
    
    auto iname = static_cast<IdentifierNode*>(node->children[1].get())->id;
    if (_catelogManager->getIndex(iname)) {
        MINISQL_LOG_ERROR("Index [%s] does not exist!", iname.c_str());
        return false;
    }

    return true;
}

bool SQLAnalyzer::validateInsert(ParseNodePtr node, Record& record) {
    return true;
    
}

bool SQLAnalyzer::validateDelete(ParseNodePtr node) {
    return true;
}

bool SQLAnalyzer::validateSelect(ParseNodePtr node, TableInfo& tableInfo) {
    auto tablename = static_cast<IdentifierNode*>(node->children[0].get())->id;
    if (_catelogManager->getCatelog(tablename, tableInfo)) {
        MINISQL_LOG_ERROR("Table [%s] does not exist!", tablename.c_str());
        return false;
    }
    if (node->children.size() == 1) {
        return true;
    }
    auto condition = node->children[1];
    for (auto it = condition->children.begin(); it != condition->children.end(); it++) {
        auto field = static_cast<IdentifierNode*>((*it)->children[0].get());
        auto fieldIt = tableInfo.recordInfo.fieldInfoMap.find(field->id);
        if (fieldIt == tableInfo.recordInfo.fieldInfoMap.end()) {
            MINISQL_LOG_ERROR("Field [%s] does not exist in table [%s]!", 
                    field->id.c_str(), tableInfo.name.c_str());
            return false;
        }
        if (fieldIt->second.type.baseType == FloatType &&
            (*it)->children[0]->token == FLOAT) {
            continue;
        } else {
            MINISQL_LOG_ERROR("Invalid Field [%s] for table [%s]!",
                    fieldIt->first.c_str(), tableInfo.name.c_str());
            return false;
        }
        if (fieldIt->second.type.baseType == IntType &&
            (*it)->children[0]->token == INTEGER) {
            continue;
        } else {
            MINISQL_LOG_ERROR("Invalid Field [%s] for table [%s]!",
                    fieldIt->first.c_str(), tableInfo.name.c_str());
            return false;
        }
        if (fieldIt->second.type.baseType == CharType &&
            (*it)->children[0]->token == CHAR) {
            auto inode = static_cast<IntNode*>((*it)->children[1]->children[0].get());
            if (inode->i != fieldIt->second.type.length) {
                MINISQL_LOG_ERROR("Invalid Field [%s] for table [%s]!",
                        fieldIt->first.c_str(), tableInfo.name.c_str());
                return false;
            }
        }
    }
    
    return true;
}

AnalyzerError SQLAnalyzer::getLastError() const {
    return _lastError;
}

}
