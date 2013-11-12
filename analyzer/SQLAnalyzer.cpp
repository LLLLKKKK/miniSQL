
std::vector<CommandPtr> SQLAnalyzer::analyze(std::list<ParseNodePtr> parseNodes) {
    std::vector<CommandPtr> commands;
    for (auto node : parseNodes) {
        switch (node.token) {
        case CREATE:
            commands.push_back(newCreateCommand(node));
            break;
        default:
            assert(false);
        }
    }
    return commands;
}

CommandPtr newCreateCommand(ParseNodePtr parseNode) {
    auto typeToken = node->children.front().token;
    if (typeToken == TABLE) {
        auto tableName = parseNode.children[1];
        if (!validateTableName(tableName)) {
            return CommandPtr();
        }
        CommandPtr command(new CreateTableCommand(tableName));
        for (size_t i = 1; i < parseNode->childrensize(); i++) {
            auto identifierNode = parseNode->children[i];
            command->identifier.push_back(std::move(identifierNode.id));

            auto typeNode = identifierNode.children.front();
            FieldType fieldType;
            switch (typeNode->id) {
            case INTEGER:
                fieldType.baseType = IntType;
                break;
            case FLOAT:
                fieldType.baseType = FloatType;
                break;
            case CHAR:
                fieldType.baseType = CharType;
                fieldType.length = static_cast<IntNode*>(typeNode.children.front().get())->i;
                break;
            default:
                assert(false);
            }
            // TODO: ignore unique for now, add it later .....
            command->fields.push_back(std::move(fieldType));
        }
        if (!validateColumnNames(command->columns)) {
            return CommandPtr();
        }
        return command;
    }
    else if (typeToken == INDEX) {
        auto& tableName = parseNode.children[1];
        auto& indexName = parseNode.children[2];
        auto& columnNmae = parseNode.chilren[3];
        if (validateIndexName(tableName, indexName) && 
            validateColumn(tableName, columnName)) {
            return CommandPtr(new CreateIndexName(tableName, indexName, columnName));
        }
    }
    else {
        assert(false);
        return CommandPtr();
    }
    
}


CommandPtr newInsertCommand() {

}


bool validateNewTableName(const string& tableName) {
    return ! validateTableName(tableName);
}

bool validateNewIndexName(const string& tableName, const string& indexName) {
    return ! validateIndexName(tableName, indexName);
}

bool validateTableName(const string& tableName) {
    return false;
}

bool validateIndexName(const string& tableName, const string& indexName) {
    return false;
}

bool validateColumn(const string& tableName, const string& columnName) {
    return false;
}

bool validateColumns(const vector<string> columnNames) {
    return std::unqiue(columnNames.begin(), columnNames.end()) == columnnames.end();
}

AnalyzerError getLastError() const {

}
