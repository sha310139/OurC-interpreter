

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
  NONE, END_OF_FILE  // ��l��
};

struct Token {
  string tokenStr;
  TokenType tokenType;
} ;

struct ParameterType {
  int address;
  string listType;
  int arrPosition;  // �barray���ĴX�Ӧ�m 
  int functionNum;  // �����O�b����function 
  vector<string> variableValue;
} ;

struct VariableType {
  string variableType;
  string variableName;
  string constant;  // �p�G�S��[], �N�|�O�Ŧr��""
   
  vector<string> variableValue;
  // ���i��O (int, float) , bool , string �T�ث��A
  // ������string�s
 
  int region;
  bool isCallByReference;  // �u��parameter�|�Ψ� 
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
  
  void SetLastTokenLine( int line ) ;  // �]�w�W�@��user_input, �̫�@��token��Line 
  void SetNewTokenLine() ;   // �]�w�̷s��user_input, �Ĥ@��token��line 
  
  int GetLastGetTokenLine() ;
  
  void GetInputBuffer( vector<char> & inputBuffer ) ;
  void ClearInputBuffer() ;

private:
  
  char mNextChar;
  string mCurToken;
  TokenType mType;
  bool mFindEOF;
  
  int mLastTokenLine;  // �W�@��user_input, �̫�@��token��Line 
  int mNewTokenLine;   // �̷s��user_input, �Ĥ@��token��line 

  int mCurCharLine;  // Ū�쪺cur char��Line 
  int mNextCharLine;  // �hŪ�@��next char��Line 
  
  int mGetTokenLine;  // �hŪ�@��next char��Line 
  
  vector<Token> mTmpTokenStr;  // �ΨӦs�ثe�o��input��token
  vector<char> mInputBuffer;
  
  void GetRealToken() ;  // �ΨӤ@�Ӥ@��char�����u����token 
  bool GetNextChar() ;   // �o��U��char 
  void SkipWhiteSpace() ;  // ���Lwhitespace 
  bool SkipComment() ;
  void CheckCaseAndStore() ;
  void GetIdent() ;
  void GetNum() ;
  void GetStringAndChar() ;
  void GetSpecial() ;
  
};

void Scanner::GetToken( string & tokenStr, TokenType & tokenType ) {

  if ( mCurToken == "" ) {  // �N��S���w�s��token 
    GetRealToken() ;
  } // if

  tokenStr = mCurToken ;
  tokenType = mType ;

  string firstChar = "";
  firstChar = firstChar + tokenStr[0];

  Token token;
  token.tokenStr = mCurToken;
  token.tokenType = mType;

  mTmpTokenStr.push_back( token ) ;  // �u�n��GetToken�N�s�_�� 
  
  /*
  if ( tokenType == NONE ) {
    InitLine();
    // �]��unrecognize�u�|�bpeek token�ɵo��
    // �ҥH�n����InitLine 
    throw ( "unrecognized token with first char '" + firstChar + "'\n" ).c_str();
  } // if
  */
  
  mCurToken = "" ;  // ��token��l�� 
  mType = NONE ;  // token��l�� 
  
  mGetTokenLine = mCurCharLine;

} // end Scanner::GetToken()

void Scanner::PeekToken( string & tokenStr, TokenType & tokenType ) {

  if ( mCurToken == "" ) {  // �N��S���w�s��token 
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
  mLastTokenLine = 0;  // �̫�@��"get"��token��Line
  mNewTokenLine = 1;   // user input���Ĥ@��token��Line 
  mCurCharLine = 1;
  mNextCharLine = 1;
} // end Scanner::Init()

void Scanner::GetRealToken() {
  // ����EOF : �S���\��token  ��  unrecognize error : ���\��token����error 
  
  mCurToken = "";
  mType = NONE ;
  bool checkWhiteSpace = true;
  
  if ( mNextChar == '\0' )  GetNextChar();

  if ( mFindEOF )  {
    mCurToken = "" ;
    mType = END_OF_FILE ;
    // throw "EOF exception" ;  // �b�W�@����token�ɴN���EOF 
  } // if

  SkipWhiteSpace() ;
  if ( mFindEOF ) {
    mCurToken = "" ;
    mType = END_OF_FILE ;
    // throw "EOF exception" ;  // �bskip whitespace�N���EOF 
  } // if

  if ( SkipComment() ) {
    // �p�G�S��get DIV�Ÿ�, ��get comment 
    // �N�ˬd���LComment���U�@��, �O�_��white space
    // �p�G��get�N���ˬd 
    while ( checkWhiteSpace AND ( mNextChar == ' ' OR mNextChar == '\t' OR mNextChar == '\n' ) ) {
      SkipWhiteSpace() ;
      if ( mFindEOF ) {
        mCurToken = "" ;
        mType = END_OF_FILE ;
        // throw "EOF exception" ;  // �bskip whitespace�N���EOF 
      } // if

      if ( NOT SkipComment() ) checkWhiteSpace = false;
      // �p�G�S��get DIV�Ÿ�, ��get comment 
      // �N�ˬd���LComment���U�@��, �O�_��white space
      // �p�G��get�N���ˬd 
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
  // mCurToken[0]�w�g�OID���Y
  // ��ID�ѤU��Ū�i��, Ū��DID��char���� 
  // �]�mtype 

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

    else  ending = true ;  // �J��EOF 
  } // while
  
  // ���ަ��S���J��EOF
  // ���ܤַ|���@��ID��char���Y
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
  // mCurToken[0]�w�g�ONUM���Y (digit��'.')
  // ��NUM�ѤU��Ū�i��, Ū��DNUM��char���� 

  bool ending = false, hasPoint = false ;

  if ( mCurToken[0] == '.' ) {
    hasPoint = true ;
  } // if 

  while ( NOT ending ) {
      
    if ( GetNextChar() ) {

      if ( mNextChar >= '0' AND mNextChar <= '9' ) {
        mCurToken = mCurToken + mNextChar ;
      } // if
      else if ( !hasPoint AND mNextChar == '.' ) {    // �٨S�X�{'.', �M��Ū��F�Ĥ@��'.' 
        mCurToken = mCurToken + mNextChar ;
        hasPoint = true ;
      } // else if

      else if ( hasPoint AND mNextChar == '.' ) {    // �w�g��'.'�F���S�X�{�ĤG��'.' 
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
  char findChar = mNextChar;  // �n��쪺char
                              // �p�G�O�r��N�O", �O�r���N�O' 
  
  while ( NOT ending ) {
    
    if ( GetNextChar() ) {
      if ( !hasBackslash AND mNextChar == '\\' ) {  // �J��ϱ׽u, �B�e���S���ϱ׽u 
        hasBackslash = true;
        mCurToken = mCurToken + mNextChar ;
      } // if
      
      else if ( hasBackslash AND mNextChar == '\\' ) {  // �J��ϱ׽u, �B�e���]�O�ϱ׽u  >> \\ 
        hasBackslash = false;
        mCurToken = mCurToken + mNextChar ;
      } // else if
      
      else if ( !hasBackslash AND mNextChar == findChar ) {   // �e�����O�ϱ׽u + ���"��'�F  >> ���� 
        mCurToken = mCurToken + mNextChar ;
        ending = true;
        GetNextChar();
      } // else if
   
      else if ( hasBackslash ) {   // �e���O�ϱ׽u+���"��'�ΧO���Ÿ� >> �٨S���� 
        mCurToken = mCurToken + mNextChar ;
        hasBackslash = false; 
      } // else if
   
      else {
        mCurToken = mCurToken + mNextChar ;
      } // else
   
    } // if

    else {
      mType = NONE;
      ending = true ;  // �J��EOF, �٨S�J��"��' >> error 
    } // else 

  } // while

} // end Scanner::GetStringAndChar()

void Scanner::GetSpecial() {
  // mCurToken[0]�w�g�OSpecial���Y
  // ��Special�ѤU��Ū�i��, Ū��DSpecial��char���� 

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
    
    // else : �Y�ĤG�Ӥ��O'='��'+', �N�u��'+'
    
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
    
    // else : �Y�ĤG�Ӥ��O'='��'-', �N�u��'-', ������ 
    
  } // else if

  
  else if ( mCurToken == "*" ) {  // maybe '*' OR '*='
    mType = MULT ;
    
    if ( GetNextChar() ) {
      
      if ( mNextChar == '=' ) {
        mType = TE ;
        mCurToken = mCurToken + mNextChar ;
        GetNextChar() ;
      } // if
      
      // �Y�ĤG�Ӥ��O'=', �N�u��'*'

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
      
      // �Y�ĤG�Ӥ��O'=', �N�u��'/'

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
      
      // �Y�ĤG�Ӥ��O'=', �N�u��'%'

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
      
      // �Y�ĤG�Ӥ��O'&', �N�u��'&'

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
      
      // �Y�ĤG�Ӥ��O'|', �N�u��'|'

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
      
      // �Y�ĤG�Ӥ��O'=', �N�u��'!'

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
      
      // �Y�ĤG�Ӥ��O'=', �N�u��'='

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
      
      // �Y�ĤG�Ӥ��O'='��'<', �N�u��'<'

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
      
      // �Y�ĤG�Ӥ��O'='��'>', �N�u��'>'

    } // if
    
  } // else if
  
  else {  // �i��error 
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
  // �ΨӽT�{�O�_��comment�ø��L
  // �Y���'/'�o�S���ĤG��'/', �|�N'/'�Ÿ��s��Token
  // �� CheckCaseAndStore() �B�z�o��token 
  
  if ( mNextChar == '/' ) {
    mCurToken = mCurToken + mNextChar;

    if ( GetNextChar() ) {
      if ( mNextChar == '/' ) {  // �O���� 
        // �����bmCurToken[0]�̪�'/'����
        mCurToken = "" ; 
        while ( mNextChar != '\n' ) {
          if ( NOT GetNextChar() )  return true;
        } // while
      } // if 
      else return false; // ���get DIV�Ÿ��n�B�z, �S��skipComment 
    } // if

    // �Y�ĤG�Ӥ��O'/'
    // ��ܫe����'/'�ODIV�Ÿ�
    // �᭱�O�Y��token���Y��whitespace 
      
  } // if

  return true;
  
} // end Scanner::SkipComment()

void Scanner::CheckCaseAndStore() {
  
  // �w�gŪ�Jtoken�Y
  // �ΨӽT�{case�éI�sŪ�Jtoken��function 
  // �Y�J��unrecognize error , �Ntype�নNONE 

  if ( mCurToken == "/" ) {  // maybe '/' OR '/='
    mType = DIV ;
    
    if ( mNextChar == '=' ) {
      mType = DE ;
      mCurToken = mCurToken + mNextChar ;
      GetNextChar() ;
    } // if
      
    // �Y�ĤG�Ӥ��O'=', �N�u��'/'  
  } // if  
  
  else if ( ( mNextChar >= 'A' AND mNextChar <= 'Z' ) OR
            ( mNextChar >= 'a' AND mNextChar <= 'z' ) ) {
    mCurToken = mCurToken + mNextChar ;
    GetIdent() ;  // ��ѤU��Ū��
  } // else if
  
  else if ( mNextChar == '.' OR ( mNextChar >= '0' AND mNextChar <= '9' ) ) {  //  Number
    mCurToken = mCurToken + mNextChar ;
    mType = CONSTANT ;
    GetNum() ;
    if ( mCurToken.size() == 1 && mCurToken[0] == '.' ) {  // �u���@��'.', ���error 
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
  // �q�`�o��error���ɭԳ��٨Speek�U�@��token 
  // �p�G�Ounexpect, �i��u��peek�ثe��token
  // �p�G�Oundefine, ��get ID �o��token 
  // ���ޫ��CurCharLine���|��n == error line 
    
  // ���i��error��token�O�٨Sget��

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
// �]�w�W�@��user_input, �̫�@��token��Line 
  mLastTokenLine = line;
} // Scanner::SetLastTokenLine()

void Scanner::SetNewTokenLine() {
// �]�w�̷s��user_input, �Ĥ@��token��line 
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

  vector<VariableType> mTempIDList;  // �ΨӦs','���j��ID, ��Ū���~��@���s�_�� 

  vector<char> mInputBuffer;
  vector<Token> mUserInputVec;
  
  vector<FunctionType> mCallStack;

  FunctionType mCurFunction;  // �ΨӦs�ثeŪ�J��function
   

  string mTokenStr;
  TokenType mTokenType;
  
  bool mIsFunction;  // definition�Ofunction, �n�h�ˬdparameter
  
  bool mPassValueIsAddress;  // �����{�b�O���Oget�@��address���Ѽ� 
  
  ParameterType mTmpParameter; 
  
  int mUserInputVecNum;
  int mCurVariableRegion; // �����ثe���ܼƶդO�d�� 
  int mSkipExp;  // �Ψӳ]�w�O�_�n���L��eexp(if, while ...) 
  int mIsCout;
  int mInParentheses;
  int mExecuteFunction; // �n����function, �ݭn��function�X�ӭp��� 


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
    mScanner.InitLine();  // �T�w�}�l��line >> �ثe��� - �W�@��input���̫�@�� 
    mScanner.ClearFunctionVec();  // ��token���k�s
    User_Input() ;
  } catch ( const char * msg ) {

    if ( strcmp( msg, "EOF exception" ) == 0 OR strcmp( msg, "Done" ) == 0 )
      return false;
    
    int curLine = mScanner.GetCurLine();
    mScanner.SetLastTokenLine( curLine );
    
    if ( curLine == 0 ) curLine++;
    printf( "Line %d : ", curLine ) ;
    printf( "%s", msg ) ;
    
    // �o��error���ܳ��Opeek���q�o�{
    // �|�bget token�᭱
    // �p�G���`����u�|�����̫�@��get token��line 
    // �ҥH�o�Ϳ��~�n���s�]�w�@��Line 
    
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
          mCurFunction.variableList[i].variableValue.clear(); // �i��O���s�ŧi 
          mCurFunction.variableList[i].variableValue.resize( size, "0" );
        } // if
        else {
          mCurFunction.variableList[i].variableValue.clear(); // �i��O���s�ŧi 
          mCurFunction.variableList[i].variableValue.resize( 1, "0" );
        } // else

        return;
      } // if
    } // for
  
    // �p�G�S���bidList�̧�쪺��
    // �N�s�W�@�� 
    // �n���Ӷ��Ǵ��J 
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
        // 4, 3, 2, 1, 0�o�˱�
        mCurFunction.variableList.insert( mCurFunction.variableList.begin() + i, newVariable );
        return;
      } // else if
    } // for
  
    // �Y�ƧǬO�̤j���N��b�̫᭱ 
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
        mVariableList[i].variableValue.clear(); // �i��O���s�ŧi 
        mVariableList[i].variableValue.resize( size, "0" );
      } // if
      else {
        mVariableList[i].variableValue.clear(); // �i��O���s�ŧi 
        mVariableList[i].variableValue.resize( 1, "0" );
      } // else
      
      if ( mCurVariableRegion == 0 ) {
        printf( "New definition of %s entered ...\n", variableStr.c_str() );
      } // if

      return;
    } // if
  } // for
  
  // �p�G�S���bidList�̧�쪺��
  // �N�s�W�@�� 
  // �n���Ӷ��Ǵ��J 
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
      // 4, 3, 2, 1, 0�o�˱�
      mVariableList.insert( mVariableList.begin() + i, newVariable );
      return;
    } // else if
  } // for
  
  // �Y�ƧǬO�̤j���N��b�̫᭱ 
  mVariableList.push_back( newVariable ) ;

} // end Parser::StoreVariable()

void Parser::GetVariableValue( double & returnValue, string & returnStr,
                               string variableStr, double constantValue ) {
  
  // �w�g�ˬd�Lundefined, ���|���䤣�쪺���D
  if ( mExecuteFunction > 0 ) {
    
    for ( int i = 0 ; i < mCurFunction.parameter.size() ; i++ ) {
      // ����Ѽ�
      if ( variableStr == mCurFunction.parameter[i].variableName ) {
        // �p�G�O�ثefunction���Ѽ�, �Ƕi�sfunction�� 
        // �B�ثe�ѼƬOreference��, ��ܦ����e����}�T�� 
        // �sfunction���o�ӰѼƭn�O��, �ثefunction�ѼƤ��e�s����}�T��
        // �����i��ثefunction�ѼƬO���arr
        // �s���u�narray���䤤�@�Ӥ���
        // �ҥH arrPosition�i�ण�P 
    
        // �Y�ثe�ѼƤ��Oreference, �N�O���ثe�Ѽƪ���}�T�� 
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
          // ���}�̮��� 
        } // if
        
        if ( mCurFunction.parameter[i].constant == "" ) {  // �o���ܼƫŧi���Oarray 
          if ( constantValue == -1.0 ) {  // ���O�narray 
            // �w�]�C���ܼƳ�����l��
            returnValue = atof( mCurFunction.parameter[i].variableValue[0].c_str() );
            returnStr = mCurFunction.parameter[i].variableValue[0];
          } // if
          else {  // �ŧi�O�ܼ�, ���nget array���� 
            throw "Not array error\n";
          } // else
        } // if
        else {  // �ŧi�Oarray 
          if ( constantValue >= 0 ) {
            returnValue = atof( mCurFunction.parameter[i].variableValue[constantValue].c_str() );
            returnStr = mCurFunction.parameter[i].variableValue[constantValue] ;
          } // if
          else if ( constantValue == -1.0 ) {  // �n���array (�u���ǰѼƷ|�Ψ�) 
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
      // �A��function�̫ŧi���ܼ� 
      // �ݭn����ϰ�d��, �]�����i�঳if 
      if ( variableStr == mCurFunction.variableList[i].variableName AND
           mCurFunction.variableList[i].region <= mCurVariableRegion ) {

        // �p�G�O�ثefunction���ܼ�, �Ƕi�sfunction�� 
        // �sfunction���o�ӰѼƭn�O��, �ثefunction�ܼƪ���e��T 
        mTmpParameter.address = i;
        mTmpParameter.listType = "variableList_F";

        if ( mExecuteFunction == 0 ) mTmpParameter.functionNum = 0;
        else                         mTmpParameter.functionNum = mExecuteFunction-1;

        if ( mCurFunction.variableList[i].constant == "" ) {  // �o���ܼƫŧi���Oarray 
          if ( constantValue == -1.0 ) {  // ���O�narray 
            // �w�]�C���ܼƳ�����l�� 
            returnValue = atof( mCurFunction.variableList[i].variableValue[0].c_str() );
            returnStr = mCurFunction.variableList[i].variableValue[0];
          } // if
        
          else {  // �ŧi�O�ܼ�, ���nget array���� 
            throw "Not array error\n";
          } // else
        } // if
        else {  // �ŧi�Oarray 
          if ( constantValue >= 0 ) {
            mTmpParameter.arrPosition = constantValue;
            returnValue = atof( mCurFunction.variableList[i].variableValue[constantValue].c_str() );
            returnStr = mCurFunction.variableList[i].variableValue[constantValue] ;
          } // if
          else if ( constantValue == -1.0 ) {  // �n���array (�u���ǰѼƷ|�Ψ�) 
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
      // ����� 
      if ( variableStr == mVariableList[i].variableName AND mVariableList[i].region == 0 ) {
        mTmpParameter.address = i;
        mTmpParameter.listType = "variableList";
        mTmpParameter.functionNum = 0;
        
        if ( mVariableList[i].constant == "" ) {  // �o���ܼƫŧi���Oarray 
          if ( constantValue == -1.0 ) {  // ���O�narray 
            // �w�]�C���ܼƳ�����l�� 
            returnValue = atof( mVariableList[i].variableValue[0].c_str() );
            returnStr = mVariableList[i].variableValue[0];
          } // if
        
          else {  // �ŧi�O�ܼ�, ���nget array���� 
            throw "Not array error\n";
          } // else
        } // if
        else {  // �ŧi�Oarray 
          if ( constantValue >= 0 ) {
            mTmpParameter.arrPosition = constantValue;
            returnValue = atof( mVariableList[i].variableValue[constantValue].c_str() );
            returnStr = mVariableList[i].variableValue[constantValue] ;
          } // if
          else if ( constantValue == -1.0 ) {  // �n���array (�u���ǰѼƷ|�Ψ�) 
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
      if ( mVariableList[i].constant == "" ) {  // �o���ܼƫŧi���Oarray 
        if ( constantValue == -1.0 ) {  // ���O�narray 
          // �w�]�C���ܼƳ�����l�� 
          returnValue = atof( mVariableList[i].variableValue[0].c_str() );
          returnStr = mVariableList[i].variableValue[0];
        } // if
        
        else {  // �ŧi�O�ܼ�, ���nget array���� 
          throw "Not array error\n";
        } // else
      } // if
      else {  // �ŧi�Oarray 
        if ( constantValue >= 0 ) {
          mTmpParameter.arrPosition = constantValue;
          returnValue = atof( mVariableList[i].variableValue[constantValue].c_str() );
          returnStr = mVariableList[i].variableValue[constantValue] ;
        } // if
        else if ( constantValue == -1.0 ) {  // �n���array (�u���ǰѼƷ|�Ψ�) 
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
      // ����Ѽ�
      if ( variableStr == mCurFunction.parameter[i].variableName ) {
        if ( mCurFunction.parameter[i].constant == "" ) {  // �o���ܼƫŧi���Oarray 
          if ( constantValue == -1.0 ) {  // ���O�narray 
            if ( mCurFunction.parameter[i].variableType == "string" OR
                 mCurFunction.parameter[i].variableType == "char" ) {
              mCurFunction.parameter[i].variableValue[0] = newValueStr;
            } // if
            else if ( mCurFunction.parameter[i].variableType == "int" ) {
              int tmp = newValue;
              mCurFunction.parameter[i].variableValue[0] = FloatToStr( tmp );
            } // else if
            else {  // bool�Τ@��0��1�s 
              mCurFunction.parameter[i].variableValue[0] = FloatToStr( newValue );
            } // else
          } // if
        
          else {  // �ŧi�O�ܼ�, ���nget array���� 
            throw "Not array error\n";
          } // else
        } // if
        else {  // �ŧi�Oarray 
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
      // �A��function�̫ŧi���ܼ� 
      // �ݭn����ϰ�d��, �]�����i�঳if
      if ( variableStr == mCurFunction.variableList[i].variableName AND
           mCurFunction.variableList[i].region <= mCurVariableRegion ) {

        if ( mCurFunction.variableList[i].constant == "" ) {  // �o���ܼƫŧi���Oarray 
          if ( constantValue == -1.0 ) {  // ���O�narray 
            // �w�]�C���ܼƳ�����l�� 
            if ( mCurFunction.variableList[i].variableType == "string" OR
                 mCurFunction.variableList[i].variableType == "char" ) {
              mCurFunction.variableList[i].variableValue[0] = newValueStr;
            } // if
            else if ( mCurFunction.variableList[i].variableType == "int" ) {
              int tmp = newValue;
              mCurFunction.variableList[i].variableValue[0] = FloatToStr( tmp );
            } // else if
            else {  // bool�Τ@��0��1�s 
              mCurFunction.variableList[i].variableValue[0] = FloatToStr( newValue );
            } // else
          } // if
        
          else {  // �ŧi�O�ܼ�, ���nget array���� 
            throw "Not array error\n";
          } // else
        } // if
        else {  // �ŧi�Oarray 
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
      // ����� 
      if ( variableStr == mVariableList[i].variableName AND mVariableList[i].region == 0 ) {

        if ( mVariableList[i].constant == "" ) {  // �o���ܼƫŧi���Oarray 
          if ( constantValue == -1.0 ) {  // ���O�narray 
            // �w�]�C���ܼƳ�����l�� 
            if ( mVariableList[i].variableType == "string" OR mVariableList[i].variableType == "char" ) {
              mVariableList[i].variableValue[0] = newValueStr;
            } // if
            else if ( mVariableList[i].variableType == "int" ) {
              int tmp = newValue;
              mVariableList[i].variableValue[0] = FloatToStr( tmp );
            } // else if
            else {  // bool�Τ@��0��1�s 
              mVariableList[i].variableValue[0] = FloatToStr( newValue );
            } // else
          } // if
        
          else {  // �ŧi�O�ܼ�, ���nget array���� 
            throw "Not array error\n";
          } // else
        } // if
        else {  // �ŧi�Oarray 
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

      if ( mVariableList[i].constant == "" ) {  // ���Oarray 
        if ( constantValue == -1 ) {
          if ( mVariableList[i].variableType == "string" OR mVariableList[i].variableType == "char" ) {
            mVariableList[i].variableValue[0] = newValueStr;
          } // if
          else if ( mVariableList[i].variableType == "int" ) {
            int tmp = newValue;
            mVariableList[i].variableValue[0] = FloatToStr( tmp );
          } // else if
          else {  // bool�Τ@��0��1�s 
            mVariableList[i].variableValue[0] = FloatToStr( newValue );
          } // else
        } // if
        else {
          throw "Not array error\n";
        } // else
      } // if
      else {  // �Oarray 
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
          // �ݧ� 
        } // else
      } // else

      return;
    } // if
  } // for 
} // Parser::UpdateVariableValue()


void Parser::GetReferenceValue( VariableType & curV, ParameterType curP ) {

  if ( mCallStack.size() > 0 ) {  // �ݭnupdate���ȬOfunction�̪� 

    FunctionType curF = mCallStack[curP.functionNum];
    
    if ( curP.listType == "parameter" ) {  // �O���ӶǶi�o��function��parameter
      if ( curP.arrPosition == -1 ) {  // �Oarray�ΨS���ŧiarray���ܼ�
        curV.variableValue = curF.parameter[curP.address].variableValue;
        // ���vec�ƻs�^�h 
      } // if
      else {  // �Oarray���u�����䤤�@�ӭ� 
        curV.variableValue[0] = curF.parameter[curP.address].variableValue[curP.arrPosition];
        // �u�⨺�ӭȽƻs�^�h 
      } // else
    } // if
    else if ( curP.listType == "variableList_F" ) {  // �Ofunction�̭���variable list
      if ( curP.arrPosition == -1 ) {  // �Oarray�ΨS���ŧiarray���ܼ� 
        curV.variableValue = curF.variableList[curP.address].variableValue;
        // ���vec�ƻs�^�h 
      } // if
      else {  // �Oarray���u�����䤤�@�ӭ� 
        curV.variableValue[0] = curF.variableList[curP.address].variableValue[curP.arrPosition];
        // �u�⨺�ӭȽƻs�^�h 
      } // else
    } // else if
    else {  // ��l��variable list 
      if ( curP.arrPosition == -1 ) {  // �Oarray�ΨS���ŧiarray���ܼ� 
        curV.variableValue = mVariableList[curP.address].variableValue;
        // ���vec�ƻs�^�h 
      } // if
      else {  // �Oarray���u�����䤤�@�ӭ� 
        curV.variableValue[0] = mVariableList[curP.address].variableValue[curP.arrPosition];
        // �u�⨺�ӭȽƻs�^�h 
      } // else
    } // else 
    
    mCallStack[curP.functionNum] = curF;
    
  } // if
  
  else {  // �ݭnupdate���ȬO��l��variable list 
    if ( curP.arrPosition == -1 ) {  // �Oarray�ΨS���ŧiarray���ܼ� 
      curV.variableValue = mVariableList[curP.address].variableValue; 
      // ���vec�ƻs�^�h 
    } // if
    else {  // �Oarray���u�����䤤�@�ӭ� 
      curV.variableValue[0] = mVariableList[curP.address].variableValue[curP.arrPosition];
      // �u�⨺�ӭȽƻs�^�h 
    } // else
  } // else
  
} // Parser::GetReferenceValue()

void Parser::UpdateReferenceValue( VariableType curV, ParameterType curP, double constantValue ) {

  
  if ( mCallStack.size() > 0 ) {  // �ݭnupdate���ȬOfunction�̪� 

    FunctionType curF = mCallStack[curP.functionNum];

    if ( curP.listType == "parameter" ) {  // �O�e�@��function��parameter
      if ( curP.arrPosition == -1 ) {  // �Oarray�ΨS���ŧiarray���ܼ� 
        if ( constantValue == -1 ) {
          curF.parameter[curP.address].variableValue[0] = curV.variableValue[0];
        } // if
        else {
          curF.parameter[curP.address].variableValue[constantValue] = curV.variableValue[constantValue];
        } // else
      } // if
      else {  // �Oarray���u�����䤤�@�ӭ� 
        curF.parameter[curP.address].variableValue[curP.arrPosition] = curV.variableValue[0];
        // �u�⨺�ӭȽƻs�^�h 
      } // else
      
    } // if
    

    else if ( curP.listType == "variableList_F" ) {  // �Ofunction�̭���variable list
      if ( curP.arrPosition == -1 ) {  // �Oarray�ΨS���ŧiarray���ܼ� 
        if ( constantValue == -1 ) {
          curF.variableList[curP.address].variableValue[0] = curV.variableValue[0];
        } // if
        else {
          curF.variableList[curP.address].variableValue[constantValue] = curV.variableValue[constantValue];
        } // else
      } // if
      else {  // �Oarray���u�����䤤�@�ӭ� 
        curF.variableList[curP.address].variableValue[curP.arrPosition] = curV.variableValue[0];
        // �u�⨺�ӭȽƻs�^�h 
      } // else
    } // else if

    else {  // ��l��variable list 
      if ( curP.arrPosition == -1 ) {  // �Oarray�ΨS���ŧiarray���ܼ� 
        if ( constantValue == -1 ) {
          mVariableList[curP.address].variableValue[0] = curV.variableValue[0];
        } // if
        else {
          mVariableList[curP.address].variableValue[constantValue] = curV.variableValue[constantValue];
        } // else
      } // if
      else {  // �Oarray���u�����䤤�@�ӭ� 
        mVariableList[curP.address].variableValue[curP.arrPosition] = curV.variableValue[0];
        // �u�⨺�ӭȽƻs�^�h 
      } // else
    } // else 
    
    mCallStack[curP.functionNum] = curF;
    
  } // if
  
  else {  // �ݭnupdate���ȬO��l��variable list 
    if ( curP.arrPosition == -1 ) {  // �Oarray�ΨS���ŧiarray���ܼ� 
      if ( constantValue == -1 ) {
        mVariableList[curP.address].variableValue[0] = curV.variableValue[0];
      } // if
      else {
        mVariableList[curP.address].variableValue[constantValue] = curV.variableValue[constantValue];
      } // else
    } // if
    else {  // �Oarray���u�����䤤�@�ӭ� 
      mVariableList[curP.address].variableValue[curP.arrPosition] = curV.variableValue[0];
      // �u�⨺�ӭȽƻs�^�h 
    } // else
  } // else
  
} // Parser::UpdateReferenceValue()



void Parser::GetVariableType( string & idType, string variableStr ) {
  
  if ( mExecuteFunction > 0 ) {
    
    for ( int i = 0 ; i < mCurFunction.parameter.size() ; i++ ) {
      // ����Ѽ�
      if ( variableStr == mCurFunction.parameter[i].variableName ) {
        idType = mCurFunction.parameter[i].variableType;
        return;
      } // if
    } // for
    
    for ( int i = 0 ; i < mCurFunction.variableList.size() ; i++ ) {
      // �A��function�̫ŧi���ܼ� 
      // �ݭn����ϰ�d��, �]�����i�঳if 
      if ( variableStr == mCurFunction.variableList[i].variableName AND
           mCurFunction.variableList[i].region <= mCurVariableRegion ) {
        idType = mCurFunction.variableList[i].variableType;
        return;
      } // if
    } // for
    
    
    for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
      // ����� 
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
  
  // �p�G�S���bidList�̧�쪺��
  // �N���Ӷ��Ǵ��J 

  printf( "Definition of %s() entered ...\n", mCurFunction.functionName.c_str() );
  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    if ( mCurFunction.functionName < mFunctionList[i].functionName ) {
      mFunctionList.insert( mFunctionList.begin() + i, mCurFunction );
      return;
    } // if
  } // for
  
  // �S���N��b�̫᭱ 
  mFunctionList.push_back( mCurFunction ) ;

} // end Parser::StoreFunction()

void Parser::SetExecuteFunction( string functionName, vector<ParameterType> & tmpValueList ) {

  if ( mExecuteFunction > 0 ) {  // ���b����function, �̭��S�n����function
    mCallStack.push_back( mCurFunction );  // ��ثe��function�s�_�� 
  } // if
  
  mExecuteFunction++;
  
  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    if ( functionName == mFunctionList[i].functionName ) {
      // �p�G�P�@��statement�����ӥH�Wfunction, �S�k�s�|�X�� 
      mCurFunction = mFunctionList[i] ;
      mCurFunction.pc = 0;  // �C�����s���n�k�s
      mCurFunction.variableList.clear();  // variable�έ��s�ŧi�� 
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
  
  if ( mExecuteFunction > 0 ) {  // �n�~�������S���槹��function 
    mCurFunction = mCallStack[mCallStack.size() - 1];
    mCallStack.pop_back();  // ���誺function pop�^�� 
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
  // �h�����޸�
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
  // �h�����޸�
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
  if ( mTokenType == END_OF_FILE ) {  // �N�����ӭn���U��token, ���S�� 
    throw "EOF exception";
  } // if
  
  if ( mTokenType == INPUTBUFFERAPPEND ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.ClearInputBuffer();
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    if ( mTokenType == END_OF_FILE ) {  // �N�����ӭn���U��token, ���S�� 
      throw "EOF exception";
    } // if
    
    while ( mTokenType != ENDINPUTBUFFERAPPEND ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      if ( mTokenType == END_OF_FILE ) {  // �N�����ӭn���U��token, ���S�� 
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
  // �n�h��EndInputBufferAppend��̫�@�ӦhŪ���r��
  
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
      else if ( !hasPoint AND mInputBuffer[i] == '.' ) {    // �٨S�X�{'.', �M��Ū��F�Ĥ@��'.' 
        returnStr = returnStr + mInputBuffer[i] ;
        hasPoint = true ;
      } // else if
      else if ( hasPoint AND mInputBuffer[i] == '.' ) {    // �w�g��'.'�F���S�X�{�ĤG��'.' 
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
      mUserInputVecNum++;  // ����R��token, �]����while 
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
  
  // ��Ĥ@��token 
  mScanner.PeekToken( mTokenStr, mTokenType );
  if ( mTokenType == END_OF_FILE ) {  // �N�����ӭn���U��token, ���S�� 
    throw "EOF exception";
  } // if    
  
  // �ˬd�O�_�Odefinition���Y 
  if ( mTokenType == VOID OR mTokenType == INT OR mTokenType == CHAR OR
       mTokenType == FLOAT OR mTokenType == STRING OR mTokenType == BOOL ) {
    Definition();
  } // if
  
  // �ˬd�O�_�Ostatement���Y 
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

  // ��L���p : unexpected error 
  else {
    throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else

} // end Parser::User_Input()

void Parser::Definition() {
// �o�̪�ID�O�ŧi��ID, �����ˬd�O�_�s�b 
// definition : VOID Identifier function_definition_without_ID |
//              type_specifier Identifier function_definition_or_declarators

  string tmpIdStr = "";

  string tmpVariableType = "";
  string tmpVariableName = "";
   
  mScanner.PeekToken( mTokenStr, mTokenType );
  if ( mTokenType == END_OF_FILE ) {  // �N�����ӭn���U��token, ���S�� 
    throw "EOF exception";
  } // if
  
  if ( mTokenType == VOID ) {  // �@�w�O�ŧifunction 
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mCurFunction.functionType = mTokenStr;
    mScanner.PeekToken( mTokenStr, mTokenType );
    if ( mTokenType == END_OF_FILE ) {  // �N�����ӭn���U��token, ���S�� 
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
            mTokenType == STRING OR mTokenType == BOOL ) {  // �i��O�ŧifunction or �ܼ� 
    Type_Specifier();
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    tmpVariableType = mTokenStr;  // �ŧi��type (int, char, ...) 
    mScanner.PeekToken( mTokenStr, mTokenType );
    if ( mTokenType == END_OF_FILE ) {  // �N�����ӭn���U��token, ���S�� 
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
  if ( mTokenType == END_OF_FILE )  // �N�����ӭn���U��token, ���S�� 
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
  if ( mTokenType == END_OF_FILE ) {  // �N�����ӭn���U��token, ���S�� 
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
// �o�̪�ID�O�ŧi��ID, �����ˬd�O�_�s�b 
// rest_of_declarators : [ '[' Constant ']' ] 
//                       { ',' Identifier [ '[' Constant ']' ] } ';'
// ���ޫ�˳��ܤ֦���';'

  VariableType tmpID;
  
  // ����o�쪺���O�MID�W����assign   
  tmpID.variableName = tmpVariableName;
  tmpID.variableType = tmpVariableType;
  tmpID.constant = "";
  tmpID.region = mCurVariableRegion; 
  
  mScanner.PeekToken( mTokenStr, mTokenType );
  // �N�����ӭn���U��token, ���S�� 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";

  // [ '[' Constant ']' ], ���@�w�n�� 
  if ( mTokenType == MLP ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == CONSTANT ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      tmpID.constant = mTokenStr; // �ˬd��Oarray
      mScanner.PeekToken( mTokenStr, mTokenType );
      // �N�����ӭn���U��token, ���S�� 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
      if ( mTokenType == MRP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        mScanner.PeekToken( mTokenStr, mTokenType ) ; // Ū�U�@��token, �ݬO','�٬O';'
        // �]���@�w�n��';', �ҥH�n�ˬd�O�_EOF 
        // �N�����ӭn���U��token, ���S�� 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";
        
      } // if
      else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // if
  
  mTempIDList.push_back( tmpID ) ;  // ��ŧi��ID�s�i�h 
  
  // { ',' Identifier [ '[' Constant ']' ] }
  // ���@�w�n�� 
  while ( mTokenType == COMMA ) {

    tmpID.variableType = "";
    tmpID.variableName = "";
    tmpID.constant = "";
    tmpID.region = mCurVariableRegion; 

    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType ) ;
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    
    if ( mTokenType == ID ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;

      tmpID.variableName = mTokenStr;
      tmpID.variableType = tmpVariableType;

      mScanner.PeekToken( mTokenStr, mTokenType ) ; // Ū�U��token, �ݬO'['�٬O';', �]�i�H�O',' 
      // �]���@�w�n��';', �ҥH�n�ˬd�O�_EOF 
      // �N�����ӭn���U��token, ���S�� 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
      
      // [ '[' Constant ']' ], ���@�w�n�� 
      if ( mTokenType == MLP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        mScanner.PeekToken( mTokenStr, mTokenType ) ;
        // �N�����ӭn���U��token, ���S�� 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";
        
        if ( mTokenType == CONSTANT ) {
          mScanner.GetToken( mTokenStr, mTokenType ) ;
          tmpID.constant = mTokenStr;
          mScanner.PeekToken( mTokenStr, mTokenType ) ;
          // �N�����ӭn���U��token, ���S�� 
          if ( mTokenType == END_OF_FILE )  throw "EOF exception";
          if ( mTokenType == MRP ) {
            mScanner.GetToken( mTokenStr, mTokenType ) ;
            mScanner.PeekToken( mTokenStr, mTokenType ) ; // Ū�U�@��token, �ݬO','�٬O';'
            // �]���@�w�n��';', �ҥH�n�ˬd�O�_EOF 
            // �N�����ӭn���U��token, ���S�� 
            if ( mTokenType == END_OF_FILE )  throw "EOF exception";
            
            // �p�G�O','�N�~��]�j��
            // �O';'�N�X�j�� 
          } // if
          else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
          
        } // if
        else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
        
      } // if
      
      mTempIDList.push_back( tmpID ) ;  // ��ŧi��ID�s�i�h 
      // �p�G���O'[', �i��O','��';' 
      // �p�G�O','�N�~��]�j��
      // �O';'�N�X�j�� 
      
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
    
  } // while
  
  // �̫�@��token�O';' , �@�w�n�� 
  if ( mTokenType == SCLN ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
  } // if
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();

  for ( int i = 0 ; i < mTempIDList.size() ; i++ ) {
    StoreVariable( mTempIDList[i].variableType, mTempIDList[i].variableName, mTempIDList[i].constant );
  } // for

  mTempIDList.clear();  // �s�L���N�M�� 

} // end Parser::Rest_Of_Declarators()

void Parser::Function_Definition_Without_ID() {
// function_definition_without_ID : 
//  '(' [ VOID | formal_parameter_list ] ')' compound_statement

  mScanner.PeekToken( mTokenStr, mTokenType );
  // �N�����ӭn���U��token, ���S�� 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  
  // '(' [ VOID | formal_parameter_list ] ')' compound_statement
  if ( mTokenType == LP ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // �i��O [ VOID | Type_Specifier ]
    // �άO')' >> �@�w�n�� 
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == VOID ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
      // ')' >> �@�w�n�� 
      // �N�����ӭn���U��token, ���S�� 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    } // if
    else if ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
              mTokenType == STRING OR mTokenType == BOOL ) {
      Formal_Parameter_List();
      mScanner.PeekToken( mTokenStr, mTokenType );
      // ')' >> �@�w�n�� 
      // �N�����ӭn���U��token, ���S�� 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    } // else if
 
    if ( mTokenType == RP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      Compound_Statement(); 
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
    // �� [ VOID | Type_Specifier ] �S�� ')' 
    // �ΨS�� [ VOID | Type_Specifier ] �]�S�� ')' 
 
  } // if 
  else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();

} // end Parser::Function_Definition_Without_ID()

void Parser::Formal_Parameter_List() {
// �o�̪�ID�O�ŧi��ID, �����ˬd�O�_�s�b 
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
  // �N�����ӭn���U��token, ���S�� 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  if ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
       mTokenType == STRING OR mTokenType == BOOL ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    tmpID.variableType = mTokenStr;
    mScanner.PeekToken( mTokenStr, mTokenType );  // �i��O'&'��ID 
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";

    // [ '&' ] ���@�w�n�� 
    if ( mTokenType == BITAND ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      tmpID.isCallByReference = true;
      mScanner.PeekToken( mTokenStr, mTokenType ); // �U�@�ӬOID 
      // �N�����ӭn���U��token, ���S�� 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    } // if  
 
    if ( mTokenType == ID ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      tmpID.variableName = mTokenStr;
      mScanner.PeekToken( mTokenStr, mTokenType ); // �U�@�ӬO'['��',' , �ΨS���]�i�H 
     
      // [ '[' Constant ']' ]
      if ( mTokenType == MLP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        mScanner.PeekToken( mTokenStr, mTokenType ); // �U�@�ӬOCONSTANT
        // �N�����ӭn���U��token, ���S�� 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";
        
        if ( mTokenType == CONSTANT ) {
          mScanner.GetToken( mTokenStr, mTokenType ) ;
          tmpID.constant = mTokenStr;
          mScanner.PeekToken( mTokenStr, mTokenType ); // �U�@�ӬO']'
          // �N�����ӭn���U��token, ���S�� 
          if ( mTokenType == END_OF_FILE )  throw "EOF exception";
          if ( mTokenType == MRP ) {
            mScanner.GetToken( mTokenStr, mTokenType ) ;
            mScanner.PeekToken( mTokenStr, mTokenType ) ; // �U�@�ӬO',' , �ΨS���]�i�H 
          } // if
          else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
        } // if
        else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
      } // if
      
      mTempIDList.push_back( tmpID ) ;  // ��ŧi��ID�s�i�h 
      
      while ( mTokenType == COMMA ) {
        tmpID.constant = "";
        tmpID.isCallByReference = false;
        tmpID.region = 0;
        tmpID.variableName = "";
        tmpID.variableType = "";
        tmpID.variableValue.clear();

        mScanner.GetToken( mTokenStr, mTokenType ) ;
        mScanner.PeekToken( mTokenStr, mTokenType ); // �U�@�ӬOtype_specifier
        // �N�����ӭn���U��token, ���S�� 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";
        if ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
             mTokenType == STRING OR mTokenType == BOOL ) {
          mScanner.GetToken( mTokenStr, mTokenType ) ;
          tmpID.variableType = mTokenStr;
          mScanner.PeekToken( mTokenStr, mTokenType );  // �i��O'&'��ID 
          // �N�����ӭn���U��token, ���S�� 
          if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    
          // [ '&' ] ���@�w�n�� 
          if ( mTokenType == BITAND ) {
            mScanner.GetToken( mTokenStr, mTokenType ) ;
            tmpID.isCallByReference = true;
            mScanner.PeekToken( mTokenStr, mTokenType ); // �U�@�ӬOID 
            // �N�����ӭn���U��token, ���S�� 
            if ( mTokenType == END_OF_FILE )  throw "EOF exception";
          } // if  
 
          if ( mTokenType == ID ) {
            mScanner.GetToken( mTokenStr, mTokenType ) ;
            tmpID.variableName = mTokenStr;
            mScanner.PeekToken( mTokenStr, mTokenType ); // �U�@�ӬO'['��',' , �ΨS���]�i�H 
     
            // [ '[' Constant ']' ]
            if ( mTokenType == MLP ) {
              mScanner.GetToken( mTokenStr, mTokenType ) ;
              mScanner.PeekToken( mTokenStr, mTokenType ); // �U�@�ӬOCONSTANT
              // �N�����ӭn���U��token, ���S�� 
              if ( mTokenType == END_OF_FILE )  throw "EOF exception";
        
              if ( mTokenType == CONSTANT ) {
                mScanner.GetToken( mTokenStr, mTokenType ) ;
                tmpID.constant = mTokenStr;
                mScanner.PeekToken( mTokenStr, mTokenType ); // �U�@�ӬO']'
                // �N�����ӭn���U��token, ���S�� 
                if ( mTokenType == END_OF_FILE )  throw "EOF exception";
                if ( mTokenType == MRP ) {
                  mScanner.GetToken( mTokenStr, mTokenType ) ;
                  mScanner.PeekToken( mTokenStr, mTokenType ) ; // �U�@�ӬO',' , �ΨS���]�i�H 
                } // if
                else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
              } // if
              else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str(); 
            } // if
            
            mTempIDList.push_back( tmpID ) ;  // ��ŧi��ID�s�i�h 
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
      mTempIDList[i].variableValue.clear(); // �i��O���s�ŧi 
      mTempIDList[i].variableValue.resize( size, "0" );
    } // if
    else {
      mTempIDList[i].variableValue.clear(); // �i��O���s�ŧi 
      mTempIDList[i].variableValue.resize( 1, "0" );
    } // else
  } // for
  
  mCurFunction.parameter.assign( mTempIDList.begin(), mTempIDList.end() );
  mTempIDList.clear();  // �s�L���N�M�� 
  
} // end Parser::Formal_Parameter_List()

void Parser::Compound_Statement() {
// compound_statement : '{' { declaration | statement } '}'
  mScanner.PeekToken( mTokenStr, mTokenType );
  // �N�����ӭn���U��token, ���S�� 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  if ( mTokenType == BLP ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mCurVariableRegion++;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // �N�����ӭn���U��token, ���S�� 
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
      // �N�����ӭn���U��token, ���S�� 
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
// �o�̪�ID�O�ŧi��ID, �����ˬd�O�_�s�b 
// declaration : type_specifier Identifier rest_of_declarators

  VariableType tmpID;
  string tmpVariableType = "";
  string tmpVariableName = "";
  mScanner.PeekToken( mTokenStr, mTokenType );
  // �N�����ӭn���U��token, ���S�� 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  if ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
       mTokenType == STRING OR mTokenType == BOOL ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    tmpVariableType = mTokenStr;  // �s�ŧi���O 
    mScanner.PeekToken( mTokenStr, mTokenType );
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == ID ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      tmpVariableName = mTokenStr;
      
      mScanner.PeekToken( mTokenStr, mTokenType );
      // �N�����ӭn���U��token, ���S�� 
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
  // �N�����ӭn���U��token, ���S�� 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  if ( mTokenType == SCLN ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
  } // if
  else if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
            mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
            mTokenType == CONSTANT OR mTokenType == LP OR mTokenType == CIN OR mTokenType == COUT ) {
    Expression();
    mScanner.PeekToken( mTokenStr, mTokenType );
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == SCLN ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else if
  else if ( mTokenType == RETURN ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
         mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
         mTokenType == CONSTANT OR mTokenType == LP ) {
      Expression();
      mScanner.PeekToken( mTokenStr, mTokenType );
      // �N�����ӭn���U��token, ���S�� 
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
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == LP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      Expression();
     
      mScanner.PeekToken( mTokenStr, mTokenType );
      // �N�����ӭn���U��token, ���S�� 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
      if ( mTokenType == RP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        Statement();
       
        mScanner.PeekToken( mTokenStr, mTokenType ); // �i�H��ELSE, �]�i�H���n�� 
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
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == LP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      Expression();
     
      mScanner.PeekToken( mTokenStr, mTokenType );
      // �N�����ӭn���U��token, ���S�� 
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
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == WHILE ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
      // �N�����ӭn���U��token, ���S�� 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
      if ( mTokenType == LP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        Expression();
        mScanner.PeekToken( mTokenStr, mTokenType );
        // �N�����ӭn���U��token, ���S�� 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";
        if ( mTokenType == RP ) {
          mScanner.GetToken( mTokenStr, mTokenType ) ;
          mScanner.PeekToken( mTokenStr, mTokenType );
          // �N�����ӭn���U��token, ���S�� 
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
  // �N�����ӭn���U��token, ���S�� 
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
          // �N�����ӭn���U��token, ���S��
          if ( mTokenType == END_OF_FILE )  throw "EOF exception";
          if ( mTokenType == MRP ) {
            mScanner.GetToken( mTokenStr, mTokenType );
            mScanner.PeekToken( mTokenStr, mTokenType );
            // ���@�w�n���U��token 
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
  // �N�����ӭn���U��token, ���S�� 
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
  mScanner.PeekToken( mTokenStr, mTokenType ); // ���@�w�n���U�@�� 
  
  while ( mTokenType == COMMA ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Basic_Expression();
    mScanner.PeekToken( mTokenStr, mTokenType );  // ���@�w�n���U�@�� 
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
  // �N�����ӭn���U��token, ���S�� 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  if ( mTokenType == ID ) {
    CheckIDExistOrNot( mTokenStr ) ;
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Rest_Of_ID_Started_Basic_Exp();
  } // if
  else if ( mTokenType == PP OR mTokenType == MM ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // �N�����ӭn���U��token, ���S�� 
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
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    while ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
      // �N�����ӭn���U��token, ���S�� 
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
    // �N�����ӭn���U��token, ���S�� 
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
  // �]���i�H����, �ҥH�����ˬdEOF

  // �o�ӬOvariable
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
      // �N�����ӭn���U��token, ���S��
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";
      if ( mTokenType == MRP ) {
        mScanner.GetToken( mTokenStr, mTokenType );
        mScanner.PeekToken( mTokenStr, mTokenType );
        // ���@�w�n���U��token 
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
    // �i�H���� 
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
        // ���@�w�n���U��token 
      } // if
    
      Romce_And_Romloe(); 

    } // else if
  } // if
  
  // '(' [ actual_parameter_list ] ')' romce_and_romloe
  else if ( mTokenType == LP ) {   // �o�ӬOfunction 
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // �N�����ӭn���U��token, ���S��
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";  // �]���ܤ֭n��')' 
    
    // [ actual_parameter_list ] ���@�w�n�� 
    if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
         mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR 
         mTokenType == CONSTANT ) {

      Actual_Parameter_List();
      mScanner.PeekToken( mTokenStr, mTokenType );
      // �N�����ӭn���U��token, ���S��
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";  // �]���ܤ֭n��')' 
    } // if 
 
    if ( mTokenType == RP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      // ���@�w�n���U��token 
      Romce_And_Romloe(); 
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // else if

} // end Parser::Rest_Of_ID_Started_Basic_Exp()

void Parser::Rest_Of_PPMM_ID_Started_Basic_Exp() {
// rest_of_PPMM_Identifier_started_basic_exp : [ '[' expression ']' ] romce_and_romloe 

  mScanner.PeekToken( mTokenStr, mTokenType );
  // �]���i�H����, �ҥH�����ˬdEOF
  
  // [ '[' expression ']' ] 
  if ( mTokenType == MLP ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Expression();
    mScanner.PeekToken( mTokenStr, mTokenType );
    // �N�����ӭn���U��token, ���S��
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == MRP ) {
      mScanner.GetToken( mTokenStr, mTokenType );
      mScanner.PeekToken( mTokenStr, mTokenType );
      // ���@�w�n���U��token 
    } // if
    else  throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // if
  
  Romce_And_Romloe(); 

} // end Parser::Rest_Of_PPMM_ID_Started_Basic_Exp()

void Parser::Sign() {
// sign : '+' | '-' | '!'
  mScanner.PeekToken( mTokenStr, mTokenType );
  if ( mTokenType == END_OF_FILE )  // �N�����ӭn���U��token, ���S�� 
    throw "EOF exception";

  if ( mTokenType != ADDORSIGN AND mTokenType != SUBORSIGN AND mTokenType != BITNOT ) {
    throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // if
} // end Parser::Sign()

void Parser::Actual_Parameter_List() {
// actual_parameter_list : basic_expression { ',' basic_expression }

  Basic_Expression();
  
  mScanner.PeekToken( mTokenStr, mTokenType );
  // �]�����@�w�n��',', �ҥH�����ˬdEOF
  while ( mTokenType == COMMA ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Basic_Expression();
    mScanner.PeekToken( mTokenStr, mTokenType );
  } // while
  
} // end Parser::Actual_Parameter_List()

void Parser::Assignment_Operator() {
// assignment_operator : '=' | TE | DE | RE | PE | ME
  mScanner.PeekToken( mTokenStr, mTokenType );
  if ( mTokenType == END_OF_FILE )  // �N�����ӭn���U��token, ���S�� 
    throw "EOF exception";

  if ( mTokenType != ASSIGN AND mTokenType != TE AND mTokenType != DE AND
       mTokenType != RE AND mTokenType != PE AND mTokenType != ME ) {
    throw ( "unexpected token '" + mTokenStr + "'\n" ).c_str();
  } // if

} // end Parser::Assignment_Operator()

void Parser::Romce_And_Romloe() {
// rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp // �Yromce_and_romloe
// : rest_of_maybe_logical_OR_exp [ '?' basic_expression ':' basic_expression ]

  Rest_Of_Maybe_Logical_Or_Exp();
  mScanner.PeekToken( mTokenStr, mTokenType );
  // �]�����@�w�n��'?', �ҥH�����ˬdEOF
  
  if ( mTokenType == QUE ) {
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    Basic_Expression() ;
    mScanner.PeekToken( mTokenStr, mTokenType );
    // �N�����ӭn���U��token, ���S��
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
  // �]�����@�w�n�� OR , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� AND , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� AND , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� '|' , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� '|' , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� '^' , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� '^' , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� '&' , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� '&' , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� EQ or NEQ , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� EQ or NEQ , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� '<' | '>' | LE | GE , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� '<' | '>' | LE | GE , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� LS or RS , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� LS or RS , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� + or - , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� + or - , �ҥH�����ˬdEOF
  
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
  // �]�����@�w�n�� '*' | '/' | '%' , �ҥH�����ˬdEOF
  
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
  // �N�����ӭn���U��token, ���S�� 
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
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";
    if ( mTokenType == ID ) {
      CheckIDExistOrNot( mTokenStr ) ; 
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
      // ���@�w�n���U�@��
      if ( mTokenType == MLP ) {
        mScanner.GetToken( mTokenStr, mTokenType ) ;
        Expression();
        mScanner.PeekToken( mTokenStr, mTokenType );
        // �@�w�n�� ']' 
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
  // �N�����ӭn���U��token, ���S�� 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  
  
  // Identifier [ '(' [ actual_parameter_list ] ')' | '[' expression ']'  ]
  if ( mTokenType == ID ) {
    CheckIDExistOrNot( mTokenStr ) ; 
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
   
    if ( mTokenType == LP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
      // �N�����ӭn���U��token, ���S�� 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  �@�w�n��')' 
     
      if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
           mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
           mTokenType == CONSTANT OR mTokenType == LP ) {
        Actual_Parameter_List();
        mScanner.PeekToken( mTokenStr, mTokenType );
        // �N�����ӭn���U��token, ���S�� 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  �@�w�n��')' 
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
      // �N�����ӭn���U��token, ���S�� 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  �@�w�n��']' 
     
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
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  �@�w�n��']' 
     
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
  // �N�����ӭn���U��token, ���S�� 
  if ( mTokenType == END_OF_FILE )  throw "EOF exception";
  
  
  // Identifier [ '(' [ actual_parameter_list ] ')' | [ '[' expression ']' ] [ ( PP | MM ) ] ]
  if ( mTokenType == ID ) {
    CheckIDExistOrNot( mTokenStr ) ;
    mScanner.GetToken( mTokenStr, mTokenType ) ;
    mScanner.PeekToken( mTokenStr, mTokenType );
   
    if ( mTokenType == LP ) {
      mScanner.GetToken( mTokenStr, mTokenType ) ;
      mScanner.PeekToken( mTokenStr, mTokenType );
      // �N�����ӭn���U��token, ���S�� 
      if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  �@�w�n��')' 
     
      if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
           mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR
           mTokenType == CONSTANT OR mTokenType == LP ) {
        Actual_Parameter_List();
        mScanner.PeekToken( mTokenStr, mTokenType );
        // �N�����ӭn���U��token, ���S�� 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  �@�w�n��')' 
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
        // �N�����ӭn���U��token, ���S�� 
        if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  �@�w�n��']' 
     
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
    // �N�����ӭn���U��token, ���S�� 
    if ( mTokenType == END_OF_FILE )  throw "EOF exception";  //  �@�w�n��']' 
     
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
        // ������statement���}�l�M���� 
        
        mVariableList.assign( tempVariableList.begin(), tempVariableList.end() );
        mFunctionList.assign( tempFunctionList.begin(), tempFunctionList.end() );
        mInputBuffer.assign( tempInputBuffer.begin(), tempInputBuffer.end() );  // restore CPU status
        mCallStack = tempCallStack;
        mCurFunction = tempFunction;
        mCurVariableRegion = tempRegion;
        
        mUserInputVecNum = statementEnd;
        mCurFunction.pc = statementEnd_F;
        
        if ( executeValue != 0 AND mSkipExp == 0 ) {
          // �ŦX����~����� 
          mUserInputVecNum = statementStart;
          mCurFunction.pc = statementStart_F;
          Statement_v( curType, returnValue, returnStr, isReturn );
          mUserInputVecNum = statementEnd;
          mCurFunction.pc = statementEnd_F;
          
          if ( isReturn AND mSkipExp == 0 ) {
            return;
          } // if
        } // if
        

        PeekInputVecToken( mTokenStr, mTokenType ); // �i�H��ELSE, �]�i�H���n�� 
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
          // ������statement���}�l�M���� 
          
          mVariableList.assign( tempVariableList.begin(), tempVariableList.end() );
          mFunctionList.assign( tempFunctionList.begin(), tempFunctionList.end() );
          mInputBuffer.assign( tempInputBuffer.begin(), tempInputBuffer.end() );  // restore CPU status
          mCallStack = tempCallStack;
          mCurFunction = tempFunction;
          mCurVariableRegion = tempRegion;
          
          mUserInputVecNum = statementEnd;
          mCurFunction.pc = statementEnd_F;
          
          if ( executeValue == 0 AND mSkipExp == 0 ) {
            // ���ŦXif������~����� 
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
        // ������statement���}�l�M���� 
        
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
    // ���ݭn���L
    // �]��do�N�O�n����statement 
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
// �o�̪�ID�O�ŧi��ID, �����ˬd�O�_�s�b 
// �j�A��(while�Bif����)�̭��ŧi��ID 
// declaration : type_specifier Identifier rest_of_declarators

  VariableType tmpID;
  string tmpVariableType = "";
  string tmpVariableName = "";
  PeekInputVecToken( mTokenStr, mTokenType );

  if ( mTokenType == INT OR mTokenType == CHAR OR mTokenType == FLOAT OR
       mTokenType == STRING OR mTokenType == BOOL ) {
    GetInputVecToken( mTokenStr, mTokenType );
    tmpVariableType = mTokenStr;  // �s�ŧi���O 
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
// �o�̪�ID�O�ŧi��ID, �����ˬd�O�_�s�b 
// �j�A���̫ŧiID������ 
// rest_of_declarators : [ '[' Constant ']' ] 
//                       { ',' Identifier [ '[' Constant ']' ] } ';'
// ���ޫ�˳��ܤ֦���';'

  VariableType tmpID;
  
  // ����o�쪺���O�MID�W����assign   
  tmpID.variableName = tmpVariableName;
  tmpID.variableType = tmpVariableType;
  tmpID.constant = "";
  tmpID.region = mCurVariableRegion; 
  
  PeekInputVecToken( mTokenStr, mTokenType );

  // [ '[' Constant ']' ], ���@�w�n�� 
  if ( mTokenType == MLP ) {
    GetInputVecToken( mTokenStr, mTokenType );
    PeekInputVecToken( mTokenStr, mTokenType );

    if ( mTokenType == CONSTANT ) {
      GetInputVecToken( mTokenStr, mTokenType );
      tmpID.constant = mTokenStr;
      PeekInputVecToken( mTokenStr, mTokenType );

      if ( mTokenType == MRP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        PeekInputVecToken( mTokenStr, mTokenType );  // Ū�U�@��token, �ݬO','�٬O';'
        // �]���@�w�n��';', �ҥH�n�ˬd�O�_EOF 

      } // if
    } // if
  } // if
  
  mTempIDList.push_back( tmpID ) ;  // ��ŧi��ID�s�i�h 
  
  // { ',' Identifier [ '[' Constant ']' ] }
  // ���@�w�n�� 
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
      PeekInputVecToken( mTokenStr, mTokenType ); // Ū�U��token, �ݬO'['�٬O';', �]�i�H�O',' 
      // �]���@�w�n��';', �ҥH�n�ˬd�O�_EOF 
      
      // [ '[' Constant ']' ], ���@�w�n�� 
      if ( mTokenType == MLP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        PeekInputVecToken( mTokenStr, mTokenType );
        
        if ( mTokenType == CONSTANT ) {
          GetInputVecToken( mTokenStr, mTokenType );
          tmpID.constant = mTokenStr;
          PeekInputVecToken( mTokenStr, mTokenType );

          if ( mTokenType == MRP ) {
            GetInputVecToken( mTokenStr, mTokenType );
            PeekInputVecToken( mTokenStr, mTokenType ); // Ū�U�@��token, �ݬO','�٬O';'
            // �]���@�w�n��';', �ҥH�n�ˬd�O�_EOF 
            
            // �p�G�O','�N�~��]�j��
            // �O';'�N�X�j�� 
          } // if
          
        } // if
        
      } // if
      
      // �p�G���O'[', �i��O','��';' 
      // �p�G�O','�N�~��]�j��
      // �O';'�N�X�j�� 
      mTempIDList.push_back( tmpID ) ;  // ��ŧi��ID�s�i�h 
    } // if
    
  } // while
  
  // �̫�@��token�O';' , �@�w�n�� 
  if ( mTokenType == SCLN ) {
    GetInputVecToken( mTokenStr, mTokenType );
  } // if

  if ( mExecuteFunction > 0 ) {
    for ( int i = 0 ; i < mTempIDList.size() ; i++ ) {
      StoreVariable( mTempIDList[i].variableType, mTempIDList[i].variableName, mTempIDList[i].constant );
    } // for
  } // if

  mTempIDList.clear();  // �s�L���N�M�� 

} // end Parser::Rest_Of_Declarators_v()

void Parser::Compound_Statement_v( string & curType, double & returnValue,
                                   string & returnStr, bool & isReturn ) {
// �j�A�� 
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
        // �p�G�Oreturn �B ���Oif��while���b�]�L�@�M���� 
        // �N��mCurFunction.pc�ը�̫�@�� '{' 
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
          // constantStr�b�o�����ӥΤ��� 
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
      mIsCout++;  // ���F���L<<�Ÿ� 
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
// ���q���ԭz�y 
// expression : basic_expression { ',' basic_expression }

  string executeStr = "";
  double executeValue = 0.0;
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 
  
  Basic_Expression_v( curType, executeValue, executeStr );
  PeekInputVecToken( mTokenStr, mTokenType ); // ���@�w�n���U�@�� 
  
  returnValue = executeValue;
  returnStr = executeStr;
  
  while ( mTokenType == COMMA ) {
    GetInputVecToken( mTokenStr, mTokenType );
    Basic_Expression_v( curType, executeValue, executeStr );
    returnValue = executeValue;
    returnStr = executeStr;
    PeekInputVecToken( mTokenStr, mTokenType );  // ���@�w�n���U�@�� 
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
  // �Ψ�id����, �sid�ȥΪ� 
  
  string executeStr = "";
  double executeValue = 0.0;
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 

  double signValue = 1.0;

  PeekInputVecToken( mTokenStr, mTokenType );
  if ( mTokenType == ID ) {
    GetInputVecToken( mTokenStr, mTokenType );
    UpdateCurType( curType, mTokenStr ) ;
    // �n��ID��str���U��, �ݬO���O�}�C, �íp�� 
    Rest_Of_ID_Started_Basic_Exp_v( curType, returnValue, returnStr, mTokenStr );
  } // if
  else if ( mTokenType == PP OR mTokenType == MM ) {  // ++i, ����id+1�A���� 
    GetInputVecToken( mTokenStr, mTokenType );
    TokenType opType = mTokenType;
    PeekInputVecToken( mTokenStr, mTokenType );
    if ( mTokenType == ID ) {
      GetInputVecToken( mTokenStr, mTokenType );
      UpdateCurType( curType, mTokenStr ) ;
      Rest_Of_PPMM_ID_Started_Basic_Exp_v( curType, returnValue, returnStr, opType, mTokenStr );
      // ��id��++��--�i�h��, �]��id�᭱���i��O[] 
    } // if
  } // else if
  else if ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT ) {
    GetInputVecToken( mTokenStr, mTokenType );
    if      ( mTokenType == ADDORSIGN )  signValue = signValue * 1.0;
    else if ( mTokenType == SUBORSIGN )  signValue = signValue * ( - 1.0 );
    else if ( mTokenType == BITNOT )     signValue = signValue * ( - 1.0 );  // �Y�̫�O�t�ƥN��NOT  
    PeekInputVecToken( mTokenStr, mTokenType );
    while ( mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT ) {
      GetInputVecToken( mTokenStr, mTokenType );
      if      ( mTokenType == ADDORSIGN )  signValue = signValue * 1.0;
      else if ( mTokenType == SUBORSIGN )  signValue = signValue * ( - 1.0 );
      else if ( mTokenType == BITNOT )     signValue = signValue * ( - 1.0 );  // �Y�̫�O�t�ƥN��NOT 
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

    Romce_And_Romloe_v( curType, returnValue, returnStr, returnValue, returnStr );  //  ��constant�i�h��
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
  // �]���i�H����, �ҥH�����ˬdEOF
    
  string idValueStr = "";
  double idValue = 0.0;
  // �Ψ�id����, �sid�ȥΪ� 
  
  string executeStr = "";
  double executeValue = 0.0;
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 
  
  string constantStr = "";
  double constantValue = -1.0;
  // �s���A��[]�̭����� 
  
  int value1 = 0, value2 = 0, tmp = 0;

  string tmpCurType = "";
  bool isReturn = false;
  
  vector<ParameterType> tmpValueList ;

  // �o�ӬOvariable
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
        // ���@�w�n���U��token 
      } // if
    } // if
    
    GetVariableValue( idValue, idValueStr, idStr, constantValue );
    returnValue = idValue;
    returnStr = idValueStr;
    // �{�b����T����variable��
    // ���]�wreturn value
    // ���ᦳ������i�H��
    // �p�G�������\�Nupdate return�� 

    // assignment_operator basic_expression
    if ( mTokenType == ASSIGN OR mTokenType == TE OR mTokenType == DE OR
         mTokenType == RE OR mTokenType == PE OR mTokenType == ME ) {
      // PE, ME, TE, DE, RE // +=, -=, *=, /=, %=

      TokenType opType = mTokenType;
      GetInputVecToken( mTokenStr, mTokenType );
      Basic_Expression_v( curType, executeValue, executeStr );
      
      
      // ���sget��, �]�����i���s (�ܵh, �W����, ����) 
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
    // �i�H���� 
    else if ( mTokenType == PP OR mTokenType == MM OR mTokenType == MULT OR
              mTokenType == DIV OR mTokenType == REM OR mTokenType == ADDORSIGN OR
              mTokenType == SUBORSIGN OR mTokenType == LS OR mTokenType == RS OR
              mTokenType == LSS OR mTokenType == GTR OR mTokenType == LE OR 
              mTokenType == GE OR mTokenType == EQ OR mTokenType == NEQ OR
              mTokenType == BITAND OR mTokenType == BITXOR OR mTokenType == BITOR OR
              mTokenType == LOGAND OR mTokenType == LOGOR OR mTokenType == QUE ) { 
    
      GetVariableValue( idValue, idValueStr, idStr, constantValue );
      // �p�G�S��[], constantValue�N�|�O-1
      // �@�˷|���쥿�T��id�� 
    
      if ( mTokenType == PP OR mTokenType == MM ) {   // i++, �����ȦA��id+1 
        GetInputVecToken( mTokenStr, mTokenType );
        if  ( mTokenType == PP )
          UpdateVariableValue( curType, idValue+1, idValueStr, idStr, constantValue ) ;
        else if ( mTokenType == MM )
          UpdateVariableValue( curType, idValue-1, idValueStr, idStr, constantValue ) ;

        PeekInputVecToken( mTokenStr, mTokenType );
        // ���@�w�n���U��token 
      } // if
     
      Romce_And_Romloe_v( curType, executeValue, executeStr, idValue, idValueStr );
      // ��쥻��idValue��(�S��++��--)�Ƕi�h���B�� 
      // �^�ǭp�⵲�G��sreturnValue
    
      returnValue = executeValue;
      returnStr = executeStr;
    } // else if
  } // if
  
  
  // '(' [ actual_parameter_list ] ')' romce_and_romloe
  else if ( mTokenType == LP ) {   // �o�ӬOfunction 
    GetInputVecToken( mTokenStr, mTokenType );

    PeekInputVecToken( mTokenStr, mTokenType );
    
    // [ actual_parameter_list ] ���@�w�n�� 
    if ( mTokenType == ID OR mTokenType == PP OR mTokenType == MM OR
         mTokenType == ADDORSIGN OR mTokenType == SUBORSIGN OR mTokenType == BITNOT OR 
         mTokenType == CONSTANT ) {

      Actual_Parameter_List_v( curType, executeValue, executeStr, tmpValueList );  // �]�wparameter���� 
      PeekInputVecToken( mTokenStr, mTokenType );
    } // if 

    if ( mTokenType == RP ) {
      GetInputVecToken( mTokenStr, mTokenType );
      if ( mSkipExp == 0 ) {
        SetExecuteFunction( idStr, tmpValueList );  // ���n���檺function�ó]��mCurFunction
        mCurFunction.recordParameters = tmpValueList;
        int tempRegion = mCurVariableRegion;
        mCurVariableRegion = 0;  // �קKfunction����O�bif�̭� 
        // �o�̭n����function����
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
  // �]���i�H����, �ҥH�����ˬdEOF

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
      // ���@�w�n���U��token 
    } // if
  } // if

  GetVariableValue( idValue, idValueStr, idStr, constantValue );  // ��id����
  
  if      ( opType == PP )  idValue++;
  else if ( opType == MM )  idValue--;
  
  UpdateVariableValue( curType, idValue, idValueStr, idStr, constantValue ) ;
  // ��++ or -- �@�w�nupdate 

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
  // �]�����@�w�n��',', �ҥH�����ˬdEOF
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
// rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp // �Yromce_and_romloe
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
  // �]�����@�w�n��'?', �ҥH�����ˬdEOF

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
  // �]�����@�w�n�� OR , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ�
  
  Maybe_Bit_Or_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� AND , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ�
  
  Rest_Of_Maybe_Bit_Or_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� AND , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ�

  int value1 = 0, value2 = 0; 
  Maybe_Bit_Ex_Or_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� '|' , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ�

  int value1 = 0, value2 = 0; 

  Rest_Of_Maybe_Bit_Ex_Or_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� '|' , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ�

  int value1 = 0, value2 = 0; 
  
  Maybe_Bit_And_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� '^' , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ�

  int value1 = 0, value2 = 0; 

  Rest_Of_Maybe_Bit_And_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� '^' , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ�

  int value1 = 0, value2 = 0; 
  
  Maybe_Equality_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� '&' , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ�

  int value1 = 0, value2 = 0; 
  
  Rest_Of_Maybe_Equality_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� '&' , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ�

  TokenType opType;

  Maybe_Relational_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� EQ or NEQ , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 

  TokenType opType;
  
  Rest_Of_Maybe_Relational_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� EQ or NEQ , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 

  TokenType opType;
  
  Maybe_Shift_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� '<' | '>' | LE | GE , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 

  TokenType opType;
  
  Rest_Of_Maybe_Shift_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� '<' | '>' | LE | GE , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 

  int value1 = 0, value2 = 0;
  TokenType opType;

  Maybe_Additive_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� LS or RS , �ҥH�����ˬdEOF
  
  if ( mTokenType == LS AND mIsCout > 0 AND mInParentheses == 0 ) return;
  // '<<' ���b�A���̭�, �Ocout�� 
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 

  int value1 = 0, value2 = 0; 
  TokenType opType;

  Rest_Of_Maybe_Additive_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� LS or RS , �ҥH�����ˬdEOF
  
  if ( mTokenType == LS AND mIsCout > 0 AND mInParentheses == 0 ) return;
  // '<<' ���b�A���̭�, �Ocout�� 

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
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 
  
  int value1 = 0, value2 = 0; 

  TokenType opType;

  Maybe_Mult_Exp_v( curType, returnValue, returnStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� + or - , �ҥH�����ˬdEOF
  
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
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 
  
  int value1 = 0, value2 = 0; 
  
  TokenType opType;
  
  Rest_Of_Maybe_Mult_Exp_v( curType, returnValue, returnStr, idValue, idValueStr );
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� + or - , �ҥH�����ˬdEOF

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
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 
  
  Unary_Exp_v( curType, executeValue, executeStr );
  Rest_Of_Maybe_Mult_Exp_v( curType, returnValue, returnStr, executeValue, executeStr );

} // end Parser::Maybe_Mult_Exp_v()

void Parser::Rest_Of_Maybe_Mult_Exp_v( string & curType, double & returnValue,
                                       string & returnStr, double idValue, string idValueStr ) {
// rest_of_maybe_mult_exp : { ( '*' | '/' | '%' ) unary_exp }
  
  string executeStr = "";
  double executeValue = 0.0;
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 
  
  int value1 = 0, value2 = 0, tmp = 0; 
  TokenType opType;
  PeekInputVecToken( mTokenStr, mTokenType );
  // �]�����@�w�n�� '*' | '/' | '%' , �ҥH�����ˬdEOF
  
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
  // �sid�W�� 
  
  string idValueStr = "";
  double idValue = 0.0;
  // �Ψ�id����, �sid�ȥΪ� 
  
  string executeStr = "";
  double executeValue = 0.0;
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 
  
  string constantStr = "";
  double constantValue = -1.0;
  // �s���A��[]�̭����� 
  
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
      // ���@�w�n���U�@��
      if ( mTokenType == MLP ) {
        GetInputVecToken( mTokenStr, mTokenType );
        tmpCurType = curType;
        Expression_v( curType, constantValue, constantStr );
        curType = tmpCurType;
        PeekInputVecToken( mTokenStr, mTokenType );
        // �@�w�n�� ']' 
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
  // �sid�W�� 
  
  string idValueStr = "";
  double idValue = 0.0;
  // �Ψ�id����, �sid�ȥΪ� 
  
  string executeStr = "";
  double executeValue = 0.0;
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 
  
  string constantStr = "";
  double constantValue = -1.0;
  // �s���A��[]�̭����� 
  
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
   
    if ( mTokenType == LP ) {  // �o�ӬOfunction
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
          SetExecuteFunction( idStr, tmpValueList );  // ���n���檺function�ó]��mCurFunction
          mCurFunction.recordParameters = tmpValueList;
          int tempRegion = mCurVariableRegion;
          mCurVariableRegion = 0;
          // �o�̭n����function����
          Compound_Statement_v( curType, returnValue, returnStr, isReturn );
          mCurVariableRegion = tempRegion;
          curType = mCurFunction.functionType;
        
          RestoreExecuteFunction();
        } // if
      } // if

    } // if
    
    else if ( mTokenType == MLP ) {  // �o�ӬOvariable
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
    
    else {  // �S��[]��variable
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
  // �sid�W�� 
  string idValueStr = "";
  double idValue = 0.0;
  // �Ψ�id����, �sid�ȥΪ� 
  
  string executeStr = "";
  double executeValue = 0.0;
  // �s��Lfunction��X�Ӫ��p��ȥΪ� 
  
  string constantStr = "";
  double constantValue = -1.0;
  // �s���A��[]�̭����� 

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
   
    if ( mTokenType == LP ) {  // �o�ӬOfunction
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
          SetExecuteFunction( idStr, tmpValueList );  // ���n���檺function�ó]��mCurFunction
          mCurFunction.recordParameters = tmpValueList;
          int tempRegion = mCurVariableRegion;
          mCurVariableRegion = 0;
          // �o�̭n����function����, �ݸ� 
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
    
    else {  // �S��PP MM []��variable 
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

