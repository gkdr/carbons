#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "jabber.h"

#include "../src/carbons.h"
#include "mocks.c"

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
 * Do nothing (and especially not crash) when the xmlnode ** is null. Some plugins do this.
 */
static void test_carbons_xml_received_cb_nullptr(void ** state) {
    (void) state;

    carbons_xml_received_cb(NULL, NULL);
}

/**
 * Do not crash when the given xmlnode * is null.
 */
static void test_carbons_xml_received_cb_null(void ** state) {
    (void) state;

    xmlnode * node_p = NULL;
    carbons_xml_received_cb(NULL, &node_p);
}

/**
 * Stop processing when the received stanza is not a 'message'.
 */
static void test_carbons_xml_received_cb_no_msg(void ** state) {
    (void) state;

    char * stanza =
        "<iq xmlns='jabber:client' "
                "to='juliet@capulet.example/balcony' "
                "from='romeo@montague.example/home' "
                "type='chat'>"
            "<body>Neither, fair saint, if either thee dislike.</body>"
            "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
        "</iq>";
    xmlnode * iq_node_p = xmlnode_from_str(stanza, -1);

    carbons_xml_received_cb(NULL, &iq_node_p);

    xmlnode * body_node_p = xmlnode_get_child(iq_node_p, "body");
    assert_string_equal("Neither, fair saint, if either thee dislike.", xmlnode_get_data(body_node_p));
}

/**
 * 'Reject' carbons messages sent by someone other than the own, bare JID.
 * Removes the inner carbons node since it's a malicious message.
 */
static void test_carbons_xml_received_cb_invalid_sender_received(void ** state) {
    (void) state;

    const char * received_carbon_copy = 
        "<message xmlns='jabber:client' "
                "from='badguy@montague.example' "
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

    expect_function_calls(__wrap_purple_debug_warning, 2);

    carbons_xml_received_cb(NULL, &received_carbons_node_p);

    assert_non_null(received_carbons_node_p);
    assert_null(xmlnode_get_child(received_carbons_node_p, "received"));
}

/**
 * Same as above, but for a carbon copy of a sent message.
 */
static void test_carbons_xml_received_cb_invalid_sender_sent(void ** state) {
    (void) state;

    const char * received_carbon_copy = 
        "<message xmlns='jabber:client' "
                "from='badguy@montague.example' "
                "to='romeo@montague.example/home' "
                "type='chat'>"
            "<sent xmlns='urn:xmpp:carbons:2'>"
                "<forwarded xmlns='urn:xmpp:forward:0'>"
                    "<message xmlns='jabber:client' "
                            "to='juliet@capulet.example/balcony' "
                            "from='romeo@montague.example/home' "
                            "type='chat'>"
                        "<body>Neither, fair saint, if either thee dislike.</body>"
                        "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
                    "</message>"
                "</forwarded>"
            "</sent>"
        "</message>";
    xmlnode * received_carbons_node_p = xmlnode_from_str(received_carbon_copy, -1);

    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, "romeo@montague.example");

    expect_function_calls(__wrap_purple_debug_warning, 2);

    carbons_xml_received_cb(NULL, &received_carbons_node_p);

    assert_non_null(received_carbons_node_p);
    assert_null(xmlnode_get_child(received_carbons_node_p, "sent"));
}

/**
 * Default case for a received carbon copy of a received message:
 * Strip outer message and give libpurple the inner one.
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
    assert_ptr_not_equal(xmlnode_get_child(received_carbons_node_p, "body"), NULL);
}

/**
 * Stop processing on malformed carbon-copy of received message: no 'forwarded' node.
 */
static void test_carbons_xml_received_cb_received_no_forwarded(void ** state) {
    (void) state;

    const char * received_carbon_copy = 
        "<message xmlns='jabber:client' "
                "from='romeo@montague.example' "
                "to='romeo@montague.example/home' "
                "type='chat'>"
            "<received xmlns='urn:xmpp:carbons:2'>"
                    "<message xmlns='jabber:client' "
                            "from='juliet@capulet.example/balcony' "
                            "to='romeo@montague.example/garden' "
                            "type='chat'>"
                        "<body>What man art thou that, thus bescreen'd in night, so stumblest on my counsel?</body>"
                        "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
                    "</message>"
            "</received>"
        "</message>";
    xmlnode * received_carbons_node_p = xmlnode_from_str(received_carbon_copy, -1);

    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, "romeo@montague.example");

    expect_function_call(__wrap_purple_debug_error);

    carbons_xml_received_cb(NULL, &received_carbons_node_p);

    // no change since there was no processing
    assert_string_equal(xmlnode_get_attrib(received_carbons_node_p, "from"), "romeo@montague.example");
    assert_string_equal(xmlnode_get_attrib(received_carbons_node_p, "to"), "romeo@montague.example/home");
    assert_ptr_equal(xmlnode_get_child(received_carbons_node_p, "body"), NULL);
}

/**
 * Stop processing on malformed carbon-copy of received message: no 'message' node.
 */
static void test_carbons_xml_received_cb_received_no_message(void ** state) {
    (void) state;

    const char * received_carbon_copy = 
        "<message xmlns='jabber:client' "
                "from='romeo@montague.example' "
                "to='romeo@montague.example/home' "
                "type='chat'>"
            "<received xmlns='urn:xmpp:carbons:2'>"
                "<forwarded />"
            "</received>"
        "</message>";
    xmlnode * received_carbons_node_p = xmlnode_from_str(received_carbon_copy, -1);

    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, "romeo@montague.example");

    expect_function_call(__wrap_purple_debug_error);

    carbons_xml_received_cb(NULL, &received_carbons_node_p);

    // no change since there was no processing
    assert_string_equal(xmlnode_get_attrib(received_carbons_node_p, "from"), "romeo@montague.example");
    assert_string_equal(xmlnode_get_attrib(received_carbons_node_p, "to"), "romeo@montague.example/home");
    assert_ptr_equal(xmlnode_get_child(received_carbons_node_p, "body"), NULL);
}

/**
 * Default case for a received carbon copy of a sent message:
 * Strip the outer message and give libpurple the inner one,
 * plus inject another '<sent/>' node for detection by second, later callback.
 */
static void test_carbons_xml_received_cb_sent_success(void ** state) {
    (void) state;

    const char * received_carbon_copy =
        "<message xmlns='jabber:client' "
                "from='romeo@montague.example' "
                "to='romeo@montague.example/garden' "
                "type='chat'>"
            "<sent xmlns='urn:xmpp:carbons:2'>"
                "<forwarded xmlns='urn:xmpp:forward:0'>"
                    "<message xmlns='jabber:client' "
                            "to='juliet@capulet.example/balcony' "
                            "from='romeo@montague.example/home' "
                            "type='chat'>"
                        "<body>Neither, fair saint, if either thee dislike.</body>"
                        "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
                    "</message>"
                "</forwarded>"
            "</sent>"
        "</message>";
    xmlnode * received_carbons_node_p = xmlnode_from_str(received_carbon_copy, -1);

    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, "romeo@montague.example");

    carbons_xml_received_cb(NULL, &received_carbons_node_p);

    assert_string_equal(xmlnode_get_attrib(received_carbons_node_p, "from"), "romeo@montague.example/home");
    assert_string_equal(xmlnode_get_attrib(received_carbons_node_p, "to"), "juliet@capulet.example/balcony");
    assert_ptr_not_equal(xmlnode_get_child(received_carbons_node_p, "body"), NULL);
    assert_ptr_not_equal(xmlnode_get_child_with_namespace(received_carbons_node_p, "sent", CARBONS_XMLNS), NULL);
}

/**
 * Stop processing on malformed carbon-copy of sent message: no 'forwaded' node.
 */
static void test_carbons_xml_received_cb_sent_no_forwarded(void ** state) {
    (void) state;

    const char * received_carbon_copy =
        "<message xmlns='jabber:client' "
                "from='romeo@montague.example' "
                "to='romeo@montague.example/garden' "
                "type='chat'>"
            "<sent xmlns='urn:xmpp:carbons:2'>"
                    "<message xmlns='jabber:client' "
                            "to='juliet@capulet.example/balcony' "
                            "from='romeo@montague.example/home' "
                            "type='chat'>"
                        "<body>Neither, fair saint, if either thee dislike.</body>"
                        "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
                    "</message>"
            "</sent>"
        "</message>";
    xmlnode * received_carbons_node_p = xmlnode_from_str(received_carbon_copy, -1);

    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, "romeo@montague.example");

    expect_function_call(__wrap_purple_debug_error);

    carbons_xml_received_cb(NULL, &received_carbons_node_p);

    // no change since there was no processing
    assert_string_equal(xmlnode_get_attrib(received_carbons_node_p, "from"), "romeo@montague.example");
    assert_string_equal(xmlnode_get_attrib(received_carbons_node_p, "to"), "romeo@montague.example/garden");
    assert_ptr_equal(xmlnode_get_child(received_carbons_node_p, "body"), NULL);
}

/**
 * Stop processing on malformed carbon-copy of sent message: no 'message' node.
 */
static void test_carbons_xml_received_cb_sent_no_message(void ** state) {
    (void) state;

    const char * received_carbon_copy =
        "<message xmlns='jabber:client' "
                "from='romeo@montague.example' "
                "to='romeo@montague.example/garden' "
                "type='chat'>"
            "<sent xmlns='urn:xmpp:carbons:2'>"
                "<forwarded />"
            "</sent>"
        "</message>";
    xmlnode * received_carbons_node_p = xmlnode_from_str(received_carbon_copy, -1);

    will_return(__wrap_purple_connection_get_account, NULL);
    will_return(__wrap_purple_account_get_username, "romeo@montague.example");

    expect_function_call(__wrap_purple_debug_error);

    carbons_xml_received_cb(NULL, &received_carbons_node_p);

    // no change since there was no processing
    assert_string_equal(xmlnode_get_attrib(received_carbons_node_p, "from"), "romeo@montague.example");
    assert_string_equal(xmlnode_get_attrib(received_carbons_node_p, "to"), "romeo@montague.example/garden");
    assert_ptr_equal(xmlnode_get_child(received_carbons_node_p, "body"), NULL);
}

static void test_carbons_xml_stripped_cb_nullptr(void ** state) {
    (void) state;

    carbons_xml_stripped_cb(NULL, NULL);
}

static void test_carbons_xml_stripped_cb_null(void ** state) {
    (void) state;

    xmlnode * node_p = NULL;
    carbons_xml_stripped_cb(NULL, &node_p);
}

/**
 * If the stanza contains an empty 'sent' node injected by the earlier callback, write the body to the conversation.
 */
static void test_carbons_xml_stripped_cb_success(void ** state) {
    (void) state;

    const char * stripped_carbon_copy =
        "<message xmlns='jabber:client' "
                "to='juliet@capulet.example/balcony' "
                "from='romeo@montague.example/home' "
                "type='chat'>"
            "<sent xmlns='urn:xmpp:carbons:2' />"
            "<body>Neither, fair saint, if either thee dislike.</body>"
            "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
        "</message>";
    xmlnode * stripped_carbons_node_p = xmlnode_from_str(stripped_carbon_copy, -1);

    will_return(__wrap_purple_connection_get_account, NULL);
    PurpleConversation * pc_p = NULL;
    will_return(__wrap_purple_find_conversation_with_account, &pc_p);

    expect_value(__wrap_purple_conversation_write, conv_p, &pc_p);
    expect_string(__wrap_purple_conversation_write, who, "romeo@montague.example/home");
    expect_string(__wrap_purple_conversation_write, message, "Neither, fair saint, if either thee dislike.");
    expect_value(__wrap_purple_conversation_write, flags, PURPLE_MESSAGE_SEND);

    carbons_xml_stripped_cb(NULL, &stripped_carbons_node_p);

    assert_non_null(stripped_carbons_node_p);
    assert_null(xmlnode_get_child(stripped_carbons_node_p, "sent"));
    assert_null(xmlnode_get_child(stripped_carbons_node_p, "body"));
}

/**
 * Like above, but also create the conversation window if it does not exist yet.
 */
static void test_carbons_xml_stripped_cb_success_new_conv(void ** state) {
    (void) state;

    const char * stripped_carbon_copy =
        "<message xmlns='jabber:client' "
                "to='juliet@capulet.example/balcony' "
                "from='romeo@montague.example/home' "
                "type='chat'>"
            "<sent xmlns='urn:xmpp:carbons:2' />"
            "<body>Neither, fair saint, if either thee dislike.</body>"
            "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
        "</message>";
    xmlnode * stripped_carbons_node_p = xmlnode_from_str(stripped_carbon_copy, -1);

    char * acc_mock = "fake purple account";
    will_return(__wrap_purple_connection_get_account, &acc_mock);
    will_return(__wrap_purple_find_conversation_with_account, NULL);

    expect_value(__wrap_purple_conversation_new, type, PURPLE_CONV_TYPE_IM);
    expect_value(__wrap_purple_conversation_new, account, &acc_mock);
    expect_string(__wrap_purple_conversation_new, name, "juliet@capulet.example");

    PurpleConversation * pc_p = NULL;
    will_return(__wrap_purple_conversation_new, &pc_p);

    expect_value(__wrap_purple_conversation_write, conv_p, &pc_p);
    expect_string(__wrap_purple_conversation_write, who, "romeo@montague.example/home");
    expect_string(__wrap_purple_conversation_write, message, "Neither, fair saint, if either thee dislike.");
    expect_value(__wrap_purple_conversation_write, flags, PURPLE_MESSAGE_SEND);

    carbons_xml_stripped_cb(NULL, &stripped_carbons_node_p);

    assert_non_null(stripped_carbons_node_p);
    assert_null(xmlnode_get_child(stripped_carbons_node_p, "sent"));
    assert_null(xmlnode_get_child(stripped_carbons_node_p, "body"));
}

/**
 * Do nothing if the stanza is not a 'message'.
 */
static void test_carbons_xml_stripped_cb_not_a_message(void ** state) {
    (void) state;

    const char * stripped_carbon_copy =
        "<iq xmlns='jabber:client' "
                "to='juliet@capulet.example/balcony' "
                "from='romeo@montague.example/home' "
                "type='chat'>"
            "<body>Neither, fair saint, if either thee dislike.</body>"
            "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
        "</iq>";
    xmlnode * stripped_carbons_node_p = xmlnode_from_str(stripped_carbon_copy, -1);

    carbons_xml_stripped_cb(NULL, &stripped_carbons_node_p);

    assert_string_equal(xmlnode_to_str(stripped_carbons_node_p, NULL), stripped_carbon_copy);
}

/**
 * Do nothing if the stanza does not contain an empty 'sent' node.
 */
static void test_carbons_xml_stripped_cb_do_nothing(void ** state) {
    (void) state;

    const char * stripped_carbon_copy =
        "<message xmlns='jabber:client' "
                "to='juliet@capulet.example/balcony' "
                "from='romeo@montague.example/home' "
                "type='chat'>"
            "<body>Neither, fair saint, if either thee dislike.</body>"
            "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
        "</message>";
    xmlnode * stripped_carbons_node_p = xmlnode_from_str(stripped_carbon_copy, -1);

    carbons_xml_stripped_cb(NULL, &stripped_carbons_node_p);

    assert_string_equal(xmlnode_to_str(stripped_carbons_node_p, NULL), stripped_carbon_copy);
}

/**
 * Add carbons to client capabilities, register the init callback for accounts signing on,
 * and also the two message handling callbacks on incoming XML.
 */
static void test_carbons_plugin_load_app_start(void ** state) {
    (void) state;

    void * plugin_mock = "plugin mock";

    void * accounts_handle_mock = "accounts handle mock";
    will_return(__wrap_purple_accounts_get_handle, accounts_handle_mock);
    expect_value(__wrap_purple_signal_connect, instance, accounts_handle_mock);
    expect_string(__wrap_purple_signal_connect, signal, "account-signed-on");
    expect_value(__wrap_purple_signal_connect, handle, plugin_mock);
    expect_value(__wrap_purple_signal_connect, func, carbons_account_connect_cb);

    void * jabber_handle_mock = "jabber handle mock";
    expect_string_count(__wrap_purple_plugins_find_with_id, id, "prpl-jabber", 2);
    will_return_count(__wrap_purple_plugins_find_with_id, jabber_handle_mock, 2);

    expect_value(__wrap_purple_signal_connect_priority, instance, jabber_handle_mock);
    expect_string(__wrap_purple_signal_connect_priority, signal, "jabber-receiving-xmlnode");
    expect_value(__wrap_purple_signal_connect_priority, handle, plugin_mock);
    expect_value(__wrap_purple_signal_connect_priority, func, carbons_xml_received_cb);
    expect_value(__wrap_purple_signal_connect_priority, priority, PURPLE_PRIORITY_LOWEST + 100);

    expect_value(__wrap_purple_signal_connect_priority, instance, jabber_handle_mock);
    expect_string(__wrap_purple_signal_connect_priority, signal, "jabber-receiving-xmlnode");
    expect_value(__wrap_purple_signal_connect_priority, handle, plugin_mock);
    expect_value(__wrap_purple_signal_connect_priority, func, carbons_xml_stripped_cb);
    expect_value(__wrap_purple_signal_connect_priority, priority, PURPLE_PRIORITY_HIGHEST - 50);

    will_return(__wrap_purple_accounts_get_all_active, NULL);

    expect_string(__wrap_jabber_add_feature, namespace, CARBONS_XMLNS);

    assert_true(carbons_plugin_load(plugin_mock));
}

/**
 * If the plugin is loaded while the application is already running, the functions which are usually called on
 * account connect have to be called manually.
 */
static void test_carbons_plugin_load_while_connected(void ** state) {
    (void) state;

    void * plugin_mock = "plugin mock";

    void * accounts_handle_mock = "accounts handle mock";
    will_return(__wrap_purple_accounts_get_handle, accounts_handle_mock);
    expect_value(__wrap_purple_signal_connect, instance, accounts_handle_mock);
    expect_string(__wrap_purple_signal_connect, signal, "account-signed-on");
    expect_value(__wrap_purple_signal_connect, handle, plugin_mock);
    expect_value(__wrap_purple_signal_connect, func, carbons_account_connect_cb);

    void * jabber_handle_mock = "jabber handle mock";
    expect_string_count(__wrap_purple_plugins_find_with_id, id, "prpl-jabber", 2);
    will_return_count(__wrap_purple_plugins_find_with_id, jabber_handle_mock, 2);

    expect_value(__wrap_purple_signal_connect_priority, instance, jabber_handle_mock);
    expect_string(__wrap_purple_signal_connect_priority, signal, "jabber-receiving-xmlnode");
    expect_value(__wrap_purple_signal_connect_priority, handle, plugin_mock);
    expect_value(__wrap_purple_signal_connect_priority, func, carbons_xml_received_cb);
    expect_value(__wrap_purple_signal_connect_priority, priority, PURPLE_PRIORITY_LOWEST + 100);

    expect_value(__wrap_purple_signal_connect_priority, instance, jabber_handle_mock);
    expect_string(__wrap_purple_signal_connect_priority, signal, "jabber-receiving-xmlnode");
    expect_value(__wrap_purple_signal_connect_priority, handle, plugin_mock);
    expect_value(__wrap_purple_signal_connect_priority, func, carbons_xml_stripped_cb);
    expect_value(__wrap_purple_signal_connect_priority, priority, PURPLE_PRIORITY_HIGHEST - 50);

    PurpleAccount * account_p = malloc(sizeof(PurpleAccount));
    will_return(__wrap_purple_accounts_get_all_active, g_list_prepend(NULL, account_p));
    will_return(__wrap_purple_account_is_connected, TRUE);

    expect_value(__wrap_purple_account_get_protocol_id, acc_p, account_p);
    will_return(__wrap_purple_account_get_protocol_id, "does not matter, just checking if function is called");

    // unfortunately, there is currently no way to check that jabber_add_feature was NOT called
    
    assert_true(carbons_plugin_load(plugin_mock));

    free(account_p);
}

/**
 * Send a discovery request on account connect, but only if the connecting account is of type XMPP-
 */
static void test_carbons_account_connect_cb(void ** state) {
    (void) state;

    const char * test_domain = "test.org";
    const char * test_jid = "me-testing@test.org/resource";

    will_return(__wrap_purple_account_get_protocol_id, "prpl-jabber");
    expect_any(__wrap_purple_account_get_protocol_id, acc_p);
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

    carbons_account_connect_cb(NULL);

    free(js_p);
}

/**
 * Notify user of an error if enabling carbons fails.
 */
static void test_carbons_enable_cb_error(void ** state) {
    (void) state;

    will_return(__wrap_purple_connection_get_account, "does not matter");
    will_return(__wrap_purple_account_get_username, "does not matter");

    expect_function_call(__wrap_purple_debug_error);

    JabberStream * js_p = malloc(sizeof (JabberStream));
    carbons_enable_cb(js_p, "", JABBER_IQ_ERROR, "", NULL, NULL);

    free(js_p);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_carbons_discover_cb_success),
        cmocka_unit_test(test_carbons_discover_cb_real_world_reply),
        cmocka_unit_test(test_carbons_discover_cb_first_child),
        cmocka_unit_test(test_carbons_discover_cb_last_child),
        cmocka_unit_test(test_carbons_discover_cb_no_carbons),
        cmocka_unit_test(test_carbons_discover_cb_error),
        cmocka_unit_test(test_carbons_discover_cb_empty_reply),
        cmocka_unit_test(test_carbons_xml_received_cb_nullptr),
        cmocka_unit_test(test_carbons_xml_received_cb_null),
        cmocka_unit_test(test_carbons_xml_received_cb_no_msg),
        cmocka_unit_test(test_carbons_xml_received_cb_invalid_sender_received),
        cmocka_unit_test(test_carbons_xml_received_cb_invalid_sender_sent),
        cmocka_unit_test(test_carbons_xml_received_cb_received_success),
        cmocka_unit_test(test_carbons_xml_received_cb_received_no_forwarded),
        cmocka_unit_test(test_carbons_xml_received_cb_received_no_message),
        cmocka_unit_test(test_carbons_xml_received_cb_sent_success),
        cmocka_unit_test(test_carbons_xml_received_cb_sent_no_forwarded),
        cmocka_unit_test(test_carbons_xml_received_cb_sent_no_message),
        cmocka_unit_test(test_carbons_xml_stripped_cb_nullptr),
        cmocka_unit_test(test_carbons_xml_stripped_cb_null),
        cmocka_unit_test(test_carbons_xml_stripped_cb_success),
        cmocka_unit_test(test_carbons_xml_stripped_cb_success_new_conv),
        cmocka_unit_test(test_carbons_xml_stripped_cb_not_a_message),
        cmocka_unit_test(test_carbons_xml_stripped_cb_do_nothing),
        cmocka_unit_test(test_carbons_plugin_load_app_start),
        cmocka_unit_test(test_carbons_plugin_load_while_connected),
        cmocka_unit_test(test_carbons_account_connect_cb),
        cmocka_unit_test(test_carbons_enable_cb_error)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}