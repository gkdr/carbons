#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "jabber.h"

#include "../src/carbons.c"

char * __wrap_purple_account_get_username(PurpleAccount * acc_p) {
    char * username;

    username = mock_ptr_type(char *);

    return username;
}

PurpleConnection * __wrap_purple_account_get_connection(PurpleAccount * acc_p) {
    PurpleConnection * connection_p;

    connection_p = mock_ptr_type(PurpleConnection *);

    return connection_p;
}

void * __wrap_purple_connection_get_protocol_data(const PurpleConnection * connection_p) {
    JabberStream * js_p;

    js_p = mock_ptr_type(JabberStream *);

    return js_p;
}

void __wrap_jabber_iq_send(JabberIq * iq_p) {
    check_expected(iq_p->type);
    check_expected(iq_p->callback);

    const char * to = xmlnode_get_attrib(iq_p->node, "to");
    check_expected(to);

    assert_non_null(xmlnode_get_child_with_namespace(iq_p->node, "query", DISCO_XMLNS));
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

static void test_carbons_discover(void ** state) {
    (void) state;

    const char * test_domain = "test.org";
    const char * test_jid = "me-testing@test.org/resource";

    will_return(__wrap_purple_account_get_connection, NULL);

    JabberStream * js_p = malloc(sizeof (JabberStream));
    js_p->next_id = 1;
    js_p->user = jabber_id_new(test_jid);
    will_return(__wrap_purple_connection_get_protocol_data, js_p);

    will_return(__wrap_purple_account_get_username, test_jid);


    expect_value(__wrap_jabber_iq_send, iq_p->type, JABBER_IQ_GET);
    expect_value(__wrap_jabber_iq_send, iq_p->callback, carbons_discover_cb);
    expect_string(__wrap_jabber_iq_send, to, test_domain);

    carbons_discover(NULL);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_carbons_is_valid_valid),
        cmocka_unit_test(test_carbons_is_valid_invalid),
        cmocka_unit_test(test_carbons_discover)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}