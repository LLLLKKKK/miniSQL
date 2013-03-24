#ifndef TOKEN_HPP
#define TOKEN_HPP

enum Token
{
	BEGIN,
	
	// sql keywords
	CREATE,
	INSERT,
	SELECT,
	DELETE,
	QUIT,

	PRIMARY,
	KEY,
	INDEX,
	VALUES,
	TABLE,

	// operators
	GREATER,
	LESS,
	GREATER_EQUAL,
	LESS_EQUAL,
	NOT_EQUAL,
	EQUAL,

	// data types
	INTEGER,
	FLOAT,
	CHAR,
	
	NULLTOKEN,
	ERROR,
	
	END, 
};

#endif
