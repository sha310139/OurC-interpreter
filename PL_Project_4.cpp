

# include <stdlib.h>
# include <stdio.h>
# include <string>
# include <string.h>
# include <iostream>
# include <vector>
# include <sstream>
# include <iomanip>

# define NOT !
# define AND &&
# define OR ||


using namespace std;

enum TokenType { 
  ID, CONSTANT, 
  INT, FLOAT, CHAR, BOOL, STRING, VOID,
  IF, ELSE, WHILE, DO, RETURN, CIN, COUT,
  LP, RP, MLP, MRP, BLP, BRP, // (), [], {}
  ADD, SUB, MULT, DIV, REM,  // +, -, *, /, %  
  BITAND, BITOR, BITXOR, BITNOT,  // &, |, ^, !
  EQ, NEQ, LSS, LE, GTR, GE, LS, RS,  // ==, !=, <, <=, >, >=, <<, >>
  LOGAND, LOGOR, PE, ME, TE, DE, RE, PP, MM, // &&, ||, +=, -=, *=, /=, %=, ++, --
  SCLN, COMMA, QUE, COLON, ASSIGN, // ';', ',', '?', ':', '='
  ADDORSIGN, SUBORSIGN, // + & -
  INPUTBUFFERAPPEND, ENDINPUTBUFFERAPPEND,
  NONE, END_OF_FILE  // 初始化
};

struct Token {
  string tokenStr;
  TokenType tokenType;
} ;

struct ParameterType {
  int address;
  string listType;
  int arrPosition;  // 在array的第幾個位置 
  int functionNum;  // 紀錄是在哪個function 
  vector<string> variableValue;
} ;

struct VariableType {
  string variableType;
  string variableName;
  string constant;  // 如果沒有[], 就會是空字串""
   
  vector<string> variableValue;
  // 有可能是 (int, float) , bool , string 三種型態
  // 全部轉string存
 
  int region;
  bool isCallByReference;  // 只有parameter會用到 
} ;

struct FunctionType {
  string functionType;
  string functionName;
  vector<VariableType> parameter;
  vector<VariableType> variableList;
  vector<Token> statements;
  int pc;
  vector<ParameterType> recordParameters;
} ;



class Scanner {

public:
  void GetToken( string & tokenStr, TokenType & tokenType ) ;
  void PeekToken( string & tokenStr, TokenType & tokenType ) ;
  void Init() ;
  void SkipErrorLine() ;
  void GetFunctionVec( vector<Token> & vTokenStr ) ;
  int GetCurLine() ;
  void ClearFunctionVec() ;
  void InitLine() ;
  
  void SetLastTokenLine( int line ) ;  // 設定上一個user_input, 最後一個token的Line 
  void SetNewTokenLine() ;   // 設定最新的user_input, 第一個token的line 
  
  int GetLastGetTokenLine() ;
  
  void GetInputBuffer( vector<char> & inputBuffer ) ;
  void ClearInputBuffer() ;

private:
  
  char mNextChar;
  string mCurToken;
  TokenType mType;
  bool mFindEOF;
  
  int mLastTokenLine;  // 上一個user_input, 最後一個token的Line 
  int mNewTokenLine;   // 最新的user_input, 第一個token的line 

  int mCurCharLine;  // 讀到的cur char的Line 
  int mNextCharLine;  // 多讀一個next char的Line 
  
  int mGetTokenLine;  // 多讀一個next char的Line 
  
  vector<Token> mTmpTokenStr;  // 用來存目前這個input的token
  vector<char> mInputBuffer;
  
  void GetRealToken() ;  // 用來一個一個char提取真正的token 
  bool GetNextChar() ;   // 得到下個char 
  void SkipWhiteSpace() ;  // 跳過whitespace 
  bool SkipComment() ;
  void CheckCaseAndStore() ;
  void GetIdent() ;
  void GetNum() ;
  void GetStringAndChar() ;
  void GetSpecial() ;
  
};

void Scanner::GetToken( string & tokenStr, TokenType & tokenType ) {

  if ( mCurToken == "" ) {  // 代表沒有庫存的token 
    GetRealToken() ;
  } // if

  tokenStr = mCurToken ;
  tokenType = mType ;

  string firstChar = "";
  firstChar = firstChar + tokenStr[0];

  Token token;
  token.tokenStr = mCurToken;
  token.tokenType = mType;

  mTmpTokenStr.push_back( token ) ;  // 只要有GetToken就存起來 
  
  /*
  if ( tokenType == NONE ) {
    InitLine();
    // 因為unrecognize只會在peek token時發生
    // 所以要先做InitLine 
    throw ( "unrecognized token with first char '" + firstChar + "'\n" ).c_str();
  } // if
  */
  
  mCurToken = "" ;  // 把token初始化 
  mType = NONE ;  // token初始化 
  
  mGetTokenLine = mCurCharLine;

} // end Scanner::GetToken()

void Scanner::PeekToken( string & tokenStr, TokenType & tokenType ) {

  if ( mCurToken == "" ) {  // 代表沒有庫存的token 
    GetRealToken() ;
  } // if

  tokenStr = mCurToken ;
  tokenType = mType ;
  string firstChar = "";
  firstChar = firstChar + tokenStr[0];
  

  if ( tokenType == NONE ) {
    throw ( "unrecognized token with first char '" + firstChar + "'\n" ).c_str();
  } // if
  
} // end Scanner::PeekToken()

void Scanner::Init() {
  mNextChar = '\0';
  mFindEOF = false;
  mCurToken = "";
  mType = NONE ;
  mTmpTokenStr.clear();
  mLastTokenLine = 0;  // 最後一個"get"的token的Line
  mNewTokenLine = 1;   // user input的第一個token的Line 
  mCurCharLine = 1;
  mNextCharLine = 1;
} // end Scanner::Init()

void Scanner::GetRealToken() {
  // 直接EOF : 沒成功切token  或  unrecognize error : 成功切token但有error 
  
  mCurToken = "";
  mType = NONE ;
  bool checkWhiteSpace = true;
  
  if ( mNextChar == '\0' )  GetNextChar();

  if ( mFindEOF )  {
    mCurToken = "" ;
    mType = END_OF_FILE ;
    // throw "EOF exception" ;  // 在上一次取token時就抓到EOF 
  } // if

  SkipWhiteSpace() ;
  if ( mFindEOF ) {
    mCurToken = "" ;
    mType = END_OF_FILE ;
    // throw "EOF exception" ;  // 在skip whitespace就抓到EOF 
  } // if

  if ( SkipComment() ) {
    // 如果沒有get DIV符號, 有get comment 
    // 就檢查跳過Comment的下一行, 是否有white space
    // 如果有get就不檢查 
    while ( checkWhiteSpace AND ( mNextChar == ' ' OR mNextChar == '\t' OR mNextChar == '\n' ) ) {
      SkipWhiteSpace() ;
      if ( mFindEOF ) {
        mCurToken = "" ;
        mType = END_OF_FILE ;
        // throw "EOF exception" ;  // 在skip whitespace就抓到EOF 
      } // if

      if ( NOT SkipComment() ) checkWhiteSpace = false;
      // 如果沒有get DIV符號, 有get comment 
      // 就檢查跳過Comment的下一行, 是否有white space
      // 如果有get就不檢查 
    } // while
  } // if 

  CheckCaseAndStore() ;
  
  if ( mCurToken.size() == 0 ) {
    mCurToken = "" ;
    mType = END_OF_FILE ;
    // throw "EOF exception" ;
  } // if  
 
} // end Scanner::GetRealToken()

bool Scanner::GetNextChar() {

  if ( scanf( "%c", &mNextChar ) != EOF ) {
    mInputBuffer.push_back( mNextChar );
    mCurCharLine = mNextCharLine;
    if ( mNextChar == '\n' ) {
      mNextCharLine++;
    } // if

    return true;
  } // if
  else {  // find EOF
    mFindEOF = true;
    return false;
  } // else

} // end Scanner::GetNextChar()

void Scanner::GetIdent() {
  // mCurToken[0]已經是ID的頭
  // 把ID剩下的讀進來, 讀到非ID的char為止 
  // 設置type 

  bool ending = false ;
  
  while ( NOT ending ) {
    if ( GetNextChar() ) {

      if ( ( mNextChar >= 'A' AND mNextChar <= 'Z' ) OR
           ( mNextChar >= 'a' AND mNextChar <= 'z' ) OR
           ( mNextChar >= '0' AND mNextChar <= '9' ) OR 
           mNextChar == '_' ) {

        mCurToken = mCurToken + mNextChar ;
      } // if

      else  ending = true ;
    } // if

    else  ending = true ;  // 遇到EOF 
  } // while
  
  // 不管有沒有遇到EOF
  // 都至少會有一個ID的char的頭
  if      ( mCurToken == "int" )     mType = INT;
  else if ( mCurToken == "float" )   mType = FLOAT;
  else if ( mCurToken == "char" )    mType = CHAR;
  else if ( mCurToken == "bool" )    mType = BOOL;
  else if ( mCurToken == "string" )  mType = STRING;
  else if ( mCurToken == "void" )    mType = VOID;
  else if ( mCurToken == "if" )      mType = IF;
  else if ( mCurToken == "else" )    mType = ELSE;
  else if ( mCurToken == "while" )   mType = WHILE;
  else if ( mCurToken == "do" )      mType = DO;
  else if ( mCurToken == "return" )  mType = RETURN;
  else if ( mCurToken == "true" OR
            mCurToken == "false" )   mType = CONSTANT;
  else if ( mCurToken == "cin" )     mType = CIN;
  else if ( mCurToken == "cout" )    mType = COUT;
  else if ( mCurToken == "InputBufferAppend" )       mType = INPUTBUFFERAPPEND;
  else if ( mCurToken == "EndInputBufferAppend" )    mType = ENDINPUTBUFFERAPPEND;
  else                               mType = ID;
   

} // end Scanner::GetIdent()

void Scanner::GetNum() {
  // mCurToken[0]已經是NUM的頭 (digit或'.')
  // 把NUM剩下的讀進來, 讀到非NUM的char為止 

  bool ending = false, hasPoint = false ;

  if ( mCurToken[0] == '.' ) {
    hasPoint = true ;
  } // if 

  while ( NOT ending ) {
      
    if ( GetNextChar() ) {

      if ( mNextChar >= '0' AND mNextChar <= '9' ) {
        mCurToken = mCurToken + mNextChar ;
      } // if
      else if ( !hasPoint AND mNextChar == '.' ) {    // 還沒出現'.', 然後讀到了第一個'.' 
        mCurToken = mCurToken + mNextChar ;
        hasPoint = true ;
      } // else if

      else if ( hasPoint AND mNextChar == '.' ) {    // 已經有'.'了但又出現第二個'.' 
        ending = true ;
      } // else if
    
      else { 
        ending = true ; 
      } // else
        
    } // if
      
    else return;

  } // while

} // end Scanner::GetNum()

void Scanner::GetStringAndChar() {
  bool ending = false, hasBackslash = false ;
  char findChar = mNextChar;  // 要找到的char
                              // 如果是字串就是", 是字元就是' 
  
  while ( NOT ending ) {
    
    if ( GetNextChar() ) {
      if ( !hasBackslash AND mNextChar == '\\' ) {  // 遇到反斜線, 且前面沒有反斜線 
        hasBackslash = true;
        mCurToken = mCurToken + mNextChar ;
      } // if
      
      else if ( hasBackslash AND mNextChar == '\\' ) {  // 遇到反斜線, 且前面也是反斜線  >> \\ 
        hasBackslash = false;
        mCurToken = mCurToken + mNextChar ;
      } // else if
      
      else if ( !hasBackslash AND mNextChar == findChar ) {   // 前面不是反斜線 + 找到"或'了  >> 結束 
        mCurToken = mCurToken + mNextChar ;
        ending = true;
        GetNextChar();
      } // else if
   
      else if ( hasBackslash ) {   // 前面是反斜線+找到"或'或別的符號 >> 還沒結束 
        mCurToken = mCurToken + mNextChar ;
        hasBackslash = false; 
      } // else if
   
      else {
        mCurToken = mCurToken + mNextChar ;
      } // else
   
    } // if

    else {
      mType = NONE;
      ending = true ;  // 遇到EOF, 還沒遇到"或' >> error 
    } // else 

  } // while

} // end Scanner::GetStringAndChar()

void Scanner::GetSpecial() {
  // mCurToken[0]已經是Special的頭
  // 把Special剩下的讀進來, 讀到非Special的char為止 

  if ( mCurToken == "(" OR mCurToken == ")" OR mCurToken == "[" OR mCurToken == "]" OR
       mCurToken == "{" OR mCurToken == "}" OR mCurToken == "^" OR mCurToken == ";" OR
       mCurToken == "," OR mCurToken == "?" OR mCurToken == ":" ) {
    
    if      ( mCurToken == "(" )  mType = LP;
    else if ( mCurToken == ")" )  mType = RP;
    else if ( mCurToken == "[" )  mType = MLP ;
    else if ( mCurToken == "]" )  mType = MRP ;
    else if ( mCurToken == "{" )  mType = BLP ;
    else if ( mCurToken == "}" )  mType = BRP ;
    else if ( mCurToken == "^" )  mType = BITXOR ;
    else if ( mCurToken == ";" )  mType = SCLN ;
    else if ( mCurToken == "," )  mType = COMMA ;
    else if ( mCurToken == "?" )  mType = QUE ;
    else if ( mCurToken == ":" )  mType = COLON ;

    GetNextChar() ;
    
  } // if 
  
  
  else if ( mCurToken == "+" ) {  // maybe '+'(ADD or SIGN) OR '+=' OR '++'
    mType = ADDORSIGN ;
    
    if ( GetNextChar() ) {

      if ( mNextChar == '=' ) {  // '+='
        mCurToken = mCurToken + mNextChar ;
        mType = PE ;
        GetNextChar() ;
      } // if
      
      else if ( mNextChar == '+' ) {  // '++'
        mCurToken = mCurToken + mNextChar ;
        mType = PP ;
        GetNextChar() ;
      } // else if
      
    } // if
    
    // else : 若第二個不是'='或'+', 就只有'+'
    
  } // else if
  
  else if ( mCurToken == "-" ) {  // maybe '-' OR '-=' OR '--'
    mType = SUBORSIGN ;
    
    if ( GetNextChar() ) {
    
      if ( mNextChar == '=' ) {  // '-='
        mType = ME ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;
      } // if
      else if ( mNextChar == '-' ) {  // '--'
        mType = MM ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;   
      } // else if 
      
    } // if 
    
    // else : 若第二個不是'='或'-', 就只有'-', 不做事 
    
  } // else if

  
  else if ( mCurToken == "*" ) {  // maybe '*' OR '*='
    mType = MULT ;
    
    if ( GetNextChar() ) {
      
      if ( mNextChar == '=' ) {
        mType = TE ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;
      } // if
      
      // 若第二個不是'=', 就只有'*'

    } // if
    
  } // else if
  
  else if ( mCurToken == "/" ) {  // maybe '/' OR '/='
    mType = DIV ;
    
    if ( GetNextChar() ) {
      
      if ( mNextChar == '=' ) {
        mType = DE ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;
      } // if
      
      // 若第二個不是'=', 就只有'/'

    } // if
    
  } // else if  
  
  else if ( mCurToken == "%" ) {  // maybe '%' OR '%='
    mType = REM ;
    
    if ( GetNextChar() ) {
      
      if ( mNextChar == '=' ) {
        mType = RE ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;
      } // if
      
      // 若第二個不是'=', 就只有'%'

    } // if
    
  } // else if 
  
  else if ( mCurToken == "&" ) {  // maybe '&' OR '&&'
    mType = BITAND ;
    
    if ( GetNextChar() ) {
      
      if ( mNextChar == '&' ) {
        mType = LOGAND ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;
      } // if
      
      // 若第二個不是'&', 就只有'&'

    } // if
    
  } // else if 
  
  else if ( mCurToken == "|" ) {  // maybe '|' OR '||'
    mType = BITOR ;
    
    if ( GetNextChar() ) {
      
      if ( mNextChar == '|' ) {
        mType = LOGOR ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;
      } // if
      
      // 若第二個不是'|', 就只有'|'

    } // if
    
  } // else if 
  
  else if ( mCurToken == "!" ) {  // maybe '!' OR '!='
    mType = BITNOT ;
    
    if ( GetNextChar() ) {
      
      if ( mNextChar == '=' ) {
        mType = NEQ ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;
      } // if
      
      // 若第二個不是'=', 就只有'!'

    } // if
    
  } // else if
  
  else if ( mCurToken == "=" ) {  // maybe '=' OR '=='
    mType = ASSIGN ;
    
    if ( GetNextChar() ) {
      
      if ( mNextChar == '=' ) {
        mType = EQ ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;
      } // if
      
      // 若第二個不是'=', 就只有'='

    } // if
    
  } // else if
  
  else if ( mCurToken == "<" ) {  // maybe '<' OR '<=' OR '<<'
    mType = LSS ;
    
    if ( GetNextChar() ) {
      
      if ( mNextChar == '=' ) {
        mType = LE ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;
      } // if
      
      else if ( mNextChar == '<' ) {
        mType = LS ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;
      } // else if
      
      // 若第二個不是'='或'<', 就只有'<'

    } // if
    
  } // else if
  
  else if ( mCurToken == ">" ) {  // maybe '>' OR '>=' OR '>>'
    mType = GTR ;
    
    if ( GetNextChar() ) {
      
      if ( mNextChar == '=' ) {
        mType = GE ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;
      } // if
      
      else if ( mNextChar == '>' ) {
        mType = RS ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;
      } // else if
      
      // 若第二個不是'='或'>', 就只有'>'

    } // if
    
  } // else if
  
  else {  // 可能error 
    mType = NONE ;
    return ;
  } // else
   
} // end Scanner::GetSpecial()

void Scanner::SkipWhiteSpace() {
  
  bool findNextChar = false ;
  
  while ( NOT findNextChar ) {
    
    if ( mNextChar == ' ' OR mNextChar == '\n' OR mNextChar == '\t' ) {  //   white-space  
      if ( !GetNextChar() )  return;
    } // if
    
    else findNextChar = true ;
    
  } // while
  
} // end Scanner::SkipWhiteSpace()

bool Scanner::SkipComment() {
  // 用來確認是否有comment並跳過
  // 若找到'/'卻沒有第二個'/', 會將'/'符號存到Token
  // 讓 CheckCaseAndStore() 處理這個token 
  
  if ( mNextChar == '/' ) {
    mCurToken = mCurToken + mNextChar;

    if ( GetNextChar() ) {
      if ( mNextChar == '/' ) {  // 是註解 
        // 把剛剛放在mCurToken[0]裡的'/'移掉
        mCurToken = "" ; 
        while ( mNextChar != '\n' ) {
          if ( NOT GetNextChar() )  return true;
        } // while
      } // if 
      else return false; // 表示get DIV符號要處理, 沒有skipComment 
    } // if

    // 若第二個不是'/'
    // 表示前面的'/'是DIV符號
    // 後面是某個token的頭或whitespace 
      
  } // if

  return true;
  
} // end Scanner::SkipComment()

void Scanner::CheckCaseAndStore() {
  
  // 已經讀入token頭
  // 用來確認case並呼叫讀入token的function 
  // 若遇到unrecognize error , 將type轉成NONE 

  if ( mCurToken == "/" ) {  // maybe '/' OR '/='
    mType = DIV ;
    
    if ( mNextChar == '=' ) {
      mType = DE ;
      mCurToken = mCurToken + mNextChar ;
      GetNextChar() ;
    } // if
      
    // 若第二個不是'=', 就只有'/'  
  } // if  
  
  else if ( ( mNextChar >= 'A' AND mNextChar <= 'Z' ) OR
            ( mNextChar >= 'a' AND mNextChar <= 'z' ) ) {
    mCurToken = mCurToken + mNextChar ;
    GetIdent() ;  // 把剩下的讀完
  } // else if
  
  else if ( mNextChar == '.' OR ( mNextChar >= '0' AND mNextChar <= '9' ) ) {  //  Number
    mCurToken = mCurToken + mNextChar ;
    mType = CONSTANT ;
    GetNum() ;
    if ( mCurToken.size() == 1 && mCurToken[0] == '.' ) {  // 只有一個'.', 表示error 
      mType = NONE ;
    } // if
  } // else if
  
  else if ( mNextChar == '"' OR mNextChar == '\'' ) {
    mCurToken = mCurToken + mNextChar ;
    mType = CONSTANT ;
    GetStringAndChar() ;
  } // else if
  
  else if ( mNextChar == '(' OR mNextChar == ')' OR mNextChar == '[' OR mNextChar == ']' OR 
            mNextChar == '{' OR mNextChar == '}' OR mNextChar == '+' OR mNextChar == '-' OR 
            mNextChar == '*' OR mNextChar == '/' OR mNextChar == '%' OR mNextChar == '^' OR
            mNextChar == '>' OR mNextChar == '<' OR mNextChar == '=' OR mNextChar == '!' OR
            mNextChar == '&' OR mNextChar == '|' OR mNextChar == ';' OR mNextChar == ',' OR
            mNextChar == '?' OR mNextChar == ':' ) {  //  Special
    mCurToken = mCurToken + mNextChar ;
    GetSpecial() ;
  } // else if

  else {
    mCurToken = mCurToken + mNextChar ;
    mType = NONE ;
  } // else
  
  
} // end Scanner::CheckCaseAndStore()

void Scanner::SkipErrorLine() {
  // 通常發生error的時候都還沒peek下一個token 
  // 如果是unexpect, 可能只有peek目前的token
  // 如果是undefine, 有get ID 這個token 
  // 不管怎樣CurCharLine都會剛好 == error line 
    
  // 有可能error的token是還沒get的

  mCurToken = "";
  mType = NONE;
  while ( mNextChar != '\n' ) {
    if ( !GetNextChar() )  return;
  } // while

} // end Scanner::SkipErrorLine()

int Scanner::GetCurLine() {
  return mCurCharLine;
} // Scanner::GetCurLine()

void Scanner::GetFunctionVec( vector<Token> & vTokenStr ) {
  vTokenStr = mTmpTokenStr;
} // Scanner::GetFunctionVec()

void Scanner::InitLine() {
  
  string tmpStr;
  TokenType tmpType;
  
  PeekToken( tmpStr, tmpType );
  SetNewTokenLine();

  mCurCharLine = mNewTokenLine - mLastTokenLine;
  mNextCharLine = mNextCharLine - mLastTokenLine;
  mNewTokenLine = mNewTokenLine - mLastTokenLine;

} // Scanner::InitLine()


void Scanner::SetLastTokenLine( int line ) {
// 設定上一個user_input, 最後一個token的Line 
  mLastTokenLine = line;
} // Scanner::SetLastTokenLine()

void Scanner::SetNewTokenLine() {
// 設定最新的user_input, 第一個token的line 
  mNewTokenLine = mCurCharLine;
} // Scanner::SetNewTokenLine()

void Scanner::ClearFunctionVec() {
  mTmpTokenStr.clear();
} // Scanner::ClearFunctionVec()

void Scanner::GetInputBuffer( vector<char> & inputBuffer ) {
  inputBuffer = mInputBuffer;
} // Scanner::GetInputBuffer()

void Scanner::ClearInputBuffer() {
  mInputBuffer.clear();
} // Scanner::ClearInputBuffer()

int Scanner::GetLastGetTokenLine() {
  return mGetTokenLine;
} // Scanner::GetLastGetTokenLine()  


class Parser {

public:
  void Init() ;
  bool CheckUserInput() ;

private:
  Scanner mScanner;
  vector<VariableType> mVariableList;
  vector<FunctionType> mFunctionList;

  vector<VariableType> mTempIDList;  // 用來存','分隔的ID, 等讀完才能一次存起來 

  vector<char> mInputBuffer;
  vector<Token> mUserInputVec;
  
  vector<FunctionType> mCallStack;

  FunctionType mCurFunction;  // 用來存目前讀入的function
   

  string mTokenStr;
  TokenType mTokenType;
  
  bool mIsFunction;  // definition是function, 要多檢查parameter
  
  bool mPassValueIsAddress;  // 紀錄現在是不是get一個address的參數 
  
  ParameterType mTmpParameter; 
  
  int mUserInputVecNum;
  int mCurVariableRegion; // 紀錄目前的變數勢力範圍 
  int mSkipExp;  // 用來設定是否要跳過當前exp(if, while ...) 
  int mIsCout;
  int mInParentheses;
  int mExecuteFunction; // 要執行function, 需要調function出來計算值 


  void StoreVariable( string variableType, string variableStr, string constant )  ;
  void GetVariableValue( double & returnValue, string & returnStr,
                         string variableStr, double constantValue ) ;
  void UpdateVariableValue( string curType, double newValue,
                            string newValueStr, string variableStr, double constantValue ) ;
  void GetVariableType( string & idType, string variableStr ) ;
  void ClearRegionVariable() ;
  
  void GetFunctionStatements() ;
  void StoreFunction() ;
  void SetExecuteFunction( string functionName, vector<ParameterType> & tmpValueList ) ;
  void RestoreExecuteFunction() ;
  void GetReferenceValue( VariableType & curV, ParameterType curP ) ;
  void UpdateReferenceValue( VariableType curV, ParameterType curP, double constantValue ) ;

  void CheckIDExistOrNot( string idStr ) ;

  void ListAllVariables() ;
  void ListAllFunctions() ;
  void ListVariable( string variableName ) ;
  void ListFunction( string functionName ) ;
  bool NoNeedSpace( TokenType type1, TokenType type2 ) ;
  
  bool IsFloat( string str ) ;
  string FloatToStr( double num ) ; 

  void SkipBufferWhiteSpace() ;
  void InputBufferAppend() ;
  void UpdateInputBuffer() ;
  void GetInputBufferItem( string idType, string & returnStr ) ;
  
  void PeekInputVecToken( string & str, TokenType & type ) ;
  void GetInputVecToken( string & str, TokenType & type ) ;


  void User_Input() ;
  void Definition() ;
  void Type_Specifier() ;
  void Function_Definition_Or_Declarators( string tmpType, string tmpName ) ;
  void Rest_Of_Declarators( string tmpVariableType, string tmpVariableName ) ;
  void Function_Definition_Without_ID() ;
  void Formal_Parameter_List() ;
  void Compound_Statement() ;
  void Declaration() ;
  void Statement() ;
  void Cin_expression() ;
  void Cout_expression() ;
  void Expression() ;
  void Basic_Expression() ;
  void Rest_Of_ID_Started_Basic_Exp() ;
  void Rest_Of_PPMM_ID_Started_Basic_Exp() ;
  void Sign() ;
  void Actual_Parameter_List() ;
  void Assignment_Operator() ;
  void Romce_And_Romloe() ;
  void Rest_Of_Maybe_Logical_Or_Exp() ;
  void Maybe_Logical_And_Exp() ;
  void Rest_Of_Maybe_Logical_And_Exp() ;
  void Maybe_Bit_Or_Exp() ;
  void Rest_Of_Maybe_Bit_Or_Exp() ;
  void Maybe_Bit_Ex_Or_Exp() ;
  void Rest_Of_Maybe_Bit_Ex_Or_Exp() ;
  void Maybe_Bit_And_Exp() ;
  void Rest_Of_Maybe_Bit_And_Exp() ;
  void Maybe_Equality_Exp() ;
  void Rest_Of_Maybe_Equality_Exp() ;
  void Maybe_Relational_Exp() ;
  void Rest_Of_Maybe_Relational_Exp() ;
  void Maybe_Shift_Exp() ;
  void Rest_Of_Maybe_Shift_Exp() ;
  void Maybe_Additive_Exp() ;
  void Rest_Of_Maybe_Additive_Exp() ;
  void Maybe_Mult_Exp() ;
  void Rest_Of_Maybe_Mult_Exp() ;
  void Unary_Exp() ;
  void Signed_Unary_Exp() ;
  void Unsigned_Unary_Exp() ;
  
  
  void UpdateCurType( string & curType, string curToken ) ;
  
  void Declaration_v() ;
  void Rest_Of_Declarators_v( string tmpVariableType, string tmpVariableName ) ;
  void Compound_Statement_v( string & curType, double & returnValue, string & returnStr, bool & isReturn ) ;
  void Statement_v( string & curType, double & returnValue, string & returnStr, bool & isReturn ) ;
  void Cin_expression_v( string & curType, double & returnValue, string & returnStr ) ;
  void Cout_expression_v( string & curType, double & returnValue, string & returnStr ) ;
  void Expression_v( string & curType, double & returnValue, string & returnStr ) ;
  void Basic_Expression_v( string & curType, double & returnValue, string & returnStr ) ;
  void Rest_Of_ID_Started_Basic_Exp_v( string & curType, double & returnValue,
                                       string & returnStr, string idStr ) ;
  void Rest_Of_PPMM_ID_Started_Basic_Exp_v( string & curType, double & returnValue,
                                            string & returnStr, TokenType opType, string idStr ) ;
  void Actual_Parameter_List_v( string & curType, double & returnValue,
                                string & returnStr, vector<ParameterType> & tmpValueList ) ;
  void Romce_And_Romloe_v( string & curType, double & returnValue,
                           string & returnStr, double idValue, string idValueStr ) ;
  void Rest_Of_Maybe_Logical_Or_Exp_v( string & curType, double & returnValue,
                                       string & returnStr, double idValue, string idValueStr ) ;
  void Maybe_Logical_And_Exp_v( string & curType, double & returnValue, string & returnStr ) ;
  void Rest_Of_Maybe_Logical_And_Exp_v( string & curType, double & returnValue,
                                        string & returnStr, double idValue, string idValueStr ) ;
  void Maybe_Bit_Or_Exp_v( string & curType, double & returnValue, string & returnStr ) ;
  void Rest_Of_Maybe_Bit_Or_Exp_v( string & curType, double & returnValue,
                                   string & returnStr, double idValue, string idValueStr ) ;
  void Maybe_Bit_Ex_Or_Exp_v( string & curType, double & returnValue, string & returnStr ) ;
  void Rest_Of_Maybe_Bit_Ex_Or_Exp_v( string & curType, double & returnValue,
                                      string & returnStr, double idValue, string idValueStr ) ;
  void Maybe_Bit_And_Exp_v( string & curType, double & returnValue, string & returnStr ) ;
  void Rest_Of_Maybe_Bit_And_Exp_v( string & curType, double & returnValue,
                                    string & returnStr, double idValue, string idValueStr ) ;
  void Maybe_Equality_Exp_v( string & curType, double & returnValue, string & returnStr ) ;
  void Rest_Of_Maybe_Equality_Exp_v( string & curType, double & returnValue,
                                     string & returnStr, double idValue, string idValueStr ) ;
  void Maybe_Relational_Exp_v( string & curType, double & returnValue, string & returnStr ) ;
  void Rest_Of_Maybe_Relational_Exp_v( string & curType, double & returnValue,
                                       string & returnStr, double idValue, string idValueStr ) ;
  void Maybe_Shift_Exp_v( string & curType, double & returnValue, string & returnStr ) ;
  void Rest_Of_Maybe_Shift_Exp_v( string & curType, double & returnValue,
                                  string & returnStr, double idValue, string idValueStr ) ;
  void Maybe_Additive_Exp_v( string & curType, double & returnValue, string & returnStr ) ;
  void Rest_Of_Maybe_Additive_Exp_v( string & curType, double & returnValue,
                                     string & returnStr, double idValue, string idValueStr ) ;
  void Maybe_Mult_Exp_v( string & curType, double & returnValue, string & returnStr ) ;
  void Rest_Of_Maybe_Mult_Exp_v( string & curType, double & returnValue,
                                 string & returnStr, double idValue, string idValueStr ) ;
  void Unary_Exp_v( string & curType, double & returnValue, string & returnStr ) ;
  void Signed_Unary_Exp_v( string & curType, double & returnValue, string & returnStr ) ;
  void Unsigned_Unary_Exp_v( string & curType, double & returnValue, string & returnStr ) ;


}; 

void Parser::Init() {
  mScanner.Init();
  mVariableList.clear();
  mFunctionList.clear();

  mTempIDList.clear();
  mUserInputVec.clear();
  
  mTokenStr = "";
  mTokenType = NONE;
  
  mIsFunction = false;
  mPassValueIsAddress = false; 
  mUserInputVecNum = 0;
  mCurVariableRegion = 0; 
  mSkipExp = 0;
  mIsCout = 0;
  mInParentheses = 0;
  mExecuteFunction = 0; 
  
  mCurFunction.functionName = "";
  mCurFunction.functionType = "";
  mCurFunction.parameter.clear();
  mCurFunction.statements.clear();
  mCurFunction.variableList.clear();
  mCurFunction.pc = 0;
  
  mTmpParameter.address = -1;
  mTmpParameter.arrPosition = 0;
  mTmpParameter.listType = "";
  mTmpParameter.variableValue.clear();
  
} // end Parser::Init()


bool Parser::CheckUserInput() {

  mTokenStr = "";
  mTokenType = NONE;

  mTempIDList.clear();
  mUserInputVec.clear();
  mUserInputVecNum = 0;
  mCurVariableRegion = 0; 
  mSkipExp = 0;
  mIsCout = 0;
  mInParentheses = 0;
  mExecuteFunction = 0;
  
  mIsFunction = false;
  mPassValueIsAddress = false; 
  
  vector<string> tempVec;
  
  mCurFunction.functionName = "";
  mCurFunction.functionType = "";
  mCurFunction.parameter.clear();
  mCurFunction.statements.clear();
  mCurFunction.variableList.clear();
  mCurFunction.pc = 0;
  
  mTmpParameter.address = -1;
  mTmpParameter.arrPosition = 0;
  mTmpParameter.listType = "";
  mTmpParameter.variableValue.clear();
  

  try {
    // mScanner.PeekToken( mTokenStr, mTokenType );
    // mScanner.SetNewTokenLine();
    mScanner.InitLine();  // 確定開始的line >> 目前行數 - 上一個input的最後一行 
    mScanner.ClearFunctionVec();  // 讓token串歸零
    User_Input() ;
  } catch ( const char * msg ) {

    if ( strcmp( msg, "EOF exception" ) == 0 OR strcmp( msg, "Done" ) == 0 )
      return false;
    
    int curLine = mScanner.GetCurLine();
    mScanner.SetLastTokenLine( curLine );
    
    if ( curLine == 0 ) curLine++;
    printf( "Line %d : ", curLine ) ;
    printf( "%s", msg ) ;
    
    // 發生error的話都是peek階段發現
    // 會在get token後面
    // 如果正常執行只會紀錄最後一個get token的line 
    // 所以發生錯誤要重新設定一次Line 
    
    mScanner.SkipErrorLine();    
    return true;
  } // catch
  
  
  mScanner.SetLastTokenLine( mScanner.GetLastGetTokenLine() );

  return true;

} // end Parser::CheckUserInput()

void Parser::StoreVariable( string variableType, string variableStr, string constant ) {
  
  if ( mExecuteFunction > 0 ) {
    int size = 0;
    for ( int i = 0 ; i < mCurFunction.variableList.size() ; i++ ) {
      if ( variableStr == mCurFunction.variableList[i].variableName AND
           mCurFunction.variableList[i].region == mCurVariableRegion ) {
        mCurFunction.variableList[i].variableType = variableType ;
        mCurFunction.variableList[i].variableName = variableStr ;
        mCurFunction.variableList[i].constant = constant;
        mCurFunction.variableList[i].region = mCurVariableRegion;
        if ( constant != "" ) {
          size = atoi( constant.c_str() );
          mCurFunction.variableList[i].variableValue.clear(); // 可能是重新宣告 
          mCurFunction.variableList[i].variableValue.resize( size, "0" );
        } // if
        else {
          mCurFunction.variableList[i].variableValue.clear(); // 可能是重新宣告 
          mCurFunction.variableList[i].variableValue.resize( 1, "0" );
        } // else

        return;
      } // if
    } // for
  
    // 如果沒有在idList裡找到的話
    // 就新增一個 
    // 要按照順序插入 
    VariableType newVariable;
    newVariable.variableType = variableType;
    newVariable.variableName = variableStr;
    newVariable.constant = constant;
    newVariable.region = mCurVariableRegion;
    newVariable.isCallByReference = false;

    if ( constant != "" ) {
      size = atoi( constant.c_str() );
      newVariable.variableValue.resize( size, "0" );
    } // if
    else {
      newVariable.variableValue.resize( 1, "0" );
    } // else

    for ( int i = 0 ; i < mCurFunction.variableList.size() ; i++ ) {
      if ( variableStr < mCurFunction.variableList[i].variableName ) {
        mCurFunction.variableList.insert( mCurFunction.variableList.begin() + i, newVariable );
        return;
      } // if
      else if ( variableStr == mCurFunction.variableList[i].variableName AND
                mCurVariableRegion > mCurFunction.variableList[i].region ) {
        // 4, 3, 2, 1, 0這樣排
        mCurFunction.variableList.insert( mCurFunction.variableList.begin() + i, newVariable );
        return;
      } // else if
    } // for
  
    // 若排序是最大的就放在最後面 
    mCurFunction.variableList.push_back( newVariable ) ;
    return;
  } // if
  
  
  int size = 0;
  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( variableStr == mVariableList[i].variableName AND mVariableList[i].region == mCurVariableRegion ) {
      mVariableList[i].variableType = variableType ;
      mVariableList[i].variableName = variableStr ;
      mVariableList[i].constant = constant;
      mVariableList[i].region = mCurVariableRegion;
      if ( constant != "" ) {
        size = atoi( constant.c_str() );
        mVariableList[i].variableValue.clear(); // 可能是重新宣告 
        mVariableList[i].variableValue.resize( size, "0" );
      } // if
      else {
        mVariableList[i].variableValue.clear(); // 可能是重新宣告 
        mVariableList[i].variableValue.resize( 1, "0" );
      } // else
      
      if ( mCurVariableRegion == 0 ) {
        printf( "New definition of %s entered ...\n", variableStr.c_str() );
      } // if

      return;
    } // if
  } // for
  
  // 如果沒有在idList裡找到的話
  // 就新增一個 
  // 要按照順序插入 
  VariableType newVariable;
  newVariable.variableType = variableType;
  newVariable.variableName = variableStr;
  newVariable.constant = constant;
  newVariable.region = mCurVariableRegion;
  newVariable.isCallByReference = false;

  if ( constant != "" ) {
    size = atoi( constant.c_str() );
    newVariable.variableValue.resize( size, "0" );
  } // if
  else {
    newVariable.variableValue.resize( 1, "0" );
  } // else

  if ( mCurVariableRegion == 0 ) {
    printf( "Definition of %s entered ...\n", variableStr.c_str() );
  } // if

  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( variableStr < mVariableList[i].variableName ) {
      mVariableList.insert( mVariableList.begin() + i, newVariable );
      return;
    } // if
    else if ( variableStr == mVariableList[i].variableName AND
              mCurVariableRegion > mVariableList[i].region ) {
      // 4, 3, 2, 1, 0這樣排
      mVariableList.insert( mVariableList.begin() + i, newVariable );
      return;
    } // else if
  } // for
  
  // 若排序是最大的就放在最後面 
  mVariableList.push_back( newVariable ) ;

} // end Parser::StoreVariable()

void Parser::GetVariableValue( double & returnValue, string & returnStr,
                               string variableStr, double constantValue ) {
  
  // 已經檢查過undefined, 不會有找不到的問題
  if ( mExecuteFunction > 0 ) {
    
    for ( int i = 0 ; i < mCurFunction.parameter.size() ; i++ ) {
      // 先找參數
      if ( variableStr == mCurFunction.parameter[i].variableName ) {
        // 如果是目前function的參數, 傳進新function裡 
        // 且目前參數是reference的, 表示有之前的位址訊息 
        // 新function的這個參數要記錄, 目前function參數之前存的位址訊息
        // 但有可能目前function參數是整個arr
        // 新的只要array的其中一個元素
        // 所以 arrPosition可能不同 
    
        // 若目前參數不是reference, 就記錄目前參數的位址訊息 
        if ( mCurFunction.parameter[i].isCallByReference OR
             mCurFunction.parameter[i].constant != "" ) {
          mTmpParameter.address = mCurFunction.recordParameters[i].address;
          mTmpParameter.listType = mCurFunction.recordParameters[i].listType;
          mTmpParameter.functionNum = mCurFunction.recordParameters[i].functionNum;
          if ( mCurFunction.recordParameters[i].arrPosition == -1 ) {
            mTmpParameter.arrPosition = constantValue;
          } // if
          else {
            mTmpParameter.arrPosition = mCurFunction.recordParameters[i].arrPosition;
          } // else
        } // if
        else {
          mTmpParameter.address = i;
          mTmpParameter.listType = "parameter";

          if ( mExecuteFunction == 0 ) mTmpParameter.functionNum = 0;
          else                         mTmpParameter.functionNum = mExecuteFunction-1;
          
          mTmpParameter.arrPosition = constantValue;
        } // else
        
        if ( mCurFunction.parameter[i].isCallByReference OR
             mCurFunction.parameter[i].constant != "" ) {
          GetReferenceValue( mCurFunction.parameter[i], mCurFunction.recordParameters[i] );
          // 到位址裡拿值 
        } // if
        
        if ( mCurFunction.parameter[i].constant == "" ) {  // 這個變數宣告不是array 
          if ( constantValue == -1.0 ) {  // 不是要array 
            // 預設每個變數都有初始化
            returnValue = atof( mCurFunction.parameter[i].variableValue[0].c_str() );
            returnStr = mCurFunction.parameter[i].variableValue[0];
          } // if
          else {  // 宣告是變數, 但要get array的值 
            throw "Not array error\n";
          } // else
        } // if
        else {  // 宣告是array 
          if ( constantValue >= 0 ) {
            returnValue = atof( mCurFunction.parameter[i].variableValue[constantValue].c_str() );
            returnStr = mCurFunction.parameter[i].variableValue[constantValue] ;
          } // if
          else if ( constantValue == -1.0 ) {  // 要整個array (只有傳參數會用到) 
            mPassValueIsAddress = true;
            mTmpParameter.variableValue = mCurFunction.parameter[i].variableValue;
            returnValue = i;
            returnStr = "" ;
          } // else if
          else {
            throw ( "Runtime error : array '" + variableStr + "' index 'i' = " +
                    FloatToStr( constantValue ) + " out of range!\n" ).c_str()  ;
          } // else
        } // else

        return;
      } // if
    } // for
    
    for ( int i = 0 ; i < mCurFunction.variableList.size() ; i++ ) {
      // 再找function裡宣告的變數 
      // 需要比較區域範圍, 因為有可能有if 
      if ( variableStr == mCurFunction.variableList[i].variableName AND
           mCurFunction.variableList[i].region <= mCurVariableRegion ) {

        // 如果是目前function的變數, 傳進新function裡 
        // 新function的這個參數要記錄, 目前function變數的當前資訊 
        mTmpParameter.address = i;
        mTmpParameter.listType = "variableList_F";

        if ( mExecuteFunction == 0 ) mTmpParameter.functionNum = 0;
        else                         mTmpParameter.functionNum = mExecuteFunction-1;

        if ( mCurFunction.variableList[i].constant == "" ) {  // 這個變數宣告不是array 
          if ( constantValue == -1.0 ) {  // 不是要array 
            // 預設每個變數都有初始化 
            returnValue = atof( mCurFunction.variableList[i].variableValue[0].c_str() );
            returnStr = mCurFunction.variableList[i].variableValue[0];
          } // if
        
          else {  // 宣告是變數, 但要get array的值 
            throw "Not array error\n";
          } // else
        } // if
        else {  // 宣告是array 
          if ( constantValue >= 0 ) {
            mTmpParameter.arrPosition = constantValue;
            returnValue = atof( mCurFunction.variableList[i].variableValue[constantValue].c_str() );
            returnStr = mCurFunction.variableList[i].variableValue[constantValue] ;
          } // if
          else if ( constantValue == -1.0 ) {  // 要整個array (只有傳參數會用到) 
            mPassValueIsAddress = true;
            mTmpParameter.variableValue = mCurFunction.variableList[i].variableValue;
            returnValue = i;
            returnStr = "" ;
          } // else if
          else {
            throw ( "Runtime error : array '" + variableStr + "' index 'i' = " +
                    FloatToStr( constantValue ) + " out of range!\n" ).c_str()  ;
          } // else
        } // else

        return;
      } // if
    } // for
    
    
    for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
      // 找全域 
      if ( variableStr == mVariableList[i].variableName AND mVariableList[i].region == 0 ) {
        mTmpParameter.address = i;
        mTmpParameter.listType = "variableList";
        mTmpParameter.functionNum = 0;
        
        if ( mVariableList[i].constant == "" ) {  // 這個變數宣告不是array 
          if ( constantValue == -1.0 ) {  // 不是要array 
            // 預設每個變數都有初始化 
            returnValue = atof( mVariableList[i].variableValue[0].c_str() );
            returnStr = mVariableList[i].variableValue[0];
          } // if
        
          else {  // 宣告是變數, 但要get array的值 
            throw "Not array error\n";
          } // else
        } // if
        else {  // 宣告是array 
          if ( constantValue >= 0 ) {
            mTmpParameter.arrPosition = constantValue;
            returnValue = atof( mVariableList[i].variableValue[constantValue].c_str() );
            returnStr = mVariableList[i].variableValue[constantValue] ;
          } // if
          else if ( constantValue == -1.0 ) {  // 要整個array (只有傳參數會用到) 
            mPassValueIsAddress = true;
            mTmpParameter.variableValue = mVariableList[i].variableValue;
            returnValue = i;
            returnStr = "" ;
          } // else if
          else {
            throw ( "Runtime error : array '" + variableStr + "' index 'i' = " +
                    FloatToStr( constantValue ) + " out of range!\n" ).c_str()  ;
          } // else
        } // else

        return;
      } // if
    } // for

    return;
  } // if
  
  
  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( variableStr == mVariableList[i].variableName AND mVariableList[i].region <= mCurVariableRegion ) {
      mTmpParameter.address = i;
      mTmpParameter.listType = "variableList";
      mTmpParameter.functionNum = 0;
      if ( mVariableList[i].constant == "" ) {  // 這個變數宣告不是array 
        if ( constantValue == -1.0 ) {  // 不是要array 
          // 預設每個變數都有初始化 
          returnValue = atof( mVariableList[i].variableValue[0].c_str() );
          returnStr = mVariableList[i].variableValue[0];
        } // if
        
        else {  // 宣告是變數, 但要get array的值 
          throw "Not array error\n";
        } // else
      } // if
      else {  // 宣告是array 
        if ( constantValue >= 0 ) {
          mTmpParameter.arrPosition = constantValue;
          returnValue = atof( mVariableList[i].variableValue[constantValue].c_str() );
          returnStr = mVariableList[i].variableValue[constantValue] ;
        } // if
        else if ( constantValue == -1.0 ) {  // 要整個array (只有傳參數會用到) 
          mPassValueIsAddress = true;
          mTmpParameter.variableValue = mVariableList[i].variableValue;
          returnValue = 0; 
          returnStr = "" ;
        } // else if
        else {
          throw ( "Runtime error : array '" + variableStr + "' index 'i' = " +
                  FloatToStr( constantValue ) + " out of range!\n" ).c_str()  ;
        } // else
      } // else

      return;
    } // if
  } // for
} // Parser::GetVariableValue()

void Parser::UpdateVariableValue( string curType, double newValue,
                                  string newValueStr, string variableStr, double constantValue ) {
    
  if ( mExecuteFunction > 0 ) {
    for ( int i = 0 ; i < mCurFunction.parameter.size() ; i++ ) {
      // 先找參數
      if ( variableStr == mCurFunction.parameter[i].variableName ) {
        if ( mCurFunction.parameter[i].constant == "" ) {  // 這個變數宣告不是array 
          if ( constantValue == -1.0 ) {  // 不是要array 
            if ( mCurFunction.parameter[i].variableType == "string" OR
                 mCurFunction.parameter[i].variableType == "char" ) {
              mCurFunction.parameter[i].variableValue[0] = newValueStr;
            } // if
            else if ( mCurFunction.parameter[i].variableType == "int" ) {
              int tmp = newValue;
              mCurFunction.parameter[i].variableValue[0] = FloatToStr( tmp );
            } // else if
            else {  // bool統一用0或1存 
              mCurFunction.parameter[i].variableValue[0] = FloatToStr( newValue );
            } // else
          } // if
        
          else {  // 宣告是變數, 但要get array的值 
            throw "Not array error\n";
          } // else
        } // if
        else {  // 宣告是array 
          if ( constantValue >= 0 ) {
            if ( mCurFunction.parameter[i].variableType == "string" OR
                 mCurFunction.parameter[i].variableType == "char" ) {
              mCurFunction.parameter[i].variableValue[constantValue] = newValueStr;
            } // if
            else if ( mCurFunction.parameter[i].variableType == "int" ) {
              int tmp = newValue;
              mCurFunction.parameter[i].variableValue[constantValue] = FloatToStr( tmp );
            } // else if
            else {
              mCurFunction.parameter[i].variableValue[constantValue] = FloatToStr( newValue );
            } // else
          } // if
          else {
            throw ( "Runtime error : array '" + variableStr + "' index 'i' = " +
                    FloatToStr( constantValue ) + " out of range!\n" ).c_str()  ;
          } // else
        } // else
        
        if ( mCurFunction.parameter[i].isCallByReference OR
             mCurFunction.parameter[i].constant != "" ) {
          UpdateReferenceValue( mCurFunction.parameter[i], 
                                mCurFunction.recordParameters[i], constantValue );
        } // if
        
        return;
      } // if
    } // for
    
    for ( int i = 0 ; i < mCurFunction.variableList.size() ; i++ ) {
      // 再找function裡宣告的變數 
      // 需要比較區域範圍, 因為有可能有if
      if ( variableStr == mCurFunction.variableList[i].variableName AND
           mCurFunction.variableList[i].region <= mCurVariableRegion ) {

        if ( mCurFunction.variableList[i].constant == "" ) {  // 這個變數宣告不是array 
          if ( constantValue == -1.0 ) {  // 不是要array 
            // 預設每個變數都有初始化 
            if ( mCurFunction.variableList[i].variableType == "string" OR
                 mCurFunction.variableList[i].variableType == "char" ) {
              mCurFunction.variableList[i].variableValue[0] = newValueStr;
            } // if
            else if ( mCurFunction.variableList[i].variableType == "int" ) {
              int tmp = newValue;
              mCurFunction.variableList[i].variableValue[0] = FloatToStr( tmp );
            } // else if
            else {  // bool統一用0或1存 
              mCurFunction.variableList[i].variableValue[0] = FloatToStr( newValue );
            } // else
          } // if
        
          else {  // 宣告是變數, 但要get array的值 
            throw "Not array error\n";
          } // else
        } // if
        else {  // 宣告是array 
          if ( constantValue >= 0 ) {
            if ( mCurFunction.variableList[i].variableType == "string" OR
                 mCurFunction.variableList[i].variableType == "char" ) {
              mCurFunction.variableList[i].variableValue[constantValue] = newValueStr;
            } // if
            else if ( mCurFunction.variableList[i].variableType == "int" ) {
              int tmp = newValue;
              mCurFunction.variableList[i].variableValue[constantValue] = FloatToStr( tmp );
            } // else if
            else {
              mCurFunction.variableList[i].variableValue[constantValue] = FloatToStr( newValue );
            } // else
          } // if
          else {
            throw ( "Runtime error : array '" + variableStr + "' index 'i' = " +
                    FloatToStr( constantValue ) + " out of range!\n" ).c_str()  ;
          } // else
        } // else

        return;
      } // if
    } // for
    
    
    for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
      // 找全域 
      if ( variableStr == mVariableList[i].variableName AND mVariableList[i].region == 0 ) {

        if ( mVariableList[i].constant == "" ) {  // 這個變數宣告不是array 
          if ( constantValue == -1.0 ) {  // 不是要array 
            // 預設每個變數都有初始化 
            if ( mVariableList[i].variableType == "string" OR mVariableList[i].variableType == "char" ) {
              mVariableList[i].variableValue[0] = newValueStr;
            } // if
            else if ( mVariableList[i].variableType == "int" ) {
              int tmp = newValue;
              mVariableList[i].variableValue[0] = FloatToStr( tmp );
            } // else if
            else {  // bool統一用0或1存 
              mVariableList[i].variableValue[0] = FloatToStr( newValue );
            } // else
          } // if
        
          else {  // 宣告是變數, 但要get array的值 
            throw "Not array error\n";
          } // else
        } // if
        else {  // 宣告是array 
          if ( constantValue >= 0 ) {
            if ( mVariableList[i].variableType == "string" OR mVariableList[i].variableType == "char" ) {
              mVariableList[i].variableValue[constantValue] = newValueStr;
            } // if
            else if ( mVariableList[i].variableType == "int" ) {
              int tmp = newValue;
              mVariableList[i].variableValue[constantValue] = FloatToStr( tmp );
            } // else if
            else {
              mVariableList[i].variableValue[constantValue] = FloatToStr( newValue );
            } // else
          } // if
          else {
            throw ( "Runtime error : array '" + variableStr + "' index 'i' = " +
                    FloatToStr( constantValue ) + " out of range!\n" ).c_str()  ;
          } // else
        } // else

        return;
      } // if
    } // for
    
    return;
  } // if


  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( variableStr == mVariableList[i].variableName AND mVariableList[i].region <= mCurVariableRegion ) {

      if ( mVariableList[i].constant == "" ) {  // 不是array 
        if ( constantValue == -1 ) {
          if ( mVariableList[i].variableType == "string" OR mVariableList[i].variableType == "char" ) {
            mVariableList[i].variableValue[0] = newValueStr;
          } // if
          else if ( mVariableList[i].variableType == "int" ) {
            int tmp = newValue;
            mVariableList[i].variableValue[0] = FloatToStr( tmp );
          } // else if
          else {  // bool統一用0或1存 
            mVariableList[i].variableValue[0] = FloatToStr( newValue );
          } // else
        } // if
        else {
          throw "Not array error\n";
        } // else
      } // if
      else {  // 是array 
        if ( constantValue >= 0 ) {
          if ( mVariableList[i].variableType == "string" OR mVariableList[i].variableType == "char" ) {
            mVariableList[i].variableValue[constantValue] = newValueStr;
          } // if
          else if ( mVariableList[i].variableType == "int" ) {
            int tmp = newValue;
            mVariableList[i].variableValue[constantValue] = FloatToStr( tmp );
          } // else if
          else {
            mVariableList[i].variableValue[constantValue] = FloatToStr( newValue );
          } // else
        } // if
        else {
          throw ( "Runtime error : array '" + variableStr + "' index 'i' = " +
                  FloatToStr( constantValue ) + " out of range!\n" ).c_str()  ;
          // 待改 
        } // else
      } // else

      return;
    } // if
  } // for 
} // Parser::UpdateVariableValue()


void Parser::GetReferenceValue( VariableType & curV, ParameterType curP ) {

  if ( mCallStack.size() > 0 ) {  // 需要update的值是function裡的 

    FunctionType curF = mCallStack[curP.functionNum];
    
    if ( curP.listType == "parameter" ) {  // 是本來傳進這個function的parameter
      if ( curP.arrPosition == -1 ) {  // 是array或沒有宣告array的變數
        curV.variableValue = curF.parameter[curP.address].variableValue;
        // 整個vec複製回去 
      } // if
      else {  // 是array但只有取其中一個值 
        curV.variableValue[0] = curF.parameter[curP.address].variableValue[curP.arrPosition];
        // 只把那個值複製回去 
      } // else
    } // if
    else if ( curP.listType == "variableList_F" ) {  // 是function裡面的variable list
      if ( curP.arrPosition == -1 ) {  // 是array或沒有宣告array的變數 
        curV.variableValue = curF.variableList[curP.address].variableValue;
        // 整個vec複製回去 
      } // if
      else {  // 是array但只有取其中一個值 
        curV.variableValue[0] = curF.variableList[curP.address].variableValue[curP.arrPosition];
        // 只把那個值複製回去 
      } // else
    } // else if
    else {  // 原始的variable list 
      if ( curP.arrPosition == -1 ) {  // 是array或沒有宣告array的變數 
        curV.variableValue = mVariableList[curP.address].variableValue;
        // 整個vec複製回去 
      } // if
      else {  // 是array但只有取其中一個值 
        curV.variableValue[0] = mVariableList[curP.address].variableValue[curP.arrPosition];
        // 只把那個值複製回去 
      } // else
    } // else 
    
    mCallStack[curP.functionNum] = curF;
    
  } // if
  
  else {  // 需要update的值是原始的variable list 
    if ( curP.arrPosition == -1 ) {  // 是array或沒有宣告array的變數 
      curV.variableValue = mVariableList[curP.address].variableValue; 
      // 整個vec複製回去 
    } // if
    else {  // 是array但只有取其中一個值 
      curV.variableValue[0] = mVariableList[curP.address].variableValue[curP.arrPosition];
      // 只把那個值複製回去 
    } // else
  } // else
  
} // Parser::GetReferenceValue()

void Parser::UpdateReferenceValue( VariableType curV, ParameterType curP, double constantValue ) {

  
  if ( mCallStack.size() > 0 ) {  // 需要update的值是function裡的 

    FunctionType curF = mCallStack[curP.functionNum];

    if ( curP.listType == "parameter" ) {  // 是前一個function的parameter
      if ( curP.arrPosition == -1 ) {  // 是array或沒有宣告array的變數 
        if ( constantValue == -1 ) {
          curF.parameter[curP.address].variableValue[0] = curV.variableValue[0];
        } // if
        else {
          curF.parameter[curP.address].variableValue[constantValue] = curV.variableValue[constantValue];
        } // else
      } // if
      else {  // 是array但只有取其中一個值 
        curF.parameter[curP.address].variableValue[curP.arrPosition] = curV.variableValue[0];
        // 只把那個值複製回去 
      } // else
      
    } // if
    

    else if ( curP.listType == "variableList_F" ) {  // 是function裡面的variable list
      if ( curP.arrPosition == -1 ) {  // 是array或沒有宣告array的變數 
        if ( constantValue == -1 ) {
          curF.variableList[curP.address].variableValue[0] = curV.variableValue[0];
        } // if
        else {
          curF.variableList[curP.address].variableValue[constantValue] = curV.variableValue[constantValue];
        } // else
      } // if
      else {  // 是array但只有取其中一個值 
        curF.variableList[curP.address].variableValue[curP.arrPosition] = curV.variableValue[0];
        // 只把那個值複製回去 
      } // else
    } // else if

    else {  // 原始的variable list 
      if ( curP.arrPosition == -1 ) {  // 是array或沒有宣告array的變數 
        if ( constantValue == -1 ) {
          mVariableList[curP.address].variableValue[0] = curV.variableValue[0];
        } // if
        else {
          mVariableList[curP.address].variableValue[constantValue] = curV.variableValue[constantValue];
        } // else
      } // if
      else {  // 是array但只有取其中一個值 
        mVariableList[curP.address].variableValue[curP.arrPosition] = curV.variableValue[0];
        // 只把那個值複製回去 
      } // else
    } // else 
    
    mCallStack[curP.functionNum] = curF;
    
  } // if
  
  else {  // 需要update的值是原始的variable list 
    if ( curP.arrPosition == -1 ) {  // 是array或沒有宣告array的變數 
      if ( constantValue == -1 ) {
        mVariableList[curP.address].variableValue[0] = curV.variableValue[0];
      } // if
      else {
        mVariableList[curP.address].variableValue[constantValue] = curV.variableValue[constantValue];
      } // else
    } // if
    else {  // 是array但只有取其中一個值 
      mVariableList[curP.address].variableValue[curP.arrPosition] = curV.variableValue[0];
      // 只把那個值複製回去 
    } // else
  } // else
  
} // Parser::UpdateReferenceValue()



void Parser::GetVariableType( string & idType, string variableStr ) {
  
  if ( mExecuteFunction > 0 ) {
    
    for ( int i = 0 ; i < mCurFunction.parameter.size() ; i++ ) {
      // 先找參數
      if ( variableStr == mCurFunction.parameter[i].variableName ) {
        idType = mCurFunction.parameter[i].variableType;
        return;
      } // if
    } // for
    
    for ( int i = 0 ; i < mCurFunction.variableList.size() ; i++ ) {
      // 再找function裡宣告的變數 
      // 需要比較區域範圍, 因為有可能有if 
      if ( variableStr == mCurFunction.variableList[i].variableName AND
           mCurFunction.variableList[i].region <= mCurVariableRegion ) {
        idType = mCurFunction.variableList[i].variableType;
        return;
      } // if
    } // for
    
    
    for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
      // 找全域 
      if ( variableStr == mVariableList[i].variableName AND mVariableList[i].region == 0 ) {
        idType = mVariableList[i].variableType;
        return;
      } // if
    } // for

    return;
  } // if

  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( variableStr == mVariableList[i].variableName AND mVariableList[i].region <= mCurVariableRegion ) {
      idType = mVariableList[i].variableType;
      return;
    } // if
  } // for
} // Parser::GetVariableType()

void Parser::ClearRegionVariable() {
  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( mVariableList[i].region != 0 ) {
      mVariableList.erase( mVariableList.begin() + i );
    } // if
  } // for
} // Parser::ClearRegionVariable()

void Parser::GetFunctionStatements() {
  vector<Token> tempVector;
  mScanner.GetFunctionVec( tempVector );
  
  while ( tempVector[0].tokenStr != "{" ) {
    tempVector.erase( tempVector.begin() );
  } // while
  
  mCurFunction.statements = tempVector;
} // Parser::GetFunctionStatements()

void Parser::StoreFunction() {

  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    if ( mCurFunction.functionName == mFunctionList[i].functionName ) {
      mFunctionList[i] = mCurFunction ;
      printf( "New definition of %s() entered ...\n", mCurFunction.functionName.c_str() );
      return;
    } // if
  } // for
  
  // 如果沒有在idList裡找到的話
  // 就按照順序插入 

  printf( "Definition of %s() entered ...\n", mCurFunction.functionName.c_str() );
  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    if ( mCurFunction.functionName < mFunctionList[i].functionName ) {
      mFunctionList.insert( mFunctionList.begin() + i, mCurFunction );
      return;
    } // if
  } // for
  
  // 沒找到就放在最後面 
  mFunctionList.push_back( mCurFunction ) ;

} // end Parser::StoreFunction()

void Parser::SetExecuteFunction( string functionName, vector<ParameterType> & tmpValueList ) {

  if ( mExecuteFunction > 0 ) {  // 正在執行function, 裡面又要執行function
    mCallStack.push_back( mCurFunction );  // 把目前的function存起來 
  } // if
  
  mExecuteFunction++;
  
  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    if ( functionName == mFunctionList[i].functionName ) {
      // 如果同一個statement執行兩個以上function, 沒歸零會出錯 
      mCurFunction = mFunctionList[i] ;
      mCurFunction.pc = 0;  // 每次取新的要歸零
      mCurFunction.variableList.clear();  // variable用重新宣告的 
      mCurFunction.recordParameters = tmpValueList;
      
      for ( int k = 0 ; k < tmpValueList.size() ; k++ ) {
        mCurFunction.parameter[k].variableValue = tmpValueList[k].variableValue;
      } // for

      return;
    } // if
  } // for
  
  
} // Parser::SetExecuteFunction()

void Parser::RestoreExecuteFunction() {

  mExecuteFunction--;
  
  if ( mExecuteFunction > 0 ) {  // 要繼續執行剛剛沒執行完的function 
    mCurFunction = mCallStack[mCallStack.size() - 1];
    mCallStack.pop_back();  // 把剛剛的function pop回來 
  } // if
  
} // Parser::RestoreExecuteFunction()

void Parser::CheckIDExistOrNot( string idStr ) {

  if ( idStr == "Done" OR idStr == "ListAllVariables" OR idStr == "ListAllFunctions" OR
       idStr == "ListVariable" OR idStr == "ListFunction" ) {
    return;
  } // if


  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( idStr == mVariableList[i].variableName AND mVariableList[i].region <= mCurVariableRegion ) {
      return;
    } // if
  } // for
  
  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    if ( idStr == mFunctionList[i].functionName ) {
      return;
    } // if
  } // for
  
  if ( idStr == mCurFunction.functionName )
    return;
  
  if ( mIsFunction ) {
    for ( int i = 0 ; i < mCurFunction.parameter.size() ; i++ ) {
      if ( idStr == mCurFunction.parameter[i].variableName ) {
        return;
      } // if
    } // for
  } // if
  
  throw ( "undefined identifier '" + idStr + "'\n" ).c_str();
  
} // Parser::CheckIDExistOrNot()

bool Parser::IsFloat( string str ) {
  int position = str.find( "." );
  if ( position != str.npos )
    return true;

  return false;
} // end Parser::IsFloat()

string Parser::FloatToStr( double num ) {
  stringstream ss;
  string floatStr;
  ss << setprecision( 15 ) << num;
  ss >> floatStr ;
  return floatStr;
} // end Parser::FloatToStr()


void Parser::ListAllVariables() {
  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( mVariableList[i].region == 0 ) {
      printf( "%s\n", mVariableList[i].variableName.c_str() ) ;
    } // if
  } // for
} // Parser::ListAllVariables()

void Parser::ListAllFunctions() {
  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    printf( "%s()\n", mFunctionList[i].functionName.c_str() ) ;
  } // for
} // Parser::ListAllFunctions()

void Parser::ListVariable( string variableName ) {
  // 去掉雙引號
  string str = "";
  for ( int i = 1 ; i < variableName.size()-1 ; i++ ) {
    str = str + variableName[i];
  } // for

  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( str == mVariableList[i].variableName AND mVariableList[i].region == 0 ) {
      printf( "%s %s", mVariableList[i].variableType.c_str(),
                       mVariableList[i].variableName.c_str() ) ;
               
      if ( mVariableList[i].constant != "" )
        printf( "[ %s ]", mVariableList[i].constant.c_str() ) ;

      printf( " ;\n" ) ;
      return;
    } // if
  } // for

} // Parser::ListVariable()

void Parser::ListFunction( string functionName ) {
  // 去掉雙引號
  string str = "";
  for ( int i = 1 ; i < functionName.size()-1 ; i++ ) {
    str = str + functionName[i];
  } // for

  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    if ( str == mFunctionList[i].functionName ) {
      vector<Token> tmpStr = mFunctionList[i].statements;
      int position = 0, vecSize = tmpStr.size() ;
      
      int spaceNum = 0;
      
      printf( "%s %s( ", mFunctionList[i].functionType.c_str(),
                       mFunctionList[i].functionName.c_str() ) ;
      
      for ( int k = 0 ; k < mFunctionList[i].parameter.size() ; k++ ) {
        printf( "%s ", mFunctionList[i].parameter[k].variableType.c_str() ) ;

        if ( mFunctionList[i].parameter[k].isCallByReference )
          printf( "& " ) ;
        
        printf( "%s", mFunctionList[i].parameter[k].variableName.c_str() ) ;
        
        if ( mFunctionList[i].parameter[k].constant != "" )
          printf( "[ %s ]", mFunctionList[i].parameter[k].constant.c_str() ) ;
        
        if ( k != mFunctionList[i].parameter.size() - 1 )
          printf( ", " ) ;

      } // for
      
      printf( " ) " ) ;
      
      while ( position < vecSize ) {
        
        if ( tmpStr[position+1].tokenType == BRP ) {
          spaceNum = spaceNum - 2;
        } // if

        printf( "%s", tmpStr[position].tokenStr.c_str() ) ;
        if ( tmpStr[position].tokenType == BLP ) {
          spaceNum = spaceNum + 2;
          printf( "\n" );
          for ( int k = 0 ; k < spaceNum ; k++ ) {
            printf( " " );
          } // for
        } // if
        else if ( tmpStr[position].tokenType == BRP ) {
          printf( "\n" );
          for ( int k = 0 ; k < spaceNum ; k++ ) {
            printf( " " );
          } // for
        } // else if
        else if ( tmpStr[position].tokenType == SCLN ) {
          printf( "\n" );
          for ( int k = 0 ; k < spaceNum ; k++ ) {
            printf( " " );
          } // for
        } // else if
        else if ( NOT NoNeedSpace( tmpStr[position].tokenType, tmpStr[position+1].tokenType ) ) {
          printf( " " );
        } // else if

        position++;
      } // while

    } // if
  } // for
  
} // Parser::ListFunction()

bool Parser::NoNeedSpace( TokenType type1, TokenType type2 ) {

  if ( ( type1 == ID AND type2 == LP )  OR
       ( type1 == ID AND type2 == MLP ) OR
       ( type1 == ID AND type2 == COMMA ) OR
       ( type1 == ID AND ( type2 == PP OR type2 == MM ) ) OR
       ( type1 == LP AND type2 == RP ) ) {

    return true;
  } // if

  return false;

} // Parser::NoNeedSpace()


void Parser::SkipBufferWhiteSpace() {

  while ( !mInputBuffer.empty() AND
          ( mInputBuffer[0] == ' ' OR mInputBuffer[0] == '\t' OR
            mInputBuffer[0] == '\n' ) ) {
  
    mInputBuffer.erase( mInputBuffer.begin() );
  } // while
} // Parser::SkipBufferWhiteSpace()

void Parser::InputBufferAppend() {

  Token token;
  
  mScanner.PeekToken( mTokenStr, mTokenType );
  if ( mTokenType == END_OF_FILE ) {  // 代表應該要有下個token, 但沒有 
    throw "EOF exception";
  } // if
  
  if ( mTokenType == INPUTBUFFERAPPEND ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.ClearInputBuffer();
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    if ( mTokenType == END_OF_FILE ) {  // 代表應該要有下個token, 但沒有 
      throw "EOF exception";
    } // if
    
    while ( mTokenType != ENDINPUTBUFFERAPPEND ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      if ( mTokenType == END_OF_FILE ) {  // 代表應該要有下個token, 但沒有 
        throw "EOF exception";
      } // if
    } // while
    
    UpdateInputBuffer();
    
  } // if
} // Parser::InputBufferAppend()

void Parser::UpdateInputBuffer() {
  vector<char> temp;
  mScanner.GetInputBuffer( temp );
  
  temp.erase( temp.end()-21, temp.end() ) ;
  // 要去掉EndInputBufferAppend跟最後一個多讀的字元
  
  mInputBuffer.insert( mInputBuffer.end(), temp.begin(), temp.end() );
} // Parser::UpdateInputBuffer()  

void Parser::GetInputBufferItem( string idType, string & returnStr ) {
  
  int i = 0;
  returnStr = "";
  
  if ( idType == "bool" ) {
    
    SkipBufferWhiteSpace();
    
    if ( mInputBuffer.size() == 4 ) {
      if ( mInputBuffer[0] == 't' AND mInputBuffer[1] == 'r' AND
           mInputBuffer[2] == 'u' AND mInputBuffer[3] == 'e' ) {
        returnStr = "true";
        mInputBuffer.erase( mInputBuffer.begin(), mInputBuffer.begin() + 3 );
      } // if
    } // if
    else if ( mInputBuffer.size() > 4 ) {
      if ( mInputBuffer[0] == 'f' AND mInputBuffer[1] == 'a' AND
           mInputBuffer[2] == 'l' AND mInputBuffer[3] == 's' AND mInputBuffer[4] == 'e' ) {
        returnStr = "false";
        mInputBuffer.erase( mInputBuffer.begin(), mInputBuffer.begin() + 4 );
      } // if
    } // else if
    
  } // if
  else if ( idType == "string" ) {
    SkipBufferWhiteSpace();
    while ( !mInputBuffer.empty() AND
            ( mInputBuffer[0] != ' ' OR mInputBuffer[0] != '\t' OR
              mInputBuffer[0] != '\n' ) ) {

      returnStr = returnStr + mInputBuffer[0] ;
      mInputBuffer.erase( mInputBuffer.begin() );
    } // while
  } // else if
  else if ( idType == "char" ) {
    if ( !mInputBuffer.empty() ) {
      returnStr = returnStr + mInputBuffer[0];
      mInputBuffer.erase( mInputBuffer.begin() );
    } // if
  } // else if
  else if ( idType == "int" OR idType == "float" ) {
    SkipBufferWhiteSpace();
    bool ending = false, hasPoint = false ;
    for ( i = 0 ; NOT ending AND !mInputBuffer.empty() ; i++ ) {
      if ( mInputBuffer[i] >= '0' AND mInputBuffer[i] <= '9' ) {
        returnStr = returnStr + mInputBuffer[i] ;
      } // if
      else if ( !hasPoint AND mInputBuffer[i] == '.' ) {    // 還沒出現'.', 然後讀到了第一個'.' 
        returnStr = returnStr + mInputBuffer[i] ;
        hasPoint = true ;
      } // else if
      else if ( hasPoint AND mInputBuffer[i] == '.' ) {    // 已經有'.'了但又出現第二個'.' 
        ending = true ;
      } // else if    
      else { 
        ending = true ; 
      } // else
    } // for
    
    if ( returnStr != "" ) {
      mInputBuffer.erase( mInputBuffer.begin(), mInputBuffer.begin() + i-1 );
    } // if

  } // else if
  else {
    throw "Error";
  } // else
  
  if ( returnStr == "" ) {
    throw "Error";
  } // if
  
} // Parser::GetInputBufferItem()


void Parser::PeekInputVecToken( string & str, TokenType & type ) {
  
  if ( mExecuteFunction > 0 ) {
    if ( mCurFunction.pc < mCurFunction.statements.size() ) {
      str = mCurFunction.statements[mCurFunction.pc].tokenStr;
      type = mCurFunction.statements[mCurFunction.pc].tokenType;
    } // if
  } // if
  else {
    if ( mUserInputVecNum < mUserInputVec.size() ) {
      str = mUserInputVec[mUserInputVecNum].tokenStr;
      type = mUserInputVec[mUserInputVecNum].tokenType;
    } // if
  } // else

} // Parser::PeekInputVecToken()

void Parser::GetInputVecToken( string & str, TokenType & type ) {

  if ( mExecuteFunction > 0 ) {
    if ( mCurFunction.pc < mCurFunction.statements.size() ) {
      str = mCurFunction.statements[mCurFunction.pc].tokenStr;
      type = mCurFunction.statements[mCurFunction.pc].tokenType;
      mCurFunction.pc++;
    } // if
  } // if
  else {
    if ( mUserInputVecNum < mUserInputVec.size() ) {
      str = mUserInputVec[mUserInputVecNum].tokenStr;
      type = mUserInputVec[mUserInputVecNum].tokenType;
      mUserInputVecNum++;  // 不能刪除token, 因為有while 
    } // if
  } // else
  
} // Parser::GetInputVecToken()

void Parser::User_Input() {
// user_input : ( definition | statement ) { definition | statement }

  vector<Token> tempVector;
  
  string curType = "None";
  double returnValue = 0;
  string returnStr = "";
  bool isReturn = false;
  
  // 抓第一個token 
  mScanner.PeekToken( mTokenStr, mTokenType );
  if ( mTokenType == END_OF_FILE ) {  // 代表應該要有下個token, 但沒有 
    throw "EOF exception";
  } // if    
  
  // 檢查是否是definition的頭 
  if ( mTokenType == VOID OR mTokenType == INT OR mTokenType == CHAR OR
       mTokenType == FLOAT OR mTokenType == STRING OR mTokenType == BOOL ) {
    Definition();
  } // if
  
  // 檢查是否是statement的頭 
  else if ( mTokenType == SCLN OR mTokenType == ID OR mTokenType == PP OR
            mTokenType == MM OR mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR
            mTokenType == BITNOT OR mTokenType == CONSTANT OR mTokenType == LP OR
            mTokenType == RETURN OR mTokenType == IF OR mTokenType == WHILE OR
            mTokenType == DO OR mTokenType == BLP OR mTokenType == CIN OR mTokenType == COUT ) {
    Statement();
    
    mScanner.GetFunctionVec( mUserInputVec );
    
    tempVector = mUserInputVec;
    mCurVariableRegion = 0;
    Statement_v( curType, returnValue, returnStr, isReturn );
    ClearRegionVariable();

    if ( tempVector.size() > 0 )  {
      if ( tempVector[0].tokenStr == "ListAllVariables" ) {
        ListAllVariables();
      } // if
      else if ( tempVector[0].tokenStr == "ListAllFunctions" ) {
        ListAllFunctions();
      } // else if
      else if ( tempVector[0].tokenStr == "ListVariable" ) {
        ListVariable( tempVector[2].tokenStr );
      } // else if
      else if ( tempVector[0].tokenStr == "ListFunction" ) {
        ListFunction( tempVector[2].tokenStr );
      } // else if 
      else if ( tempVector[0].tokenStr == "Done" ) {
        throw "Done";
      } // else if
    } // if

    printf( "Statement executed ...\n" );
  } // else if
  
  else if ( mTokenType == INPUTBUFFERAPPEND ) {
    InputBufferAppend();
    printf( "Input-buffer appended ...\n" );
  } // else if

  // 其他情況 : unexpected error 
  else {
    throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else

} // end Parser::User_Input()

void Parser::Definition() {
// 這裡的ID是宣告的ID, 不用檢查是否存在 
// definition : VOID Identifier function_definition_without_ID |
//              type_specifier Identifier function_definition_or_declarators

  string tmpIdStr = "";

  string tmpVariableType = "";
  string tmpVariableName = "";
   
  mScanner.PeekToken( mTokenStr, mTokenType );
  if ( mTokenType == END_OF_FILE ) {  // 代表應該要有下個token, 但沒有 
    throw "EOF exception";
  } // if
  
  if ( mTokenType == VOID ) {  // 一定是宣告function 
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mCurFunction.functionType = mTokenStr;
    mScanner.PeekToken( mTokenStr, mTokenType );
    if ( mTokenType == END_OF_FILE ) {  // 代表應該要有下個token, 但沒有 
      throw "EOF exception";
    } // if
    
    if ( mTokenType == ID ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mCurFunction.functionName = mTokenStr;
      mIsFunction = true;
      Function_Definition_Without_ID();

      GetFunctionStatements();
      ClearRegionVariable();
      StoreFunction();
    } // if
    else {
      throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
    } // else
    
  } // if
  
  else if ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
            mTokenType == STRING OR mTokenType == BOOL ) {  // 可能是宣告function or 變數 
    Type_Specifier();
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    tmpVariableType = mTokenStr;  // 宣告的type (int, char, ...) 
    mScanner.PeekToken( mTokenStr, mTokenType );
    if ( mTokenType == END_OF_FILE ) {  // 代表應該要有下個token, 但沒有 
      throw "EOF exception";
    } // if
    
    if ( mTokenType == ID ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      tmpVariableName = mTokenStr;
      
      Function_Definition_Or_Declarators( tmpVariableType, tmpVariableName );

      if ( mIsFunction ) {
        GetFunctionStatements();
        ClearRegionVariable();
        StoreFunction();
      } // if
    } // if
    else {
      throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
    } // else
  } // else if
  else {
    throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else

} // end Parser::Definition()

void Parser::Type_Specifier() {
// type_specifier : INT | CHAR | FLOAT | STRING | BOOL
  mScanner.PeekToken( mTokenStr, mTokenType );
  if ( mTokenType == END_OF_FILE )  // 代表應該要有下個token, 但沒有 
    throw "EOF exception";

  if ( mTokenType != INT AND mTokenType != CHAR AND mTokenType != FLOAT AND
       mTokenType != STRING AND mTokenType != BOOL ) {
    throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // if

} // end Parser::Type_Specifier()

void Parser::Function_Definition_Or_Declarators( string tmpType, string tmpName ) {
// function_definition_or_declarators : 
// function_definition_without_ID | rest_of_declarators

  mScanner.PeekToken( mTokenStr, mTokenType );
  if ( mTokenType == END_OF_FILE ) {  // 代表應該要有下個token, 但沒有 
    throw "EOF exception";
  } // if

  if ( mTokenType == LP ) {
    mCurFunction.functionType = tmpType;
    mCurFunction.functionName = tmpName;
    mIsFunction = true;
    Function_Definition_Without_ID();
  } // if
  else if ( mTokenType == MLP OR mTokenType == COMMA OR mTokenType == SCLN ) {
    mIsFunction = false;
    Rest_Of_Declarators( tmpType, tmpName );
  } // else if
  else {
    throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else 
} // end Parser::Function_Definition_Or_Declarators()

void Parser::Rest_Of_Declarators( string tmpVariableType, string tmpVariableName ) {
// 這裡的ID是宣告的ID, 不用檢查是否存在 
// rest_of_declarators : [ '[' Constant ']' ] 
//                       { ',' Identifier [ '[' Constant ']' ] } ';'
// 不管怎樣都至少有個';'

  VariableType tmpID;
  
  // 把剛剛得到的型別和ID名都先assign   
  tmpID.variableName = tmpVariableName;
  tmpID.variableType = tmpVariableType;
  tmpID.constant = "";
  tmpID.region = mCurVariableRegion; 
  
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 代表應該要有下個token, 但沒有 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";

  // [ '[' Constant ']' ], 不一定要有 
  if ( mTokenType == MLP ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == CONSTANT ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      tmpID.constant = mTokenStr; // 檢查到是array
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
      if ( mTokenType == MRP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        mScanner.PeekToken( mTokenStr, mTokenType ) ; // 讀下一個token, 看是','還是';'
        // 因為一定要有';', 所以要檢查是否EOF 
        // 代表應該要有下個token, 但沒有 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";
        
      } // if
      else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // if
  
  mTempIDList.push_back( tmpID ) ;  // 把宣告的ID存進去 
  
  // { ',' Identifier [ '[' Constant ']' ] }
  // 不一定要有 
  while ( mTokenType == COMMA ) {

    tmpID.variableType = "";
    tmpID.variableName = "";
    tmpID.constant = "";
    tmpID.region = mCurVariableRegion; 

    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType ) ;
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    
    if ( mTokenType == ID ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;

      tmpID.variableName = mTokenStr;
      tmpID.variableType = tmpVariableType;

      mScanner.PeekToken( mTokenStr, mTokenType ) ; // 讀下個token, 看是'['還是';', 也可以是',' 
      // 因為一定要有';', 所以要檢查是否EOF 
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
      
      // [ '[' Constant ']' ], 不一定要有 
      if ( mTokenType == MLP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        mScanner.PeekToken( mTokenStr, mTokenType ) ;
        // 代表應該要有下個token, 但沒有 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";
        
        if ( mTokenType == CONSTANT ) {
          mScanner.GetToken( mTokenStr, mTokenType ) ;
          tmpID.constant = mTokenStr;
          mScanner.PeekToken( mTokenStr, mTokenType ) ;
          // 代表應該要有下個token, 但沒有 
          if ( mTokenType == END_OF_FILE )  throw "EOF exception";
          if ( mTokenType == MRP ) {
            mScanner.GetToken( mTokenStr, mTokenType ) ;
            mScanner.PeekToken( mTokenStr, mTokenType ) ; // 讀下一個token, 看是','還是';'
            // 因為一定要有';', 所以要檢查是否EOF 
            // 代表應該要有下個token, 但沒有 
            if ( mTokenType == END_OF_FILE )  throw "EOF exception";
            
            // 如果是','就繼續跑迴圈
            // 是';'就出迴圈 
          } // if
          else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
          
        } // if
        else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
        
      } // if
      
      mTempIDList.push_back( tmpID ) ;  // 把宣告的ID存進去 
      // 如果不是'[', 可能是','或';' 
      // 如果是','就繼續跑迴圈
      // 是';'就出迴圈 
      
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
    
  } // while
  
  // 最後一個token是';' , 一定要有 
  if ( mTokenType == SCLN ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
  } // if
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();

  for ( int i = 0 ; i < mTempIDList.size() ; i++ ) {
    StoreVariable( mTempIDList[i].variableType, mTempIDList[i].variableName, mTempIDList[i].constant );
  } // for

  mTempIDList.clear();  // 存過的就清掉 

} // end Parser::Rest_Of_Declarators()

void Parser::Function_Definition_Without_ID() {
// function_definition_without_ID : 
//  '(' [ VOID | formal_parameter_list ] ')' compound_statement

  mScanner.PeekToken( mTokenStr, mTokenType );
  // 代表應該要有下個token, 但沒有 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  
  // '(' [ VOID | formal_parameter_list ] ')' compound_statement
  if ( mTokenType == LP ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 可能是 [ VOID | Type_Specifier ]
    // 或是')' >> 一定要有 
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == VOID ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
      // ')' >> 一定要有 
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    } // if
    else if ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
              mTokenType == STRING OR mTokenType == BOOL ) {
      Formal_Parameter_List();
      mScanner.PeekToken( mTokenStr, mTokenType );
      // ')' >> 一定要有 
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    } // else if
 
    if ( mTokenType == RP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      Compound_Statement(); 
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
    // 有 [ VOID | Type_Specifier ] 沒有 ')' 
    // 或沒有 [ VOID | Type_Specifier ] 也沒有 ')' 
 
  } // if 
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();

} // end Parser::Function_Definition_Without_ID()

void Parser::Formal_Parameter_List() {
// 這裡的ID是宣告的ID, 不用檢查是否存在 
// formal_parameter_list : type_specifier [ '&' ] Identifier [ '[' Constant ']' ] 
//  { ',' type_specifier [ '&' ] Identifier [ '[' Constant ']' ] }

  VariableType tmpID;
  tmpID.constant = "";
  tmpID.isCallByReference = false;
  tmpID.region = 0;
  tmpID.variableName = "";
  tmpID.variableType = "";
  tmpID.variableValue.clear();
  
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 代表應該要有下個token, 但沒有 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  if ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
       mTokenType == STRING OR mTokenType == BOOL ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    tmpID.variableType = mTokenStr;
    mScanner.PeekToken( mTokenStr, mTokenType );  // 可能是'&'或ID 
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";

    // [ '&' ] 不一定要有 
    if ( mTokenType == BITAND ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      tmpID.isCallByReference = true;
      mScanner.PeekToken( mTokenStr, mTokenType ); // 下一個是ID 
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    } // if  
 
    if ( mTokenType == ID ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      tmpID.variableName = mTokenStr;
      mScanner.PeekToken( mTokenStr, mTokenType ); // 下一個是'['或',' , 或沒有也可以 
     
      // [ '[' Constant ']' ]
      if ( mTokenType == MLP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        mScanner.PeekToken( mTokenStr, mTokenType ); // 下一個是CONSTANT
        // 代表應該要有下個token, 但沒有 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";
        
        if ( mTokenType == CONSTANT ) {
          mScanner.GetToken( mTokenStr, mTokenType ) ;
          tmpID.constant = mTokenStr;
          mScanner.PeekToken( mTokenStr, mTokenType ); // 下一個是']'
          // 代表應該要有下個token, 但沒有 
          if ( mTokenType == END_OF_FILE )  throw "EOF exception";
          if ( mTokenType == MRP ) {
            mScanner.GetToken( mTokenStr, mTokenType ) ;
            mScanner.PeekToken( mTokenStr, mTokenType ) ; // 下一個是',' , 或沒有也可以 
          } // if
          else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
        } // if
        else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
      } // if
      
      mTempIDList.push_back( tmpID ) ;  // 把宣告的ID存進去 
      
      while ( mTokenType == COMMA ) {
        tmpID.constant = "";
        tmpID.isCallByReference = false;
        tmpID.region = 0;
        tmpID.variableName = "";
        tmpID.variableType = "";
        tmpID.variableValue.clear();

        mScanner.GetToken( mTokenStr, mTokenType ) ;
        mScanner.PeekToken( mTokenStr, mTokenType ); // 下一個是type_specifier
        // 代表應該要有下個token, 但沒有 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";
        if ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
             mTokenType == STRING OR mTokenType == BOOL ) {
          mScanner.GetToken( mTokenStr, mTokenType ) ;
          tmpID.variableType = mTokenStr;
          mScanner.PeekToken( mTokenStr, mTokenType );  // 可能是'&'或ID 
          // 代表應該要有下個token, 但沒有 
          if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    
          // [ '&' ] 不一定要有 
          if ( mTokenType == BITAND ) {
            mScanner.GetToken( mTokenStr, mTokenType ) ;
            tmpID.isCallByReference = true;
            mScanner.PeekToken( mTokenStr, mTokenType ); // 下一個是ID 
            // 代表應該要有下個token, 但沒有 
            if ( mTokenType == END_OF_FILE )  throw "EOF exception";
          } // if  
 
          if ( mTokenType == ID ) {
            mScanner.GetToken( mTokenStr, mTokenType ) ;
            tmpID.variableName = mTokenStr;
            mScanner.PeekToken( mTokenStr, mTokenType ); // 下一個是'['或',' , 或沒有也可以 
     
            // [ '[' Constant ']' ]
            if ( mTokenType == MLP ) {
              mScanner.GetToken( mTokenStr, mTokenType ) ;
              mScanner.PeekToken( mTokenStr, mTokenType ); // 下一個是CONSTANT
              // 代表應該要有下個token, 但沒有 
              if ( mTokenType == END_OF_FILE )  throw "EOF exception";
        
              if ( mTokenType == CONSTANT ) {
                mScanner.GetToken( mTokenStr, mTokenType ) ;
                tmpID.constant = mTokenStr;
                mScanner.PeekToken( mTokenStr, mTokenType ); // 下一個是']'
                // 代表應該要有下個token, 但沒有 
                if ( mTokenType == END_OF_FILE )  throw "EOF exception";
                if ( mTokenType == MRP ) {
                  mScanner.GetToken( mTokenStr, mTokenType ) ;
                  mScanner.PeekToken( mTokenStr, mTokenType ) ; // 下一個是',' , 或沒有也可以 
                } // if
                else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
              } // if
              else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
            } // if
            
            mTempIDList.push_back( tmpID ) ;  // 把宣告的ID存進去 
          } // if
          else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
        } // if
        else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
      } // while
    } // if  
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
   
  } // if
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
  
  for ( int i = 0 ; i < mTempIDList.size() ; i++ ) {
    if ( mTempIDList[i].constant != "" ) {
      int size = atoi( mTempIDList[i].constant.c_str() );
      mTempIDList[i].variableValue.clear(); // 可能是重新宣告 
      mTempIDList[i].variableValue.resize( size, "0" );
    } // if
    else {
      mTempIDList[i].variableValue.clear(); // 可能是重新宣告 
      mTempIDList[i].variableValue.resize( 1, "0" );
    } // else
  } // for
  
  mCurFunction.parameter.assign( mTempIDList.begin(), mTempIDList.end() );
  mTempIDList.clear();  // 存過的就清掉 
  
} // end Parser::Formal_Parameter_List()

void Parser::Compound_Statement() {
// compound_statement : '{' { declaration | statement } '}'
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 代表應該要有下個token, 但沒有 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  if ( mTokenType == BLP ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mCurVariableRegion++;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    while ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
            mTokenType == STRING OR mTokenType == BOOL OR mTokenType == SCLN OR
            mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
            mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
            mTokenType == CONSTANT OR mTokenType == LP OR mTokenType == RETURN OR
            mTokenType == BLP OR mTokenType == IF OR mTokenType == WHILE OR mTokenType == DO OR
            mTokenType == CIN OR mTokenType == COUT ) {

      if ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
           mTokenType == STRING OR mTokenType == BOOL ) {
        Declaration();
      } // if
      else if ( mTokenType == SCLN OR mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
                mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
                mTokenType == CONSTANT OR mTokenType == LP OR mTokenType == RETURN OR
                mTokenType == BLP OR mTokenType == IF OR mTokenType == WHILE OR mTokenType == DO OR
                mTokenType == CIN OR mTokenType == COUT ) {
        Statement();
      } // else if
      else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
   
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    } // while
   
    if ( mTokenType == BRP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mCurVariableRegion--;
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // if
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 

} // end Parser::Compound_Statement()

void Parser::Declaration() {
// 這裡的ID是宣告的ID, 不用檢查是否存在 
// declaration : type_specifier Identifier rest_of_declarators

  VariableType tmpID;
  string tmpVariableType = "";
  string tmpVariableName = "";
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 代表應該要有下個token, 但沒有 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  if ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
       mTokenType == STRING OR mTokenType == BOOL ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    tmpVariableType = mTokenStr;  // 存宣告型別 
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == ID ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      tmpVariableName = mTokenStr;
      
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
      if ( mTokenType == MLP OR mTokenType == COMMA OR mTokenType == SCLN ) {
        Rest_Of_Declarators( tmpVariableType, tmpVariableName );
      } // if
      else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // if
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();

} // end Parser::Declaration()

void Parser::Statement() {
// statement : ';'     // the null statement
//  | expression ';'  // expression here should not be empty 
//  | RETURN [ expression ] ';'
//  | compound_statement
//  | IF '(' expression ')' statement [ ELSE statement ]
//  | WHILE '(' expression ')' statement
//  | DO statement WHILE '(' expression ')' ';'

  mScanner.PeekToken( mTokenStr, mTokenType );
  // 代表應該要有下個token, 但沒有 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  if ( mTokenType == SCLN ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
  } // if
  else if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
            mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
            mTokenType == CONSTANT OR mTokenType == LP OR mTokenType == CIN OR mTokenType == COUT ) {
    Expression();
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == SCLN ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else if
  else if ( mTokenType == RETURN ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
         mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
         mTokenType == CONSTANT OR mTokenType == LP ) {
      Expression();
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    } // if
    
    if ( mTokenType == SCLN ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else if
  else if ( mTokenType == BLP ) {
    Compound_Statement();
  } // else if
  else if ( mTokenType == IF ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == LP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      Expression();
     
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
      if ( mTokenType == RP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        Statement();
       
        mScanner.PeekToken( mTokenStr, mTokenType ); // 可以有ELSE, 也可以不要有 
        if ( mTokenType == ELSE ) {
          mScanner.GetToken( mTokenStr, mTokenType ) ;
          Statement() ;
        } // if
      } // if
      else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else if
  else if ( mTokenType == WHILE ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == LP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      Expression();
     
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
      if ( mTokenType == RP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        Statement() ;
      } // if
      else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else if
  else if ( mTokenType == DO ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Statement() ;
   
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == WHILE ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
      if ( mTokenType == LP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        Expression();
        mScanner.PeekToken( mTokenStr, mTokenType );
        // 代表應該要有下個token, 但沒有 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";
        if ( mTokenType == RP ) {
          mScanner.GetToken( mTokenStr, mTokenType ) ;
          mScanner.PeekToken( mTokenStr, mTokenType );
          // 代表應該要有下個token, 但沒有 
          if ( mTokenType == END_OF_FILE )  throw "EOF exception";
          if ( mTokenType == SCLN ) {
            mScanner.GetToken( mTokenStr, mTokenType ) ;
          } // if
          else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
        } // if
        else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
      } // if
      else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();

  } // else if
  
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();

} // end Parser::Statement()

void Parser::Cin_expression() {
// cin_expression : cin { RS ID }

  mScanner.PeekToken( mTokenStr, mTokenType );
  // 代表應該要有下個token, 但沒有 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  
  if ( mTokenType == CIN ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    
    while ( mTokenType == RS ) {  // '>>'
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
      if ( mTokenType == ID ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        mScanner.PeekToken( mTokenStr, mTokenType );
        // [ '[' expression ']' ] 
        if ( mTokenType == MLP ) {
          mScanner.GetToken( mTokenStr, mTokenType ) ;
          Expression();
          mScanner.PeekToken( mTokenStr, mTokenType );
          // 代表應該要有下個token, 但沒有
          if ( mTokenType == END_OF_FILE )  throw "EOF exception";
          if ( mTokenType == MRP ) {
            mScanner.GetToken( mTokenStr, mTokenType );
            mScanner.PeekToken( mTokenStr, mTokenType );
            // 不一定要有下個token 
          } // if
          else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
        } // if
      } // if
      else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
    } // while
    
  } // if
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
} // Parser::Cin_expression()

void Parser::Cout_expression() {
// cout_expression : cout { LS expression }

  mScanner.PeekToken( mTokenStr, mTokenType );
  // 代表應該要有下個token, 但沒有 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  
  if ( mTokenType == COUT ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    
    while ( mTokenType == LS ) {  // '<<'
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      Expression();
      mScanner.PeekToken( mTokenStr, mTokenType );
    } // while
    
  } // if
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
} // Parser::Cout_expression()

void Parser::Expression() {
// expression : basic_expression { ',' basic_expression }

  Basic_Expression();
  mScanner.PeekToken( mTokenStr, mTokenType ); // 不一定要有下一個 
  
  while ( mTokenType == COMMA ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Basic_Expression();
    mScanner.PeekToken( mTokenStr, mTokenType );  // 不一定要有下一個 
  } // while

} // end Parser::Expression()

void Parser::Basic_Expression() {
// basic_expression : Identifier rest_of_Identifier_started_basic_exp
//  | ( PP | MM ) Identifier rest_of_PPMM_Identifier_started_basic_exp
//  | sign { sign } signed_unary_exp romce_and_romloe
//  | ( Constant | '(' expression ')' ) romce_and_romloe
//  | cin_exp | cout_exp

  bool isFunction = false;
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 代表應該要有下個token, 但沒有 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  if ( mTokenType == ID ) {
    CheckIDExistOrNot( mTokenStr ) ;
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Rest_Of_ID_Started_Basic_Exp();
  } // if
  else if ( mTokenType == PP OR mTokenType == MM ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == ID ) {
      CheckIDExistOrNot( mTokenStr ) ;
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      Rest_Of_PPMM_ID_Started_Basic_Exp();
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else if
  else if ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    while ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    } // while
    
    Signed_Unary_Exp();
    Romce_And_Romloe();
  } // else if
  else if ( mTokenType == CONSTANT ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Romce_And_Romloe();
  } // else if 
  else if ( mTokenType == LP ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Expression();
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == RP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      Romce_And_Romloe();
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else if
  else if ( mTokenType == CIN ) {
    Cin_expression();
  } // else if
  else if ( mTokenType == COUT ) {
    Cout_expression();
  } // else if
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();

} // end Parser::Basic_Expression()

void Parser::Rest_Of_ID_Started_Basic_Exp() {
// rest_of_Identifier_started_basic_exp : [ '[' expression ']' ]
//  ( assignment_operator basic_expression | [ PP | MM ] romce_and_romloe )
//  | '(' [ actual_parameter_list ] ')' romce_and_romloe
  
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為可以為空, 所以不需檢查EOF

  // 這個是variable
  if ( mTokenType == MLP OR mTokenType == ASSIGN OR mTokenType == TE OR
       mTokenType == DE OR mTokenType == RE OR mTokenType == PE OR 
       mTokenType == ME OR mTokenType == PP OR mTokenType == MM OR 
       mTokenType == MULT OR mTokenType == DIV OR mTokenType == REM OR
       mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == LS OR
       mTokenType == RS OR mTokenType == LSS OR mTokenType == GTR OR
       mTokenType == LE OR mTokenType == GE OR mTokenType == EQ OR
       mTokenType == NEQ OR mTokenType == BITAND OR mTokenType == BITXOR OR
       mTokenType == BITOR OR mTokenType == LOGAND OR mTokenType == LOGOR OR
       mTokenType == QUE ) { 
    
    // [ '[' expression ']' ] 
    if ( mTokenType == MLP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      Expression();
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 代表應該要有下個token, 但沒有
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
      if ( mTokenType == MRP ) {
        mScanner.GetToken( mTokenStr, mTokenType );
        mScanner.PeekToken( mTokenStr, mTokenType );
        // 不一定要有下個token 
      } // if
      else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
    } // if

    // assignment_operator basic_expression
    if ( mTokenType == ASSIGN OR mTokenType == TE OR mTokenType == DE OR
         mTokenType == RE OR mTokenType == PE OR mTokenType == ME ) {
      Assignment_Operator();
      mScanner.GetToken( mTokenStr, mTokenType );
      Basic_Expression();
    } // if
    
    // [ PP | MM ] romce_and_romloe
    // 可以為空 
    else if ( mTokenType == PP OR mTokenType == MM OR mTokenType == MULT OR
              mTokenType == DIV OR mTokenType == REM OR mTokenType == ADDORSIGN OR
              mTokenType == SUBORSIGN OR mTokenType == LS OR mTokenType == RS OR
              mTokenType == LSS OR mTokenType == GTR OR mTokenType == LE OR 
              mTokenType == GE OR mTokenType == EQ OR mTokenType == NEQ OR
              mTokenType == BITAND OR mTokenType == BITXOR OR mTokenType == BITOR OR
              mTokenType == LOGAND OR mTokenType == LOGOR OR mTokenType == QUE ) { 
    
      if ( mTokenType == PP OR mTokenType == MM ) {
        mScanner.GetToken( mTokenStr, mTokenType );
        mScanner.PeekToken( mTokenStr, mTokenType );
        // 不一定要有下個token 
      } // if
    
      Romce_And_Romloe(); 

    } // else if
  } // if
  
  // '(' [ actual_parameter_list ] ')' romce_and_romloe
  else if ( mTokenType == LP ) {   // 這個是function 
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";  // 因為至少要有')' 
    
    // [ actual_parameter_list ] 不一定要有 
    if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
         mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR 
         mTokenType == CONSTANT ) {

      Actual_Parameter_List();
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 代表應該要有下個token, 但沒有
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";  // 因為至少要有')' 
    } // if 
 
    if ( mTokenType == RP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      // 不一定要有下個token 
      Romce_And_Romloe(); 
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else if

} // end Parser::Rest_Of_ID_Started_Basic_Exp()

void Parser::Rest_Of_PPMM_ID_Started_Basic_Exp() {
// rest_of_PPMM_Identifier_started_basic_exp : [ '[' expression ']' ] romce_and_romloe 

  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為可以為空, 所以不需檢查EOF
  
  // [ '[' expression ']' ] 
  if ( mTokenType == MLP ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Expression();
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == MRP ) {
      mScanner.GetToken( mTokenStr, mTokenType );
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 不一定要有下個token 
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // if
  
  Romce_And_Romloe(); 

} // end Parser::Rest_Of_PPMM_ID_Started_Basic_Exp()

void Parser::Sign() {
// sign : '+' | '-' | '!'
  mScanner.PeekToken( mTokenStr, mTokenType );
  if ( mTokenType == END_OF_FILE )  // 代表應該要有下個token, 但沒有 
    throw "EOF exception";

  if ( mTokenType != ADDORSIGN AND mTokenType != SUBORSIGN AND mTokenType != BITNOT ) {
    throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // if
} // end Parser::Sign()

void Parser::Actual_Parameter_List() {
// actual_parameter_list : basic_expression { ',' basic_expression }

  Basic_Expression();
  
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有',', 所以不需檢查EOF
  while ( mTokenType == COMMA ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Basic_Expression();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while
  
} // end Parser::Actual_Parameter_List()

void Parser::Assignment_Operator() {
// assignment_operator : '=' | TE | DE | RE | PE | ME
  mScanner.PeekToken( mTokenStr, mTokenType );
  if ( mTokenType == END_OF_FILE )  // 代表應該要有下個token, 但沒有 
    throw "EOF exception";

  if ( mTokenType != ASSIGN AND mTokenType != TE AND mTokenType != DE AND
       mTokenType != RE AND mTokenType != PE AND mTokenType != ME ) {
    throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // if

} // end Parser::Assignment_Operator()

void Parser::Romce_And_Romloe() {
// rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp // 即romce_and_romloe
// : rest_of_maybe_logical_OR_exp [ '?' basic_expression ':' basic_expression ]

  Rest_Of_Maybe_Logical_Or_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有'?', 所以不需檢查EOF
  
  if ( mTokenType == QUE ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Basic_Expression() ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == COLON ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      Basic_Expression() ;
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // if
  
} // end Parser::Romce_And_Romloe()

void Parser::Rest_Of_Maybe_Logical_Or_Exp() {
// rest_of_maybe_logical_OR_exp : rest_of_maybe_logical_AND_exp { OR maybe_logical_AND_exp }
  Rest_Of_Maybe_Logical_And_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 OR , 所以不需檢查EOF
  
  while ( mTokenType == LOGOR ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Logical_And_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while
  
} // end Parser::Rest_Of_Maybe_Logical_Or_Exp()

void Parser::Maybe_Logical_And_Exp() {
// maybe_logical_AND_exp : maybe_bit_OR_exp { AND maybe_bit_OR_exp }
  
  Maybe_Bit_Or_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 AND , 所以不需檢查EOF
  
  while ( mTokenType == LOGAND ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Bit_Or_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Logical_And_Exp()

void Parser::Rest_Of_Maybe_Logical_And_Exp() {
// rest_of_maybe_logical_AND_exp : rest_of_maybe_bit_OR_exp { AND maybe_bit_OR_exp }
  
  Rest_Of_Maybe_Bit_Or_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 AND , 所以不需檢查EOF
  
  while ( mTokenType == LOGAND ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Bit_Or_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Logical_And_Exp()

void Parser::Maybe_Bit_Or_Exp() {
// maybe_bit_OR_exp : maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }

  Maybe_Bit_Ex_Or_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 '|' , 所以不需檢查EOF
  
  while ( mTokenType == BITOR ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Bit_Ex_Or_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Bit_Or_Exp()

void Parser::Rest_Of_Maybe_Bit_Or_Exp() {
// rest_of_maybe_bit_OR_exp : rest_of_maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }

  Rest_Of_Maybe_Bit_Ex_Or_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 '|' , 所以不需檢查EOF
  
  while ( mTokenType == BITOR ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Bit_Ex_Or_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Bit_Or_Exp()

void Parser::Maybe_Bit_Ex_Or_Exp() {
// maybe_bit_ex_OR_exp : maybe_bit_AND_exp { '^' maybe_bit_AND_exp }

  Maybe_Bit_And_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 '^' , 所以不需檢查EOF
  
  while ( mTokenType == BITXOR ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Bit_And_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Bit_Ex_Or_Exp()

void Parser::Rest_Of_Maybe_Bit_Ex_Or_Exp() {
// rest_of_maybe_bit_ex_OR_exp : rest_of_maybe_bit_AND_exp { '^' maybe_bit_AND_exp }

  Rest_Of_Maybe_Bit_And_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 '^' , 所以不需檢查EOF
  
  while ( mTokenType == BITXOR ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Bit_And_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Bit_Ex_Or_Exp()

void Parser::Maybe_Bit_And_Exp() {
// maybe_bit_AND_exp : maybe_equality_exp { '&' maybe_equality_exp }

  Maybe_Equality_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 '&' , 所以不需檢查EOF
  
  while ( mTokenType == BITAND ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Equality_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Bit_And_Exp()

void Parser::Rest_Of_Maybe_Bit_And_Exp() {
// rest_of_maybe_bit_AND_exp : rest_of_maybe_equality_exp { '&' maybe_equality_exp }

  Rest_Of_Maybe_Equality_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 '&' , 所以不需檢查EOF
  
  while ( mTokenType == BITAND ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Equality_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Bit_And_Exp()

void Parser::Maybe_Equality_Exp() {
// maybe_equality_exp : maybe_relational_exp { ( EQ | NEQ ) maybe_relational_exp}

  Maybe_Relational_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 EQ or NEQ , 所以不需檢查EOF
  
  while ( mTokenType == EQ OR mTokenType == NEQ ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Relational_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Equality_Exp()

void Parser::Rest_Of_Maybe_Equality_Exp() {
// rest_of_maybe_equality_exp : rest_of_maybe_relational_exp { ( EQ | NEQ ) maybe_relational_exp }

  Rest_Of_Maybe_Relational_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 EQ or NEQ , 所以不需檢查EOF
  
  while ( mTokenType == EQ OR mTokenType == NEQ ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Relational_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Equality_Exp()

void Parser::Maybe_Relational_Exp() {
// maybe_relational_exp : maybe_shift_exp { ( '<' | '>' | LE | GE ) maybe_shift_exp }

  Maybe_Shift_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 '<' | '>' | LE | GE , 所以不需檢查EOF
  
  while ( mTokenType == LSS OR mTokenType == GTR OR
          mTokenType == LE OR mTokenType == GE ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Shift_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Relational_Exp()

void Parser::Rest_Of_Maybe_Relational_Exp() {
// rest_of_maybe_relational_exp : rest_of_maybe_shift_exp { ( '<' | '>' | LE | GE ) maybe_shift_exp }

  Rest_Of_Maybe_Shift_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 '<' | '>' | LE | GE , 所以不需檢查EOF
  
  while ( mTokenType == LSS OR mTokenType == GTR OR
          mTokenType == LE OR mTokenType == GE ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Shift_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Relational_Exp()

void Parser::Maybe_Shift_Exp() {
// maybe_shift_exp : maybe_additive_exp { ( LS | RS ) maybe_additive_exp }

  Maybe_Additive_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 LS or RS , 所以不需檢查EOF
  
  while ( mTokenType == LS OR mTokenType == RS ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Additive_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Shift_Exp()

void Parser::Rest_Of_Maybe_Shift_Exp() {
// rest_of_maybe_shift_exp : rest_of_maybe_additive_exp { ( LS | RS ) maybe_additive_exp }

  Rest_Of_Maybe_Additive_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 LS or RS , 所以不需檢查EOF
  
  while ( mTokenType == LS OR mTokenType == RS ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Additive_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Shift_Exp()

void Parser::Maybe_Additive_Exp() {
// maybe_additive_exp : maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }

  Maybe_Mult_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 + or - , 所以不需檢查EOF
  
  while ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Mult_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Additive_Exp()

void Parser::Rest_Of_Maybe_Additive_Exp() {
// rest_of_maybe_additive_exp : rest_of_maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }

  Rest_Of_Maybe_Mult_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 + or - , 所以不需檢查EOF
  
  while ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Maybe_Mult_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Additive_Exp()

void Parser::Maybe_Mult_Exp() {
// maybe_mult_exp : unary_exp rest_of_maybe_mult_exp

  Unary_Exp();
  Rest_Of_Maybe_Mult_Exp();

} // end Parser::Maybe_Mult_Exp()

void Parser::Rest_Of_Maybe_Mult_Exp() {
// rest_of_maybe_mult_exp : { ( '*' | '/' | '%' ) unary_exp }

  mScanner.PeekToken( mTokenStr, mTokenType );
  // 因為不一定要有 '*' | '/' | '%' , 所以不需檢查EOF
  
  while ( mTokenType == MULT OR mTokenType == DIV OR mTokenType == REM ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Unary_Exp();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Mult_Exp()

void Parser::Unary_Exp() {
// unary_exp : sign { sign } signed_unary_exp
//    | unsigned_unary_exp
//    | ( PP | MM ) Identifier [ '[' expression ']' ]
  
  mScanner.PeekToken( mTokenStr, mTokenType );
  // 代表應該要有下個token, 但沒有 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  
  if ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
   
    while ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
    } // while
   
    Signed_Unary_Exp() ;
  } // if
  else if ( mTokenType == ID OR mTokenType == CONSTANT OR mTokenType == LP ) {
    Unsigned_Unary_Exp();
  } // else if
  else if ( mTokenType == PP OR mTokenType == MM ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == ID ) {
      CheckIDExistOrNot( mTokenStr ) ; 
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 不一定要有下一個
      if ( mTokenType == MLP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        Expression();
        mScanner.PeekToken( mTokenStr, mTokenType );
        // 一定要有 ']' 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";
        if ( mTokenType == MRP ) {
          mScanner.GetToken( mTokenStr, mTokenType ) ;
        } // if
        else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
      } // if 
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else if
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();

} // end Parser::Unary_Exp()

void Parser::Signed_Unary_Exp() {
// signed_unary_exp : Identifier 
//  [ '(' [ actual_parameter_list ] ')' | '[' expression ']'  ]
//  | Constant 
//  | '(' expression ')'

  mScanner.PeekToken( mTokenStr, mTokenType );
  // 代表應該要有下個token, 但沒有 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  
  
  // Identifier [ '(' [ actual_parameter_list ] ')' | '[' expression ']'  ]
  if ( mTokenType == ID ) {
    CheckIDExistOrNot( mTokenStr ) ; 
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
   
    if ( mTokenType == LP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  一定要有')' 
     
      if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
           mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
           mTokenType == CONSTANT OR mTokenType == LP ) {
        Actual_Parameter_List();
        mScanner.PeekToken( mTokenStr, mTokenType );
        // 代表應該要有下個token, 但沒有 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  一定要有')' 
      } // if
     
      if ( mTokenType == RP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
      } // if
      else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();

    } // if
    
    else if ( mTokenType == MLP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      Expression();
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  一定要有']' 
     
      if ( mTokenType == MRP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
      } // if
      else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
    } // else if
   
  } // if
  
  else if ( mTokenType == CONSTANT ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
  } // else if
  
  // '(' expression ')'
  else if ( mTokenType == LP ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Expression();
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  一定要有']' 
     
    if ( mTokenType == RP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
   
  } // else if
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();


} // end Parser::Signed_Unary_Exp()

void Parser::Unsigned_Unary_Exp() {
// unsigned_unary_exp : Identifier [ '(' [ actual_parameter_list ] ')' 
//   | [ '[' expression ']' ] [ ( PP | MM ) ] ]
//   | Constant 
//   | '(' expression ')'

  mScanner.PeekToken( mTokenStr, mTokenType );
  // 代表應該要有下個token, 但沒有 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  
  
  // Identifier [ '(' [ actual_parameter_list ] ')' | [ '[' expression ']' ] [ ( PP | MM ) ] ]
  if ( mTokenType == ID ) {
    CheckIDExistOrNot( mTokenStr ) ;
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
   
    if ( mTokenType == LP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
      // 代表應該要有下個token, 但沒有 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  一定要有')' 
     
      if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
           mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
           mTokenType == CONSTANT OR mTokenType == LP ) {
        Actual_Parameter_List();
        mScanner.PeekToken( mTokenStr, mTokenType );
        // 代表應該要有下個token, 但沒有 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  一定要有')' 
      } // if
     
      if ( mTokenType == RP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
      } // if
      else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
     
    } // if
    else if ( mTokenType == MLP OR mTokenType == PP OR mTokenType == MM ) {
      if ( mTokenType == MLP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        Expression();
        mScanner.PeekToken( mTokenStr, mTokenType );
        // 代表應該要有下個token, 但沒有 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  一定要有']' 
     
        if ( mTokenType == MRP ) {
          mScanner.GetToken( mTokenStr, mTokenType ) ;
          mScanner.PeekToken( mTokenStr, mTokenType );
        } // if
        else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
      } // if
     
      if ( mTokenType == PP OR mTokenType == MM ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
      } // if
    } // else if
   
  } // if
  
  else if ( mTokenType == CONSTANT ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
  } // else if
  
  // '(' expression ')'
  else if ( mTokenType == LP ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Expression();
    mScanner.PeekToken( mTokenStr, mTokenType );
    // 代表應該要有下個token, 但沒有 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  一定要有']' 
     
    if ( mTokenType == RP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
   
  } // else if
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();

} // end Parser::Unsigned_Unary_Exp()

// -------------------------------------------------------------

void Parser::UpdateCurType( string & curType, string curToken ) {
  
  if ( curToken[0] == '"' )
    curType = "string" ;
  else if ( curToken[0] == '\'' )
    curType = "char" ;
  else if ( curToken == "true" OR curToken == "false" )
    curType = "bool" ;
  else if ( curToken[0] == '.' OR ( curToken[0] >= '0' AND curToken[0] <= '9' ) ) {
    if ( IsFloat( curToken ) ) {
      curType = "float" ;
    } // if
    else  curType = "int" ;
  } // else if
  else {  // id
    GetVariableType( curType, curToken );
  } // else

} // Parser::UpdateCurType()

void Parser::Statement_v( string & curType, double & returnValue, string & returnStr, bool & isReturn ) {
// statement : ';'     // the null statement
//  | expression ';'  // expression here should not be empty 
//  | RETURN [ expression ] ';'
//  | compound_statement
//  | IF '(' expression ')' statement [ ELSE statement ]
//  | WHILE '(' expression ')' statement
//  | DO statement WHILE '(' expression ')' ';'

  
  vector<VariableType> tempVariableList;
  vector<FunctionType> tempFunctionList;
  vector<char> tempInputBuffer;
  FunctionType tempFunction;
  vector<FunctionType> tempCallStack;
  
  string executeStr = "";
  double executeValue = 0.0;
  
  int tempRegion = 0;
  
  int conditionStart = 0, conditionEnd = 0;
  int statementStart = 0, statementEnd = 0;
  
  int conditionStart_F = 0, conditionEnd_F = 0;
  int statementStart_F = 0, statementEnd_F = 0;
  
  int curEnd = 0, curEnd_F = 0;
  
  isReturn = false;
  
  PeekInputVecToken( mTokenStr, mTokenType );
  if ( mTokenType == SCLN ) {
    GetInputVecToken( mTokenStr, mTokenType );
  } // if
  else if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
            mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
            mTokenType == CONSTANT OR mTokenType == LP OR mTokenType == CIN OR mTokenType == COUT ) {
    Expression_v( curType, executeValue, executeStr );
    PeekInputVecToken( mTokenStr, mTokenType );
    if ( mTokenType == SCLN ) {
      GetInputVecToken( mTokenStr, mTokenType );
    } // if
  } // else if
  else if ( mTokenType == RETURN ) {
    GetInputVecToken( mTokenStr, mTokenType );
    if ( mSkipExp == 0 )
      isReturn = true;
    PeekInputVecToken( mTokenStr, mTokenType );

    if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
         mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
         mTokenType == CONSTANT OR mTokenType == LP ) {
      Expression_v( curType, executeValue, executeStr );
      returnValue = executeValue;
      returnStr = executeStr;
      
      PeekInputVecToken( mTokenStr, mTokenType );
    } // if
    
    if ( mTokenType == SCLN ) {
      GetInputVecToken( mTokenStr, mTokenType );
    } // if
  } // else if
  else if ( mTokenType == BLP ) {
    Compound_Statement_v( curType, returnValue, returnStr, isReturn );
  } // else if
  else if ( mTokenType == IF ) {
    GetInputVecToken( mTokenStr, mTokenType );
    PeekInputVecToken( mTokenStr, mTokenType );
    if ( mTokenType == LP ) {
      GetInputVecToken( mTokenStr, mTokenType );
      Expression_v( curType, executeValue, executeStr );
      PeekInputVecToken( mTokenStr, mTokenType );

      if ( mTokenType == RP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        tempVariableList.assign( mVariableList.begin(), mVariableList.end() );
        tempFunctionList.assign( mFunctionList.begin(), mFunctionList.end() );
        tempInputBuffer.assign( mInputBuffer.begin(), mInputBuffer.end() );  // save CPU status
        tempCallStack = mCallStack;
        tempFunction = mCurFunction;
        tempRegion = mCurVariableRegion;
        mSkipExp++;
        statementStart = mUserInputVecNum;
        statementStart_F = mCurFunction.pc;
        Statement_v( curType, returnValue, returnStr, isReturn );
        statementEnd = mUserInputVecNum;
        statementEnd_F = mCurFunction.pc;
        mSkipExp--;
        // 先紀錄statement的開始和結束 
        
        mVariableList.assign( tempVariableList.begin(), tempVariableList.end() );
        mFunctionList.assign( tempFunctionList.begin(), tempFunctionList.end() );
        mInputBuffer.assign( tempInputBuffer.begin(), tempInputBuffer.end() );  // restore CPU status
        mCallStack = tempCallStack;
        mCurFunction = tempFunction;
        mCurVariableRegion = tempRegion;
        
        mUserInputVecNum = statementEnd;
        mCurFunction.pc = statementEnd_F;
        
        if ( executeValue != 0 AND mSkipExp == 0 ) {
          // 符合條件才能執行 
          mUserInputVecNum = statementStart;
          mCurFunction.pc = statementStart_F;
          Statement_v( curType, returnValue, returnStr, isReturn );
          mUserInputVecNum = statementEnd;
          mCurFunction.pc = statementEnd_F;
          
          if ( isReturn AND mSkipExp == 0 ) {
            return;
          } // if
        } // if
        

        PeekInputVecToken( mTokenStr, mTokenType ); // 可以有ELSE, 也可以不要有 
        if ( mTokenType == ELSE ) {
          GetInputVecToken( mTokenStr, mTokenType );
          tempVariableList.assign( mVariableList.begin(), mVariableList.end() );
          tempFunctionList.assign( mFunctionList.begin(), mFunctionList.end() );
          tempInputBuffer.assign( mInputBuffer.begin(), mInputBuffer.end() );  // save CPU status
          tempCallStack = mCallStack;
          tempFunction = mCurFunction;
          tempRegion = mCurVariableRegion;

          statementStart = mUserInputVecNum;
          statementStart_F = mCurFunction.pc;
          mSkipExp++;
          Statement_v( curType, returnValue, returnStr, isReturn );
          mSkipExp--;
          statementEnd = mUserInputVecNum;
          statementEnd_F = mCurFunction.pc;
          // 先紀錄statement的開始和結束 
          
          mVariableList.assign( tempVariableList.begin(), tempVariableList.end() );
          mFunctionList.assign( tempFunctionList.begin(), tempFunctionList.end() );
          mInputBuffer.assign( tempInputBuffer.begin(), tempInputBuffer.end() );  // restore CPU status
          mCallStack = tempCallStack;
          mCurFunction = tempFunction;
          mCurVariableRegion = tempRegion;
          
          mUserInputVecNum = statementEnd;
          mCurFunction.pc = statementEnd_F;
          
          if ( executeValue == 0 AND mSkipExp == 0 ) {
            // 不符合if的條件才能執行 
            mUserInputVecNum = statementStart;
            mCurFunction.pc = statementStart_F;
            Statement_v( curType, returnValue, returnStr, isReturn );
            mUserInputVecNum = statementEnd;
            mCurFunction.pc = statementEnd_F;
            
            if ( isReturn AND mSkipExp == 0 ) {
              return;
            } // if
            
          } // if
          
        } // if
      } // if
    } // if
  } // else if
  else if ( mTokenType == WHILE ) {
    
    GetInputVecToken( mTokenStr, mTokenType );
    PeekInputVecToken( mTokenStr, mTokenType );
    if ( mTokenType == LP ) {
      GetInputVecToken( mTokenStr, mTokenType );
      conditionStart = mUserInputVecNum;
      conditionStart_F = mCurFunction.pc;
      Expression_v( curType, executeValue, executeStr );
      conditionEnd = mUserInputVecNum;
      conditionEnd_F = mCurFunction.pc;
     
      PeekInputVecToken( mTokenStr, mTokenType );
      if ( mTokenType == RP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        tempVariableList.assign( mVariableList.begin(), mVariableList.end() );
        tempFunctionList.assign( mFunctionList.begin(), mFunctionList.end() );
        tempInputBuffer.assign( mInputBuffer.begin(), mInputBuffer.end() );  // save CPU status
        tempCallStack = mCallStack;
        tempFunction = mCurFunction;
        tempRegion = mCurVariableRegion;
        
        mSkipExp++;
        statementStart = mUserInputVecNum;
        statementStart_F = mCurFunction.pc;
        Statement_v( curType, returnValue, returnStr, isReturn ) ;
        statementEnd = mUserInputVecNum;
        statementEnd_F = mCurFunction.pc;
        mSkipExp--;
        // 先紀錄statement的開始和結束 
        
        curEnd = mUserInputVecNum;
        curEnd_F = mCurFunction.pc;
    
        mVariableList.assign( tempVariableList.begin(), tempVariableList.end() );
        mFunctionList.assign( tempFunctionList.begin(), tempFunctionList.end() );
        mInputBuffer.assign( tempInputBuffer.begin(), tempInputBuffer.end() );  // restore CPU status
        mCallStack = tempCallStack;
        mCurFunction = tempFunction;
        mCurVariableRegion = tempRegion;
        
        mUserInputVecNum = curEnd;
        mCurFunction.pc = curEnd_F;

        while ( executeValue != 0 AND mSkipExp == 0 ) {
          mUserInputVecNum = statementStart;
          mCurFunction.pc = statementStart_F;
          Statement_v( curType, returnValue, returnStr, isReturn ) ;
          mUserInputVecNum = statementEnd;
          mCurFunction.pc = statementEnd_F;
          
          if ( isReturn AND mSkipExp == 0 ) {
            return;
          } // if

          mUserInputVecNum = conditionStart;
          mCurFunction.pc = conditionStart_F;
          Expression_v( curType, executeValue, executeStr );
          mUserInputVecNum = conditionEnd;
          mCurFunction.pc = conditionEnd_F;
        } // while

        mUserInputVecNum = curEnd;
        mCurFunction.pc = curEnd_F;

      } // if
    } // if
  } // else if
  else if ( mTokenType == DO ) {

    GetInputVecToken( mTokenStr, mTokenType );
    statementStart = mUserInputVecNum;
    statementStart_F = mCurFunction.pc;
    Statement_v( curType, returnValue, returnStr, isReturn ) ;
    statementEnd = mUserInputVecNum;
    statementEnd_F = mCurFunction.pc;
    // 不需要跳過
    // 因為do就是要先做statement 
    if ( isReturn AND mSkipExp == 0 ) {
      return;
    } // if
   
    PeekInputVecToken( mTokenStr, mTokenType );
    if ( mTokenType == WHILE ) {
      GetInputVecToken( mTokenStr, mTokenType );
      PeekInputVecToken( mTokenStr, mTokenType );

      if ( mTokenType == LP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        conditionStart = mUserInputVecNum;
        conditionStart_F = mCurFunction.pc;
        Expression_v( curType, executeValue, executeStr );
        conditionEnd = mUserInputVecNum;
        conditionEnd_F = mCurFunction.pc;
        PeekInputVecToken( mTokenStr, mTokenType );

        if ( mTokenType == RP ) {
          GetInputVecToken( mTokenStr, mTokenType );
          PeekInputVecToken( mTokenStr, mTokenType );

          if ( mTokenType == SCLN ) {
            GetInputVecToken( mTokenStr, mTokenType );
            curEnd = mUserInputVecNum;
            curEnd_F = mCurFunction.pc;
            
            while ( executeValue != 0 ) {
              mUserInputVecNum = statementStart;
              mCurFunction.pc = statementStart_F;
              Statement_v( curType, returnValue, returnStr, isReturn ) ;
              mUserInputVecNum = statementEnd;
              mCurFunction.pc = statementEnd_F;

              if ( isReturn AND mSkipExp == 0 ) {
                return;
              } // if
  
              mUserInputVecNum = conditionStart;
              mCurFunction.pc = conditionStart_F;
              Expression_v( curType, executeValue, executeStr );
              mUserInputVecNum = conditionEnd;
              mCurFunction.pc = conditionEnd_F;
            } // while
        
            mUserInputVecNum = curEnd;
            mCurFunction.pc = curEnd_F;
            
          } // if
        } // if
      } // if
    } // if

  } // else if

} // end Parser::Statement_v()

void Parser::Declaration_v() {
// 這裡的ID是宣告的ID, 不用檢查是否存在 
// 大括號(while、if等等)裡面宣告的ID 
// declaration : type_specifier Identifier rest_of_declarators

  VariableType tmpID;
  string tmpVariableType = "";
  string tmpVariableName = "";
  PeekInputVecToken( mTokenStr, mTokenType );

  if ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
       mTokenType == STRING OR mTokenType == BOOL ) {
    GetInputVecToken( mTokenStr, mTokenType );
    tmpVariableType = mTokenStr;  // 存宣告型別 
    PeekInputVecToken( mTokenStr, mTokenType );

    if ( mTokenType == ID ) {
      GetInputVecToken( mTokenStr, mTokenType );
      tmpVariableName = mTokenStr;
      
      PeekInputVecToken( mTokenStr, mTokenType );
      if ( mTokenType == MLP OR mTokenType == COMMA OR mTokenType == SCLN ) {
        Rest_Of_Declarators_v( tmpVariableType, tmpVariableName );
      } // if
    } // if
  } // if

} // end Parser::Declaration_v()

void Parser::Rest_Of_Declarators_v( string tmpVariableType, string tmpVariableName ) {
// 這裡的ID是宣告的ID, 不用檢查是否存在 
// 大括號裡宣告ID的延續 
// rest_of_declarators : [ '[' Constant ']' ] 
//                       { ',' Identifier [ '[' Constant ']' ] } ';'
// 不管怎樣都至少有個';'

  VariableType tmpID;
  
  // 把剛剛得到的型別和ID名都先assign   
  tmpID.variableName = tmpVariableName;
  tmpID.variableType = tmpVariableType;
  tmpID.constant = "";
  tmpID.region = mCurVariableRegion; 
  
  PeekInputVecToken( mTokenStr, mTokenType );

  // [ '[' Constant ']' ], 不一定要有 
  if ( mTokenType == MLP ) {
    GetInputVecToken( mTokenStr, mTokenType );
    PeekInputVecToken( mTokenStr, mTokenType );

    if ( mTokenType == CONSTANT ) {
      GetInputVecToken( mTokenStr, mTokenType );
      tmpID.constant = mTokenStr;
      PeekInputVecToken( mTokenStr, mTokenType );

      if ( mTokenType == MRP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        PeekInputVecToken( mTokenStr, mTokenType );  // 讀下一個token, 看是','還是';'
        // 因為一定要有';', 所以要檢查是否EOF 

      } // if
    } // if
  } // if
  
  mTempIDList.push_back( tmpID ) ;  // 把宣告的ID存進去 
  
  // { ',' Identifier [ '[' Constant ']' ] }
  // 不一定要有 
  while ( mTokenType == COMMA ) {
    
    tmpID.variableType = "";
    tmpID.variableName = "";
    tmpID.constant = "";
    tmpID.region = mCurVariableRegion; 
    
    GetInputVecToken( mTokenStr, mTokenType );
    PeekInputVecToken( mTokenStr, mTokenType );
   
    if ( mTokenType == ID ) {
      GetInputVecToken( mTokenStr, mTokenType );
      tmpID.variableName = mTokenStr;
      tmpID.variableType = tmpVariableType;
      PeekInputVecToken( mTokenStr, mTokenType ); // 讀下個token, 看是'['還是';', 也可以是',' 
      // 因為一定要有';', 所以要檢查是否EOF 
      
      // [ '[' Constant ']' ], 不一定要有 
      if ( mTokenType == MLP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        PeekInputVecToken( mTokenStr, mTokenType );
        
        if ( mTokenType == CONSTANT ) {
          GetInputVecToken( mTokenStr, mTokenType );
          tmpID.constant = mTokenStr;
          PeekInputVecToken( mTokenStr, mTokenType );

          if ( mTokenType == MRP ) {
            GetInputVecToken( mTokenStr, mTokenType );
            PeekInputVecToken( mTokenStr, mTokenType ); // 讀下一個token, 看是','還是';'
            // 因為一定要有';', 所以要檢查是否EOF 
            
            // 如果是','就繼續跑迴圈
            // 是';'就出迴圈 
          } // if
          
        } // if
        
      } // if
      
      // 如果不是'[', 可能是','或';' 
      // 如果是','就繼續跑迴圈
      // 是';'就出迴圈 
      mTempIDList.push_back( tmpID ) ;  // 把宣告的ID存進去 
    } // if
    
  } // while
  
  // 最後一個token是';' , 一定要有 
  if ( mTokenType == SCLN ) {
    GetInputVecToken( mTokenStr, mTokenType );
  } // if

  if ( mExecuteFunction > 0 ) {
    for ( int i = 0 ; i < mTempIDList.size() ; i++ ) {
      StoreVariable( mTempIDList[i].variableType, mTempIDList[i].variableName, mTempIDList[i].constant );
    } // for
  } // if

  mTempIDList.clear();  // 存過的就清掉 

} // end Parser::Rest_Of_Declarators_v()

void Parser::Compound_Statement_v( string & curType, double & returnValue,
                                   string & returnStr, bool & isReturn ) {
// 大括號 
// compound_statement : '{' { declaration | statement } '}'
  PeekInputVecToken( mTokenStr, mTokenType );
  if ( mTokenType == BLP ) {
    GetInputVecToken( mTokenStr, mTokenType );
    mCurVariableRegion++;
    PeekInputVecToken( mTokenStr, mTokenType );
    while ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
            mTokenType == STRING OR mTokenType == BOOL OR mTokenType == SCLN OR
            mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
            mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
            mTokenType == CONSTANT OR mTokenType == LP OR mTokenType == RETURN OR
            mTokenType == BLP OR mTokenType == IF OR mTokenType == WHILE OR mTokenType == DO OR
            mTokenType == CIN OR mTokenType == COUT ) {

      if ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
           mTokenType == STRING OR mTokenType == BOOL ) {
        Declaration_v();
      } // if
      else if ( mTokenType == SCLN OR mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
                mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
                mTokenType == CONSTANT OR mTokenType == LP OR mTokenType == RETURN OR
                mTokenType == BLP OR mTokenType == IF OR mTokenType == WHILE OR mTokenType == DO OR
                mTokenType == CIN OR mTokenType == COUT ) {
        Statement_v( curType, returnValue, returnStr, isReturn );
        // 如果是return 且 不是if或while正在跑過一遍測試 
        // 就把mCurFunction.pc調到最後一個 '{' 
        if ( isReturn AND mSkipExp == 0 ) {
          mCurFunction.pc = mCurFunction.statements.size() - 1;
        } // if
      } // else if
   
      PeekInputVecToken( mTokenStr, mTokenType );

    } // while
   
    if ( mTokenType == BRP ) {
      GetInputVecToken( mTokenStr, mTokenType );
      mCurVariableRegion--;
    } // if
  } // if

} // end Parser::Compound_Statement_v()

void Parser::Cin_expression_v( string & curType, double & returnValue, string & returnStr ) {
// cin_expression : cin { RS ID }
  
  string idType = "";
  string inputStr = "";
  
  string idNameStr = "";
  
  double constantValue = -1.0;
  string constantStr = "";
  
  string tmpCurType = "";
  
  PeekInputVecToken( mTokenStr, mTokenType );
 
  if ( mTokenType == CIN ) {
    GetInputVecToken( mTokenStr, mTokenType );
    PeekInputVecToken( mTokenStr, mTokenType );
    
    while ( mTokenType == RS ) {  // '>>'
      GetInputVecToken( mTokenStr, mTokenType );
      PeekInputVecToken( mTokenStr, mTokenType );
      if ( mTokenType == ID ) {
        GetInputVecToken( mTokenStr, mTokenType );
        idNameStr = mTokenStr;
        
        GetVariableType( idType, idNameStr );
        UpdateCurType( curType, idNameStr ) ;
        GetInputBufferItem( idType, inputStr );
        
        if ( mTokenType == MLP ) {
          GetInputVecToken( mTokenStr, mTokenType );
          tmpCurType = curType;
          Expression_v( curType, constantValue, constantStr );
          // constantStr在這裡應該用不到 
          curType = tmpCurType ;
          PeekInputVecToken( mTokenStr, mTokenType );
 
          if ( mTokenType == MRP ) {
            GetInputVecToken( mTokenStr, mTokenType );
          } // if
        } // if

        if ( idType == "bool" ) {
          if ( inputStr == "true" )  inputStr = "1";
          else                       inputStr = "0";
        } // if

        UpdateVariableValue( idType, atof( inputStr.c_str() ), inputStr, idNameStr, constantValue ) ;
        PeekInputVecToken( mTokenStr, mTokenType );
      } // if
    } // while
    
  } // if
} // Parser::Cin_expression_v()

void Parser::Cout_expression_v( string & curType, double & returnValue, string & returnStr ) {
  
// cout_expression : cout { LS expression }

  double executeValue = 0.0;
  string executeStr = "";
  PeekInputVecToken( mTokenStr, mTokenType );

  if ( mTokenType == COUT ) {
    GetInputVecToken( mTokenStr, mTokenType );
    PeekInputVecToken( mTokenStr, mTokenType );

    while ( mTokenType == LS ) {  // '<<'
      GetInputVecToken( mTokenStr, mTokenType );
      mIsCout++;  // 為了跳過<<符號 
      Expression_v( curType, executeValue, executeStr ); 
      mIsCout--;
      if ( mSkipExp == 0 ) {
        if ( curType == "char" OR curType == "string" ) {
          string temp = "";
          int i = 0;
          bool skip = false;
          for ( i = 0 ; i < executeStr.size()-1 ; i++ ) {
            if ( skip ) {
              skip = false;
            } // if
            else if ( executeStr[i] == '\\' AND executeStr[i+1] == 'n' ) {
              temp = temp + "\n";
              skip = true; 
            } // if
            else if ( executeStr[i] == '\\' AND executeStr[i+1] == 't' ) {
              temp = temp + "\t";
              skip = true; 
            } // else if
            else if ( executeStr[i] == '\\' AND
                      ( executeStr[i+1] == '\\' OR executeStr[i+1] == '\'' OR
                        executeStr[i+1] == '\"' ) ) {
              temp = temp + executeStr[i+1];
              skip = true; 
            } // else if
            else {
              temp = temp + executeStr[i];
            } // else
          } // for
          
          if ( !skip ) {
            temp = temp + executeStr[ executeStr.size() - 1 ];
          } // if
  
          printf( "%s", temp.c_str() );
        } // if
        else if ( curType == "bool" ) {
          if ( executeValue == 0 )  printf( "false" );
          else                      printf( "true" );
        } // else if
        else if ( curType == "float" ) {
          printf( "%1.3f", executeValue );
        } // else if
        else {
          int tmp = executeValue;
          printf( "%d", tmp );
        } // else
      } // if

      returnValue = executeValue;
      returnStr = executeStr;
      PeekInputVecToken( mTokenStr, mTokenType );
    } // while


  } // if
} // Parser::Cout_expression_v()

void Parser::Expression_v( string & curType, double & returnValue, string & returnStr ) {
// 普通的敘述句 
// expression : basic_expression { ',' basic_expression }

  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 
  
  Basic_Expression_v( curType, executeValue, executeStr );
  PeekInputVecToken( mTokenStr, mTokenType ); // 不一定要有下一個 
  
  returnValue = executeValue;
  returnStr = executeStr;
  
  while ( mTokenType == COMMA ) {
    GetInputVecToken( mTokenStr, mTokenType );
    Basic_Expression_v( curType, executeValue, executeStr );
    returnValue = executeValue;
    returnStr = executeStr;
    PeekInputVecToken( mTokenStr, mTokenType );  // 不一定要有下一個 
  } // while

} // end Parser::Expression_v()

void Parser::Basic_Expression_v( string & curType, double & returnValue, string & returnStr ) {
// basic_expression : Identifier rest_of_Identifier_started_basic_exp
//  | ( PP | MM ) Identifier rest_of_PPMM_Identifier_started_basic_exp
//  | sign { sign } signed_unary_exp romce_and_romloe
//  | ( Constant | '(' expression ')' ) romce_and_romloe
//  | cin_exp ';' | cout_exp ';'
  
  
  string idValueStr = "";
  double idValue = 0.0;
  // 用到id的話, 存id值用的 
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 

  double signValue = 1.0;

  PeekInputVecToken( mTokenStr, mTokenType );
  if ( mTokenType == ID ) {
    GetInputVecToken( mTokenStr, mTokenType );
    UpdateCurType( curType, mTokenStr ) ;
    // 要把ID的str往下傳, 看是不是陣列, 並計算 
    Rest_Of_ID_Started_Basic_Exp_v( curType, returnValue, returnStr, mTokenStr );
  } // if
  else if ( mTokenType == PP OR mTokenType == MM ) {  // ++i, 先把id+1再給值 
    GetInputVecToken( mTokenStr, mTokenType );
    TokenType opType = mTokenType;
    PeekInputVecToken( mTokenStr, mTokenType );
    if ( mTokenType == ID ) {
      GetInputVecToken( mTokenStr, mTokenType );
      UpdateCurType( curType, mTokenStr ) ;
      Rest_Of_PPMM_ID_Started_Basic_Exp_v( curType, returnValue, returnStr, opType, mTokenStr );
      // 傳id跟++或--進去算, 因為id後面有可能是[] 
    } // if
  } // else if
  else if ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT ) {
    GetInputVecToken( mTokenStr, mTokenType );
    if      ( mTokenType == ADDORSIGN )  signValue = signValue * 1.0;
    else if ( mTokenType == SUBORSIGN )  signValue = signValue * ( - 1.0 );
    else if ( mTokenType == BITNOT )     signValue = signValue * ( - 1.0 );  // 若最後是負數代表NOT  
    PeekInputVecToken( mTokenStr, mTokenType );
    while ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT ) {
      GetInputVecToken( mTokenStr, mTokenType );
      if      ( mTokenType == ADDORSIGN )  signValue = signValue * 1.0;
      else if ( mTokenType == SUBORSIGN )  signValue = signValue * ( - 1.0 );
      else if ( mTokenType == BITNOT )     signValue = signValue * ( - 1.0 );  // 若最後是負數代表NOT 
      PeekInputVecToken( mTokenStr, mTokenType );
    } // while
    
    Signed_Unary_Exp_v( curType, executeValue, executeStr );

    if ( curType == "bool" ) { 
      if ( signValue == ( - 1.0 ) ) {
        if ( executeValue == 0 )  executeValue = 1.0;
        else                      executeValue = 0.0;
      } // if
    } // if
    else {
      executeValue = signValue * executeValue;
    } // else

    Romce_And_Romloe_v( curType, returnValue, returnStr, executeValue, executeStr );
  } // else if
  else if ( mTokenType == CONSTANT ) {
    GetInputVecToken( mTokenStr, mTokenType );
    UpdateCurType( curType, mTokenStr ) ;

    if ( curType == "string" OR curType == "char" ) {
      returnStr = mTokenStr;
      returnStr.erase( returnStr.begin() );
      returnStr.erase( returnStr.end() - 1 );
    } // if
    else if ( curType == "bool" ) {
      if ( mTokenStr == "true" )  returnValue = 1;
      else                        returnValue = 0;
    } // else if
    else {
      returnValue = atof( mTokenStr.c_str() );
    } // else

    Romce_And_Romloe_v( curType, returnValue, returnStr, returnValue, returnStr );  //  傳constant進去算
  } // else if 
  else if ( mTokenType == LP ) {
    GetInputVecToken( mTokenStr, mTokenType );
    mInParentheses++;
    Expression_v( curType, executeValue, executeStr );
    PeekInputVecToken( mTokenStr, mTokenType );
    if ( mTokenType == RP ) {
      GetInputVecToken( mTokenStr, mTokenType );
      mInParentheses--;
      Romce_And_Romloe_v( curType, returnValue, returnStr, executeValue, executeStr );
    } // if
  } // else if
  else if ( mTokenType == CIN ) {
    Cin_expression_v( curType, returnValue, returnStr );
  } // else if
  else if ( mTokenType == COUT ) {
    Cout_expression_v( curType, returnValue, returnStr );
  } // else if

} // end Parser::Basic_Expression_v()

void Parser::Rest_Of_ID_Started_Basic_Exp_v( string & curType, double & returnValue,
                                             string & returnStr, string idStr ) {
// rest_of_Identifier_started_basic_exp : [ '[' expression ']' ]
//  ( assignment_operator basic_expression | [ PP | MM ] romce_and_romloe )
//  | '(' [ actual_parameter_list ] ')' romce_and_romloe
  
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為可以為空, 所以不需檢查EOF
    
  string idValueStr = "";
  double idValue = 0.0;
  // 用到id的話, 存id值用的 
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 
  
  string constantStr = "";
  double constantValue = -1.0;
  // 存中括號[]裡面的值 
  
  int value1 = 0, value2 = 0, tmp = 0;

  string tmpCurType = "";
  bool isReturn = false;
  
  vector<ParameterType> tmpValueList ;

  // 這個是variable
  if ( mTokenType == MLP OR mTokenType == ASSIGN OR mTokenType == TE OR
       mTokenType == DE OR mTokenType == RE OR mTokenType == PE OR 
       mTokenType == ME OR mTokenType == PP OR mTokenType == MM OR 
       mTokenType == MULT OR mTokenType == DIV OR mTokenType == REM OR
       mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == LS OR
       mTokenType == RS OR mTokenType == LSS OR mTokenType == GTR OR
       mTokenType == LE OR mTokenType == GE OR mTokenType == EQ OR
       mTokenType == NEQ OR mTokenType == BITAND OR mTokenType == BITXOR OR
       mTokenType == BITOR OR mTokenType == LOGAND OR mTokenType == LOGOR OR
       mTokenType == QUE ) { 
    // [ '[' expression ']' ] 
    if ( mTokenType == MLP ) {
      GetInputVecToken( mTokenStr, mTokenType );
      tmpCurType = curType;
      Expression_v( curType, constantValue, constantStr );
      curType = tmpCurType;
      PeekInputVecToken( mTokenStr, mTokenType );
 
      if ( mTokenType == MRP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        PeekInputVecToken( mTokenStr, mTokenType );
        // 不一定要有下個token 
      } // if
    } // if
    
    GetVariableValue( idValue, idValueStr, idStr, constantValue );
    returnValue = idValue;
    returnStr = idValueStr;
    // 現在拿到確切的variable值
    // 先設定return value
    // 之後有兩條路可以走
    // 如果有走成功就update return值 

    // assignment_operator basic_expression
    if ( mTokenType == ASSIGN OR mTokenType == TE OR mTokenType == DE OR
         mTokenType == RE OR mTokenType == PE OR mTokenType == ME ) {
      // PE, ME, TE, DE, RE // +=, -=, *=, /=, %=

      TokenType opType = mTokenType;
      GetInputVecToken( mTokenStr, mTokenType );
      Basic_Expression_v( curType, executeValue, executeStr );
      
      
      // 重新get值, 因為有可能更新 (很痛, 超難找, 嗚嗚嗚) 
      GetVariableValue( idValue, idValueStr, idStr, constantValue );
      returnValue = idValue;
      returnStr = idValueStr;
      
      value1 = idValue;
      value2 = executeValue;

      if ( opType == ASSIGN ) {
        idValue = executeValue;
        idValueStr = executeStr;
      } // if
      else if ( opType == PE ) {
        idValue += executeValue;
        idValueStr += executeStr;
      } // else if
      else if ( opType == ME )      idValue -= executeValue;
      else if ( opType == TE )      idValue *= executeValue;
      else if ( opType == DE )      idValue /= executeValue;
      else if ( opType == RE )      idValue = value1 % value2;
      
      if ( idValue == ( int ) idValue AND executeValue == ( int ) executeValue ) {
        curType = "int" ;
        tmp = returnValue;
        returnValue = tmp;
      } // if

      UpdateVariableValue( curType, idValue, idValueStr, idStr, constantValue ) ;
      
      GetVariableType( curType, idStr ) ;

      returnValue = idValue;
      returnStr = idValueStr;
    } // if
    
    // [ PP | MM ] romce_and_romloe
    // 可以為空 
    else if ( mTokenType == PP OR mTokenType == MM OR mTokenType == MULT OR
              mTokenType == DIV OR mTokenType == REM OR mTokenType == ADDORSIGN OR
              mTokenType == SUBORSIGN OR mTokenType == LS OR mTokenType == RS OR
              mTokenType == LSS OR mTokenType == GTR OR mTokenType == LE OR 
              mTokenType == GE OR mTokenType == EQ OR mTokenType == NEQ OR
              mTokenType == BITAND OR mTokenType == BITXOR OR mTokenType == BITOR OR
              mTokenType == LOGAND OR mTokenType == LOGOR OR mTokenType == QUE ) { 
    
      GetVariableValue( idValue, idValueStr, idStr, constantValue );
      // 如果沒有[], constantValue就會是-1
      // 一樣會取到正確的id值 
    
      if ( mTokenType == PP OR mTokenType == MM ) {   // i++, 先給值再把id+1 
        GetInputVecToken( mTokenStr, mTokenType );
        if  ( mTokenType == PP )
          UpdateVariableValue( curType, idValue+1, idValueStr, idStr, constantValue ) ;
        else if ( mTokenType == MM )
          UpdateVariableValue( curType, idValue-1, idValueStr, idStr, constantValue ) ;

        PeekInputVecToken( mTokenStr, mTokenType );
        // 不一定要有下個token 
      } // if
     
      Romce_And_Romloe_v( curType, executeValue, executeStr, idValue, idValueStr );
      // 把原本的idValue值(沒有++或--)傳進去做運算 
      // 回傳計算結果更新returnValue
    
      returnValue = executeValue;
      returnStr = executeStr;
    } // else if
  } // if
  
  
  // '(' [ actual_parameter_list ] ')' romce_and_romloe
  else if ( mTokenType == LP ) {   // 這個是function 
    GetInputVecToken( mTokenStr, mTokenType );

    PeekInputVecToken( mTokenStr, mTokenType );
    
    // [ actual_parameter_list ] 不一定要有 
    if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
         mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR 
         mTokenType == CONSTANT ) {

      Actual_Parameter_List_v( curType, executeValue, executeStr, tmpValueList );  // 設定parameter的值 
      PeekInputVecToken( mTokenStr, mTokenType );
    } // if 

    if ( mTokenType == RP ) {
      GetInputVecToken( mTokenStr, mTokenType );
      if ( mSkipExp == 0 ) {
        SetExecuteFunction( idStr, tmpValueList );  // 找到要執行的function並設為mCurFunction
        mCurFunction.recordParameters = tmpValueList;
        int tempRegion = mCurVariableRegion;
        mCurVariableRegion = 0;  // 避免function執行是在if裡面 
        // 這裡要拿到function的值
        Compound_Statement_v( curType, idValue, idValueStr, isReturn );
        mCurVariableRegion = tempRegion; 
        curType = mCurFunction.functionType;
      
        RestoreExecuteFunction();
      } // if
      
      Romce_And_Romloe_v( curType, returnValue, returnStr, idValue, idValueStr ); 
    } // if
  } // else if
  
  else {
    GetVariableValue( returnValue, returnStr, idStr, constantValue );
  } // else

} // end Parser::Rest_Of_ID_Started_Basic_Exp_v()

void Parser::Rest_Of_PPMM_ID_Started_Basic_Exp_v( string & curType, double & returnValue,
                                                  string & returnStr, TokenType opType, string idStr ) {
// rest_of_PPMM_Identifier_started_basic_exp : [ '[' expression ']' ] romce_and_romloe 
  
  double idValue = 0.0;
  string idValueStr = "";
  double constantValue = -1.0;
  string constantStr = "";
  
  string tmpCurType = "";

  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為可以為空, 所以不需檢查EOF

  // [ '[' expression ']' ] 
  if ( mTokenType == MLP ) {
    GetInputVecToken( mTokenStr, mTokenType );
    tmpCurType = curType;
    Expression_v( curType, constantValue, constantStr );
    curType = tmpCurType;
    PeekInputVecToken( mTokenStr, mTokenType );
    if ( mTokenType == MRP ) {
      GetInputVecToken( mTokenStr, mTokenType );
      PeekInputVecToken( mTokenStr, mTokenType );
      // 不一定要有下個token 
    } // if
  } // if

  GetVariableValue( idValue, idValueStr, idStr, constantValue );  // 取id的值
  
  if      ( opType == PP )  idValue++;
  else if ( opType == MM )  idValue--;
  
  UpdateVariableValue( curType, idValue, idValueStr, idStr, constantValue ) ;
  // 有++ or -- 一定要update 

  Romce_And_Romloe_v( curType, returnValue, returnStr, idValue, idValueStr ); 

} // end Parser::Rest_Of_PPMM_ID_Started_Basic_Exp_v()

void Parser::Actual_Parameter_List_v( string & curType, double & returnValue,
                                      string & returnStr, vector<ParameterType> & tmpValueList ) {
// actual_parameter_list : basic_expression { ',' basic_expression }
  
  int i = 0 ;
  string executeStr = "";
  double executeValue = 0.0;
  
  mTmpParameter.address = -1;
  mTmpParameter.arrPosition = -1;
  mTmpParameter.listType = "";
  mTmpParameter.variableValue.clear();
  mTmpParameter.variableValue.resize( 1, "0" );
  mTmpParameter.functionNum = 0;
  mPassValueIsAddress = false;
  
  
  Basic_Expression_v( curType, executeValue, executeStr );
  
  if ( !mPassValueIsAddress ) {
    if ( curType == "string" OR curType == "char" ) {
      mTmpParameter.variableValue[0] = executeStr;
    } // if
    else {
      mTmpParameter.variableValue[0] = FloatToStr( executeValue ) ;
    } // else
  } // if
    
  tmpValueList.push_back( mTmpParameter ) ;
  
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有',', 所以不需檢查EOF
  while ( mTokenType == COMMA ) {
    i++;
    GetInputVecToken( mTokenStr, mTokenType );
    
    mTmpParameter.address = -1;
    mTmpParameter.arrPosition = -1;
    mTmpParameter.listType = "";
    mTmpParameter.variableValue.clear();
    mTmpParameter.variableValue.resize( 1, "0" );
    mTmpParameter.functionNum = 0;
    mPassValueIsAddress = false;
    
    Basic_Expression_v( curType, executeValue, executeStr );
    
    if ( !mPassValueIsAddress ) {
      if ( curType == "string" OR curType == "char" ) {
        mTmpParameter.variableValue[0] = executeStr;
      } // if
      else {
        mTmpParameter.variableValue[0] = FloatToStr( executeValue ) ;
      } // else
    } // if
    
    tmpValueList.push_back( mTmpParameter ) ;

    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Actual_Parameter_List_v()

void Parser::Romce_And_Romloe_v( string & curType, double & returnValue,
                                 string & returnStr, double idValue, string idValueStr ) {
// rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp // 即romce_and_romloe
// : rest_of_maybe_logical_OR_exp [ '?' basic_expression ':' basic_expression ]
  
  double executeValue1 = 0.0;
  double executeValue2 = 0.0;
  string executeStr = "";
  
  vector<VariableType> tempVariableList;
  vector<FunctionType> tempFunctionList;
  vector<char> tempInputBuffer;
  
  int exp1Start = 0, exp1End = 0;
  int exp2Start = 0, exp2End = 0;

  Rest_Of_Maybe_Logical_Or_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有'?', 所以不需檢查EOF

  if ( mTokenType == QUE ) {
    GetInputVecToken( mTokenStr, mTokenType );
    tempVariableList.assign( mVariableList.begin(), mVariableList.end() );
    tempFunctionList.assign( mFunctionList.begin(), mFunctionList.end() );
    tempInputBuffer.assign( mInputBuffer.begin(), mInputBuffer.end() );  // save CPU status

    exp1Start = mUserInputVecNum;
    Basic_Expression_v( curType, executeValue1, executeStr ) ;
    exp1End = mUserInputVecNum;

    PeekInputVecToken( mTokenStr, mTokenType );
    if ( mTokenType == COLON ) {
      GetInputVecToken( mTokenStr, mTokenType );

      exp2Start = mUserInputVecNum;
      Basic_Expression_v( curType, executeValue2, executeStr ) ;
      exp2End = mUserInputVecNum;
      
      mVariableList.assign( tempVariableList.begin(), tempVariableList.end() );
      mFunctionList.assign( tempFunctionList.begin(), tempFunctionList.end() );
      mInputBuffer.assign( tempInputBuffer.begin(), tempInputBuffer.end() );  // restore CPU status

      if ( returnValue != 0 ) {
        mUserInputVecNum = exp1Start;
        Basic_Expression_v( curType, executeValue1, executeStr ) ;
        mUserInputVecNum = exp2End;
        returnValue = executeValue1;
      } // if
      else {
        mUserInputVecNum = exp2Start;
        Basic_Expression_v( curType, executeValue2, executeStr ) ;
        mUserInputVecNum = exp2End;
        returnValue = executeValue2;
      } // else

    } // if
  } // if

 
} // end Parser::Romce_And_Romloe_v()

void Parser::Rest_Of_Maybe_Logical_Or_Exp_v( string & curType, double & returnValue,
                                             string & returnStr, double idValue, string idValueStr ) {
// rest_of_maybe_logical_OR_exp : rest_of_maybe_logical_AND_exp { OR maybe_logical_AND_exp }

  double executeValue = 0.0;
  string executeStr = "";
  Rest_Of_Maybe_Logical_And_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 OR , 所以不需檢查EOF
  
  while ( mTokenType == LOGOR ) {
    GetInputVecToken( mTokenStr, mTokenType );
    Maybe_Logical_And_Exp_v( curType, executeValue, executeStr );
    returnValue = returnValue || executeValue;
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while
  
} // end Parser::Rest_Of_Maybe_Logical_Or_Exp_v()

void Parser::Maybe_Logical_And_Exp_v( string & curType, double & returnValue, string & returnStr ) {
// maybe_logical_AND_exp : maybe_bit_OR_exp { AND maybe_bit_OR_exp }

  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的
  
  Maybe_Bit_Or_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 AND , 所以不需檢查EOF
  
  while ( mTokenType == LOGAND ) {
    GetInputVecToken( mTokenStr, mTokenType );
    Maybe_Bit_Or_Exp_v( curType, executeValue, executeStr );
    returnValue = returnValue && executeValue ;
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Logical_And_Exp_v()

void Parser::Rest_Of_Maybe_Logical_And_Exp_v( string & curType, double & returnValue,
                                              string & returnStr, double idValue, string idValueStr ) {
// rest_of_maybe_logical_AND_exp : rest_of_maybe_bit_OR_exp { AND maybe_bit_OR_exp }
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的
  
  Rest_Of_Maybe_Bit_Or_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 AND , 所以不需檢查EOF
  
  while ( mTokenType == LOGAND ) {
    GetInputVecToken( mTokenStr, mTokenType );
    Maybe_Bit_Or_Exp_v( curType, executeValue, executeStr );
    returnValue = returnValue && executeValue;
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Logical_And_Exp_v()

void Parser::Maybe_Bit_Or_Exp_v( string & curType, double & returnValue, string & returnStr ) {
// maybe_bit_OR_exp : maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }

  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的

  int value1 = 0, value2 = 0; 
  Maybe_Bit_Ex_Or_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 '|' , 所以不需檢查EOF
  
  while ( mTokenType == BITOR ) {
    GetInputVecToken( mTokenStr, mTokenType );
    Maybe_Bit_Ex_Or_Exp_v( curType, executeValue, executeStr );
    value1 = returnValue;
    value2 = executeValue;
    // returnValue = value1 | value2 ;
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Bit_Or_Exp_v()

void Parser::Rest_Of_Maybe_Bit_Or_Exp_v( string & curType, double & returnValue,
                                         string & returnStr, double idValue, string idValueStr ) {
// rest_of_maybe_bit_OR_exp : rest_of_maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }

  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的

  int value1 = 0, value2 = 0; 

  Rest_Of_Maybe_Bit_Ex_Or_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 '|' , 所以不需檢查EOF
  
  while ( mTokenType == BITOR ) {
    GetInputVecToken( mTokenStr, mTokenType );
    Maybe_Bit_Ex_Or_Exp_v( curType, executeValue, executeStr );
    value1 = returnValue;
    value2 = executeValue;
    // returnValue = value1 | value2 ;
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Bit_Or_Exp_v()

void Parser::Maybe_Bit_Ex_Or_Exp_v( string & curType, double & returnValue, string & returnStr ) {
// maybe_bit_ex_OR_exp : maybe_bit_AND_exp { '^' maybe_bit_AND_exp }
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的

  int value1 = 0, value2 = 0; 
  
  Maybe_Bit_And_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 '^' , 所以不需檢查EOF
  
  while ( mTokenType == BITXOR ) {
    GetInputVecToken( mTokenStr, mTokenType );
    Maybe_Bit_And_Exp_v( curType, executeValue, executeStr );
    value1 = returnValue;
    value2 = executeValue;
    // returnValue = value1 ^ value2 ;
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Bit_Ex_Or_Exp_v()

void Parser::Rest_Of_Maybe_Bit_Ex_Or_Exp_v( string & curType, double & returnValue,
                                            string & returnStr, double idValue, string idValueStr ) {
// rest_of_maybe_bit_ex_OR_exp : rest_of_maybe_bit_AND_exp { '^' maybe_bit_AND_exp }
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的

  int value1 = 0, value2 = 0; 

  Rest_Of_Maybe_Bit_And_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 '^' , 所以不需檢查EOF
  
  while ( mTokenType == BITXOR ) {
    GetInputVecToken( mTokenStr, mTokenType );
    Maybe_Bit_And_Exp_v( curType, executeValue, executeStr );
    value1 = returnValue;
    value2 = executeValue;
    // returnValue = value1 ^ value2 ;
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Bit_Ex_Or_Exp_v()

void Parser::Maybe_Bit_And_Exp_v( string & curType, double & returnValue, string & returnStr ) {
// maybe_bit_AND_exp : maybe_equality_exp { '&' maybe_equality_exp }
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的

  int value1 = 0, value2 = 0; 
  
  Maybe_Equality_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 '&' , 所以不需檢查EOF
  
  while ( mTokenType == BITAND ) {
    GetInputVecToken( mTokenStr, mTokenType );
    Maybe_Equality_Exp_v( curType, executeValue, executeStr );
    value1 = returnValue;
    value2 = executeValue;
    // returnValue = value1 & value2 ;
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Bit_And_Exp_v()

void Parser::Rest_Of_Maybe_Bit_And_Exp_v( string & curType, double & returnValue,
                                          string & returnStr, double idValue, string idValueStr ) {
// rest_of_maybe_bit_AND_exp : rest_of_maybe_equality_exp { '&' maybe_equality_exp }
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的

  int value1 = 0, value2 = 0; 
  
  Rest_Of_Maybe_Equality_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 '&' , 所以不需檢查EOF
  
  while ( mTokenType == BITAND ) {
    GetInputVecToken( mTokenStr, mTokenType );
    Maybe_Equality_Exp_v( curType, executeValue, executeStr );
    value1 = returnValue;
    value2 = executeValue;
    // returnValue = value1 & value2 ;
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Bit_And_Exp_v()

void Parser::Maybe_Equality_Exp_v( string & curType, double & returnValue, string & returnStr ) {
// maybe_equality_exp : maybe_relational_exp { ( EQ | NEQ ) maybe_relational_exp}

  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的

  TokenType opType;

  Maybe_Relational_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 EQ or NEQ , 所以不需檢查EOF
  
  while ( mTokenType == EQ OR mTokenType == NEQ ) {
    GetInputVecToken( mTokenStr, mTokenType );
    opType = mTokenType;
    Maybe_Relational_Exp_v( curType, executeValue, executeStr );
    if ( opType == EQ ) {
      returnValue = returnValue == executeValue;
    } // if
    else if ( opType == NEQ ) {
      returnValue = returnValue != executeValue;
    } // else if

    curType = "bool";
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Equality_Exp_v()

void Parser::Rest_Of_Maybe_Equality_Exp_v( string & curType, double & returnValue,
                                           string & returnStr, double idValue, string idValueStr ) {
// rest_of_maybe_equality_exp : rest_of_maybe_relational_exp { ( EQ | NEQ ) maybe_relational_exp }

  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 

  TokenType opType;
  
  Rest_Of_Maybe_Relational_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 EQ or NEQ , 所以不需檢查EOF
  
  while ( mTokenType == EQ OR mTokenType == NEQ ) {
    GetInputVecToken( mTokenStr, mTokenType );
    opType = mTokenType;
    Maybe_Relational_Exp_v( curType, executeValue, executeStr );
    if ( opType == EQ ) {
      returnValue = returnValue == executeValue;
    } // if
    else if ( opType == NEQ ) {
      returnValue = returnValue != executeValue;
    } // else if

    curType = "bool";
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Equality_Exp_v()

void Parser::Maybe_Relational_Exp_v( string & curType, double & returnValue, string & returnStr ) {
// maybe_relational_exp : maybe_shift_exp { ( '<' | '>' | LE | GE ) maybe_shift_exp }

  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 

  TokenType opType;
  
  Maybe_Shift_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 '<' | '>' | LE | GE , 所以不需檢查EOF
  
  while ( mTokenType == LSS OR mTokenType == GTR OR
          mTokenType == LE OR mTokenType == GE ) {
    GetInputVecToken( mTokenStr, mTokenType );
    opType = mTokenType;
    Maybe_Shift_Exp_v( curType, executeValue, executeStr );
    if      ( opType == LSS )  returnValue = returnValue < executeValue;
    else if ( opType == GTR )  returnValue = returnValue > executeValue;
    else if ( opType == LE )   returnValue = returnValue <= executeValue;
    else if ( opType == GE )   returnValue = returnValue >= executeValue;
    curType = "bool";
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Relational_Exp_v()

void Parser::Rest_Of_Maybe_Relational_Exp_v( string & curType, double & returnValue,
                                             string & returnStr, double idValue, string idValueStr ) {
// rest_of_maybe_relational_exp : rest_of_maybe_shift_exp { ( '<' | '>' | LE | GE ) maybe_shift_exp }

  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 

  TokenType opType;
  
  Rest_Of_Maybe_Shift_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 '<' | '>' | LE | GE , 所以不需檢查EOF
  
  while ( mTokenType == LSS OR mTokenType == GTR OR
          mTokenType == LE OR mTokenType == GE ) {
    GetInputVecToken( mTokenStr, mTokenType );
    opType = mTokenType;
    Maybe_Shift_Exp_v( curType, executeValue, executeStr );

    if      ( opType == LSS )  returnValue = returnValue < executeValue;
    else if ( opType == GTR )  returnValue = returnValue > executeValue;
    else if ( opType == LE )   returnValue = returnValue <= executeValue;
    else if ( opType == GE )   returnValue = returnValue >= executeValue;

    curType = "bool";
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Relational_Exp_v()

void Parser::Maybe_Shift_Exp_v( string & curType, double & returnValue, string & returnStr ) {
// maybe_shift_exp : maybe_additive_exp { ( LS | RS ) maybe_additive_exp }

  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 

  int value1 = 0, value2 = 0;
  TokenType opType;

  Maybe_Additive_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 LS or RS , 所以不需檢查EOF
  
  if ( mTokenType == LS AND mIsCout > 0 AND mInParentheses == 0 ) return;
  // '<<' 不在括號裡面, 是cout的 
  
  while ( mTokenType == LS OR mTokenType == RS ) {
    GetInputVecToken( mTokenStr, mTokenType );
    opType = mTokenType;
    Maybe_Additive_Exp_v( curType, executeValue, executeStr );
    
    value1 = returnValue;
    value2 = executeValue;
    if      ( opType == LS )  returnValue = ( value1 << value2 );
    else if ( opType == RS )  returnValue = ( value1 >> value2 );
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Shift_Exp_v()

void Parser::Rest_Of_Maybe_Shift_Exp_v( string & curType, double & returnValue,
                                        string & returnStr, double idValue, string idValueStr ) {
// rest_of_maybe_shift_exp : rest_of_maybe_additive_exp { ( LS | RS ) maybe_additive_exp }

  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 

  int value1 = 0, value2 = 0; 
  TokenType opType;

  Rest_Of_Maybe_Additive_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 LS or RS , 所以不需檢查EOF
  
  if ( mTokenType == LS AND mIsCout > 0 AND mInParentheses == 0 ) return;
  // '<<' 不在括號裡面, 是cout的 

  while ( mTokenType == LS OR mTokenType == RS ) {
    GetInputVecToken( mTokenStr, mTokenType );
    opType = mTokenType;
    Maybe_Additive_Exp_v( curType, executeValue, executeStr );

    value1 = returnValue;
    value2 = executeValue;
    if      ( opType == LS )  returnValue = ( value1 << value2 );
    else if ( opType == RS )  returnValue = ( value1 >> value2 );
    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Shift_Exp_v()

void Parser::Maybe_Additive_Exp_v( string & curType, double & returnValue, string & returnStr ) {
// maybe_additive_exp : maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 
  
  int value1 = 0, value2 = 0; 

  TokenType opType;

  Maybe_Mult_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 + or - , 所以不需檢查EOF
  
  while ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN ) {
    GetInputVecToken( mTokenStr, mTokenType );
    opType = mTokenType;
    Maybe_Mult_Exp_v( curType, executeValue, executeStr );

    value1 = returnValue;
    value2 = executeValue;

    if ( value1 == ( int ) returnValue AND value2 == ( int ) executeValue ) {
      if ( opType == ADDORSIGN ) {
        returnValue = value1 + value2;
        returnStr = returnStr + executeStr;
      } // if
      else if ( opType == SUBORSIGN ) {
        returnValue = value1 - value2;
      } // else if
    } // if
    else {
      if ( opType == ADDORSIGN ) {
        returnValue = returnValue + executeValue;
        returnStr = returnStr + executeStr;
      } // if
      else if ( opType == SUBORSIGN ) {
        returnValue = returnValue - executeValue;
      } // else if
    } // else

    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Maybe_Additive_Exp_v()

void Parser::Rest_Of_Maybe_Additive_Exp_v( string & curType, double & returnValue,
                                           string & returnStr, double idValue, string idValueStr ) {
// rest_of_maybe_additive_exp : rest_of_maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 
  
  int value1 = 0, value2 = 0; 
  
  TokenType opType;
  
  Rest_Of_Maybe_Mult_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 + or - , 所以不需檢查EOF

  while ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN ) {
    GetInputVecToken( mTokenStr, mTokenType );
    opType = mTokenType;
    Maybe_Mult_Exp_v( curType, executeValue, executeStr );
    
    value1 = returnValue;
    value2 = executeValue;

    if ( value1 == ( int ) returnValue AND value2 == ( int ) executeValue ) {
      if ( opType == ADDORSIGN ) {
        returnValue = value1 + value2;
        returnStr = returnStr + executeStr;
      } // if
      else if ( opType == SUBORSIGN ) {
        returnValue = value1 - value2;
      } // else if
    } // if
    else {
      if ( opType == ADDORSIGN ) {
        returnValue = returnValue + executeValue;
        returnStr = returnStr + executeStr;
      } // if
      else if ( opType == SUBORSIGN ) {
        returnValue = returnValue - executeValue;
      } // else if
    } // else


    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Additive_Exp_v()

void Parser::Maybe_Mult_Exp_v( string & curType, double & returnValue, string & returnStr ) {
// maybe_mult_exp : unary_exp rest_of_maybe_mult_exp
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 
  
  Unary_Exp_v( curType, executeValue, executeStr );
  Rest_Of_Maybe_Mult_Exp_v( curType, returnValue, returnStr, executeValue, executeStr );

} // end Parser::Maybe_Mult_Exp_v()

void Parser::Rest_Of_Maybe_Mult_Exp_v( string & curType, double & returnValue,
                                       string & returnStr, double idValue, string idValueStr ) {
// rest_of_maybe_mult_exp : { ( '*' | '/' | '%' ) unary_exp }
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 
  
  int value1 = 0, value2 = 0, tmp = 0; 
  TokenType opType;
  PeekInputVecToken( mTokenStr, mTokenType );
  // 因為不一定要有 '*' | '/' | '%' , 所以不需檢查EOF
  
  returnValue = idValue;
  returnStr = idValueStr;

  while ( mTokenType == MULT OR mTokenType == DIV OR mTokenType == REM ) {
    GetInputVecToken( mTokenStr, mTokenType );
    opType = mTokenType;
    Unary_Exp_v( curType, executeValue, executeStr );
    
    value1 = returnValue;
    value2 = executeValue;
    
    
    if ( value1 == ( int ) returnValue AND value2 == ( int ) executeValue ) {
      if      ( opType == MULT )  returnValue = value1 * value2;
      else if ( opType == DIV )   returnValue = value1 / value2;
    } // if
    else {
      if      ( opType == MULT )  returnValue = returnValue * executeValue;
      else if ( opType == DIV )   returnValue = returnValue / executeValue;
    } // else
    
    if ( opType == REM )   returnValue = value1 % value2;
    
    if ( idValue == ( int ) idValue AND executeValue == ( int ) executeValue ) {
      curType = "int" ;
      tmp = returnValue;
      returnValue = tmp;
    } // if

    PeekInputVecToken( mTokenStr, mTokenType );
  } // while

} // end Parser::Rest_Of_Maybe_Mult_Exp_v()

void Parser::Unary_Exp_v( string & curType, double & returnValue, string & returnStr ) {
// unary_exp : sign { sign } signed_unary_exp
//    | unsigned_unary_exp
//    | ( PP | MM ) Identifier [ '[' expression ']' ]
  
  string idStr = "";
  // 存id名稱 
  
  string idValueStr = "";
  double idValue = 0.0;
  // 用到id的話, 存id值用的 
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 
  
  string constantStr = "";
  double constantValue = -1.0;
  // 存中括號[]裡面的值 
  
  double signValue = 1.0;
  TokenType signType, opType;
  
  string tmpCurType = "";

  PeekInputVecToken( mTokenStr, mTokenType );
  
  if ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT ) {
    GetInputVecToken( mTokenStr, mTokenType );
    
    if      ( mTokenType == ADDORSIGN )  signValue = signValue * 1.0;
    else if ( mTokenType == SUBORSIGN )  signValue = signValue * ( - 1.0 );
    else if ( mTokenType == BITNOT )     signValue = signValue * ( - 1.0 );
    
    PeekInputVecToken( mTokenStr, mTokenType );

    while ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT ) {
      GetInputVecToken( mTokenStr, mTokenType );
      if      ( mTokenType == ADDORSIGN )  signValue = signValue * 1.0;
      else if ( mTokenType == SUBORSIGN )  signValue = signValue * ( - 1.0 );
      else if ( mTokenType == BITNOT )     signValue = signValue * ( - 1.0 );
      PeekInputVecToken( mTokenStr, mTokenType );
    } // while
   
    Signed_Unary_Exp_v( curType, executeValue, executeStr ) ;

    if ( curType == "bool" ) { 
      if ( signValue == ( - 1.0 ) ) {
        if ( executeValue == 0 )  returnValue = 1.0;
        else                      returnValue = 0.0;
      } // if
    } // if
    else {
      returnValue = signValue * executeValue;
    } // else

    returnStr = executeStr;
  } // if
  else if ( mTokenType == ID OR mTokenType == CONSTANT OR mTokenType == LP ) {
    Unsigned_Unary_Exp_v( curType, returnValue, returnStr );
  } // else if
  else if ( mTokenType == PP OR mTokenType == MM ) {
    GetInputVecToken( mTokenStr, mTokenType );
    opType = mTokenType;
    PeekInputVecToken( mTokenStr, mTokenType );
    if ( mTokenType == ID ) {
      GetInputVecToken( mTokenStr, mTokenType );
      UpdateCurType( curType, mTokenStr ) ;
      idStr = mTokenStr;
      PeekInputVecToken( mTokenStr, mTokenType );
      // 不一定要有下一個
      if ( mTokenType == MLP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        tmpCurType = curType;
        Expression_v( curType, constantValue, constantStr );
        curType = tmpCurType;
        PeekInputVecToken( mTokenStr, mTokenType );
        // 一定要有 ']' 
        if ( mTokenType == MRP ) {
          GetInputVecToken( mTokenStr, mTokenType );
        } // if
      } // if
    
      GetVariableValue( idValue, idValueStr, idStr, constantValue );
      if      ( opType == PP )  idValue++;
      else if ( opType == MM )  idValue--;
      UpdateVariableValue( curType, idValue, idValueStr, idStr, constantValue );
      returnValue = idValue;
      returnStr = idValueStr;
    } // if
  } // else if

} // end Parser::Unary_Exp_v()

void Parser::Signed_Unary_Exp_v( string & curType, double & returnValue, string & returnStr ) {
// signed_unary_exp : Identifier 
//  [ '(' [ actual_parameter_list ] ')' | '[' expression ']'  ]
//  | Constant 
//  | '(' expression ')'
  
  
  string idStr = "";
  // 存id名稱 
  
  string idValueStr = "";
  double idValue = 0.0;
  // 用到id的話, 存id值用的 
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 
  
  string constantStr = "";
  double constantValue = -1.0;
  // 存中括號[]裡面的值 
  
  string tmpCurType = "";
  bool isReturn = false;
  
  vector<ParameterType> tmpValueList ;
  
  PeekInputVecToken( mTokenStr, mTokenType );
  
  // Identifier [ '(' [ actual_parameter_list ] ')' | '[' expression ']'  ]
  if ( mTokenType == ID ) {
    GetInputVecToken( mTokenStr, mTokenType );
    UpdateCurType( curType, mTokenStr ) ;
    idStr = mTokenStr;
    PeekInputVecToken( mTokenStr, mTokenType );
   
    if ( mTokenType == LP ) {  // 這個是function
      GetInputVecToken( mTokenStr, mTokenType );
      PeekInputVecToken( mTokenStr, mTokenType );
     
      if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
           mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
           mTokenType == CONSTANT OR mTokenType == LP ) {
        Actual_Parameter_List_v( curType, executeValue, executeStr, tmpValueList );
        PeekInputVecToken( mTokenStr, mTokenType );
      } // if
     
      if ( mTokenType == RP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        if ( mSkipExp == 0 ) {
          SetExecuteFunction( idStr, tmpValueList );  // 找到要執行的function並設為mCurFunction
          mCurFunction.recordParameters = tmpValueList;
          int tempRegion = mCurVariableRegion;
          mCurVariableRegion = 0;
          // 這裡要拿到function的值
          Compound_Statement_v( curType, returnValue, returnStr, isReturn );
          mCurVariableRegion = tempRegion;
          curType = mCurFunction.functionType;
        
          RestoreExecuteFunction();
        } // if
      } // if

    } // if
    
    else if ( mTokenType == MLP ) {  // 這個是variable
      GetInputVecToken( mTokenStr, mTokenType );
      tmpCurType = curType;
      Expression_v( curType, constantValue, constantStr );
      curType = tmpCurType;
      PeekInputVecToken( mTokenStr, mTokenType );
     
      if ( mTokenType == MRP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        GetVariableValue( idValue, idValueStr, idStr, constantValue );
        returnValue = idValue;
        returnStr = idValueStr;
      } // if
    } // else if
    
    else {  // 沒有[]的variable
      GetVariableValue( returnValue, returnStr, idStr, constantValue );
    } // else 
   
  } // if
  
  else if ( mTokenType == CONSTANT ) { 
    GetInputVecToken( mTokenStr, mTokenType );
    UpdateCurType( curType, mTokenStr ) ;
    if ( curType == "string" OR curType == "char" ) {
      returnStr = mTokenStr;
      returnStr.erase( returnStr.begin() );
      returnStr.erase( returnStr.end() - 1 );
    } // if
    else if ( curType == "bool" ) {
      if ( mTokenStr == "true" )  returnValue = 1;
      else                        returnValue = 0;
    } // else if
    else {
      returnValue = atof( mTokenStr.c_str() );
    } // else
  } // else if
  
  // '(' expression ')'
  else if ( mTokenType == LP ) {
    GetInputVecToken( mTokenStr, mTokenType );
    Expression_v( curType, executeValue, executeStr );
    PeekInputVecToken( mTokenStr, mTokenType );
     
    if ( mTokenType == RP ) {
      GetInputVecToken( mTokenStr, mTokenType );
      returnValue = executeValue;
      returnStr = executeStr;
    } // if 
  } // else if

} // end Parser::Signed_Unary_Exp_v()

void Parser::Unsigned_Unary_Exp_v( string & curType, double & returnValue, string & returnStr ) {
// unsigned_unary_exp : Identifier [ '(' [ actual_parameter_list ] ')' 
//   | [ '[' expression ']' ] [ ( PP | MM ) ] ]
//   | Constant 
//   | '(' expression ')'
  
  string idStr = "";
  // 存id名稱 
  string idValueStr = "";
  double idValue = 0.0;
  // 用到id的話, 存id值用的 
  
  string executeStr = "";
  double executeValue = 0.0;
  // 存其他function算出來的計算值用的 
  
  string constantStr = "";
  double constantValue = -1.0;
  // 存中括號[]裡面的值 

  TokenType opType;
  
  string tmpCurType = "";
  bool isReturn = false;

  vector<ParameterType> tmpValueList ;
  
  PeekInputVecToken( mTokenStr, mTokenType );
  
  // Identifier [ '(' [ actual_parameter_list ] ')' | [ '[' expression ']' ] [ ( PP | MM ) ] ]
  if ( mTokenType == ID ) {
    GetInputVecToken( mTokenStr, mTokenType );
    UpdateCurType( curType, mTokenStr ) ;
    idStr = mTokenStr;
    PeekInputVecToken( mTokenStr, mTokenType );
   
    if ( mTokenType == LP ) {  // 這個是function
      GetInputVecToken( mTokenStr, mTokenType );
      PeekInputVecToken( mTokenStr, mTokenType );
     
      if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
           mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
           mTokenType == CONSTANT OR mTokenType == LP ) {
        Actual_Parameter_List_v( curType, executeValue, executeStr, tmpValueList );
        PeekInputVecToken( mTokenStr, mTokenType );
      } // if
     
      if ( mTokenType == RP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        if ( mSkipExp == 0 ) {
          SetExecuteFunction( idStr, tmpValueList );  // 找到要執行的function並設為mCurFunction
          mCurFunction.recordParameters = tmpValueList;
          int tempRegion = mCurVariableRegion;
          mCurVariableRegion = 0;
          // 這裡要拿到function的值, 待補 
          Compound_Statement_v( curType, returnValue, returnStr, isReturn );
          mCurVariableRegion = tempRegion;
          curType = mCurFunction.functionType;
        
          RestoreExecuteFunction();
        } // if
      } // if
     
    } // if
    else if ( mTokenType == MLP OR mTokenType == PP OR mTokenType == MM ) {
      if ( mTokenType == MLP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        tmpCurType = curType;
        Expression_v( curType, constantValue, constantStr );
        curType = tmpCurType;
        PeekInputVecToken( mTokenStr, mTokenType );
     
        if ( mTokenType == MRP ) {
          GetInputVecToken( mTokenStr, mTokenType );
          PeekInputVecToken( mTokenStr, mTokenType );
        } // if
      } // if
     
      if ( mTokenType == PP OR mTokenType == MM ) {
        GetInputVecToken( mTokenStr, mTokenType );
        opType = mTokenType;
      } // if
      
      GetVariableValue( idValue, idValueStr, idStr, constantValue );

      returnValue = idValue;
      returnStr = idValueStr;

      if      ( opType == PP )  UpdateVariableValue( curType, idValue+1, idValueStr, idStr, constantValue );
      else if ( opType == MM )  UpdateVariableValue( curType, idValue-1, idValueStr, idStr, constantValue );
      
    } // else if
    
    else {  // 沒有PP MM []的variable 
      GetVariableValue( returnValue, returnStr, idStr, constantValue );
    } // else
   
  } // if
  
  else if ( mTokenType == CONSTANT ) {
    GetInputVecToken( mTokenStr, mTokenType );
    UpdateCurType( curType, mTokenStr ) ;
    if ( curType == "string" OR curType == "char" ) {
      returnStr = mTokenStr;
      returnStr.erase( returnStr.begin() );
      returnStr.erase( returnStr.end() - 1 );
    } // if
    else if ( curType == "bool" ) {
      if ( mTokenStr == "true" )  returnValue = 1;
      else                        returnValue = 0;
    } // else if
    else {
      returnValue = atof( mTokenStr.c_str() );
    } // else
  } // else if
  
  // '(' expression ')'
  else if ( mTokenType == LP ) {
    GetInputVecToken( mTokenStr, mTokenType );
    Expression_v( curType, executeValue, executeStr );
    PeekInputVecToken( mTokenStr, mTokenType );

    if ( mTokenType == RP ) {
      GetInputVecToken( mTokenStr, mTokenType );
      returnValue = executeValue;
      returnStr = executeStr;
    } // if
   
  } // else if

} // end Parser::Unsigned_Unary_Exp_v()



int main() {
  Parser parser;
  parser.Init();
  int uTestNum = 0 ;
  char ch = '\0';

  scanf( "%d", &uTestNum );
  scanf( "%c", &ch );

  printf( "Our-C running ...\n" ) ;
  printf( "> " );
  

  while ( parser.CheckUserInput() ) {
    printf( "> " );
  } // while
  
  printf( "Our-C exited ..." ) ;
} // main()

