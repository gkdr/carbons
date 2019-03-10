#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "jabber.h"

#define CARBONS_XMLNS   "urn:xmpp:carbons:2"
#define DISCO_XMLNS     "http://jabber.org/protocol/disco#info"

gboolean __wrap_purple_account_is_connected(const PurpleAccount * acc_p) {
    gboolean connected;
    connected = mock_type(gboolean);
    return connected;
}

const char * __wrap_purple_account_get_protocol_id(const PurpleAccount * acc_p) {
    check_expected_ptr(acc_p);

    char * protocol_id;
    protocol_id = mock_ptr_type(char *);
    return protocol_id;
}

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

void __wrap_purple_debug_warning(const char * category, const char * format, ...) {
    function_called();
}

PurpleConversation * __wrap_purple_find_conversation_with_account(PurpleConversationType type, const char * name, const PurpleAccount *account) {
    PurpleConversation * pc_p;

    pc_p = mock_ptr_type(PurpleConversation *);

    return pc_p;
}

void __wrap_purple_conversation_write(PurpleConversation * conv_p, const char * who,
		const char * message, PurpleMessageFlags flags, time_t mtime) {
    check_expected_ptr(conv_p);
    check_expected(who);
    check_expected(message);
    check_expected(flags);
}

PurpleConversation * __wrap_purple_conversation_new(PurpleConversationType type,
										            PurpleAccount * account,
										            const char * name) {
    check_expected(type);
    check_expected_ptr(account);
    check_expected(name);

    PurpleConversation * conv_p;
    conv_p = mock_ptr_type(PurpleConversation *);
    return conv_p;
}

void __wrap_jabber_add_feature(const gchar * namespace, JabberFeatureEnabled cb) {
    check_expected(namespace);
}

void * __wrap_purple_accounts_get_handle(void) {
    void * handle_p;
    handle_p = mock_ptr_type(void *);
    return handle_p;    
}

PurplePlugin * __wrap_purple_plugins_find_with_id(const char * id) {
    check_expected(id);

    PurplePlugin * plugin_p;
    plugin_p = mock_ptr_type(PurplePlugin *);
    return plugin_p;
}

gulong __wrap_purple_signal_connect(void * instance,
                                    const char * signal,
                                    void * handle,
                                    PurpleCallback func,
                                    void * data) {
    check_expected_ptr(instance);
    check_expected(signal);
    check_expected_ptr(handle);
    check_expected_ptr(func);

    return 1; // ignored
}

gulong __wrap_purple_signal_connect_priority(void * instance,
                                             const char * signal, 
                                             void * handle,
                                             PurpleCallback func,
                                             void * data,
                                             int priority) {
    check_expected_ptr(instance);
    check_expected(signal);
    check_expected_ptr(handle);
    check_expected_ptr(func);
    check_expected(priority);

    return 2; // ignored
}

GList * __wrap_purple_accounts_get_all_active(void) {
    GList * active_accounts;
    active_accounts = mock_ptr_type(GList *);
    return active_accounts;
}