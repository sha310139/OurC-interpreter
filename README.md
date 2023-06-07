# OurC-interpreter
A C-like interpreter of about 5000 lines (executable loops and functions)<br>
Divided into 4 parts<br>
Project 1 is checking, evaluating and declaring variables of basic expressions<br>
Project 2 is a grammar checker for simple C language<br>
Project 3 is an advanced project of Project 2. It is necessary to confirm the correctness of the grammar and find the value<br>
Project 4 is an advanced version of Project 3, plus the function of Call function and execution<br>
Each project will have 10-16 test questions, and each question has about 3-4 test data (including visible data and hidden data)<br>
I completed 4 projects, passed all the tests, and got full marks for this homework, and there are only two full marks in this class. <br>
The code part is Project4, which has integrated all the functions of Project2-4<br>

<br>
<br>

## OS
Windows10<br>
<br>
<br>

## Environment
Dev C++<br>
<br>
<br>

## Files
* Our-C-Grammar-2016-07-15.doc
* PL_Project_4.cpp
* PL_Project_4.exe
* 測資 : Input & Output
<br>
<br>

## Functions
* According to the input string, judge whether it is correct C language grammar and calculate the operation value
* When an error is entered, the line number and error type of the error will be printed (lexical error, syntactical error (token recognized), semantic error (grammar ok))
* Support syntax includes if, else, while, cout, define function, etc.
* Support array declaration and operation of array value
* Support declaration of global and regional variables
* Provide 4 functional functions such as ListVariable("variable name"), ListAllVariables(), ListFunction("name"), ListAllFunctions()
* Can judge whether it is correct C language grammar and output operation according to the input string
* Can define functions (including recursion, call other functions in function, etc.) and execute functions
* Function parameter transfer supports call by reference & call by value
* Enter Done(); to end the execution of the program
<br>
<br>

## Use
The first line must enter a number (limited to integer) as a test number<br>
After enter, the interactive interface information will be displayed<br>
Then you can input simple code in C language<br>
This interface will translate directly and check for grammar errors<br>
Then execute the action of the code and display the relevant result<br>
Currently supported syntax includes if, else, while, cout, define function, etc.<br>
<br>
<br>

## I/O example
* function defined and run<br>
<img src="https://github.com/sha310139/OurC-interpreter/blob/main/Pictures/2.JPG" alt="Sample"  width="500">
<br>
<br>
-----------------------------------------------------------------<br>

* array and for loop calculation<br>
<img src="https://github.com/sha310139/OurC-interpreter/blob/main/Pictures/3.JPG" alt="Sample"  width="300">
<br>
<br>
-----------------------------------------------------------------<br>

* function defined, run, and output errors<br>
<img src="https://github.com/sha310139/OurC-interpreter/blob/main/Pictures/5.JPG" alt="Sample"  width="500">
<br>
<br>

## function call example

    1
    Our-C running ...
    > // ========== Proj. 4    Prob. 9   Test 1/3 (viewable) ============

      // =========== Proj. 3 tests (from Prob. 3&4) START =========

      // string *cs1 = new string( "Hi" ), *cs2 = new string( "Hello" ) ;
      // cs1->append( *cs2 ) ;
      // cout << *cs1 << "\n" ;

      // preparation

      int a1, a2, a3 ;
    Definition of a1 entered ...
    Definition of a2 entered ...
    Definition of a3 entered ...
    >   float f1, f2, f3 ;
    Definition of f1 entered ...
    Definition of f2 entered ...
    Definition of f3 entered ...
    >   bool b1, b2, b3 ;
    Definition of b1 entered ...
    Definition of b2 entered ...
    Definition of b3 entered ...
    >   string s1, s2, s3 ;
    Definition of s1 entered ...
    Definition of s2 entered ...
    Definition of s3 entered ...
    >   char c1, c2, c3 ;
    Definition of c1 entered ...
    Definition of c2 entered ...
    Definition of c3 entered ...
    >   int a[10], b[10] ;
    Definition of a entered ...
    Definition of b entered ...
    >   string s[10] ;
    Definition of s entered ...
    >   int i, j, k ;
    Definition of i entered ...
    Definition of j entered ...
    Definition of k entered ...
    >
      // Problem 3 : test data 1/3 ( viewable )

      a1 = 10 ;
    Statement executed ...
    >   a2 = 20 ;
    Statement executed ...
    >   a3 = -30 ;
    Statement executed ...
    >   f1 = 1.5 ;
    Statement executed ...
    >   f2 = 0.25 ;
    Statement executed ...
    >   f3 = -0.125 ;
    Statement executed ...
    >   s1 = "Hi" ;
    Statement executed ...
    >   s2 = "Hello" ;
    Statement executed ...
    >   s3 = "What" ;
    Statement executed ...
    >   c1 = 'a' ;
    Statement executed ...
    >   c2 = 'b' ;
    Statement executed ...
    >   c3 = 'c' ;
    Statement executed ...
    >   b1 = b2 = b3 = false ;
    Statement executed ...
    >
      i = j = k = 0 ;
    Statement executed ...
    >
      while ( i < 10 ) {
        a[i] = 10 ;
        b[i] = 20 ;
        s[i] = "Bravo" ;
        i++ ;
      } // while i < 10
    Statement executed ...
    >
      // + - * / %
      cout << ( a[1] = a2 = a[2]+a[3]*a[3]-a1-a2-a[4]%50 ) << "\n" ;
    70
    Statement executed ...
    >   cout << a[1]-305%a[2]-300/a[3]- ( a[3] = a2-a[4] ) %50 << "\n" ;
    25
    Statement executed ...
    >   cout << a[1]-305%a[2]- ( a[3] = a2-a[4] ) %50-300/a[3] << "\n" ;
    50
    Statement executed ...
    >   cout << ( a[2] = 300 / ( a[2]+a[1] ) +a1 ) +10+a[2] << "\n" ;
    36
    Statement executed ...
    >   cout << a[1]- - ( a[2]-a3*a[3] ) << "\n" ;
    1883
    Statement executed ...
    >
      // << >>
      cout << a[2] << "\n" ;
    13
    Statement executed ...
    >   cout << ( a[2] = ( a[2] << 2 ) -a2-a[1]/2*3%25 ) << "\n" ;
    -23
    Statement executed ...
    >   cout << ( a[1] = a[2] = ( a[2] >> 2 ) + a[1]%a[2] ) << "\n" ;
    -5
    Statement executed ...
    >
      // && || ! < <= > >=  ==   !=
      cout << ( b1 = f1-a[2]*f1+a2 > f2-a[2]*f1+a2 ) << "\n" ;
    true
    Statement executed ...
    >   cout << ( b2 = a[1]+a[2] >= a[3]%35 ) << "\n" ;
    false
    Statement executed ...
    >   cout << ( b3 = b1 && ( b2 || a[3] >= a[2]%a[1] ) ) << "\n" ;
    true
    Statement executed ...
    >   cout << ( b3 = b3 || ( a[1]*a3 < a[2]*a2 && ! ( b2 || a[3] >= a[2]%a[1] ) ) ) << "\n" ;
    true
    Statement executed ...
    >
      cout << ( ! ( a[1] == a[1]*2-a[1] ) && b3 ) << "\n" ;
    false
    Statement executed ...
    >   cout << ( b3 || a[1] != a[1]*2-a[1] || ! ( a2 > a[2] ) ) << "\n" ;
    true
    Statement executed ...
    >   cout << ( b3 || ( a[1] != a[1]*2-a[1] && true ) || ! ( a2 > a[2] || false ) ) << "\n" ;
    true
    Statement executed ...
    >   // cout << ( true && false == false ) << "\n" ; // '==' and '!=' should not be
      // cout << ( true || true != false ) << "\n" ;  // applied to booleans

      // string
      cout << ( s[2] = ( s[1] = s[1] + " " + s[2] ) + "!Hello!" + s[1] ) << "\n" ;
    Bravo Bravo!Hello!Bravo Bravo
    Statement executed ...
    >   cout << s[1] << "\n" << s[2] << "\n" ;
    Bravo Bravo
    Bravo Bravo!Hello!Bravo Bravo
    Statement executed ...
    >   cout << ( s[3] = s[2] + c1 + ( s[2] = s1 + s3 ) + s3 + s[2] ) << "\n" ;
    Bravo Bravo!Hello!Bravo BravoaHiWhatWhatHiWhat
    Statement executed ...
    >   cout << s[1] + "\n" + s[2] + "\n" + s[3] << "\n" ;
    Bravo Bravo
    HiWhat
    Bravo Bravo!Hello!Bravo BravoaHiWhatWhatHiWhat
    Statement executed ...
    >
      // =========== Proj. 3 tests (from Prob. 3&4) + while END   =========

      cout << "------------ Proj. 3 test end --------------\n" ;
    ------------ Proj. 3 test end --------------
    Statement executed ...
    >
    int gA, gY ; // 'gY', a to be used global, has to be declared in advance
    Definition of gA entered ...
    Definition of gY entered ...
    > int gW[ 10 ], gV[ 10 ] ;
    Definition of gW entered ...
    Definition of gV entered ...
    > string gS[ 10 ] ;
    Definition of gS entered ...
    >   gY = 3 ;
    Statement executed ...
    >   gA = 20 ;
    Statement executed ...
    >
      i = 0 ;
    Statement executed ...
    >   while ( i < 10 ) {
        gW[i] = a1*i + 10 ;
        gV[i] = a2*i + 20 ;
        gS[i] = "Q" ;
        i++ ;
      } // while i < 10
    Statement executed ...
    >
      i = 0 ;
    Statement executed ...
    >
      cout << "------------ global init end --------------\n" ;
    ------------ global init end --------------
    Statement executed ...
    >
    void F91( int a, int & b, int c, int & d ) {
      a = a + 5 ;
      b = b - 2 ;
      c = c - 1 ;
      d = d + 3 ;
    } // F91()
    Definition of F91() entered ...
    >
    int F81( int c, int & d, int w[ 10 ], int p[ 10 ], string t, string & u, string ss[ 10 ] ) {

      // new
      F91( c, d, w[2], d ) ;
      F91( w[3], p[4], w[9], p[0] ) ;
      F91( w[2], p[5], p[1], d ) ;

      int c1, d1 ;
      c1 = c ;
      d1 = d ;
      if ( c1 < 0 )
        c1 = -c1 ;
      if ( d1 < 0 )
        d1 = -d1 ;

      c = gW[ c1%10 ] + gV[ d1%10 ] ;
      d = gA + c ;

      int i ;
      i = 0 ;
      while ( i < 10 ) {
        w[i] = w[i]+1 ;
        p[i] = 2+p[i] ;
        ss[i] = ss[i]+"3" ;
        i++ ;
      } // while

      t = t+"4" ;
      u = "5"+u ;

      return c+d ;

    } // F81()
    Definition of F81() entered ...
    >
    int F82( int c, int & d, int w[ 10 ], int p[ 10 ], string t, string & u, string ss[ 10 ] ) {

      cout << "F82() starts.\n" ;

      int f81a, f81b, f81c ;
      int f81d ;

      f81c = F81( c, c, w, w, t, t, ss ) ;

      cout << "f81c : " << f81c << " c : " << c
           << " t : '" << t << "' u : '" << u << "'\n" ;

      f81d = F81( d, d, p, p, u, u, ss ) ;

      cout << "f81d : " << f81d << " d : " << d
           << " t : '" << t << "' u : '" << u << "'\n" ;

      f81a = F81( c, d, w, p, t, u, ss ) ;

      cout << "f81a : " << f81a << " c : " << c << " d : " << d
           << " t : '" << t << "' u : '" << u << "'\n" ;

      f81b = F81( d, c, p, w, u, t, ss ) ;

      cout << "f81b : " << f81b << " c : " << c << " d : " << d
           << " t : '" << t << "' u : '" << u << "'\n" ;

      cout << "F82() ends.\n" ;

      return f81a+f81b-f81c-f81d ;

    } // F82()
    Definition of F82() entered ...
    >
    int F51( int c, int & d, int w[ 10 ], int p[ 10 ], string t, string & u, string ss[ 10 ] ) {
    // be sure to initialize all parameters and all globals before calling this routine

      cout << "\nF51() - start.\n" ;

      int gA ;

      gA = gY = c - d + F82( c, d, w, p, t, u, ss ) ;
      c -= d += gA + gY - F82( c, d, w, p, t, u, ss ) ;

      int gW[ 10 ] ;

      int i ;
      i = 0 ;
      while ( i < 10 ) {
        gW[i] = w[i]+p[i] ;
        gV[i] = 2*p[i]-w[i] ;
        w[i] = w[i] - i ;
        p[i] = p[i] + i ;
        ss[i] = ss[i] + " Wa " ;
        i++ ;
      } // while i < 10

      int gY ;

      gA += gY = 3 ;

      int gV[ 10 ] ;

      i = 0 ;
      while ( i < 10 ) {
        gW[i] += gY ;
        gV[i] = gA-gW[i] ;
        w[i] = w[i] - gY ;
        p[i] = p[i] + gA ;
        i++ ;
      } // while i < 10

      int a[ 10 ] ; // int b[ 10 ] ;
      string s[ 10 ] ;
      int a1 ; // a2, a3 ;
      string s1 ; // s2, s3 ;

      s1 = t ;
      t = u + " Ha " ;
      u = s1 + " Hi " ;

      i = 0 ;
      while ( i < 10 ) {
        a[i] = w[i] + 10 ;
        s[i] = ss[i] + " Hi " ;
        if ( i%2 == 0 )
          ss[i] = s[i] ;
        i++ ;
      } // while i < 10

      a1 = gW[ 3 ] ;
      s1 = s[ 5 ] ;

      a1 -= gA ;
      a[3] += a[5] += gW[2] - a1 ;
      s1 = s1 + " ChungLi " ;

      if ( gW[7] < 0 )
        gW[7] = -gW[7] ;

      if ( gV[8] < 0 )
        gV[8] = -gV[8] ;

      if ( a[3] < 0 )
        a[3] = -a[3] ;

      cout << "F51() - end.\n" ;

      return gW[ gV[8]%10 ] + gV[ ( 3+gW[7] ) %10 ] + a[ a[3]%10 ] ;

    } // F51()
    Definition of F51() entered ...
    >
    string F52( int c, int & d, int w[ 10 ], int p[ 10 ], string t, string & u, string ss[ 10 ] ) {
    // be sure to initialize all parameters and all globals before calling this routine

      cout << "\nF52() - start.\n" ;

      int gA ;

      gA = gY = c - d ;
      c -= d += gA + gY ;

      int gW[ 10 ] ;

      int i ;
      i = 0 ;
      while ( i < 10 ) {
        gW[i] = w[i]+p[i] ;
        gV[i] = 2*p[i]-w[i] ;
        w[i] = w[i] - i ;
        p[i] = p[i] + i ;
        ss[i] = ss[i] + " Wa " ;
        i++ ;
      } // while i < 10

      int gY ;

      gA += gY = 3 ;

      int gV[ 10 ] ;

      i = 0 ;
      while ( i < 10 ) {
        gW[i] += gY ;
        gV[i] = gA-gW[i] ;
        w[i] = w[i] - gY ;
        p[i] = p[i] + gA ;
        i++ ;
      } // while i < 10

      int a[ 10 ] ; // int b[ 10 ] ;
      string s[ 10 ] ;
      int a1 ; // a2, a3 ;
      string s1 ; // s2, s3 ;

      s1 = t ;
      t = u + " Ha " ;
      u = s1 + " Hi " ;

      i = 0 ;
      while ( i < 10 ) {
        a[i] = w[i] + 10 + i ;
        s[i] = ss[i] + " Hi " ;
        if ( i%2 == 1 )
          ss[i] = s[i] ;
        i++ ;
      } // while i < 10

      a1 = gW[ 3 ] ;
      s1 = s[ 5 ] ;

      a1 -= gA ;
      a[3] += a[5] += gW[2] - a1 ;
      s1 = s1 + " ChungLi " ;

      if ( gW[5] < 0 )
        gW[5] = -gW[5] ;

      if ( gV[4] < 0 )
        gV[4] = -gV[4] ;

      if ( a[9] < 0 )
        a[9] = -a[9] ;

      p[5] = w[2] = gW[ gV[4]%10 ] + gV[ ( 3+gW[5] ) %10 ] + a[ a[9]%10 ] ;

      cout << "F52() - end.\n" ;

      return s1 + ss[ 5 ] + s[ 2 ] ;

    } // F52()
    Definition of F52() entered ...
    >
      cout << "------------ global init end --------------\n" ;
    ------------ global init end --------------
    Statement executed ...
    >
      cout << "F51( a1, gY, a, gV, s1, s2, s ) : "
           <<  F51( a1, gY, a, gV, s1, s2, s )
           << "\n" ;
    F51( a1, gY, a, gV, s1, s2, s ) :
    F51() - start.
    F82() starts.
    f81c : 640 c : 330 t : '5Hi' u : 'Hello'
    f81d : 1120 d : 570 t : '5Hi' u : '5Hello'
    f81a : 638 c : 330 d : 329 t : '5Hi' u : '55Hello'
    f81b : 822 c : 421 d : 329 t : '55Hi' u : '55Hello'
    F82() ends.
    F82() starts.
    f81c : 644 c : 332 t : '5Hi' u : '55Hello'
    f81d : 1412 d : 716 t : '5Hi' u : '555Hello'
    f81a : 162 c : 332 d : 91 t : '5Hi' u : '5555Hello'
    f81b : 962 c : 491 d : 91 t : '55Hi' u : '5555Hello'
    F82() ends.
    F51() - end.
    509
    Statement executed ...
    >
      cout << "a1 : " << a1 << " gY : " << gY << " s1 : " << s1 << " s2 : " << s2 << "\n" ;
    a1 : 10 gY : 437 s1 : Hi s2 : Hi Hi
    Statement executed ...
    >
      cout << "F51( a[3], b[7], a, gV, s1, s2, s ) : "
           <<  F51( a[3], b[7], a, gV, s1, s2, s )
           << "\n" ;
    F51( a[3], b[7], a, gV, s1, s2, s ) :
    F51() - start.
    F82() starts.
    f81c : -312 c : -146 t : '5Hi' u : 'Hi Hi '
    f81d : 652 d : 336 t : '5Hi' u : '5Hi Hi '
    f81a : -294 c : -146 d : -137 t : '5Hi' u : '55Hi Hi '
    f81b : 288 c : 154 d : -137 t : '55Hi' u : '55Hi Hi '
    F82() ends.
    F82() starts.
    f81c : -288 c : -134 t : '5Hi' u : '55Hi Hi '
    f81d : 442 d : 231 t : '5Hi' u : '555Hi Hi '
    f81a : 1020 c : -134 d : 520 t : '5Hi' u : '5555Hi Hi '
    f81b : -386 c : -183 d : 520 t : '55Hi' u : '5555Hi Hi '
    F82() ends.
    F51() - end.
    574
    Statement executed ...
    >
      cout << "a[3] : " << a[3] << " b[7] : " << b[7] << " s1 : " << s1 << " s2 : " << s2 << "\n" ;
    a[3] : 72 b[7] : -560 s1 : Hi s2 : Hi Hi
    Statement executed ...
    >
      cout << "F51( b[9], a[5], b, a, s2, s1, s ) : "
           <<  F51( b[9], a[5], b, a, s2, s1, s )
           << "\n" ;
    F51( b[9], a[5], b, a, s2, s1, s ) :
    F51() - start.
    F82() starts.
    f81c : 680 c : 350 t : '5Hi Hi ' u : 'Hi'
    f81d : 723 d : 373 t : '5Hi Hi ' u : '5Hi'
    f81a : 1250 c : 350 d : 636 t : '5Hi Hi ' u : '55Hi'
    f81b : 800 c : 410 d : 637 t : '55Hi Hi ' u : '55Hi'
    F82() ends.
    F82() starts.
    f81c : 680 c : 350 t : '5Hi Hi ' u : '55Hi'
    f81d : 3695 d : 1859 t : '5Hi Hi ' u : '555Hi'
    f81a : -900 c : 350 d : -439 t : '5Hi Hi ' u : '5555Hi'
    f81b : 860 c : 440 d : -438 t : '55Hi Hi ' u : '5555Hi'
    F82() ends.
    F51() - end.
    121
    Statement executed ...
    >
      cout << "a[5] : " << a[5] << " b[9] : " << b[9] << " s1 : " << s1 << " s2 : " << s2 << "\n" ;
    a[5] : 5980 b[9] : 20 s1 : Hi Hi  Hi  s2 : Hi Hi
    Statement executed ...
    >
      cout << "gA+a2+2+F51( gA, a2, gW, b, s2, s1, gS )+3+gA+a2 : "
           <<  gA+a2+2+F51( gA, a2, gW, b, s2, s1, gS )+3+gA+a2
           << "\n" ;
    gA+a2+2+F51( gA, a2, gW, b, s2, s1, gS )+3+gA+a2 :
    F51() - start.
    F82() starts.
    f81c : 30 c : 25 t : '5Hi Hi ' u : 'Hi Hi  Hi '
    f81d : 36 d : 28 t : '5Hi Hi ' u : '5Hi Hi  Hi '
    f81a : 162 c : 25 d : 91 t : '5Hi Hi ' u : '55Hi Hi  Hi '
    f81b : 92 c : 56 d : 91 t : '55Hi Hi ' u : '55Hi Hi  Hi '
    F82() ends.
    F82() starts.
    f81c : 54 c : 37 t : '5Hi Hi ' u : '55Hi Hi  Hi '
    f81d : 21258 d : 10639 t : '5Hi Hi ' u : '555Hi Hi  Hi '
    f81a : 470 c : 37 d : 245 t : '5Hi Hi ' u : '5555Hi Hi  Hi '
    f81b : 172 c : 96 d : 245 t : '55Hi Hi ' u : '5555Hi Hi  Hi '
    F82() ends.
    F51() - end.
    21519
    Statement executed ...
    >
      cout << "gA : " << gA << " a2 : " << a2 << " s1 : " << s1 << " s2 : " << s2 << "\n" ;
    gA : 20 a2 : 21191 s1 : Hi Hi  Hi  s2 : Hi Hi
    Statement executed ...
    >
      cout << "a1+a2+2+F51( a1, a2, a, b, s2, s1, gS )+3+a1+a2 : "
           <<  a1+a2+2+F51( a1, a2, a, b, s2, s1, gS )+3+a1+a2
           << "\n" ;
    a1+a2+2+F51( a1, a2, a, b, s2, s1, gS )+3+a1+a2 :
    F51() - start.
    F82() starts.
    f81c : 58 c : 39 t : '5Hi Hi ' u : 'Hi Hi  Hi '
    f81d : 28 d : 24 t : '5Hi Hi ' u : '5Hi Hi  Hi '
    f81a : 148 c : 39 d : 84 t : '5Hi Hi ' u : '55Hi Hi  Hi '
    f81b : 162 c : 91 d : 84 t : '55Hi Hi ' u : '55Hi Hi  Hi '
    F82() ends.
    F82() starts.
    f81c : 58 c : 39 t : '5Hi Hi ' u : '55Hi Hi  Hi '
    f81d : 42 d : 31 t : '5Hi Hi ' u : '555Hi Hi  Hi '
    f81a : 172 c : 39 d : 96 t : '5Hi Hi ' u : '5555Hi Hi  Hi '
    f81b : 214 c : 117 d : 96 t : '55Hi Hi ' u : '5555Hi Hi  Hi '
    F82() ends.
    F51() - end.
    -41132
    Statement executed ...
    >
      cout << "a1 : " << a1 << " a2 : " << a2 << " s1 : " << s1 << " s2 : " << s2 << "\n" ;
    a1 : 10 a2 : -42104 s1 : Hi Hi  Hi  s2 : Hi Hi
    Statement executed ...
    >
      cout << "a[6]+a[6]+2+F51( a[6], a[6], a, b, s2, s1, gS )+3+a[6]+a[6] : "
           <<  a[6]+a[6]+2+F51( a[6], a[6], a, b, s2, s1, gS )+3+a[6]+a[6]
           << "\n" ;
    a[6]+a[6]+2+F51( a[6], a[6], a, b, s2, s1, gS )+3+a[6]+a[6] :
    F51() - start.
    F82() starts.
    f81c : -514 c : -247 t : '5Hi Hi ' u : 'Hi Hi  Hi '
    f81d : -428 d : -204 t : '5Hi Hi ' u : '5Hi Hi  Hi '
    f81a : -431 c : -247 d : -205 t : '5Hi Hi ' u : '55Hi Hi  Hi '
    f81b : -626 c : -303 d : -203 t : '55Hi Hi ' u : '55Hi Hi  Hi '
    F82() ends.
    F82() starts.
    f81c : -514 c : -247 t : '5Hi Hi ' u : '55Hi Hi  Hi '
    f81d : -570 d : -275 t : '5Hi Hi ' u : '555Hi Hi  Hi '
    f81a : -447 c : -247 d : -213 t : '5Hi Hi ' u : '5555Hi Hi  Hi '
    f81b : -646 c : -313 d : -211 t : '55Hi Hi ' u : '5555Hi Hi  Hi '
    F82() ends.
    F51() - end.
    8142
    Statement executed ...
    >
      cout << "a[6] : " << a[6] << " a[6] : " << a[6] << " s1 : " << s1 << " s2 : " << s2 << "\n" ;
    a[6] : -441 a[6] : -441 s1 : Hi Hi  Hi  s2 : Hi Hi
    Statement executed ...
    >
      cout << "a[1]+b[0]+2+F51( a[1], b[0], a, b, s2, s1, gS )+3+a[1]+b[0] : "
           <<  a[1]+b[0]+2+F51( a[1], b[0], a, b, s2, s1, gS )+3+a[1]+b[0]
           << "\n" ;
    a[1]+b[0]+2+F51( a[1], b[0], a, b, s2, s1, gS )+3+a[1]+b[0] :
    F51() - start.
    F82() starts.
    f81c : -84202 c : -42091 t : '5Hi Hi ' u : 'Hi Hi  Hi '
    f81d : -94963 d : -47470 t : '5Hi Hi ' u : '5Hi Hi  Hi '
    f81a : -84448 c : -42091 d : -42213 t : '5Hi Hi ' u : '55Hi Hi  Hi '
    f81b : -86602 c : -43291 d : -42212 t : '55Hi Hi ' u : '55Hi Hi  Hi '
    F82() ends.
    F82() starts.
    f81c : -84202 c : -42091 t : '5Hi Hi ' u : '55Hi Hi  Hi '
    f81d : -94963 d : -47470 t : '5Hi Hi ' u : '555Hi Hi  Hi '
    f81a : -84448 c : -42091 d : -42213 t : '5Hi Hi ' u : '5555Hi Hi  Hi '
    f81b : -86602 c : -43291 d : -42212 t : '55Hi Hi ' u : '5555Hi Hi  Hi '
    F82() ends.
    F51() - end.
    85022
    Statement executed ...
    >
      cout << "a[1] : " << a[1] << " b[0] : " << b[0] << " s1 : " << s1 << " s2 : " << s2 << "\n" ;
    a[1] : 716 b[0] : 38581 s1 : Hi Hi  Hi  s2 : Hi Hi
    Statement executed ...
    >
      cout << "a[8]+b[4]+2+F51( a[8], b[4], b, a, s1, s1, gS )+3+a[8]+b[4] : "
           <<  a[8]+b[4]+2+F51( a[8], b[4], b, a, s1, s1, gS )+3+a[8]+b[4]
           << "\n" ;
    a[8]+b[4]+2+F51( a[8], b[4], b, a, s1, s1, gS )+3+a[8]+b[4] :
    F51() - start.
    F82() starts.
    f81c : -86978 c : -43479 t : '5Hi Hi  Hi ' u : 'Hi Hi  Hi '
    f81d : -86978 d : -43479 t : '5Hi Hi  Hi ' u : '5Hi Hi  Hi '
    f81a : -95243 c : -43479 d : -47611 t : '5Hi Hi  Hi ' u : '55Hi Hi  Hi '
    f81b : -95388 c : -47684 d : -47611 t : '55Hi Hi  Hi ' u : '55Hi Hi  Hi '
    F82() ends.
    F82() starts.
    f81c : -86978 c : -43479 t : '5Hi Hi  Hi ' u : '55Hi Hi  Hi '
    f81d : -82404 d : -41192 t : '5Hi Hi  Hi ' u : '555Hi Hi  Hi '
    f81a : -84535 c : -43479 d : -42257 t : '5Hi Hi  Hi ' u : '5555Hi Hi  Hi '
    f81b : -95280 c : -47630 d : -42257 t : '55Hi Hi  Hi ' u : '5555Hi Hi  Hi '
    F82() ends.
    F51() - end.
    -37991
    Statement executed ...
    >
      cout << "a[8] : " << a[8] << " b[4] : " << b[4] << " s1 : " << s1 << " s2 : " << s2 << "\n" ;
    a[8] : -24008 b[4] : -81299 s1 : Hi Hi  Hi  Hi  s2 : Hi Hi
    Statement executed ...
    >
      cout << "a[8]+b[4]+2+F51( a[8], b[4], a, a, s1, s1, gS )+3+a[8]+b[4] : "
           <<  a[8]+b[4]+2+F51( a[8], b[4], a, a, s1, s1, gS )+3+a[8]+b[4]
           << "\n" ;
    a[8]+b[4]+2+F51( a[8], b[4], a, a, s1, s1, gS )+3+a[8]+b[4] :
    F51() - start.
    F82() starts.
    f81c : 165486 c : 82753 t : '5Hi Hi  Hi  Hi ' u : 'Hi Hi  Hi  Hi '
    f81d : 6570 d : 3295 t : '5Hi Hi  Hi  Hi ' u : '5Hi Hi  Hi  Hi '
    f81a : -14702 c : 82753 d : -7341 t : '5Hi Hi  Hi  Hi ' u : '55Hi Hi  Hi  Hi '
    f81b : -13534 c : -6757 d : -7341 t : '55Hi Hi  Hi  Hi ' u : '55Hi Hi  Hi  Hi '
    F82() ends.
    F82() starts.
    f81c : 165486 c : 82753 t : '5Hi Hi  Hi  Hi ' u : '55Hi Hi  Hi  Hi '
    f81d : -13534 d : -6757 t : '5Hi Hi  Hi  Hi ' u : '555Hi Hi  Hi  Hi '
    f81a : -14370 c : 82753 d : -7175 t : '5Hi Hi  Hi  Hi ' u : '5555Hi Hi  Hi  Hi '
    f81b : -13478 c : -6729 d : -7175 t : '55Hi Hi  Hi  Hi ' u : '5555Hi Hi  Hi  Hi '
    F82() ends.
    F51() - end.
    -695516
    Statement executed ...
    >
      cout << "a[8] : " << a[8] << " b[4] : " << b[4] << " s1 : " << s1 << " s2 : " << s2 << "\n" ;
    a[8] : -166985 b[4] : -113377 s1 : Hi Hi  Hi  Hi  Hi  s2 : Hi Hi
    Statement executed ...
    >
      cout << "F52( gA, gY, gW, gV, s2, s2, s ) + ' OK?' : "
           <<  F52( gA, gY, gW, gV, s2, s2, s ) + " OK?" << "\n" ;
    F52( gA, gY, gW, gV, s2, s2, s ) + ' OK?' :
    F52() - start.
    F52() - end.
    Bravo33333333 Wa 33333333 Wa 33333333 Wa  Wa  Hi  ChungLi Bravo33333333 Wa 33333333 Wa 33333333 Wa  Wa  Hi HiWhat33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi  Wa  Hi  OK?
    Statement executed ...
    >
      cout << "-----------Simple Variables-------------\n" ;
    -----------Simple Variables-------------
    Statement executed ...
    >
      cout << "gA : " << gA << " gY : " << gY
           << " a1 : " << a1 << " a2 : " << a2
           << " s1 : " << s1 << " s2 : " << s2 << "\n" ;
    gA : 20 gY : 429063 a1 : 10 a2 : -42104 s1 : Hi Hi  Hi  Hi  Hi  s2 : Hi Hi  Hi
    Statement executed ...
    >
      cout << "i : " << i << "\n" ;
    i : 0
    Statement executed ...
    >
      cout << "-----------gW[]-------------\n" ;
    -----------gW[]-------------
    Statement executed ...
    >
      i = 0 ;
    Statement executed ...
    >   while ( i < 10 ) {
        cout << gW[ i ] << " " ;
        i++ ;
      } // while
    28 24 143041 40 40 48 64 72 80 88 Statement executed ...
    >
      cout << "\n-----------gV[]-------------\n" ;

    -----------gV[]-------------
    Statement executed ...
    >
      i = 0 ;
    Statement executed ...
    >   while ( i < 10 ) {
        cout << gV[ i ] << " " ;
        i++ ;
      } // while
    95289 95041 95027 95143 94893 143041 92703 94987 94973 94959 Statement executed ...
    >
      cout << "\n-----------a[]-------------\n" ;

    -----------a[]-------------
    Statement executed ...
    >
      i = 0 ;
    Statement executed ...
    >   while ( i < 10 ) {
        cout << a[ i ] << " " ;
        i++ ;
      } // while
    -166853 -166979 -166982 -166920 -167045 -161747 -168128 -166982 -166985 -166988 Statement executed ...
    >
      cout << "\n-----------b[]-------------\n" ;

    -----------b[]-------------
    Statement executed ...
    >
      i = 0 ;
    Statement executed ...
    >   while ( i < 10 ) {
        cout << b[ i ] << " " ;
        i++ ;
      } // while
    38602 8801 8803 8805 -113377 8761 8811 8233 8815 8817 Statement executed ...
    >
      cout << "\n-----------gS[]-------------\n" ;

    -----------gS[]-------------
    Statement executed ...
    >
      i = 0 ;
    Statement executed ...
    >   while ( i < 5 ) {
        cout << gS[ i ] << "\n" ;
        i++ ;
      } // while
    Q33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi
    Q33333333 Wa 33333333 Wa 33333333 Wa 33333333 Wa 33333333 Wa 33333333 Wa
    Q33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi
    Q33333333 Wa 33333333 Wa 33333333 Wa 33333333 Wa 33333333 Wa 33333333 Wa
    Q33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi
    Statement executed ...
    >
      cout << "-----------s[]-------------\n" ;
    -----------s[]-------------
    Statement executed ...
    >
      i = 0 ;
    Statement executed ...
    >   while ( i < 5 ) {
        cout << s[ i ] << "\n" ;
        i++ ;
      } // while
    Bravo33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi  Wa
    Bravo Bravo33333333 Wa 33333333 Wa 33333333 Wa  Wa  Hi
    HiWhat33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi  Wa
    Bravo Bravo!Hello!Bravo BravoaHiWhatWhatHiWhat33333333 Wa 33333333 Wa 33333333 Wa  Wa  Hi
    Bravo33333333 Wa  Hi 33333333 Wa  Hi 33333333 Wa  Hi  Wa
    Statement executed ...
    >
      // ===========================================================
      // ============= also copy the following to PAL ==============
      // ===========================================================

      cout << "---This cout-stmt is not part of the test data.---" ;
    ---This cout-stmt is not part of the test data.---Statement executed ...
    >
    Done   (
      )
         ;cout << 10 ; // Program execution
    Our-C exited ...
