


#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    private:                               \
    TypeName(const TypeName&);             \
    TypeName operator=(const TypeName&)    \
        
#define DISALLOW_CONSTRUCT(TypeName)            \
    private:                                    \
    TypeName();                                 \

