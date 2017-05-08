/*
carbons - XEP-0280 plugin for libpurple
Copyright (C) 2017, Richard Bayerle <riba@firemail.cc>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <glib.h>

#include <stdlib.h>
#include <string.h>

#include "cmds.h"
#include "debug.h"
#include "notify.h"
#include "plugin.h"
#include "version.h"

#include "iq.h"

#define CARBONS_SETTING_NAME "carbons-enabled"

#define CARBONS_XMLNS "urn:xmpp:carbons:2"
#define XMLNS_ATTR_NAME "xmlns"

#define CARBONS_ENABLE 1
#define CARBONS_DISABLE 0

int carbons_cmd_id;
int mode_global;

static int carbons_is_valid(PurpleAccount * acc_p, xmlnode * outer_msg_stanza_p) {
  char ** split;

  split = g_strsplit(purple_account_get_username(acc_p), "/", 2);

  if (g_strcmp0(split[0], xmlnode_get_attrib(outer_msg_stanza_p, "from"))) {
    purple_debug_warning("carbons", "Invalid sender: %s (should be: %s)\n", xmlnode_get_attrib(outer_msg_stanza_p, "from"), split[0]);
    g_strfreev(split);
    return 0;
  } else {
    g_strfreev(split);
    return 1;
  }
}

static void carbons_xml_received_cb(PurpleConnection * gc_p, xmlnode ** stanza_pp) {
  xmlnode * carbons_node_p = (void *) 0;
  xmlnode * forwarded_node_p = (void *) 0;
  xmlnode * msg_node_p = (void *) 0;
  xmlnode * body_node_p = (void *) 0;

  char * buddy_name_bare = (void *) 0;
  PurpleConversation * conv_p = (void *) 0;

  carbons_node_p = xmlnode_get_child_with_namespace(*stanza_pp, "received", CARBONS_XMLNS);
  if (carbons_node_p) {
    purple_debug_info("carbons", "Received carbon copy of a received message.\n");

    if (!carbons_is_valid(purple_connection_get_account(gc_p), *stanza_pp)) {
      purple_debug_warning("carbons", "Ignoring carbon copy of received message with invalid sender.\n");
      return;
    }

    forwarded_node_p = xmlnode_get_child(carbons_node_p, "forwarded");
    if (!forwarded_node_p) {
      purple_debug_error("carbons", "Ignoring carbon copy of received message that does not contain a 'forwarded' node.\n");
      return;
    }

    msg_node_p = xmlnode_get_child(forwarded_node_p, "message");
    if (!msg_node_p) {
      purple_debug_error("carbons", "Ignoring carbon copy of received message that does not contain a 'message' node.\n");
      return;
    }

    msg_node_p = xmlnode_copy(msg_node_p);
    xmlnode_free(*stanza_pp);
    *stanza_pp = msg_node_p;
    return;
  }

  carbons_node_p = xmlnode_get_child_with_namespace(*stanza_pp, "sent", CARBONS_XMLNS);
  if (carbons_node_p) {
    purple_debug_info("carbons", "Received carbon copy of a sent message.\n");

    if (!carbons_is_valid(purple_connection_get_account(gc_p), *stanza_pp)) {
      purple_debug_warning("carbons", "Ignoring carbon copy of sent message with invalid sender.\n");
      return;
    }

    forwarded_node_p = xmlnode_get_child(carbons_node_p, "forwarded");
    if (!forwarded_node_p) {
      purple_debug_error("carbons", "Ignoring carbon copy of sent message that does not contain a 'forwarded' node.\n");
      return;
    }

    msg_node_p = xmlnode_get_child(forwarded_node_p, "message");
    if (!msg_node_p) {
      purple_debug_error("carbons", "Ignoring carbon copy of sent message that does not contain a 'message' node.\n");
      return;
    }

    body_node_p = xmlnode_get_child(msg_node_p, "body");
    if (!body_node_p) {
      purple_debug_info("carbons", "Carbon copy of sent message does not contain a body - stripping and passing it through.\n");
      msg_node_p = xmlnode_copy(msg_node_p);
      xmlnode_free(*stanza_pp);
      *stanza_pp = msg_node_p;

      return;
    }

    buddy_name_bare = jabber_get_bare_jid(xmlnode_get_attrib(msg_node_p, "to"));
    conv_p = purple_find_conversation_with_account(PURPLE_CONV_TYPE_IM, buddy_name_bare, purple_connection_get_account(gc_p));
    if (!conv_p) {
      conv_p = purple_conversation_new(PURPLE_CONV_TYPE_IM, purple_connection_get_account(gc_p), buddy_name_bare);
    }

    purple_debug_info("carbons", "Writing body of the carbon copy of a sent message to the conversation window with %s.\n", buddy_name_bare);
    purple_conversation_write(conv_p, xmlnode_get_attrib(msg_node_p, "from"), xmlnode_get_data(body_node_p), PURPLE_MESSAGE_SEND, time((void *) 0));

    g_free(buddy_name_bare);
  }
}

static void carbons_autoenable_cb(JabberStream * js_p, const char * from,
                                    JabberIqType type, const char * id,
                                    xmlnode * packet_p, gpointer data_p) {

  const char * accname = purple_account_get_username(purple_connection_get_account(js_p->gc));

  if (type == JABBER_IQ_ERROR) {
    purple_debug_error("carbons", "Server returned an error when trying to automatically activate carbons for %s.\n", accname);
    purple_account_set_bool(purple_connection_get_account(js_p->gc), CARBONS_SETTING_NAME, FALSE);
  } else {
    purple_debug_info("carbons", "Successfully automatically activated carbons for %s.\n", accname);
    purple_account_set_bool(purple_connection_get_account(js_p->gc), CARBONS_SETTING_NAME, TRUE);
  }
}

static void carbons_autoenable(PurpleAccount * acc_p) {
  JabberIq * jiq_p = (void *) 0;
  xmlnode * req_node_p = (void *) 0;
  JabberStream * js_p = purple_connection_get_protocol_data(purple_account_get_connection(acc_p));

  jiq_p = jabber_iq_new(js_p, JABBER_IQ_SET);
  req_node_p = xmlnode_new_child(jiq_p->node, "enable");
  xmlnode_set_namespace(req_node_p, CARBONS_XMLNS);

  jabber_iq_set_callback(jiq_p, carbons_autoenable_cb, (void *) 0);
  jabber_iq_send(jiq_p);

  purple_debug_info("carbons", "Sent startup enable request for %s\n", purple_account_get_username(acc_p));
}

static void carbons_account_connect_cb(PurpleAccount * acc_p) {
  if (strcmp(purple_account_get_protocol_id(acc_p), "prpl-jabber")) {
    return;
  }

  if (!purple_account_get_bool(acc_p, CARBONS_SETTING_NAME, FALSE)) {
    return;
  }

  carbons_autoenable(acc_p);
}

static void carbons_switch_cb(JabberStream * js_p, const char * from,
                                    JabberIqType type, const char * id,
                                    xmlnode * packet_p, gpointer data_p) {
  PurpleConversation * conv_p = (PurpleConversation *) data_p;

  if (type == JABBER_IQ_ERROR) {
    purple_conversation_write(conv_p, "carbons", "Server returned an error. See the debug log for more info.", PURPLE_MESSAGE_ERROR | PURPLE_MESSAGE_NO_LOG, time((void *) 0));

    if (mode_global) {
      purple_account_set_bool(purple_conversation_get_account(conv_p), CARBONS_SETTING_NAME, FALSE);
    } else {
      purple_account_set_bool(purple_conversation_get_account(conv_p), CARBONS_SETTING_NAME, TRUE);
    }
  } else {
    purple_conversation_write(conv_p, "carbons", "Success!", PURPLE_MESSAGE_SYSTEM | PURPLE_MESSAGE_NO_LOG, time((void *) 0));

    if (mode_global) {
      purple_account_set_bool(purple_conversation_get_account(conv_p), CARBONS_SETTING_NAME, TRUE);
    } else {
      purple_account_set_bool(purple_conversation_get_account(conv_p), CARBONS_SETTING_NAME, FALSE);
    }
  }
}

static void carbons_switch_do(PurpleConversation * conv_p, int mode) {
  JabberIq * jiq_p = (void *) 0;
  xmlnode * req_node_p = (void *) 0;

  mode_global = mode;
  const char * mode_str = mode ? "enable" : "disable";
  JabberStream * js_p = purple_connection_get_protocol_data(purple_account_get_connection(purple_conversation_get_account(conv_p)));

  jiq_p = jabber_iq_new(js_p, JABBER_IQ_SET);
  req_node_p = xmlnode_new_child(jiq_p->node, mode_str);
  xmlnode_set_namespace(req_node_p, CARBONS_XMLNS);

  jabber_iq_set_callback(jiq_p, carbons_switch_cb, conv_p);
  jabber_iq_send(jiq_p);

  purple_debug_info("carbons", "Sent %s request for %s\n", mode_str, purple_account_get_username(purple_connection_get_account(js_p->gc)));
}

static PurpleCmdRet carbons_cmd_func(PurpleConversation * conv_p,
                                      const gchar * cmd,
                                      gchar ** args,
                                      gchar ** error,
                                      void * data_p) {

  char * msg = (void *) 0;
  const char * username = purple_account_get_username(purple_conversation_get_account(conv_p));

  if (!g_strcmp0(args[0], "on")) {
    msg = g_strdup_printf("Turning carbons ON for %s...", username);
    carbons_switch_do(conv_p, CARBONS_ENABLE);
  } else if (!g_strcmp0(args[0], "off")) {
    msg = g_strdup_printf("Turning carbons OFF for %s...", username);
    carbons_switch_do(conv_p, CARBONS_DISABLE);
  } else {
    msg = g_strdup_printf("No such command. Usage: /carbons [on|off]");
  }

  if (msg) {
    purple_conversation_write(conv_p, "carbons", msg, PURPLE_MESSAGE_SYSTEM | PURPLE_MESSAGE_NO_LOG, time((void *) 0));
  }

  g_free(msg);

  return PURPLE_CMD_RET_OK;
}

static gboolean
carbons_plugin_load(PurplePlugin * plugin_p) {

  (void) jabber_add_feature(CARBONS_XMLNS, (void *) 0);

  carbons_cmd_id = purple_cmd_register("carbons",
                                     "w",
                                     PURPLE_CMD_P_PLUGIN,
                                     PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT | PURPLE_CMD_FLAG_PRPL_ONLY | PURPLE_CMD_FLAG_ALLOW_WRONG_ARGS,
                                     "prpl-jabber",
                                     carbons_cmd_func,
                                     "carbons &lt;on|off&gt;:  "
                                     "Turns Message Carbons on or off for the calling account.",
                                     (void *) 0);

  (void) purple_signal_connect(purple_accounts_get_handle(), "account-signed-on", plugin_p, PURPLE_CALLBACK(carbons_account_connect_cb), NULL);
  (void) purple_signal_connect_priority(purple_plugins_find_with_id("prpl-jabber"), "jabber-receiving-xmlnode", plugin_p, PURPLE_CALLBACK(carbons_xml_received_cb), NULL, PURPLE_PRIORITY_LOWEST + 100);

  return TRUE;
}

static PurplePluginInfo info = {
    PURPLE_PLUGIN_MAGIC,
    PURPLE_MAJOR_VERSION,
    PURPLE_MINOR_VERSION,
    PURPLE_PLUGIN_STANDARD,
    NULL,
    0,
    NULL,
    PURPLE_PRIORITY_DEFAULT,

    "core-riba-carbons",
    "XMPP Message Carbons",
    "0.1.3",

    "Implements XEP-0280: Message Carbons as a plugin.",
    "This plugin enables a consistent history view across multiple devices which are online at the same time.",
    "Richard Bayerle <riba@firemail.cc>",
    "https://github.com/gkdr/carbons",

    carbons_plugin_load,
    NULL,
    NULL,

    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

static void
carbons_plugin_init(PurplePlugin * plugin_p)
{
  PurplePluginInfo * info_p = plugin_p->info;

  info_p->dependencies = g_list_prepend(info_p->dependencies, "prpl-jabber");
}

PURPLE_INIT_PLUGIN(carbons, carbons_plugin_init, info)
