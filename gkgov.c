
#include <gkrellm2/gkrellm.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

static GkrellmMonitor *monitor;
static GkrellmPanel *panel;
static GkrellmDecal *decal_text;
static int style_id;


static void read_gov(int cpu_id, char *buffer_, int bufsz_)
{
        int i = 0;
        
	FILE *f;
	char syspath[] = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor";
        
	char gov[30];

	syspath[27] = cpu_id + '0';

	if ((f = fopen(syspath, "r")) == NULL) {
		gov[0] =0 ;
	} else {
		fscanf(f, "%s", gov);
		fclose(f);
	}
	
	while(gov[i])
        {
            gov[i]=toupper(gov[i]);
            i++;
        }
        gov[i]=0;
	
        strcpy(buffer_, gov);
        
}

static gint panel_expose_event(GtkWidget *widget, GdkEventExpose *ev)
{
	gdk_draw_pixmap(widget->window,
	                widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
	                panel->pixmap,
	                ev->area.x,
	                ev->area.y,
	                ev->area.x,
	                ev->area.y,
	                ev->area.width,
	                ev->area.height);

	return 0;
}


static void update_plugin()
{
	static char info[32];
        static int w, x_scroll;
        gboolean scrolling;
        gint w_decal, w_scroll;
        
        w = gkrellm_chart_width();
        
        read_gov(0, info, 31);
        
       // printf ("info: %s\n",  info);
        
        w_scroll = gdk_string_width(gdk_font_from_description(decal_text->text_style.font),info);
        
        decal_text->x_off = (w - w_scroll) / 2;
        
        scrolling = (decal_text->x_off < 0)? TRUE : FALSE;
        
        if (scrolling) {
            
#if defined(GKRELLM_HAVE_DECAL_SCROLL_TEXT)
                gkrellm_decal_scroll_text_set_text(panel, decal_text, info);

                gkrellm_decal_scroll_text_get_size(decal_text,&w_scroll,NULL);

                gkrellm_decal_get_size(decal_text, &w_decal, NULL);
                        
                x_scroll = (x_scroll + 1) % (2 * w);
                        
                gkrellm_decal_scroll_text_horizontal_loop(decal_text,scrolling);
                        
                gkrellm_decal_text_set_offset(decal_text, x_scroll, 0);
#else
                decal_text->x_off = 0;
                        
                gkrellm_draw_decal_text(panel, decal_text, info, 0);
#endif
        } else {
                gkrellm_draw_decal_text(panel, decal_text, info, 0);
        }
		
                
        gkrellm_draw_panel_layers(panel);
}


static void create_plugin(GtkWidget *vbox, gint first_create) 
{
        
	GkrellmStyle *style;
	GkrellmTextstyle *ts;

	if (first_create)
		panel = gkrellm_panel_new0();

	style = gkrellm_meter_style(style_id);

	ts = gkrellm_meter_textstyle(style_id);

        decal_text = gkrellm_create_decal_text(panel,"GOVERNOR",ts,style,-1,0,-1);


	gkrellm_panel_configure(panel, NULL, style);
	gkrellm_panel_create(vbox, monitor, panel);

	if (first_create)
		g_signal_connect(G_OBJECT(panel->drawing_area),
		                 "expose_event",
		                 G_CALLBACK (panel_expose_event),
		                 NULL);
}



static void config_plugin(GtkWidget *vbox) {
  gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("CPU Governor reader. Heavaly based to the gkfreq plugin\n\n"
                                                  "contact me: firew4lker@gmail.com\n"), TRUE, TRUE, 0);
}

 

static GkrellmMonitor plugin_mon = {
	"gkgov",                   /* Name, for config tab */
	0,                          /* Id, 0 if a plugin */
	create_plugin,              /* The create function */
	update_plugin,              /* The update function */
	config_plugin,              /* The config tab create function */
	NULL,                       /* Apply the config function */
        
	NULL,                       /* Save user config */
	NULL,                       /* Load user config */
	NULL,                       /* config keyword */

	NULL,                       /* Undefined 2 */
	NULL,                       /* Undefined 1 */
	NULL,                       /* private */

	MON_CPU,                    /* Insert plugin before this monitor */
	NULL,                       /* Handle if a plugin, filled in by GKrellM */
	NULL                        /* path if a plugin, filled in by GKrellM */
};


GkrellmMonitor* gkrellm_init_plugin()
{
	style_id = gkrellm_add_meter_style(&plugin_mon, "gkgov");
	monitor = &plugin_mon;

	return &plugin_mon;
}
