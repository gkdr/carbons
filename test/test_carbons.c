#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/carbons.c"

char * __wrap_purple_account_get_username(PurpleAccount * acc_p) {
    char * username;

    username = mock_ptr_type(char *);

    return username;
}

static void test_carbons_is_valid_valid(void ** state) {
    (void) state;

    xmlnode * test_outer_msg_stanza_p = xmlnode_from_str("<message from='me@test.org' />", -1);

    will_return(__wrap_purple_account_get_username, "me@test.org");

    assert_int_equal(carbons_is_valid(NULL, test_outer_msg_stanza_p), 1);
}

static void test_carbons_is_valid_invalid(void ** state) {
    (void) state;

    xmlnode * test_outer_msg_stanza_p = xmlnode_from_str("<message from='evilguy@test.org' />", -1);

    will_return(__wrap_purple_account_get_username, "me@test.org");

    assert_int_equal(carbons_is_valid(NULL, test_outer_msg_stanza_p), 0);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_carbons_is_valid_valid),
        cmocka_unit_test(test_carbons_is_valid_invalid)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}