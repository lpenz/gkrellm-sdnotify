// Copyright (C) 2019 Leandro Lisboa Penz <lpenz@lpenz.org>
// This file is subject to the terms and conditions defined in
// file 'LICENSE', which is part of this source code package.

/**
 * \file
 * \brief  A gkrellm plugin that calls sd_notify when loaded
 */

#include <sstream>
#include <string>

#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <systemd/sd-daemon.h>

extern "C" {
#include <gkrellm2/gkrellm.h>
}

#define PROGRAM_NAME "gkrellm-sdnotify"
#define PROGRAM_VERSION "0.1"

#define GKRELLM_SDNOTIFY_VERSION PROGRAM_VERSION
#define PLUGIN_PLACEMENT MON_MAIL

static char CONFIG_KEYWORD[] = PROGRAM_NAME;

static gchar AboutText[] =
    "GKrellM sdnotify version " GKRELLM_SDNOTIFY_VERSION
    "\n\n\n"
    "MIT License\n"
    "\n"
    "Copyright (c) 2019 Leandro Lisboa Penz\n"
    "\n"
    "Permission is hereby granted, free of charge, to any person\n"
    "obtaining a copy of this software and associated documentation\n"
    "files (the \"Software\"), to deal in the Software without\n"
    "restriction, including without limitation the rights to use, copy,\n"
    "modify, merge, publish, distribute, sublicense, and/or sell copies\n"
    "of the Software, and to permit persons to whom the Software is\n"
    "furnished to do so, subject to the following conditions:\n"
    "\n"
    "The above copyright notice and this permission notice shall be\n"
    "included in all copies or substantial portions of the Software.\n"
    "\n"
    "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,\n"
    "EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF\n"
    "MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND\n"
    "NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT\n"
    "HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,\n"
    "WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
    "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER\n"
    "DEALINGS IN THE SOFTWARE.\n";

static gchar HelpText[][256] = {
    "<h>" PROGRAM_NAME "\n\n",
    "gkellm-sdnotify calls sd_notify (3) after a user-defined delay.\n\n"
    "That allows you to load gkrellm as a Type=notify service unit in "
    "systemd.\n\n",
};

static struct {
    GkrellmMonitor *plugin_mon;
    struct {
        time_t start;
        bool done;
    } sts;
    struct {
        int delay;
    } cfg;
    struct {
        GtkWidget *delay;
    } widget;
} GkrSdN;

/****************************************************************************/

static void update_plugin(void) {
    GkrellmTicks *t = gkrellm_ticks();

    if (GkrSdN.sts.done || !t->second_tick) {
        return;
    }
    time_t now = time(NULL);
    int elapsed = now - GkrSdN.sts.start;
    if (elapsed <= GkrSdN.cfg.delay) {
        return;
    }
    sd_notify(0, "READY=1");
    GkrSdN.sts.done = true;
}

/****************************************************************************/

/**
 * \brief  Create an option in config screen.
 * \param  parent Parent vbox.
 * \param  name Option name.
 * \param  size Max length of user text.
 * \param  starttext Initial text.
 * \return The option widget.
 */
static GtkWidget *create_option(GtkWidget *parent, const char *name, int size,
                                const char *starttext) {
    GtkWidget *hbox;
    GtkWidget *label;
    GtkWidget *rv;

    hbox = gtk_hbox_new(FALSE, 10);
    gtk_box_pack_start(GTK_BOX(parent), hbox, FALSE, TRUE, 5);
    label = gtk_label_new(name);
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
    rv = gtk_entry_new_with_max_length(size);
    gtk_box_pack_start(GTK_BOX(hbox), rv, FALSE, TRUE, 0);
    gtk_entry_set_text(GTK_ENTRY(rv), starttext);

    return rv;
}

static void create_plugin_tab(GtkWidget *tab_vbox) {
    GtkWidget *tabs;

    tabs = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(tabs), GTK_POS_TOP);
    gtk_box_pack_start(GTK_BOX(tab_vbox), tabs, TRUE, TRUE, 0);

    /* Config: */
    GtkWidget *cfgVbox;
    char options[] = "Options";
    cfgVbox = gkrellm_gtk_framed_notebook_page(tabs, options);
    std::ostringstream tmp;
    tmp << GkrSdN.cfg.delay;
    GkrSdN.widget.delay = create_option(cfgVbox, "Delay:", 256, tmp.str().c_str());

    /* Help */
    char help[] = "Help";
    GtkWidget *helpWvbox = gkrellm_gtk_framed_notebook_page(tabs, help);
    GtkWidget *helpWtext = gkrellm_gtk_scrolled_text_view(
        helpWvbox, NULL, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    for (size_t i = 0; i < sizeof(HelpText) / sizeof(*HelpText); ++i)
        gkrellm_gtk_text_view_append(helpWtext, HelpText[i]);

    /* About */
    GtkWidget *aboutWlabel = gtk_label_new("About");
    GtkWidget *aboutWtext = gtk_label_new(AboutText);
    gtk_notebook_append_page(GTK_NOTEBOOK(tabs), aboutWtext, aboutWlabel);
}

static void apply_plugin_config(void) {
    GkrSdN.cfg.delay = atoi(gkrellm_gtk_entry_get_text(&GkrSdN.widget.delay));
}

/****************************************************************************/

static void save_plugin_config(FILE *f) {
    fprintf(f, PROGRAM_NAME " delay %d\n", GkrSdN.cfg.delay);
}

static void load_plugin_config(gchar *arg) {
    std::istringstream ss(arg);
    std::string config;
    std::string item;

    ss >> config;
    if (config == "delay") {
        ss >> GkrSdN.cfg.delay;
    }
}

/****************************************************************************/

static char sdnotifyPluginName[] = "sd_notify";

static GkrellmMonitor plugin_mon = {
    sdnotifyPluginName,  /* Name, for config tab.        */
    0,                   /* Id,  0 if a plugin           */
    NULL,                /* The create_plugin() function */
    update_plugin,       /* The update_plugin() function */
    create_plugin_tab,   /* The create_plugin_tab() config function */
    apply_plugin_config, /* The apply_plugin_config() function      */
    save_plugin_config,  /* The save_plugin_config() function  */
    load_plugin_config,  /* The load_plugin_config() function  */
    CONFIG_KEYWORD,      /* config keyword                     */
    NULL,                /* Undefined 2  */
    NULL,                /* Undefined 1  */
    NULL,                /* Undefined 0  */
    PLUGIN_PLACEMENT,    /* Insert plugin before this monitor.       */
    NULL,                /* Handle if a plugin, filled in by GKrellM */
    NULL                 /* path if a plugin, filled in by GKrellM   */
};

extern "C" GkrellmMonitor *gkrellm_init_plugin(void) {
    time(&GkrSdN.sts.start);
    GkrSdN.plugin_mon = &plugin_mon;
    return &plugin_mon;
}
