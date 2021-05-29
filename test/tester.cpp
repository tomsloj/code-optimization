#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE program_test


#include <boost/test/unit_test.hpp>

#include "../include/lexer/Lexer.hpp"
#include "../src/lexer/Lexer.cpp"

#include "../include/lexer/Source.hpp"
#include "../src/lexer/Source.cpp"

#include "../include/parser/Parser.hpp"
#include "../src/parser/Parser.cpp"

#include "../include/ParserTree/Utilities.hpp"
#include "../src/ParserTree/Utilities.cpp"

#include "../include/analyzer/Analyzer.hpp"
#include "../src/analyzer/Analyzer.cpp"

#include "../include/optimizer/Optimizer.hpp"
#include "../src/optimizer/Optimizer.cpp"


using namespace std;

/**
 * lexer tests
 */
BOOST_AUTO_TEST_SUITE( lexerTests )


BOOST_AUTO_TEST_CASE( empty_source )
{
    string source = "";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( identyfier )
{
    string source = "_tekst_NR1";
    Lexer lexer(source);
    auto token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, IDENTYFIER);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "_tekst_NR1");
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( identyfiers_with_many_spaces )
{
    string source = "     _tekst_NR1      test2_   ";
    Lexer lexer(source);
    auto token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, IDENTYFIER);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "_tekst_NR1");
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, IDENTYFIER);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "test2_");
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

bool returnTrue(AnalizeError error)
{
    BOOST_CHECK_EQUAL(error.message, "Expected whitespace after number but get: z");
    BOOST_CHECK_EQUAL(error.module, LEXER);
    BOOST_CHECK_EQUAL(error.codePart, "4zmienna");
    BOOST_CHECK_EQUAL(error.line, 0);
    BOOST_CHECK_EQUAL(error.signNumber, 1);
    BOOST_CHECK_EQUAL(error.type, WRONG_NUMBER);
    
    return true;
}

BOOST_AUTO_TEST_CASE( identyfier_started_by_number )
{
    string source = " 4zmienna ";
    Lexer lexer(source);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, NUMBER);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, IDENTYFIER);
}

BOOST_AUTO_TEST_CASE( nothing_after_point_in_number )
{
    string source = " 42. ";
    Lexer lexer(source);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, ERROR_TOKEN);
}

BOOST_AUTO_TEST_CASE( wrong_sign_after_point_in_number )
{
    string source = " 42.t ";
    Lexer lexer(source);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, ERROR_TOKEN);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}


BOOST_AUTO_TEST_CASE( number )
{
    string source = "4324";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, NUMBER);
    BOOST_CHECK_EQUAL(std::get<unsigned long long>(token.value), 4324);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( number_with_point_and_many_spaces )
{
    string source = "113.234   897";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, NUMBER);
    BOOST_CHECK_EQUAL(std::get<double>(token.value), 113.234);
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, NUMBER);
    BOOST_CHECK_EQUAL(std::get<unsigned long long>(token.value), 897);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( aritmetic_operators )
{
    string source = "= ==  =  +  ++ - -- +- * /";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, ASSIGMENT_OPERATOR);
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, RELATIONAL_OPERATOR);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "==");
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, ASSIGMENT_OPERATOR);
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, ADDITIVE_OPERATOR);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "+");
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, INCREMENTAL_OPERATOR);
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, ADDITIVE_OPERATOR);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "-");
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, DECREMENTAL_OPERATOR);
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, ADDITIVE_OPERATOR);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "+");
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, ADDITIVE_OPERATOR);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "-");
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, MULTIPLICATIVE_OPERATOR);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "*");
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, MULTIPLICATIVE_OPERATOR);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "/");
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( logical_operators )
{
    string source = "<  >";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, RELATIONAL_OPERATOR);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "<");
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, RELATIONAL_OPERATOR);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), ">");
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( round_brackets )
{
    string source = "()";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, OPENING_ROUND_BRACKET);
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, CLOSING_ROUND_BRACKET);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( block_brackets )
{
    string source = "{}";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, OPENING_BLOCK_BRACKET);
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, CLOSING_BLOCK_BRACKET);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( square_brackets )
{
    string source = "[]";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, OPENING_SQUARE_BRACKET);
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, CLOSING_SQUARE_BRACKET);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( unknow_token )
{
    string source = "$";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, UNKNOW);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( number_and_other_sign )
{
    string source = "12+";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, NUMBER);
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, ADDITIVE_OPERATOR);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( variable_pluas_variable )
{
    string source = "zmienna+zmienna";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, IDENTYFIER);
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, ADDITIVE_OPERATOR);
    token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, IDENTYFIER);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( for_test )
{
    string source = "for";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, KEY_WORD);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "for");
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( data_type_test )
{
    string source = "double";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, DATA_TYPE);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "double");
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( semicolon_test )
{
    string source = "test;";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, IDENTYFIER);
    BOOST_CHECK_EQUAL(std::get<string>(token.value), "test");
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, SEMICOLON);
    BOOST_CHECK_EQUAL(lexer.getNextToken().type, END_OF_FILE);
}

BOOST_AUTO_TEST_CASE( integer_out_of_range_test )
{
    string source = "102030378238924;";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, ERROR_TOKEN);
}

BOOST_AUTO_TEST_CASE( double_out_of_range_test1 )
{
    string source = "5000000.123;";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, ERROR_TOKEN);
}

BOOST_AUTO_TEST_CASE( double_out_of_range_test2 )
{
    string source = "5.14217419164932747533489769482123;";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, ERROR_TOKEN);
}

BOOST_AUTO_TEST_CASE( too_long_identyfier )
{
    string source = "fhdjskaldnsmajdnsjakeotmgnsjakworkfjsndjfkwpeirjgkbxmzfikdjfosadpofprsgjspfjgxnlnvxvnilosjiorwjhigoshlfsnvlznvlsijfojwfoisfjlkslgjsljfio";
    Lexer lexer(source);
    Token token = lexer.getNextToken();
    BOOST_CHECK_EQUAL(token.type, ERROR_TOKEN);
}

BOOST_AUTO_TEST_SUITE_END()

/**
 * parser tests
 */
BOOST_AUTO_TEST_SUITE( parserTest )

BOOST_AUTO_TEST_CASE( parse_check_throw_missing_semicolon )
{
    string source = "dsas=var";
    Parser parser(source);

    BOOST_CHECK(!parser.parse());
}

BOOST_AUTO_TEST_CASE( parse_check_throw_unknown_operation )
{
    string source = "-a";
    Parser parser(source);

    BOOST_CHECK(!parser.parse());
}

BOOST_AUTO_TEST_CASE( parse_check_throw_for_bad_initiation )
{
    string source = "for(a;a < 0; ++a)";
    Parser parser(source);

    BOOST_CHECK(!parser.parse());
}

BOOST_AUTO_TEST_CASE( parse_check_throw_missing_condition )
{
    string source = "for(int a = 0;; ++a)";
    Parser parser(source);

    BOOST_CHECK(!parser.parse());
}

BOOST_AUTO_TEST_CASE( parse_check_throw_bad_update )
{
    string source = "for(int a = 0;a < 10; a)";
    Parser parser(source);

    BOOST_CHECK(!parser.parse());
}

BOOST_AUTO_TEST_CASE( parse_number_assigment )
{
    string source = "dsas = 9;";
    Parser parser(source);

    optional<ParserTree> tree = parser.parse();
    BOOST_CHECK(tree);
    Program program = *tree->getProgram();
    BOOST_CHECK_EQUAL(program.operations.size(), 1);
    Operation* operation = program.operations[0];
    BOOST_CHECK_EQUAL(operation->getOper().index(), 1);
    pair<Variable*, Assigment*> p = std::get<pair<Variable*, Assigment*>>(operation->getOper());
    Variable var = *p.first;
    Assigment ass = *p.second;
    BOOST_CHECK_EQUAL(std::get<string>(var.getVariableName().value), "dsas");
    BOOST_CHECK_EQUAL((*ass.getArithmeticExpression()).primaryExpressions.size(), 1);
    BOOST_CHECK_EQUAL((*ass.getArithmeticExpression()).operators.size(), 0);
    PrimaryExpression* pe = (*ass.getArithmeticExpression()).primaryExpressions[0];
    BOOST_CHECK_EQUAL(pe->getPExpression().index(), 2);
    Token t = get<Token>(pe->getPExpression());
    BOOST_CHECK_EQUAL(std::get<unsigned long long>(t.value), 9);
}

BOOST_AUTO_TEST_CASE( parse_variable_assigment )
{
    string source = "dsas = var;";
    Parser parser(source);

    optional<ParserTree> tree = parser.parse();
    BOOST_CHECK(tree);
    Program program = *tree->getProgram();
    BOOST_CHECK_EQUAL(program.operations.size(), 1);
    Operation* operation = program.operations[0];
    BOOST_CHECK_EQUAL(operation->getOper().index(), 1);
    pair<Variable*, Assigment*> p = std::get<pair<Variable*, Assigment*>>(operation->getOper());
    Variable var = *p.first;
    Assigment ass = *p.second;
    BOOST_CHECK_EQUAL(std::get<string>(var.getVariableName().value), "dsas");
    BOOST_CHECK_EQUAL((*ass.getArithmeticExpression()).primaryExpressions.size(), 1);
    BOOST_CHECK_EQUAL((*ass.getArithmeticExpression()).operators.size(), 0);
    PrimaryExpression* pe = (*ass.getArithmeticExpression()).primaryExpressions[0];
    BOOST_CHECK_EQUAL(pe->getPExpression().index(), 0);
    var = *get<Variable*>(pe->getPExpression());
    BOOST_CHECK_EQUAL(std::get<string>(var.getVariableName().value), "var");
}

BOOST_AUTO_TEST_CASE( parse_variable_assigment_string )
{
    string source = "dsas=var;";
    Parser parser(source);

    optional<ParserTree> tree = parser.parse();
    BOOST_CHECK(tree);
    ParserTree t = *tree;
    string newSource = treeToString(t);
    BOOST_CHECK_EQUAL(source, newSource);
}

BOOST_AUTO_TEST_CASE( parse_array_assigment_string )
{
    string source = "dsas[48]=var[sa+5];";
    Parser parser(source);

    optional<ParserTree> tree = parser.parse();
    BOOST_CHECK(tree);
    ParserTree t = *tree;
    string newSource = treeToString(t);
    BOOST_CHECK_EQUAL(source, newSource);
}

BOOST_AUTO_TEST_CASE( parse_table_string )
{
    string source = "for(int a=0;a<14;++a){int c[10]=a*3;}";
    Parser parser(source);

    optional<ParserTree> tree = parser.parse();
    BOOST_CHECK(tree);
    ParserTree t = *tree;
    string newSource = treeToString(t);
    BOOST_CHECK_EQUAL(source, newSource);
}

BOOST_AUTO_TEST_CASE( parse_loop_in_loop_string )
{
    string source = "for(int a=0;a<14;++a){for(b[10]=asd;www==vff;){int c[10]=++a*3;}}";
    Parser parser(source);

    optional<ParserTree> tree = parser.parse();
    BOOST_CHECK(tree);
    ParserTree t = *tree;
    string newSource = treeToString(t);
    BOOST_CHECK_EQUAL(source, newSource);
}

BOOST_AUTO_TEST_CASE( parse_complex_tree_string )
{
    string source = "for(int a[3278+ds*ad[21/33]]=0;a<14;++a){for(b[1+0]=asd-2131.3;www==vff[a[b[c]]];){int c[10+10+10+19+20+fad]=++a*3-++b-++s;}}";
    Parser parser(source);

    optional<ParserTree> tree = parser.parse();
    BOOST_CHECK(tree);
    ParserTree t = *tree;
    string newSource = treeToString(t);
    BOOST_CHECK_EQUAL(source, newSource);
}

BOOST_AUTO_TEST_CASE( parse_initiation_without_assigment_string )
{
    string source = "int a;int b;a=20;b=a/a;";
    Parser parser(source);

    optional<ParserTree> tree = parser.parse();
    BOOST_CHECK(tree);
    ParserTree t = *tree;
    string newSource = treeToString(t);
    BOOST_CHECK_EQUAL(source, newSource);
}

BOOST_AUTO_TEST_CASE( parse_incrementations_string )
{
    string source = "int a;++a;a++;c=a;";
    Parser parser(source);

    optional<ParserTree> tree = parser.parse();
    BOOST_CHECK(tree);
    ParserTree t = *tree;
    string newSource = treeToString(t);
    BOOST_CHECK_EQUAL(source, newSource);
}

BOOST_AUTO_TEST_CASE( parse_decrementations_string )
{
    string source = "int a;--a;a--;c=a;";
    Parser parser(source);

    optional<ParserTree> tree = parser.parse();
    BOOST_CHECK(tree);
    ParserTree t = *tree;
    string newSource = treeToString(t);
    BOOST_CHECK_EQUAL(source, newSource);
}

BOOST_AUTO_TEST_CASE( parse_table_initiation_string )
{
    string source = "int tab[5/2];";
    Parser parser(source);

    optional<ParserTree> tree = parser.parse();
    BOOST_CHECK(tree);
    ParserTree t = *tree;
    string newSource = treeToString(t);
    BOOST_CHECK_EQUAL(source, newSource);
}

BOOST_AUTO_TEST_SUITE_END()


/**
 * analyzer tests
 */
BOOST_AUTO_TEST_SUITE( analyzerTest )

BOOST_AUTO_TEST_CASE( analyzer_parser_error )
{
    string source = "dsas=var";
    Analyzer analyzer(source);

    BOOST_CHECK(!analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_assign_undefined_error )
{
    string source = "dsas=5;";
    Analyzer analyzer(source);

    BOOST_CHECK(!analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_correct_initiation_variable )
{
    string source = "int dsas=5;";
    Analyzer analyzer(source);

    BOOST_CHECK(analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_correct_initiation_table )
{
    string source = "int dsas[5/2];";
    Analyzer analyzer(source);

    BOOST_CHECK(analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_table_initiation_error )
{
    string source = "int x[10] = 1;";
    Analyzer analyzer(source);

    BOOST_CHECK(!analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_correct_initiation_table_with_variable_index )
{
    string source = "int s = 0;int dsas[s];";
    Analyzer analyzer(source);

    BOOST_CHECK(analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_second_definition_error )
{
    string source = "int s;int s;";
    Analyzer analyzer(source);

    BOOST_CHECK(!analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_second_definition_table_and_varaible_error )
{
    string source = "int s;int s[10];";
    Analyzer analyzer(source);

    BOOST_CHECK(!analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_second_definition_varaible_and_table_error )
{
    string source = "int s[10];int s;";
    Analyzer analyzer(source);

    BOOST_CHECK(!analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_increment_table_without_index_error )
{
    string source = "int s[10];++s;";
    Analyzer analyzer(source);

    BOOST_CHECK(!analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_increment_table )
{
    string source = "int s[10];++s[8];";
    Analyzer analyzer(source);

    BOOST_CHECK(analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_simple_for )
{
    string source = "for(int i = 0; i < 10; ++i){int a = 0;}";
    Analyzer analyzer(source);

    BOOST_CHECK(analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_local_variable )
{
    string source = "int a = 1; for(int i = 0; i < 10; ++i){int a = 0;}";
    Analyzer analyzer(source);

    BOOST_CHECK(analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_local_table_with_the_same_name )
{
    string source = "int a = 1; for(int i = 0; i < 10; ++i){int a[10];}";
    Analyzer analyzer(source);

    BOOST_CHECK(analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_same_name_of_counter_and_variable_name )
{
    string source = "for(int a = 0; a < 10; ++a){int a = 1;}";
    Analyzer analyzer(source);

    BOOST_CHECK(analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_table_use_counter )
{
    string source = "for(int a = 0; a < 10; ++a){int b = a;}";
    Analyzer analyzer(source);

    BOOST_CHECK(analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_removing_level )
{
    string source = "for(int a = 0; a < 10; ++a){int b = a;}int a = 0;";
    Analyzer analyzer(source);

    BOOST_CHECK(analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_removing_level_error )
{
    string source = "for(int a = 0; a < 10; ++a){int b = a;}int b = a;";
    Analyzer analyzer(source);

    BOOST_CHECK(!analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_different_levels_in_loop )
{
    string source = "int a;for(int a = 0; a < 10; ++a){int b = a;}int b = a;";
    Analyzer analyzer(source);

    BOOST_CHECK(analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_duplicate_initiation_after_loop_error )
{
    string source = "int a;for(int a = 0; a < 10; ++a){int b = a;}int a = 0;";
    Analyzer analyzer(source);

    BOOST_CHECK(!analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_inititaion_of_array_with_doesnt_existing_varaible_error )
{
    string source = "int a[10 * b];";
    Analyzer analyzer(source);

    BOOST_CHECK(!analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_inititaion_variable_instead_array_error )
{
    string source = "int a[10];int b=a;";
    Analyzer analyzer(source);

    BOOST_CHECK(!analyzer.analyze());
}

BOOST_AUTO_TEST_CASE( analyzer_correct_array_in_loop )
{
    string source = "int a[10];for(int b = 0; b < 10; ++b){a[5] = 0;}";
    Analyzer analyzer(source);

    BOOST_CHECK(analyzer.analyze());
}

BOOST_AUTO_TEST_SUITE_END()


/**
 * optimizer tests
 */
BOOST_AUTO_TEST_SUITE(optimizerTest )

BOOST_AUTO_TEST_CASE( optimizer_simple_assigment_optimalization )
{
    string source = "int a;for(int b=0;b<10;++b){a=5;}";
    string optimized = "int a;a=5;for(int b=0;b<10;++b){}";
    Optimizer optimizer(source);
    optimizer.optimize();

    BOOST_CHECK_EQUAL(treeToString(*optimizer.getTree()), optimized);
}

BOOST_AUTO_TEST_CASE( optimizer_two_level_assigment_optimalization )
{
    string source = "int a;for(int b=0;b<10;++b){for(int b=0;b<20;++b){a=5;}}";
    string optimized = "int a;a=5;for(int b=0;b<10;++b){for(int b=0;b<20;++b){}}";
    Optimizer optimizer(source);
    optimizer.optimize();

    BOOST_CHECK_EQUAL(treeToString(*optimizer.getTree()), optimized);
}

BOOST_AUTO_TEST_CASE( optimizer_assigment_cannot_do_optimalization )
{
    string source = "int a;for(int b=0;b<10;++b){for(int b=0;b<a;++b){a=5;}}";
    string optimized = "int a;for(int b=0;b<10;++b){for(int b=0;b<a;++b){a=5;}}";
    Optimizer optimizer(source);
    optimizer.optimize();

    BOOST_CHECK_EQUAL(treeToString(*optimizer.getTree()), optimized);
}

BOOST_AUTO_TEST_CASE( optimizer_assigment_part_move_optimalization )
{
    string source = "int a;for(int b=0;b<a;++b){for(int b=0;b<15;++b){a=5;}}";
    string optimized = "int a;for(int b=0;b<a;++b){a=5;for(int b=0;b<15;++b){}}";
    Optimizer optimizer(source);
    optimizer.optimize();

    BOOST_CHECK_EQUAL(treeToString(*optimizer.getTree()), optimized);
}

BOOST_AUTO_TEST_CASE( optimizer_assigment_with_multiple_levels_initiations )
{
    string source = "int a;for(int b=0;b<0;++b){int a = 10*30+b;for(int b=0;b<15;++b){a=5;}}";
    string optimized = "int a;for(int b=0;b<0;++b){int a=10*30+b;a=5;for(int b=0;b<15;++b){}}";
    Optimizer optimizer(source);
    optimizer.optimize();

    BOOST_CHECK_EQUAL(treeToString(*optimizer.getTree()), optimized);
}

BOOST_AUTO_TEST_CASE( optimizer_assigment_with_self_refference )
{
    string source = "int a;for(int b=0;b<0;++b){a = a;}";
    string optimized = "int a;for(int b=0;b<0;++b){a=a;}";
    Optimizer optimizer(source);
    optimizer.optimize();

    BOOST_CHECK_EQUAL(treeToString(*optimizer.getTree()), optimized);
}

BOOST_AUTO_TEST_CASE( optimizer_initiation_with_exist_variable )
{
    string source = "int x = 1;for(int a = 0; a < 10; ++a){for(int a = 0; a < 0; ++a){for(int a = 0; a < 0; ++a){for(int a = 0; a < 0; ++a){int x = 11;}}}}";
    string optimized = "int x=1;for(int a=0;a<10;++a){int x=11;for(int a=0;a<0;++a){for(int a=0;a<0;++a){for(int a=0;a<0;++a){}}}}";
    Optimizer optimizer(source);
    optimizer.optimize();

    BOOST_CHECK_EQUAL(treeToString(*optimizer.getTree()), optimized);
}

BOOST_AUTO_TEST_CASE( optimizer_initiation_with_used_in_for1 )
{
    string source = "int x = 1;for(int a = x; a < 10; ++a){for(int a = x; a < 0; ++a){for(int a = 0; a < 0; ++a){for(int a = 0; a < 0; ++a){int x = 11;}}}}";
    string optimized = "int x=1;for(int a=x;a<10;++a){for(int a=x;a<0;++a){for(int a=0;a<0;++a){int x=11;for(int a=0;a<0;++a){}}}}";
    Optimizer optimizer(source);
    optimizer.optimize();

    BOOST_CHECK_EQUAL(treeToString(*optimizer.getTree()), optimized);
}

BOOST_AUTO_TEST_CASE( optimizer_initiation_with_used_in_for2 )
{
    string source = "int x = 1;for(int a = x; a < 10; ++a){for(int a = 0; a < 0; ++a){for(int a = 0; a < 0; ++a){for(int a = 0; a < 0; ++a){int x = 11;}}}}";
    string optimized = "int x=1;for(int a=x;a<10;++a){for(int a=0;a<0;++a){int x=11;for(int a=0;a<0;++a){for(int a=0;a<0;++a){}}}}";
    Optimizer optimizer(source);
    optimizer.optimize();

    BOOST_CHECK_EQUAL(treeToString(*optimizer.getTree()), optimized);
}


BOOST_AUTO_TEST_SUITE_END()