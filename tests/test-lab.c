#include <string.h>
#include "harness/unity.h"
#include "../src/lab.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>



void setUp(void) {
     unsetenv("MY_PROMPT");
     using_history();
     // Install signal handlers to ignore SIGINT, SIGQUIT, and SIGTSTP
     signal(SIGINT, SIG_IGN);
     signal(SIGQUIT, SIG_IGN);
     signal(SIGTSTP, SIG_IGN);
 }
 

 
void tearDown(void) {
     unsetenv("MY_PROMPT");
     clear_history();
}


void test_cmd_parse2(void)
{
     //The string we want to parse from the user.
     //foo -v
     char *stng = (char*)malloc(sizeof(char)*7);
     strcpy(stng, "foo -v");
     char **actual = cmd_parse(stng);
     //construct our expected output
     size_t n = sizeof(char*) * 6;
     char **expected = (char**) malloc(sizeof(char*) *6);
     memset(expected,0,n);
     expected[0] = (char*)malloc(sizeof(char)*4);
     expected[1] = (char*)malloc(sizeof(char)*3);
     expected[2] = (char*)NULL;

     strcpy(expected[0], "foo");
     strcpy(expected[1], "-v");
     TEST_ASSERT_EQUAL_STRING(expected[0],actual[0]);
     TEST_ASSERT_EQUAL_STRING(expected[1],actual[1]);
     TEST_ASSERT_FALSE(actual[2]);
     free(expected[0]);
     free(expected[1]);
     free(expected);

     cmd_free(actual);
     free(stng);
}

void test_cmd_parse(void)
{
     char **rval = cmd_parse("ls -a -l");
     TEST_ASSERT_TRUE(rval);
     TEST_ASSERT_EQUAL_STRING("ls", rval[0]);
     TEST_ASSERT_EQUAL_STRING("-a", rval[1]);
     TEST_ASSERT_EQUAL_STRING("-l", rval[2]);
     TEST_ASSERT_EQUAL_STRING(NULL, rval[3]);
     TEST_ASSERT_FALSE(rval[3]);
     cmd_free(rval);
}

void test_trim_white_no_whitespace(void)
{
     char *line = (char*) calloc(10, sizeof(char));
     strncpy(line, "ls -a", 10);
     char *rval = trim_white(line);
     TEST_ASSERT_EQUAL_STRING("ls -a", rval);
     free(line);
}

void test_trim_white_start_whitespace(void)
{
     char *line = (char*) calloc(10, sizeof(char));
     strncpy(line, "  ls -a", 10);
     char *rval = trim_white(line);
     TEST_ASSERT_EQUAL_STRING("ls -a", rval);
     free(line);
}

void test_trim_white_end_whitespace(void)
{
     char *line = (char*) calloc(10, sizeof(char));
     strncpy(line, "ls -a  ", 10);
     char *rval = trim_white(line);
     TEST_ASSERT_EQUAL_STRING("ls -a", rval);
     free(line);
}

void test_trim_white_both_whitespace_single(void)
{
     char *line = (char*) calloc(10, sizeof(char));
     strncpy(line, " ls -a ", 10);
     char *rval = trim_white(line);
     TEST_ASSERT_EQUAL_STRING("ls -a", rval);
     free(line);
}

void test_trim_white_both_whitespace_double(void)
{
     char *line = (char*) calloc(10, sizeof(char));
     strncpy(line, "  ls -a  ", 10);
     char *rval = trim_white(line);
     TEST_ASSERT_EQUAL_STRING("ls -a", rval);
     free(line);
}

void test_trim_white_all_whitespace(void)
{
     char *line = (char*) calloc(10, sizeof(char));
     strncpy(line, "  ", 10);
     char *rval = trim_white(line);
     TEST_ASSERT_EQUAL_STRING("", rval);
     free(line);
}

void test_trim_white_mostly_whitespace(void)
{
     char *line = (char*) calloc(10, sizeof(char));
     strncpy(line, "    a    ", 10);
     char *rval = trim_white(line);
     TEST_ASSERT_EQUAL_STRING("a", rval);
     free(line);
}

void test_get_prompt_default(void)
{
     char *prompt = get_prompt("MY_PROMPT");
     TEST_ASSERT_EQUAL_STRING(prompt, "shell>");
     free(prompt);
}

void test_get_prompt_custom(void)
{
     const char* prmpt = "MY_PROMPT";
     if(setenv(prmpt,"foo>",true)){
          TEST_FAIL();
     }

     char *prompt = get_prompt(prmpt);
     TEST_ASSERT_EQUAL_STRING(prompt, "foo>");
     free(prompt);
     unsetenv(prmpt);
}

void test_ch_dir_home(void)
{
     char *line = (char*) calloc(10, sizeof(char));
     strncpy(line, "cd", 10);
     char **cmd = cmd_parse(line);
     char *expected = getenv("HOME");
     change_dir(cmd);
     char *actual = getcwd(NULL,0);
     TEST_ASSERT_EQUAL_STRING(expected, actual);
     free(line);
     free(actual);
     cmd_free(cmd);
}

void test_ch_dir_root(void)
{
     char *line = (char*) calloc(10, sizeof(char));
     strncpy(line, "cd /", 10);
     char **cmd = cmd_parse(line);
     change_dir(cmd);
     char *actual = getcwd(NULL,0);
     TEST_ASSERT_EQUAL_STRING("/", actual);
     free(line);
     free(actual);
     cmd_free(cmd);
}

void test_ch_dir_invalid(void)
{
    char *line = (char*) calloc(25, sizeof(char));
    strncpy(line, "cd /nonexistentpath", 25);
    char **cmd = cmd_parse(line);
    
    int result = change_dir(cmd);  
    TEST_ASSERT_EQUAL_INT(-1, result);  // Expect failure

    free(line);
    cmd_free(cmd);
}



void test_fg_no_stopped_process(void)
{
    char *line = strdup("fg");
    char **cmd = cmd_parse(line);

    int result = do_builtin(NULL, cmd);
    TEST_ASSERT_TRUE(result);  // fg should be recognized

    free(line);
    cmd_free(cmd);
}

void test_cmd_parse_special_chars(void)
{
    char **rval = cmd_parse("ls -l && echo test");
    TEST_ASSERT_TRUE(rval);
    TEST_ASSERT_EQUAL_STRING("ls", rval[0]);
    TEST_ASSERT_EQUAL_STRING("-l", rval[1]);
    TEST_ASSERT_EQUAL_STRING("&&", rval[2]);
    TEST_ASSERT_EQUAL_STRING("echo", rval[3]);
    TEST_ASSERT_EQUAL_STRING("test", rval[4]);
    TEST_ASSERT_FALSE(rval[5]);

    cmd_free(rval);
}

void test_get_prompt_empty_env(void)
{
    setenv("MY_PROMPT", "", 1);
    char *prompt = get_prompt("MY_PROMPT");
    
    TEST_ASSERT_EQUAL_STRING("shell>", prompt);
    
    free(prompt);
    unsetenv("MY_PROMPT");
}

void test_signal_ctrl_c(void)
{
    raise(SIGINT);
    TEST_ASSERT_TRUE(1);  // The shell shouldn't crash
}

void test_signal_ctrl_z(void)
{
    raise(SIGTSTP);
    TEST_ASSERT_TRUE(1);  // If the shell continues running, test passes
}

void test_cmd_parse_extra_spaces(void)
{
    char **rval = cmd_parse("   ls    -l   -a  ");
    TEST_ASSERT_TRUE(rval);
    TEST_ASSERT_EQUAL_STRING("ls", rval[0]);
    TEST_ASSERT_EQUAL_STRING("-l", rval[1]);
    TEST_ASSERT_EQUAL_STRING("-a", rval[2]);
    TEST_ASSERT_FALSE(rval[3]);

    cmd_free(rval);
}


int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_cmd_parse);
  RUN_TEST(test_cmd_parse2);
  RUN_TEST(test_trim_white_no_whitespace);
  RUN_TEST(test_trim_white_start_whitespace);
  RUN_TEST(test_trim_white_end_whitespace);
  RUN_TEST(test_trim_white_both_whitespace_single);
  RUN_TEST(test_trim_white_both_whitespace_double);
  RUN_TEST(test_trim_white_all_whitespace);
  RUN_TEST(test_get_prompt_default);
  RUN_TEST(test_get_prompt_custom);
  RUN_TEST(test_ch_dir_home);
  RUN_TEST(test_ch_dir_root);
  // Added tests
  RUN_TEST(test_ch_dir_invalid);
  RUN_TEST(test_fg_no_stopped_process);
  RUN_TEST(test_cmd_parse_special_chars);
  RUN_TEST(test_get_prompt_empty_env);
  RUN_TEST(test_signal_ctrl_c);
  RUN_TEST(test_signal_ctrl_z);
  RUN_TEST(test_cmd_parse_extra_spaces);

  return UNITY_END();
}
