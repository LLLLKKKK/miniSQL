

#define DISALLOW_COPY_AND_ASSIGN(TypeName)          \
    TypeName(const TypeName&) = delete;             \
    TypeName operator=(const TypeName&) = delete
                      
#define DISALLOW_CONSTRUCT(TypeName)            \
    TypeName() = delete

