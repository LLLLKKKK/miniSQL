#ifndef _QUERY_H_
#define _QUERY_H_

#include <vector>
#include <memory>
#include "common.h"

namespace miniSQL {

enum QueryType {

};

typedef uint8_t FieldBaseType;

const FieldBaseType FloatType = 0;
const FieldBaseType IntType = 1;
const FieldBaseType CharType = 2;

const FieldBaseType UniqueType = 1 << 7;

struct FieldType {
    FieldBaseType baseType;
    uint8_t length;
};

struct Field {
    FieldType fieldType;
    union {
        float f;
        int i;
        std::string c;
    }
};

enum CommandType {
    CREATE,
    DROP,
    INSERT,
    SELECT,
    DELETE,
    QUIT,
    EXECFILE    
};

enum QueryType {
    AND,
    OR,
    COMP
};

enum CompType {

};


class Command {
public:
    CommandType commandType;
};

class DropIndexCommand : public Command {
public:
    std::string name;
};

class CreateIndexCommand : public Command {
public:
    std::string tableName;
    std::string indexName;
    std::string columnName;
    CreateIndexCommand(const string& table, 
                       const string& index, 
                       const string& column) 
  : tableName(table), indexName(index), columnName(column) { }
};

class DropTableCommand : public Command {
public:
    std::string name;
};

class CreateTableCommand : public Command {
public:
    std::string tableName;
    std::vector<std::string> columns;
    std::vector<FieldType> fieldTypes;
    CreateTableCommand() : commandType(CREATE) {};
};

class InsertCommand : public Command {
    std::string name;
    std::vector<Field> fields;
};

class DeleteCommand : public Command {
    std::unique_ptr<Query> query;
};

class SelectCommand : public Command {
    std::vector<std::string> requiredFields;
    std::unique_ptr<Query> query;
};

class Query {
    QueryType queryType;
    std::string tableName;
};

class AndQuery {
    std::vector<std::unique_ptr<Query>> querys;
};

class AndQuery {
    std::vector<std::unique_ptr<Query>> querys;
};

class CompQuery {
    Field field;
    CompType compType;
};


}

#endif
