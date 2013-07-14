#ifndef TOKEN_HPP
#define TOKEN_HPP

enum Token
{
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
  ON,
  WHERE,
  FROM,

  // operators
  GREATER,
  LESS,
  GREATER_EQUAL,
  LESS_EQUAL,
  NOT_EQUAL,
  EQUAL,
  LEFT_BRACE,
  RIGHT_BRACE,
  STAR,
  SLICE,

  // data types
  INTEGER,
  FLOAT,
  CHAR,
  
  // identifier
  IDENTIFIER,

  TERMINATOR,

  NULLTOKEN,
  ERROR,

  FIRST_TOKEN = CREATE,
  LAST_TOKEN = ERROR
  
};

extern const char* tokenStr[];

#endif
