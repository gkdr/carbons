#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "jabber.h"

#include "../src/carbons.h"

#define CARBONS_XMLNS   "urn:xmpp:carbons:2"
#define DISCO_XMLNS     "http://jabber.org/protocol/disco#info"

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

PurpleAccount * __wrap_purple_connection_get_account(const PurpleConnection * gc_p) {
    PurpleAccount * account_p;

    account_p = mock_ptr_type(PurpleAccount *);

    return account_p;
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

    xmlnode * query_node_p = xmlnode_get_child_with_namespace(iq_p->node, "query", DISCO_XMLNS);
    check_expected(query_node_p);

    xmlnode * enable_node_p = xmlnode_get_child_with_namespace(iq_p->node, "enable", CARBONS_XMLNS);
    check_expected(enable_node_p);
}

void __wrap_purple_debug_error(const char * category, const char * format, ...) {
    function_called();
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
    expect_not_value(__wrap_jabber_iq_send, query_node_p, NULL);

    // not set here
    expect_value(__wrap_jabber_iq_send, enable_node_p, NULL);

    carbons_discover(NULL);

    free(js_p);
}

/**
 * Shuould send well-formed "enable" request if feature is contained in discovery response.
 */
static void test_carbons_discover_cb_success(void ** state) {
    (void) state;

    const char * test_jid = "romeo@montague.example/garden";

    // example from docs
    const char * reply = "<iq xmlns='jabber:client' from='montague.example' id='info1' "
                             "to='romeo@montague.example/garden' type='result'>"
                            "<query xmlns='http://jabber.org/protocol/disco#info'>"
                                "<feature var='urn:xmpp:carbons:2'/>"
                            "</query>"
                          "</iq>";
    xmlnode * reply_node_p = xmlnode_from_str(reply, -1);

    JabberStream * js_p = malloc(sizeof (JabberStream));
    js_p->next_id = 1;
    js_p->user = jabber_id_new(test_jid);

    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, test_jid);

    expect_value(__wrap_jabber_iq_send, iq_p->type, JABBER_IQ_SET);
    expect_value(__wrap_jabber_iq_send, iq_p->callback, carbons_enable_cb);
    expect_not_value(__wrap_jabber_iq_send, enable_node_p, NULL);

    // not set here
    expect_value(__wrap_jabber_iq_send, to, NULL);
    expect_value(__wrap_jabber_iq_send, query_node_p, NULL);

    carbons_discover_cb(js_p, "from", JABBER_IQ_RESULT, "id", reply_node_p, NULL);

    free(js_p);
}

/**
 * Success case with a real world example (reply received from Prosody 0.11.0).
 */
static void test_carbons_discover_cb_real_world_reply(void ** state) {
    (void) state;

    const char * own_jid = "b@localhost/pidgin";
    const char * reply = "<iq id='purpleece2bdb2' type='result' to='b@localhost/pidgin' from='localhost'>"
                            "<query xmlns='http://jabber.org/protocol/disco#info'>"
                                "<identity type='im' name='Prosody' category='server'/>"
                                "<identity type='pep' name='Prosody' category='pubsub'/>"
                                "<feature var='urn:xmpp:blocking'/>"
                                "<feature var='urn:xmpp:carbons:2'/>"
                                "<feature var='vcard-temp'/>"
                                "<feature var='http://jabber.org/protocol/commands'/>"
                                "<feature var='jabber:iq:private'/>"
                                "<feature var='jabber:iq:register'/>"
                                "<feature var='urn:xmpp:ping'/>"
                                "<feature var='http://jabber.org/protocol/disco#info'/>"
                                "<feature var='http://jabber.org/protocol/disco#items'/>"
                                "<feature var='http://jabber.org/protocol/pubsub#publish'/>"
                                "<feature var='jabber:iq:version'/>"
                                "<feature var='urn:xmpp:time'/>"
                                "<feature var='jabber:iq:time'/>"
                                "<feature var='msgoffline'/>"
                                "<feature var='jabber:iq:last'/>"
                                "<feature var='jabber:iq:roster'/>"
                            "</query>"
                        "</iq>";
    xmlnode * reply_node_p = xmlnode_from_str(reply, -1);

    JabberStream * js_p = malloc(sizeof (JabberStream));
    js_p->next_id = 1;
    js_p->user = jabber_id_new(own_jid);

    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, own_jid);

    expect_value(__wrap_jabber_iq_send, iq_p->type, JABBER_IQ_SET);
    expect_value(__wrap_jabber_iq_send, iq_p->callback, carbons_enable_cb);
    expect_not_value(__wrap_jabber_iq_send, enable_node_p, NULL);

    // not set here
    expect_value(__wrap_jabber_iq_send, to, NULL);
    expect_value(__wrap_jabber_iq_send, query_node_p, NULL);

    carbons_discover_cb(js_p, "from", JABBER_IQ_RESULT, "id", reply_node_p, NULL);

    free(js_p);
}

/**
 * Positive reply and the feature is the first child of the query node.
 */
static void test_carbons_discover_cb_first_child(void ** state) {
    (void) state;

    const char * own_jid = "b@localhost/pidgin";
    const char * reply = "<iq id='purpleece2bdb2' type='result' to='b@localhost/pidgin' from='localhost'>"
                            "<query xmlns='http://jabber.org/protocol/disco#info'>"
                                "<feature var='urn:xmpp:carbons:2'/>"
                                "<identity type='im' name='Prosody' category='server'/>"
                                "<identity type='pep' name='Prosody' category='pubsub'/>"
                                "<feature var='urn:xmpp:blocking'/>"
                                "<feature var='vcard-temp'/>"
                                "<feature var='http://jabber.org/protocol/commands'/>"
                                "<feature var='jabber:iq:private'/>"
                                "<feature var='jabber:iq:register'/>"
                                "<feature var='urn:xmpp:ping'/>"
                                "<feature var='http://jabber.org/protocol/disco#info'/>"
                                "<feature var='http://jabber.org/protocol/disco#items'/>"
                                "<feature var='http://jabber.org/protocol/pubsub#publish'/>"
                                "<feature var='jabber:iq:version'/>"
                                "<feature var='urn:xmpp:time'/>"
                                "<feature var='jabber:iq:time'/>"
                                "<feature var='msgoffline'/>"
                                "<feature var='jabber:iq:last'/>"
                                "<feature var='jabber:iq:roster'/>"
                            "</query>"
                        "</iq>";
    xmlnode * reply_node_p = xmlnode_from_str(reply, -1);

    JabberStream * js_p = malloc(sizeof (JabberStream));
    js_p->next_id = 1;
    js_p->user = jabber_id_new(own_jid);

    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, own_jid);

    expect_value(__wrap_jabber_iq_send, iq_p->type, JABBER_IQ_SET);
    expect_value(__wrap_jabber_iq_send, iq_p->callback, carbons_enable_cb);
    expect_not_value(__wrap_jabber_iq_send, enable_node_p, NULL);

    // not set here
    expect_value(__wrap_jabber_iq_send, to, NULL);
    expect_value(__wrap_jabber_iq_send, query_node_p, NULL);

    carbons_discover_cb(js_p, "from", JABBER_IQ_RESULT, "id", reply_node_p, NULL);

    free(js_p);
}

/**
 * Positive reply and the feature is the last cild of the query node.
 */
static void test_carbons_discover_cb_last_child(void ** state) {
    (void) state;

    const char * own_jid = "b@localhost/pidgin";
    const char * reply = "<iq id='purpleece2bdb2' type='result' to='b@localhost/pidgin' from='localhost'>"
                            "<query xmlns='http://jabber.org/protocol/disco#info'>"
                                "<identity type='im' name='Prosody' category='server'/>"
                                "<identity type='pep' name='Prosody' category='pubsub'/>"
                                "<feature var='urn:xmpp:blocking'/>"
                                "<feature var='vcard-temp'/>"
                                "<feature var='http://jabber.org/protocol/commands'/>"
                                "<feature var='jabber:iq:private'/>"
                                "<feature var='jabber:iq:register'/>"
                                "<feature var='urn:xmpp:ping'/>"
                                "<feature var='http://jabber.org/protocol/disco#info'/>"
                                "<feature var='http://jabber.org/protocol/disco#items'/>"
                                "<feature var='http://jabber.org/protocol/pubsub#publish'/>"
                                "<feature var='jabber:iq:version'/>"
                                "<feature var='urn:xmpp:time'/>"
                                "<feature var='jabber:iq:time'/>"
                                "<feature var='msgoffline'/>"
                                "<feature var='jabber:iq:last'/>"
                                "<feature var='jabber:iq:roster'/>"
                                "<feature var='urn:xmpp:carbons:2'/>"
                            "</query>"
                        "</iq>";
    xmlnode * reply_node_p = xmlnode_from_str(reply, -1);

    JabberStream * js_p = malloc(sizeof (JabberStream));
    js_p->next_id = 1;
    js_p->user = jabber_id_new(own_jid);

    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, own_jid);

    expect_value(__wrap_jabber_iq_send, iq_p->type, JABBER_IQ_SET);
    expect_value(__wrap_jabber_iq_send, iq_p->callback, carbons_enable_cb);
    expect_not_value(__wrap_jabber_iq_send, enable_node_p, NULL);

    // not set here
    expect_value(__wrap_jabber_iq_send, to, NULL);
    expect_value(__wrap_jabber_iq_send, query_node_p, NULL);

    carbons_discover_cb(js_p, "from", JABBER_IQ_RESULT, "id", reply_node_p, NULL);

    free(js_p);
}

/**
 * Query successful, but carbons is not contained in the reply.
 */
static void test_carbons_discover_cb_no_carbons(void ** state) {
    (void) state;

    const char * own_jid = "b@localhost/pidgin";
    const char * reply = "<iq id='purpleece2bdb2' type='result' to='b@localhost/pidgin' from='localhost'>"
                            "<query xmlns='http://jabber.org/protocol/disco#info'>"
                                "<identity type='im' name='Prosody' category='server'/>"
                                "<identity type='pep' name='Prosody' category='pubsub'/>"
                                "<feature var='urn:xmpp:blocking'/>"
                                "<feature var='vcard-temp'/>"
                                "<feature var='http://jabber.org/protocol/commands'/>"
                                "<feature var='jabber:iq:private'/>"
                                "<feature var='jabber:iq:register'/>"
                                "<feature var='urn:xmpp:ping'/>"
                                "<feature var='http://jabber.org/protocol/disco#info'/>"
                                "<feature var='http://jabber.org/protocol/disco#items'/>"
                                "<feature var='http://jabber.org/protocol/pubsub#publish'/>"
                                "<feature var='jabber:iq:version'/>"
                                "<feature var='urn:xmpp:time'/>"
                                "<feature var='jabber:iq:time'/>"
                                "<feature var='msgoffline'/>"
                                "<feature var='jabber:iq:last'/>"
                                "<feature var='jabber:iq:roster'/>"
                            "</query>"
                        "</iq>";
    xmlnode * reply_node_p = xmlnode_from_str(reply, -1);

    JabberStream * js_p = malloc(sizeof (JabberStream));
    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, own_jid);

    // for now, no idea how to check for "not called", so at least make sure it doesn't crash in this case

    carbons_discover_cb(js_p, "from", JABBER_IQ_RESULT, "id", reply_node_p, NULL);

    free(js_p);
}

/**
 * Should abort when receiving an error reply.
 */
static void test_carbons_discover_cb_error(void ** state) {
    (void) state;

    const char * test_jid = "romeo@montague.example/garden";

    JabberStream * js_p = malloc(sizeof (JabberStream));

    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, test_jid);

    expect_function_call(__wrap_purple_debug_error);

    carbons_discover_cb(js_p, "from", JABBER_IQ_ERROR, "id", NULL, NULL);

    free(js_p);
}

static void test_carbons_discover_cb_empty_reply(void ** state) {
    (void) state;

    const char * own_jid = "romeo@montague.example/garden";

    const char * reply = "<iq xmlns='jabber:client' from='montague.example' id='info1' "
                             "to='romeo@montague.example/garden' type='result'>"
                          "</iq>";
    xmlnode * reply_node_p = xmlnode_from_str(reply, -1);

    JabberStream * js_p = malloc(sizeof (JabberStream));

    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, own_jid);

    expect_function_call(__wrap_purple_debug_error);

    carbons_discover_cb(js_p, "montague.example", JABBER_IQ_RESULT, "id", reply_node_p, NULL);

    free(js_p);

}

/**
 * Checks that the callback does not crash when the xmlnode ** is null.
 * Some plugins do that to prevent sending.
 */
static void test_carbons_xml_received_cb_nullptr(void ** state) {
    (void) state;

    carbons_xml_received_cb(NULL, NULL);
}

/**
 * Checks that the callback does not crash when the given xmlnode * is null.
 */
static void test_carbons_xml_received_cb_null(void ** state) {
    (void) state;

    xmlnode * node_p = NULL;
    carbons_xml_received_cb(NULL, &node_p);
}

/**
 * Default case for a received carbon copy of a received message.
 */
static void test_carbons_xml_received_cb_received_success(void ** state) {
    (void) state;

    const char * received_carbon_copy = 
    "<message xmlns='jabber:client' "
             "from='romeo@montague.example' "
             "to='romeo@montague.example/home' "
             "type='chat'>"
        "<received xmlns='urn:xmpp:carbons:2'>"
            "<forwarded xmlns='urn:xmpp:forward:0'>"
                "<message xmlns='jabber:client' "
                         "from='juliet@capulet.example/balcony' "
                         "to='romeo@montague.example/garden' "
                         "type='chat'>"
                    "<body>What man art thou that, thus bescreen'd in night, so stumblest on my counsel?</body>"
                    "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
                "</message>"
            "</forwarded>"
        "</received>"
    "</message>";

    xmlnode * received_carbons_node_p = xmlnode_from_str(received_carbon_copy, -1);

    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, "romeo@montague.example");

    carbons_xml_received_cb(NULL, &received_carbons_node_p);

    assert_string_equal(xmlnode_get_attrib(received_carbons_node_p, "from"), "juliet@capulet.example/balcony");
    assert_string_equal(xmlnode_get_attrib(received_carbons_node_p, "to"), "romeo@montague.example/garden");
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_carbons_is_valid_valid),
        cmocka_unit_test(test_carbons_is_valid_invalid),
        cmocka_unit_test(test_carbons_discover),
        cmocka_unit_test(test_carbons_discover_cb_success),
        cmocka_unit_test(test_carbons_discover_cb_real_world_reply),
        cmocka_unit_test(test_carbons_discover_cb_first_child),
        cmocka_unit_test(test_carbons_discover_cb_last_child),
        cmocka_unit_test(test_carbons_discover_cb_no_carbons),
        cmocka_unit_test(test_carbons_discover_cb_error),
        cmocka_unit_test(test_carbons_discover_cb_empty_reply),
        cmocka_unit_test(test_carbons_xml_received_cb_nullptr),
        cmocka_unit_test(test_carbons_xml_received_cb_null),
        cmocka_unit_test(test_carbons_xml_received_cb_received_success)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}