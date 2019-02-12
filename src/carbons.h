#ifndef __CARBONS_H
# define __CARBONS_H

# define CARBONS_VERSION "0.2.1"
# define CARBONS_AUTHOR "Richard Bayerle <riba@firemail.cc>"

int carbons_is_valid(PurpleAccount * acc_p, xmlnode * outer_msg_stanza_p);
void carbons_xml_received_cb(PurpleConnection * gc_p, xmlnode ** stanza_pp);
void carbons_discover(PurpleAccount * acc_p);
void carbons_discover_cb(JabberStream * js_p, const char * from,
                                JabberIqType type,   const char * id,
                                xmlnode * packet_p,  gpointer data_p);
void carbons_enable_cb(JabberStream * js_p, const char * from,
                                  JabberIqType type,   const char * id,
                                  xmlnode * packet_p,  gpointer data_p);

#endif /* __CARBONS_H */
