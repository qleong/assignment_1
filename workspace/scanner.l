%{
// $Id: scanner.l,v 1.4 2014-10-20 14:27:17-07 - - $

#include "auxlib.h"
#include "lyutils.h"

#define YY_USER_ACTION  { scanner_useraction (); }
#define IGNORE(THING)   { }

%}

%option 8bit
%option debug
%option nodefault
%option nounput
%option noyywrap
%option verbose
%option warn

LETTER          [A-Za-z_]
DIGIT           [0-9]
MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
EXPONENT        ([Ee][+-]?{DIGIT}+)
NUMBER          ({MANTISSA}{EXPONENT}?)
NOTNUMBER       ({MANTISSA}[Ee][+-]?)
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
CHAR			(’([^\\’\n]|\\[\\’"0nt])’)
STRING			(\"([^\\"\n]|\\[\\’"0nt])*\")


%%

"#".*           { scanner_include(); }
[ \t]+          { IGNORE (white space) }
\n              { scanner_newline(); }

{NUMBER}        { return yylval_token (NUMBER); }
{IDENT}         { return yylval_token (IDENT); }
{STRING}		{ return yylval_token (STRING); }
{CHAR}			{ return yylval_token (CHAR); }

"=="            { return yylval_token ('=='); }
"="             { return yylval_token ('='); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"%"             { return yylval_token ('%'); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
";"             { return yylval_token (';'); }
"[]"            { return yylval_token ('[]'); }
"["             { return yylval_token ('['); }
"]"             { return yylval_token (']'); }
"{"             { return yylval_token ('{'); }
","             { return yylval_token (','); }
"\."             { return yylval_token ('\.'); }
"!"             { return yylval_token ('!'); }

"void"			{ return yylval_token ('void'); }
"bool"			{ return yylval_token ('bool'); }
"char"			{ return yylval_token ('char'); }
"int"			{ return yylval_token ('int'); }
"string"		{ return yylval_token ('string'); }
"struct"		{ return yylval_token ('struct'); }
"if"			{ return yylval_token ('if'); }
"else"			{ return yylval_token ('else'); }
"while"			{ return yylval_token ('while'); }
"return"		{ return yylval_token ('return'); }
"false"			{ return yylval_token ('false'); }
"true"			{ return yylval_token ('true'); }
"null"			{ return yylval_token ('null'); }
"ord"			{ return yylval_token ('ord'); }
"chr"			{ return yylval_token ('chr'); }
"new"			{ return yylval_token ('new'); }


{NOTNUMBER}     { scanner_badtoken (yytext);
                  return yylval_token (NUMBER); }

.               { scanner_badchar (*yytext); }

%%

RCSC("$Id: scanner.l,v 1.4 2014-10-20 14:27:17-07 - - $")

