#ifndef __CARBONS_INT_H
# define __CARBONS_INT_H

void carbons_xml_received_cb(PurpleConnection * gc_p, xmlnode ** stanza_pp);
void carbons_xml_stripped_cb(PurpleConnection * gc_p, xmlnode ** stanza_pp);
void carbons_discover(PurpleAccount * acc_p);
void carbons_discover_cb(JabberStream * js_p, const char * from,
                                JabberIqType type,   const char * id,
                                xmlnode * packet_p,  gpointer data_p);
void carbons_enable_cb(JabberStream * js_p, const char * from,
                                  JabberIqType type,   const char * id,
                                  xmlnode * packet_p,  gpointer data_p);

void carbons_account_connect_cb(PurpleAccount * acc_p);
gboolean carbons_plugin_load(PurplePlugin * plugin_p);

#endif /* CARBONS_INTERNAL_H */
