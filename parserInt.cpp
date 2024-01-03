/* Implementation of Recursive-Descent Parser
	for a Simple Pasacal-Like Language
 * parser.cpp
 * Programming Assignment 2
 * Fall 2023
*/

#include "parserInterp.h"
#include <algorithm>
map <string, bool> defVar;
map<string, Token> SymTable;

map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value>* ValQue; //declare a pointer variable to a queue of Value objects

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if (pushed_back) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem& t) {
		if (pushed_back) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;
	}

}

static int error_count = 0;

int ErrCount()
{
	return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);


//Program is: Prog ::= PROGRAM IDENT ; DeclPart CompoundStmt .
bool Prog(istream& in, int& line)
{
	bool f1, f2;
	LexItem tok = Parser::GetNextToken(in, line);
	//Value retVal();

	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			string identstr = tok.GetLexeme();
			auto it = defVar.find(identstr);

			if (it!=defVar.end())
			{
				if(!it->second)
					defVar[identstr] = true;

			}
			else
				defVar.insert({ tok.GetLexeme(), true });//if is not declared, insert the variable into defVar and return true

			tok = Parser::GetNextToken(in, line);
			if (tok.GetToken() == SEMICOL) {
				f1 = DeclPart(in, line);

				if (f1) {
					tok = Parser::GetNextToken(in, line);
					if (tok == BEGIN)
					{
						f2 = CompoundStmt(in, line);
 						if (!f2)
						{
							ParseError(line, "Incorrect Program Body.");
							return false;
						}
						tok = Parser::GetNextToken(in, line);
						if (tok.GetToken() == DOT)
							return true;//Successful Parsing is completed
						else
						{
							ParseError(line, "Missing end of program dot.");
							return false;
						}
					}
					else
					{
						ParseError(line, "Missing program body.");
						return false;
					}

				}
				else
				{
					ParseError(line, "Incorrect Declaration Section.");
					return false;
				}
			}
			else
			{

				ParseError(line - 1, "Missing Semicolon.");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program Name.");
			return false;
		}
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else if (tok.GetToken() == DONE && tok.GetLinenum() <= 1) {
		ParseError(line, "Empty File");
		return true;
	}
	ParseError(line, "Missing PROGRAM Keyword.");
	return false;
}//End of Prog

//DeclPart ::= VAR DeclStmt { ; DeclStmt }
bool DeclPart(istream& in, int& line) {
	bool status = false;
	LexItem tok;
	//Value retVal();
	LexItem t = Parser::GetNextToken(in, line);
	if (t == VAR)
	{
		status = DeclStmt(in, line);

		while (status)
		{
			tok = Parser::GetNextToken(in, line);
			if (tok != SEMICOL)
			{

				ParseError(line, "Missing semicolon in Declaration Statement.");
				return false;
			}
			status = DeclStmt(in, line);
		}
		tok = Parser::GetNextToken(in, line);
		if (tok == BEGIN)
		{
			Parser::PushBackToken(tok);
			return true;
		}
		else
		{
			ParseError(line, "Syntactic error in Declaration Block.");
			return false;
		}
	}
	else
	{
		ParseError(line, "Non-recognizable Declaration Part.");
		return false;
	}

}//end of DeclBlock function

//DeclStmt ::= IDENT {, IDENT } : Type [:= Rxpr]
bool DeclStmt(istream& in, int& line)
{
	LexItem t;
	bool status = IdentList(in, line);
	bool flag;
	Value val1;
	Value retVal(VERR);


	if (!status)
	{
		ParseError(line, "Incorrect identifiers list in Declaration Statement.");
		return status;
	}

	t = Parser::GetNextToken(in, line);
	if (t == COLON)
	{
		t = Parser::GetNextToken(in, line);
		
		if (t == INTEGER || t == REAL || t == STRING || t == BOOLEAN)
		{	
			//string identstr;
			
			for (auto it = defVar.begin(); it != defVar.end(); ++it) {
				// 'it->first' is the key, and 'it->second' is the value
				if (!it->second)
				{	
					
					if (SymTable.find(it->first) == SymTable.end())
					{
						it->second = true;
						SymTable[it->first] = t.GetToken();
						
						TempsResults[it->first] = val1;
					}
					else
					{
						ParseError(line, "Variable Redefinition");
						return false;
					}
				}
			}	

			t = Parser::GetNextToken(in, line);
			if (t == ASSOP)
			{
				flag = Expr(in, line, retVal);
				if (!flag)
				{
					ParseError(line, "Incorrect initialization expression.");
					return false;
				}
				for (auto it = TempsResults.begin(); it != TempsResults.end(); ++it) {
					// 'it->first' is the key, and 'it->second' is the value
					if (it->second.GetType() == VERR)
					{
						TempsResults[it->first] = retVal;
					}
				}


			}
			else
			{
				

				auto it = TempsResults.begin();
				while (it != TempsResults.end()) {
					if (it->second.GetType() == VERR) {
						it = TempsResults.erase(it);
					}
					else {
						++it;
					}
				}
				Parser::PushBackToken(t);
			}
			return true;
		}
		else
		{
			ParseError(line, "Incorrect Declaration Type.");
			return false;
		}
	}
	else
	{
		Parser::PushBackToken(t);

		return false;
	}

}//End of DeclStmt

//IdList:= IDENT {,IDENT}
bool IdentList(istream& in, int& line) {
	bool status;
	string identstr;

	LexItem tok = Parser::GetNextToken(in, line);
	if (tok == IDENT)
	{
		//set IDENT lexeme to the type tok value
		identstr = tok.GetLexeme();
		auto it = defVar.find(identstr);

		if (it != defVar.end())
		{
			if (!it->second)
				defVar[identstr] = true;
			else
			{
				ParseError(line, "Variable Redefinition");
				return false;
			}
		}
		else
			defVar.insert({ tok.GetLexeme(), false });//if is not declared, insert the variable into defVar and return true

	}
	else
	{
		Parser::PushBackToken(tok);

		return true;
	}

	tok = Parser::GetNextToken(in, line);

	if (tok == COMMA) {
		status = IdentList(in, line);
	}

	else if (tok == COLON)
	{
		Parser::PushBackToken(tok);
		return true;
	}
	else if (tok == IDENT)
	{
		ParseError(line, "Missing comma in declaration statement.");
		return false;
	}
	else {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return status;
}//End of IdentList


//Stmt is either a simple statement or a structured one
//Stmt ::= SimpleStmt | StructuredStmt 
bool Stmt(istream& in, int& line) {
	bool status = false;
	//cout << "in ContrlStmt" << endl;
	LexItem t = Parser::GetNextToken(in, line);
	if (t == WRITELN || t == WRITE || t == IDENT)
	{
		Parser::PushBackToken(t);
		status = SimpleStmt(in, line);
		if (!status)
		{
			ParseError(line, "Incorrect Simple Statement.");
			return status;
		}
		return status;
	}
	else if (t == IF || t == BEGIN)
	{
		Parser::PushBackToken(t);
		status = StructuredStmt(in, line);
		t = Parser::GetNextToken(in, line);
	Parser::PushBackToken(t);
		if (!status)
		{
			ParseError(line, "Incorrect Structured Statement.");
			return status;
		}
		return status;
	}
	else if (t == END)
	{
		cout << "End of Program"<< endl;
		Parser::PushBackToken(t);
		return true;
	}
	return status;
}//End of Stmt

//StructuredStmt ::= IfStmt | CompoundStmt
bool StructuredStmt(istream& in, int& line) {
	bool status;

	LexItem t = Parser::GetNextToken(in, line);
	switch (t.GetToken()) {

	case IF: //Keyword consumed
		status = IfStmt(in, line);

		break;

	case BEGIN: //Keyword consumed
		status = CompoundStmt(in, line);
		break;

	default:
		;
	}

	return status;
}//End of StructuredStmt


//CompoundStmt ::= BEGIN Stmt {; Stmt } END
bool CompoundStmt(istream& in, int& line) {
	bool status;
	LexItem tok;
	status = Stmt(in, line);
	tok = Parser::GetNextToken(in, line);
	while (status && tok == SEMICOL)
	{

		tok = Parser::GetNextToken(in, line);
		Parser::PushBackToken(tok);
		status = Stmt(in, line);
		tok = Parser::GetNextToken(in, line);
	}
	if (!status)
	{
		ParseError(line, "Syntactic error in the statement.");
		Parser::PushBackToken(tok);
		return false;
	}
	if (tok == END)
	{
		return true;
	
	}	/*else if (tok == ELSE)
	{
		Parser::PushBackToken(tok);
		return true;
	}*/
	else
	{

		ParseError(line, "Missing end of compound statement.");
		return false;
	}
}//end of CompoundStmt



//SimpleStmt ::= AssignStmt | WriteLnStmt | WriteStmt
bool SimpleStmt(istream& in, int& line) {
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken()) {

	case WRITELN: //Keyword is consumed
		status = WriteLnStmt(in, line);

		break;

	case WRITE: //Keyword is consumed
		status = WriteStmt(in, line);
		break;

	case IDENT: //Keyword is not consumed
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);

		break;
	default:
		;
	}

	return status;
}//End of SimpleStmt


//WriteLnStmt ::= writeln (ExprList) 
bool WriteLnStmt(istream& in, int& line) {
	LexItem t;
	//cout << "in WriteStmt" << endl;
	ValQue = new queue<Value>;

	t = Parser::GetNextToken(in, line);
	if (t != LPAREN) {

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	bool ex = ExprList(in, line);

	if (!ex) {
		ParseError(line, "Missing expression list for WriteLn statement");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN) {

		ParseError(line, "Missing Right Parenthesis");
		return false;
	}

	//Evaluate: print out the list of expressions' values
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;
	return ex;
}//End of WriteLnStmt

//WriteStmt ::= write (ExprList) 
bool WriteStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;
	t = Parser::GetNextToken(in, line);
	if (t != LPAREN) {

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	bool ex = ExprList(in, line);

	if (!ex) {
		ParseError(line, "Missing expression list for Write statement");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN) {

		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	//Evaluate: print out the list of expressions' values
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	//cout << endl;
	return ex;

	return ex;
}//End of WriteStmt

//IfStmt ::= IF Expr THEN Stmt [ ELSE Stmt ] 
bool IfStmt(istream& in, int& line) {
	bool ex = false, status;
	LexItem t;
	Value val1;
	//Value retVal(VERR);
	t = Parser::GetNextToken(in, line);
	Parser::PushBackToken(t);
	


	ex = Expr(in, line,val1);

	//if (val1.GetType() == VERR)
		//return false;

	t = Parser::GetNextToken(in, line);
	Parser::PushBackToken(t);
	if (!ex) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}



	t = Parser::GetNextToken(in, line);
	
	if (t != THEN)
	{
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}
	if (val1.GetType() != VBOOL) 
	{
		ParseError(line, "Run-Time Error-Illegal Type for If statement condition");
		return false;
	}
	if (!val1.GetBool())
	{
		t = Parser::GetNextToken(in, line);
		if (t == BEGIN)
		{
			while (t != END)
			{
				t = Parser::GetNextToken(in, line);
			}
			//Parser::PushBackToken(t);
			//return true;
		}
		else
		while (t != SEMICOL)
		{
			t = Parser::GetNextToken(in, line);
		}
		//Parser::PushBackToken(t);
		//return true;
	}
	else
	{
		status = Stmt(in, line);
		if (!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Then-Part");
			return false;
		}
	}
	t = Parser::GetNextToken(in, line);
	if (t == ELSE) {
		if (val1.GetBool())
		{
			t = Parser::GetNextToken(in, line);
			if (t == BEGIN)
			{
				while (t != END )
				{
					t = Parser::GetNextToken(in, line);
				}
				//return true;
			}
			else
			while (t != SEMICOL)
			{
				t = Parser::GetNextToken(in, line);
			}
		//	return true;
			t = Parser::GetNextToken(in, line);
			if (t == SEMICOL)
			{
				Parser::PushBackToken(t);
				return true;
			}
		}
		else {
			status = Stmt(in, line);
			if (!status)
			{
				ParseError(line, "Missing Statement for If-Stmt Else-Part");
				return false;
			}
			t = Parser::GetNextToken(in, line);
			Parser::PushBackToken(t);
			return true;
		}
	}

	Parser::PushBackToken(t);// semicolon pushed back or anything else
	return true;
}//End of IfStmt function

//Var ::= ident
bool Var(istream& in, int& line, LexItem& idtok)
{
	string identstr;

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == IDENT) {
		identstr = tok.GetLexeme();

		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}
		Parser::PushBackToken(tok);
		return true;
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}//End of Var

//AssignStmt:= Var := Expr
bool AssignStmt(istream& in, int& line) {
	bool varstatus = false, status = false;
	LexItem t;
	Value val1, val2;
	string identstr;
	varstatus = Var(in, line,t);
	t = Parser::GetNextToken(in, line);

	if (varstatus) {

		identstr = t.GetLexeme();
		if (SymTable[identstr] == BOOLEAN)
			val1.SetType(VBOOL);
		else if (SymTable[identstr] == INTEGER)
			val1.SetType(VINT);
		else if (SymTable[identstr] == REAL)
			val1.SetType(VREAL);
		else if (SymTable[identstr] == STRING)
			val1.SetType(VSTRING);


		t = Parser::GetNextToken(in, line);

		if (t == ASSOP) {
			status = Expr(in, line, val2);//--------------------------
			if (!status) {
				ParseError(line, "Missing Expression in Assignment Statement");
				return status;
			}

		}
		else if (t.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	if (val1.GetType() == VREAL)
	{
		if (val2.GetType() == VINT)
		{
			//val2.SetType(VREAL);

			val1.SetReal((float)(val2.GetInt()));
			TempsResults[identstr] = val1;
			return status;
		}
		if (val2.GetType() == VSTRING)
		{
			ParseError(line, "Illegal mixed-mode assignment operation");
			return false;
		}
		TempsResults[identstr] = val2;
	}
	if (val1.GetType() == VINT)
	{
		if (val2.GetType() == VREAL)
		{
			val1.SetInt(int(val2.GetReal()));
			TempsResults[identstr] = val1;
			return status;
		}
		if (val2.GetType() == VSTRING)
		{
			ParseError(line, "STRING DIFERENT OF INT wrong assignment");
			return false;
		}
		TempsResults[identstr] = val2;
	}
	if (val1.GetType() == VSTRING)
	{
		if (val2.GetType() == VINT || val2.GetType() == VREAL)
		{

			ParseError(line, "Illegal mixed-mode assignment operation");
			return false;
		}
		TempsResults[identstr] = val2;
	}

	if (val1.GetType() == VBOOL)
	{
		if (val2.GetType() == VINT || val2.GetType() == VREAL || val2.GetType() == VSTRING)
		{

			ParseError(line, "Illegal mixed-mode assignment operation");
			return false;
		}
		TempsResults[identstr] = val2;
	}


	return status;
}

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;
	//cout << "in ExprList and before calling Expr" << endl;
	status = Expr(in, line, retVal);
	if (!status) {
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == COMMA) {
		//cout << "before calling ExprList" << endl;
		status = ExprList(in, line);
		//cout << "after calling ExprList" << endl;
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else {
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//ExprList


//Expr ::= LogORExpr ::= LogANDExpr { || LogANDRxpr }

bool Expr(istream& in, int& line, Value& retVal) {
	//LogORExpr
	Value val1, val2;
	LexItem tok;
	bool t1 = LogANDExpr(in, line,val1);

	if (!t1) {
		return false;
	}
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == OR)
	{
		t1 = LogANDExpr(in, line,val2);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		retVal = retVal || val2;
		if (retVal.IsErr())
		{
			cout << "Error occurred during logical AND operation." << endl;
		}
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}

	}
	Parser::PushBackToken(tok);
	return true;
}//End of Expr/LogORExpr

// LogAndExpr ::= RelExpr {AND RelExpr }
bool LogANDExpr(istream& in, int& line, Value& retVal) {
	Value val1, val2;
	LexItem t;
	t = Parser::GetNextToken(in, line);
	Parser::PushBackToken(t);
	bool t1 = RelExpr(in, line, val1);
	LexItem tok;
	
	if (!t1) {
		//retVal.SetType(VERR);
		return false;
	}

	retVal = val1;
	tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		//retVal.SetType(VERR);
		return false;
	}

	while (tok == AND) {
		t1 = RelExpr(in, line, val2);

		if (!t1) {
			ParseError(line, "Missing operand after operator");
			//retVal.SetType(VERR);
			return false;
		}

		retVal = retVal && val2;

		if (retVal.IsErr())
		{
			ParseError(line, "Illegal operand types for AND operation.");
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			//retVal.SetType(VERR);
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
}

//RelExpr ::= SimpleExpr  [ ( = | < | > ) SimpleExpr ]
bool RelExpr(istream& in, int& line, Value& retVal) {
	LexItem tok;
	Value val1, val2;
	LexItem t;
	t = Parser::GetNextToken(in, line);
	Parser::PushBackToken(t);
	bool t1 = SimpleExpr(in, line, val1);

	if (!t1) {
		//retVal.SetType(VERR);
		return false;
	}
	retVal = val1;
	
	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		//retVal.SetType(VERR);
		return false;
	}
	
	if (tok == EQ)
	{
		t1 = SimpleExpr(in, line, val2);
		if (!t1)
		{
			ParseError(line, "Missing expression after relational operator");
			return false;
		}
		retVal = val1 == val2;
		if (retVal.GetType() == VERR)
		{
			ParseError(line, "Run-Time Error-Illegal Mixed Type operation");
			return false;
		}
	//	if (retVal.GetBool())
			return true;
		//else
			//return false;
	}
	else if (tok == LTHAN)//||tok == LTHAN 
	{
		t1 = SimpleExpr(in, line, val2);
		if (!t1)
		{
			ParseError(line, "Missing expression after relational operator");
			return false;
		}

		retVal = val1 < val2;
		if (retVal.GetType() == VERR)
		{
			ParseError(line, "Missing expression after relational operator");
			return false;
		}
	//	if (retVal.GetBool())
			return true;
		//else
		//	return false;
	}
	else if (tok == GTHAN)// 
	{

	t = Parser::GetNextToken(in, line);
	Parser::PushBackToken(t);
 		t1 = SimpleExpr(in, line, val2);
		if (!t1)
		{
			ParseError(line, "Missing expression after relational operator");
			return false;
		}
		retVal = val1 > val2;
		if (retVal.GetType() == VERR)
		{
			ParseError(line, "Missing expression after relational operator");
			return false;
		}
			return true;
	}
	
	Parser::PushBackToken(tok);
	return true;
}//End of RelExpr

//Expr:= Term {(+|-) Term}
bool SimpleExpr(istream& in, int& line, Value& retVal) {

	Value val1, val2;
	LexItem t;
	t = Parser::GetNextToken(in, line);
	Parser::PushBackToken(t);
	bool t1 = Term(in, line, val1);
	LexItem tok;

	if (!t1) {
		return false;
	}
	retVal = val1;
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		//retVal.SetType(VERR);
		return false;
	}
	while (tok == PLUS || tok == MINUS)
	{
		t1 = Term(in, line,val2);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if (retVal.GetType() == VSTRING || val2.GetType() == VSTRING)
		{
			ParseError(line, "Run-Time Error-Illegal Mixed Type Operands");
			return false;
		}
		else
		{
			if (tok == PLUS)
			{
				retVal = retVal + val2;
			}
			else if (tok == MINUS)
			{
				retVal = retVal - val2;
			}
		}
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			//retVal.SetType(VERR);
			return false;
		}

	}
	Parser::PushBackToken(tok);
	return true;
}//End of Expr

//Term:= SFactor {( * | / | DIV | MOD) SFactor}
bool Term(istream& in, int& line, Value& retVal) {
	Value val1, val2;

	bool t1 = SFactor(in, line,val1);
	LexItem tok;

	if (!t1) {
	//	retVal.SetType(VERR);
		return false;
	}
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
  	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		//retVal.SetType(VERR);
		return false;
	}
	while (tok == MULT || tok == DIV || tok == IDIV || tok == MOD)
	{
		t1 = SFactor(in, line,val1);
		if (tok == MULT)
			retVal = retVal * val1;
		else if (tok == MOD)
		{
			retVal = retVal % val1;
		}
		else
		{
			if (val1.GetInt() == 0)
			{
				ParseError(line, "Run-Time Error-Illegal division by Zero");
				return false;
			}
			else {
				retVal = retVal / val1;
			}
		}
		if (!t1) {
			ParseError(line, "Missing operand after operator");
			//retVal.SetType(VERR);
			return false;
		}
		if (retVal.GetType() == VERR)
		{
			ParseError(line, "Illegal operand types for an arithmetic operator");
			return false;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			//retVal.SetType(VERR);
			return false;
		}

	}
	Parser::PushBackToken(tok);
	return true;
}//End of Term

//SFactor ::= [( - | + | NOT )] Factor
bool SFactor(istream& in, int& line, Value& retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	int sign = 1;
	if (t == MINUS)
	{
		sign = -1;
	}
	else if (t == PLUS)
	{
		sign = 1;
	}
	else if (t == NOT)
	{
		sign = -2;//sign is a NOT op for logic operands
	}
	else
		Parser::PushBackToken(t);

	status = Factor(in, line,retVal, sign);
	return status;
}//End of SFactor

//Factor ::= IDENT | ICONST | RCONST | SCONST | BCONST | (Expr)
bool Factor(istream& in, int& line, Value& retVal,int sign) {

	LexItem tok = Parser::GetNextToken(in, line);


	if (tok == IDENT) {
		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			//retVal.SetType(VERR);
			return false;
		}
		retVal = TempsResults[lexeme];
		if (TempsResults[lexeme].GetType() == VERR) {
			ParseError(line, "Using uninitialized Variable");
			return false;
		}
		if (sign == -1  && (retVal.GetType() == VSTRING || retVal.GetType() == VBOOL))
		{
			ParseError(line, "Illegal Operand Type for Sign/NOT Operator");
			return false;
		}
		if (sign == -2 && (retVal.GetType() == VINT || retVal.GetType() == VREAL))
		{
			ParseError(line, "Illegal Operand Type for Sign/NOT Operator");
			return false;
		}
		if (retVal.GetType() == VINT)
			retVal.SetInt(sign * retVal.GetInt());
		if (retVal.GetType() == VREAL)
			retVal.SetReal(sign * retVal.GetReal());
		
		return true;
	}
	else if (tok == ICONST) {
		string lexeme = tok.GetLexeme();
		double temp = stoi(lexeme);

		retVal.SetType(VINT);
		retVal.SetInt(sign * temp);
		return true;
	}
	else if (tok == SCONST) {
		retVal.SetType(VSTRING);
		retVal.SetString( tok.GetLexeme());
		return true;
	}
	else if (tok == RCONST) {
		retVal.SetType(VREAL);
		retVal.SetReal(sign * stof(tok.GetLexeme()));
		return true;
	}
	else if (tok == BCONST) {
		bool temp =false;
		if (tok.GetLexeme() == "true")
			temp = true;

		retVal.SetType(VBOOL);

		if (sign == -2)
			retVal.SetBool(!temp );
		else
			retVal.SetBool(temp);
		return true;
	}
	else if (tok == LPAREN) {
		bool ex = Expr(in, line,retVal);
		if (!ex) {
			ParseError(line, "Missing expression after (");
			//retVal.SetType(VERR);
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if (tok == RPAREN)
		{
			return ex;
		}
		else
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			//retVal.SetType(VERR);
			return false;
		}
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		//retVal.SetType(VERR);
		return false;
	}
	Parser::PushBackToken(tok);
	//retVal.SetType(VERR);
	return false;
}

