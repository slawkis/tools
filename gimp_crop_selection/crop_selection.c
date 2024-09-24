/*

*/

#include <libgimp/gimp.h>
#define PLUG_IN_PROC "plug-in-remove-selection"

/* Declare local functions. */

static void            query        (void);

static void            run          (const gchar      *name,
                                     gint              nparams,
                                     const GimpParam  *param,
                                     gint             *nreturn_vals,
                                     GimpParam       **return_vals);

static void            do_selcrop    (gint32    drawable_id,
                                     gint32    image_id);

const GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,  /* init_proc  */
  NULL,  /* quit_proc  */
  query, /* query_proc */
  run,   /* run_proc   */
};


MAIN ()

static void
query (void)
{
  static const GimpParamDef args[] =
  {
    { GIMP_PDB_INT32,    "run-mode", "The run mode { RUN-INTERACTIVE (0), RUN-NONINTERACTIVE (1) }" },
    { GIMP_PDB_IMAGE,    "image",    "Input image"    },
    { GIMP_PDB_DRAWABLE, "drawable", "Input drawable" }
  };

  gimp_install_procedure (PLUG_IN_PROC,
                          "Crop out selection from image",
                          "",
                          "Steev",
                          "Steev",
                          "2024",
                          "Remove selection",
                          "RGB*, GRAY*, INDEXED*",
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (args), 0,
                          args, NULL);

  gimp_plugin_menu_register (PLUG_IN_PROC, "<Image>/Layer");
}

static void
run (const gchar      *name,
     gint              n_params,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
  static GimpParam   values[1];
  GimpRunMode        run_mode;
  GimpPDBStatusType  status = GIMP_PDB_SUCCESS;
  gint32             drawable_id;
  gint32             image_id;

  gegl_init (NULL, NULL);

  *nreturn_vals = 1;
  *return_vals  = values;

  run_mode = param[0].data.d_int32;

  if (run_mode == GIMP_RUN_NONINTERACTIVE)
    {
	g_message ("noninteractive");
        status = GIMP_PDB_CALLING_ERROR;
    }

  if (status == GIMP_PDB_SUCCESS)
    {
      /*  Get the specified drawable  */
      image_id    = param[1].data.d_int32;
      drawable_id = param[2].data.d_int32;

      /*  Make sure that the drawable is gray or RGB or indexed  */
      if (gimp_drawable_is_rgb (drawable_id) ||
          gimp_drawable_is_gray (drawable_id) ||
          gimp_drawable_is_indexed (drawable_id))
        {
          gimp_progress_init ("Crop out selection");

          do_selcrop (drawable_id, image_id);

        }
      else
        {
          status = GIMP_PDB_EXECUTION_ERROR;
        }
    }

  values[0].type          = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  gegl_exit ();
}


static void
do_selcrop (gint32  drawable_id,
          gint32  image_id)
{

    gboolean       non_empty;
    gint           x1, y1, x2, y2;
    gint	   iWidth, iHeight;
    gint	   sWidth, sHeight;
    
    if (!gimp_selection_bounds(image_id, &non_empty,&x1,&y1,&x2,&y2) ) {
	gimp_message ("Cannot read selection bounds");
	return;
    }
    if (!non_empty) {
	gimp_message ("Nothing to crop.");
	return;
    }

    sWidth  = x2 - x1;
    sHeight = y2 - y1;
    iWidth  = gimp_image_width(image_id);
    iHeight = gimp_image_height(image_id);

    if (sWidth == sHeight) {
	gimp_message("FATAL: selection cannot be a square!");
	return;
    }

 gint selY, selH, selX, selW, selYp, selXp, selHp, selWp, cropW, cropH;
    
 if (sWidth>sHeight){
    selY=y2;
    selH=iHeight-y2;
    selX=0;
    selW=iWidth;
    selYp=y1;
    selHp=iHeight-y2;
    selXp=0;
    selWp=iWidth;
    cropW=iWidth;
    cropH=iHeight-y2+y1;
  } else {
    selX=x2;
    selW=iWidth-x2;
    selY=0;
    selH=iHeight;
    selXp=x1;
    selWp=iWidth-x2;
    selYp=0;
    selHp=iHeight;
    cropW=iWidth-x2+x1;
    cropH=iHeight;
 }
 
  gimp_image_undo_group_start(image_id);

  gimp_selection_none(image_id);
  gimp_image_select_rectangle(image_id,GIMP_CHANNEL_OP_REPLACE,selX,selY,selW,selH);
  
  gimp_edit_copy(drawable_id);

  gimp_selection_none(image_id);
  gimp_image_select_rectangle(image_id,GIMP_CHANNEL_OP_REPLACE,selXp,selYp,selWp,selHp);
  
  gimp_edit_paste(drawable_id,1);
  gimp_floating_sel_anchor(gimp_image_get_floating_sel(image_id));
  
  gimp_selection_none(image_id);
  gimp_image_crop(image_id,cropW,cropH,0,0);
  
  gimp_image_undo_group_end(image_id);
  
  gimp_displays_flush();
  
}
