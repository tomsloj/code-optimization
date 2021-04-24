#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE program_test


#include <boost/test/unit_test.hpp>

#include "../include/lexer/lexer.hpp"
#include "../src/lexer/lexer.cpp"

#include "../include/lexer/Source.hpp"
#include "../src/lexer/Source.cpp"


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
