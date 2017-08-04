/*									tab:8
 *
 * photo.c - photo display functions
 *
 * "Copyright (c) 2011 by Steven S. Lumetta."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO 
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
 * EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE 
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE, 
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:	    Steve Lumetta
 * Version:	    3
 * Creation Date:   Fri Sep  9 21:44:10 2011
 * Filename:	    photo.c
 * History:
 *	SL	1	Fri Sep  9 21:44:10 2011
 *		First written (based on mazegame code).
 *	SL	2	Sun Sep 11 14:57:59 2011
 *		Completed initial implementation of functions.
 *	SL	3	Wed Sep 14 21:49:44 2011
 *		Cleaned up code for distribution.
 */


#include <string.h>

#include "assert.h"
#include "modex.h"
#include "photo.h"
#include "photo_headers.h"
#include "world.h"


/* types local to this file (declared in types.h) */

/* 
 * A room photo.  Note that you must write the code that selects the
 * optimized palette colors and fills in the pixel data using them as 
 * well as the code that sets up the VGA to make use of these colors.
 * Pixel data are stored as one-byte values starting from the upper
 * left and traversing the top row before returning to the left of
 * the second row, and so forth.  No padding should be used.
 */
struct photo_t {
    photo_header_t hdr;			/* defines height and width */
    uint8_t        palette[192][3];     /* optimized palette colors */
    uint8_t*       img;                 /* pixel data               */
};

/* 
 * An object image.  The code for managing these images has been given
 * to you.  The data are simply loaded from a file, where they have 
 * been stored as 2:2:2-bit RGB values (one byte each), including 
 * transparent pixels (value OBJ_CLR_TRANSP).  As with the room photos, 
 * pixel data are stored as one-byte values starting from the upper 
 * left and traversing the top row before returning to the left of the 
 * second row, and so forth.  No padding is used.
 */
struct image_t {
    photo_header_t hdr;			/* defines height and width */
    uint8_t*       img;                 /* pixel data               */
};



// wo da !
<<<<<<< .mine
struct octnode{
=======
typedef struct octnode{
>>>>>>> .r10680
	int index;
	uint16_t   SumRed;
	uint16_t   SumGreen;
	uint16_t   SumBlue;
	int SumPixel;
	int PaletteIdx;
} octnode;

<<<<<<< .mine
struct octree{
	octnode levelone[8];
	octnode leveltwo[64];
	octnode levelthree[512];
	octnode levelfour[4096];
};
=======
typedef struct octree{
	//octnode levelone[8];
	struct octnode leveltwo[64];
	//octnode levelthree[512];
	struct octnode levelfour[4096];
} octree;
>>>>>>> .r10680

/* file-scope variables */

/* 
 * The room currently shown on the screen.  This value is not known to 
 * the mode X code, but is needed when filling buffers in callbacks from 
 * that code (fill_horiz_buffer/fill_vert_buffer).  The value is set 
 * by calling prep_room.
 */
static const room_t* cur_room = NULL; 


/* 
 * fill_horiz_buffer
 *   DESCRIPTION: Given the (x,y) map pixel coordinate of the leftmost 
 *                pixel of a line to be drawn on the screen, this routine 
 *                produces an image of the line.  Each pixel on the line
 *                is represented as a single byte in the image.
 *
 *                Note that this routine draws both the room photo and
 *                the objects in the room.
 *
 *   INPUTS: (x,y) -- leftmost pixel of line to be drawn 
 *   OUTPUTS: buf -- buffer holding image data for the line
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void
fill_horiz_buffer (int x, int y, unsigned char buf[SCROLL_X_DIM])
{
    int            idx;   /* loop index over pixels in the line          */ 
    object_t*      obj;   /* loop index over objects in the current room */
    int            imgx;  /* loop index over pixels in object image      */ 
    int            yoff;  /* y offset into object image                  */ 
    uint8_t        pixel; /* pixel from object image                     */
    const photo_t* view;  /* room photo                                  */
    int32_t        obj_x; /* object x position                           */
    int32_t        obj_y; /* object y position                           */
    const image_t* img;   /* object image                                */

    /* Get pointer to current photo of current room. */
    view = room_photo (cur_room);

    /* Loop over pixels in line. */
    for (idx = 0; idx < SCROLL_X_DIM; idx++) {
        buf[idx] = (0 <= x + idx && view->hdr.width > x + idx ?
		    view->img[view->hdr.width * y + x + idx] : 0);
    }

    /* Loop over objects in the current room. */
    for (obj = room_contents_iterate (cur_room); NULL != obj;
    	 obj = obj_next (obj)) {
	obj_x = obj_get_x (obj);
	obj_y = obj_get_y (obj);
	img = obj_image (obj);

        /* Is object outside of the line we're drawing? */
	if (y < obj_y || y >= obj_y + img->hdr.height ||
	    x + SCROLL_X_DIM <= obj_x || x >= obj_x + img->hdr.width) {
	    continue;
	}

	/* The y offset of drawing is fixed. */
	yoff = (y - obj_y) * img->hdr.width;

	/* 
	 * The x offsets depend on whether the object starts to the left
	 * or to the right of the starting point for the line being drawn.
	 */
	if (x <= obj_x) {
	    idx = obj_x - x;
	    imgx = 0;
	} else {
	    idx = 0;
	    imgx = x - obj_x;
	}

	/* Copy the object's pixel data. */
	for (; SCROLL_X_DIM > idx && img->hdr.width > imgx; idx++, imgx++) {
	    pixel = img->img[yoff + imgx];

	    /* Don't copy transparent pixels. */
	    if (OBJ_CLR_TRANSP != pixel) {
		buf[idx] = pixel;
	    }
	}
    }
}


/* 
 * fill_vert_buffer
 *   DESCRIPTION: Given the (x,y) map pixel coordinate of the top pixel of 
 *                a vertical line to be drawn on the screen, this routine 
 *                produces an image of the line.  Each pixel on the line
 *                is represented as a single byte in the image.
 *
 *                Note that this routine draws both the room photo and
 *                the objects in the room.
 *
 *   INPUTS: (x,y) -- top pixel of line to be drawn 
 *   OUTPUTS: buf -- buffer holding image data for the line
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void
fill_vert_buffer (int x, int y, unsigned char buf[SCROLL_Y_DIM])
{
    int            idx;   /* loop index over pixels in the line          */ 
    object_t*      obj;   /* loop index over objects in the current room */
    int            imgy;  /* loop index over pixels in object image      */ 
    int            xoff;  /* x offset into object image                  */ 
    uint8_t        pixel; /* pixel from object image                     */
    const photo_t* view;  /* room photo                                  */
    int32_t        obj_x; /* object x position                           */
    int32_t        obj_y; /* object y position                           */
    const image_t* img;   /* object image                                */

    /* Get pointer to current photo of current room. */
    view = room_photo (cur_room);

    /* Loop over pixels in line. */
    for (idx = 0; idx < SCROLL_Y_DIM; idx++) {
        buf[idx] = (0 <= y + idx && view->hdr.height > y + idx ?
		    view->img[view->hdr.width * (y + idx) + x] : 0);
    }

    /* Loop over objects in the current room. */
    for (obj = room_contents_iterate (cur_room); NULL != obj;
    	 obj = obj_next (obj)) {
	obj_x = obj_get_x (obj);
	obj_y = obj_get_y (obj);
	img = obj_image (obj);

        /* Is object outside of the line we're drawing? */
	if (x < obj_x || x >= obj_x + img->hdr.width ||
	    y + SCROLL_Y_DIM <= obj_y || y >= obj_y + img->hdr.height) {
	    continue;
	}

	/* The x offset of drawing is fixed. */
	xoff = x - obj_x;

	/* 
	 * The y offsets depend on whether the object starts below or 
	 * above the starting point for the line being drawn.
	 */
	if (y <= obj_y) {
	    idx = obj_y - y;
	    imgy = 0;
	} else {
	    idx = 0;
	    imgy = y - obj_y;
	}

	/* Copy the object's pixel data. */
	for (; SCROLL_Y_DIM > idx && img->hdr.height > imgy; idx++, imgy++) {
	    pixel = img->img[xoff + img->hdr.width * imgy];

	    /* Don't copy transparent pixels. */
	    if (OBJ_CLR_TRANSP != pixel) {
		buf[idx] = pixel;
	    }
	}
    }
}


/* 
 * image_height
 *   DESCRIPTION: Get height of object image in pixels.
 *   INPUTS: im -- object image pointer
 *   OUTPUTS: none
 *   RETURN VALUE: height of object image im in pixels
 *   SIDE EFFECTS: none
 */
uint32_t 
image_height (const image_t* im)
{
    return im->hdr.height;
}


/* 
 * image_width
 *   DESCRIPTION: Get width of object image in pixels.
 *   INPUTS: im -- object image pointer
 *   OUTPUTS: none
 *   RETURN VALUE: width of object image im in pixels
 *   SIDE EFFECTS: none
 */
uint32_t 
image_width (const image_t* im)
{
    return im->hdr.width;
}

/* 
 * photo_height
 *   DESCRIPTION: Get height of room photo in pixels.
 *   INPUTS: p -- room photo pointer
 *   OUTPUTS: none
 *   RETURN VALUE: height of room photo p in pixels
 *   SIDE EFFECTS: none
 */
uint32_t 
photo_height (const photo_t* p)
{
    return p->hdr.height;
}


/* 
 * photo_width
 *   DESCRIPTION: Get width of room photo in pixels.
 *   INPUTS: p -- room photo pointer
 *   OUTPUTS: none
 *   RETURN VALUE: width of room photo p in pixels
 *   SIDE EFFECTS: none
 */
uint32_t 
photo_width (const photo_t* p)
{
    return p->hdr.width;
}


/* 
 * prep_room
 *   DESCRIPTION: Prepare a new room for display.  You might want to set
 *                up the VGA palette registers according to the color
 *                palette that you chose for this room.
 *   INPUTS: r -- pointer to the new room
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes recorded cur_room for this file
 */
void
prep_room (const room_t* r)
{
    /* Record the current room. */
    photo_t * p= room_photo(r);
    fill_mypalette(p->palette); // wo de
    cur_room = r;
}


/* 
 * read_obj_image
 *   DESCRIPTION: Read size and pixel data in 2:2:2 RGB format from a
 *                photo file and create an image structure from it.
 *   INPUTS: fname -- file name for input
 *   OUTPUTS: none
 *   RETURN VALUE: pointer to newly allocated photo on success, or NULL
 *                 on failure
 *   SIDE EFFECTS: dynamically allocates memory for the image
 */
image_t*
read_obj_image (const char* fname)
{
    FILE*    in;		/* input file               */
    image_t* img = NULL;	/* image structure          */
    uint16_t x;			/* index over image columns */
    uint16_t y;			/* index over image rows    */
    uint8_t  pixel;		/* one pixel from the file  */

    /* 
     * Open the file, allocate the structure, read the header, do some
     * sanity checks on it, and allocate space to hold the image pixels.
     * If anything fails, clean up as necessary and return NULL.
     */
    if (NULL == (in = fopen (fname, "r+b")) ||
	NULL == (img = malloc (sizeof (*img))) ||
	NULL != (img->img = NULL) || /* false clause for initialization */
	1 != fread (&img->hdr, sizeof (img->hdr), 1, in) ||
	MAX_OBJECT_WIDTH < img->hdr.width ||
	MAX_OBJECT_HEIGHT < img->hdr.height ||
	NULL == (img->img = malloc 
		 (img->hdr.width * img->hdr.height * sizeof (img->img[0])))) {
	if (NULL != img) {
	    if (NULL != img->img) {
	        free (img->img);
	    }
	    free (img);
	}
	if (NULL != in) {
	    (void)fclose (in);
	}
	return NULL;
    }

    /* 
     * Loop over rows from bottom to top.  Note that the file is stored
     * in this order, whereas in memory we store the data in the reverse
     * order (top to bottom).
     */
    for (y = img->hdr.height; y-- > 0; ) {

	/* Loop over columns from left to right. */
	for (x = 0; img->hdr.width > x; x++) {

	    /* 
	     * Try to read one 8-bit pixel.  On failure, clean up and 
	     * return NULL.
	     */
	    if (1 != fread (&pixel, sizeof (pixel), 1, in)) {
		free (img->img);
		free (img);
	        (void)fclose (in);
		return NULL;
	    }

	    /* Store the pixel in the image data. */
	    img->img[img->hdr.width * y + x] = pixel;
	}
    }

    /* All done.  Return success. */
    (void)fclose (in);
    return img;
}


/* 
 * read_photo
 *   DESCRIPTION: Read size and pixel data in 5:6:5 RGB format from a
 *                photo file and create a photo structure from it.
 *                Code provided simply maps to 2:2:2 RGB.  You must
 *                replace this code with palette color selection, and
 *                must map the image pixels into the palette colors that
 *                you have defined.
 *   INPUTS: fname -- file name for input
 *   OUTPUTS: none
 *   RETURN VALUE: pointer to newly allocated photo on success, or NULL
 *                 on failure
 *   SIDE EFFECTS: dynamically allocates memory for the photo
 */

static int compare (const void * a, const void * b) //wo de 
{
		int i= (*(octnode*)b).SumPixel - (*(octnode*)a).SumPixel ;
  	 return i;
}



photo_t*
read_photo (const char* fname)
{
    FILE*    in;	/* input file               */
    photo_t* p = NULL;	/* photo structure          */
    uint16_t x;		/* index over image columns */
    uint16_t y;		/* index over image rows    */
    uint16_t pixel;	/* one pixel from the file  */

    /* 
     * Open the file, allocate the structure, read the header, do some
     * sanity checks on it, and allocate space to hold the photo pixels.
     * If anything fails, clean up as necessary and return NULL.
     */
    if (NULL == (in = fopen (fname, "r+b")) ||
	NULL == (p = malloc (sizeof (*p))) ||
	NULL != (p->img = NULL) || /* false clause for initialization */
	1 != fread (&p->hdr, sizeof (p->hdr), 1, in) ||
	MAX_PHOTO_WIDTH < p->hdr.width ||
	MAX_PHOTO_HEIGHT < p->hdr.height ||
	NULL == (p->img = malloc 
		 (p->hdr.width * p->hdr.height * sizeof (p->img[0])))) {
	if (NULL != p) {
	    if (NULL != p->img) {
	        free (p->img);
	    }
	    free (p);
	}
	if (NULL != in) {
	    (void)fclose (in);
	}
	return NULL;
    }

    /* 
     * Loop over rows from bottom to top.  Note that the file is stored
     * in this order, whereas in memory we store the data in the reverse
     * order (top to bottom).

     */

<<<<<<< .mine
     octree tree;       //wo da 
     octnode* fourthptr=tree.levelfour;
     octnode* secondptr=tree.leveltwo;
=======
     struct octree tree;       //wo da 
     /*octnode* fourthptr=tree.levelfour;
     octnode* secondptr=tree.leveltwo;*/
>>>>>>> .r10680
		int i=0;
<<<<<<< .mine
		int j=0;
		for(j=0;j<64;j++)
		{
			tree.leveltwo[j].index=0;
			tree.leveltwo[j].SumRed=0;
			tree.leveltwo[j].SumGreen=0;
			tree.leveltwo[j].SumBlue=0;
			tree.leveltwo[j].SumPixel=0;
			tree.leveltwo[j].PaletteIdx=0;

		}
=======
		for(i=0;i<64;i++)
		{	
			tree.leveltwo[i].index=0;
			tree.leveltwo[i].SumRed=0;
			tree.leveltwo[i].SumGreen=0;
			tree.leveltwo[i].SumBlue=0;
			tree.leveltwo[i].SumPixel=0;
			tree.leveltwo[i].PaletteIdx=0;
		}
>>>>>>> .r10680
		for(i=0;i<4096;i++)
		{
			tree.levelfour[i].index=0;
			tree.levelfour[i].SumRed=0;
			tree.levelfour[i].SumGreen=0;
			tree.levelfour[i].SumBlue=0;
			tree.levelfour[i].SumPixel=0;
			tree.levelfour[i].PaletteIdx=0;
		}


    for (y = p->hdr.height; y-- > 0; ) {

	/* Loop over columns from left to right. */
	for (x = 0; p->hdr.width > x; x++) {

	    /* 
	     * Try to read one 16-bit pixel.  On failure, clean up and 
	     * return NULL.
	     */
	    if (1 != fread (&pixel, sizeof (pixel), 1, in)) {
		free (p->img);
		free (p);
	        (void)fclose (in);
		return NULL;

	    }
	    /* 
	     * 16-bit pixel is coded as 5:6:5 RGB (5 bits red, 6 bits green,
	     * and 6 bits blue).  We change to 2:2:2, which we've set for the
	     * game objects.  You need to use the other 192 palette colors
	     * to specialize the appearance of each photo.
	     *
	     * In this code, you need to calculate the p->palette values,
	     * which encode 6-bit RGB as arrays of three uint8_t's.  When
	     * the game puts up a photo, you should then change the palette 
	     * to match the colors needed for that photo.
	     */
<<<<<<< .mine
	    /*p->img[p->hdr.width * y + x] = (((pixel >> 14) << 4) |
=======
	   /* p->img[p->hdr.width * y + x] = (((pixel >> 14) << 4) |
>>>>>>> .r10680
					    (((pixel >> 9) & 0x3) << 2) |
					    ((pixel >> 3) & 0x3));*/





	    //start!!!
		

<<<<<<< .mine
		int fourbitred= pixel>>12;
		int fourbitgreen=   (pixel<<5)>>7;
		int fourbitblue=  (pixel<<11)>>1;
		int position= fourbitred<<8+fourbitgreen<<4+fourbitblue;
=======
		int fourbitred= (pixel & 0xF000) >> 12;
		int fourbitgreen= (pixel & 0x0780)>>7;
		int fourbitblue=  (pixel & 0x001E)>>1;
		int position= (fourbitred<<8)+ (fourbitgreen<<4) +fourbitblue;
>>>>>>> .r10680

		int sixbitred= (pixel & 0xF800) >>10;
		int sixbitgreen=   (pixel & 0x07E0)>>5;
		int sixbitblue=  (pixel & 0x001F)<<1;

		tree.levelfour[position].SumPixel++; 
		tree.levelfour[position].SumRed=tree.levelfour[position].SumRed+sixbitred;
		tree.levelfour[position].SumGreen=tree.levelfour[position].SumGreen+sixbitgreen;
		tree.levelfour[position].SumBlue=tree.levelfour[position].SumBlue+sixbitblue;

		tree.levelfour[position].index = position;
		
		

	
		int twobitred= (pixel & 0xC000 )>>14;
		int twobitgreen= (pixel & 0x0600)>>5;
		int twobitblue= (pixel  & 0x0018)>>3;
		int lvtwo = (twobitred<<4) + (twobitgreen<<2) + twobitblue;

		tree.leveltwo[lvtwo].SumPixel++; 
<<<<<<< .mine
		tree.leveltwo[lvtwo].SumRed=tree.leveltwo[lvtwo].SumRed+twobitred;
		tree.leveltwo[lvtwo].SumGreen=tree.leveltwo[lvtwo].SumGreen+twobitgreen;
		tree.leveltwo[lvtwo].SumBlue=tree.leveltwo[lvtwo].SumBlue+twobitblue;
		tree.leveltwo[lvtwo].index=lvtwo;
=======
		tree.leveltwo[lvtwo].SumRed=tree.leveltwo[lvtwo].SumRed+sixbitred;
		tree.leveltwo[lvtwo].SumGreen=tree.leveltwo[lvtwo].SumGreen+sixbitgreen;
		tree.leveltwo[lvtwo].SumBlue=tree.leveltwo[lvtwo].SumBlue+sixbitblue;
		tree.leveltwo[lvtwo].index= lvtwo;
		
>>>>>>> .r10680







		}
    }
   

<<<<<<< .mine
    qsort(fourthptr,4096,sizeof(octnode),compare);
    qsort(secondptr,64,sizeof(octnode),compare);
=======
    qsort(tree.levelfour,4096,sizeof(struct octnode),compare);
    //qsort(secondptr,64,sizeof(octnode),compare);
>>>>>>> .r10680

    int avgred=0;
    int avggreen=0;
    int avgblue=0;
    for(i=0;i<128;i++)
    {
     	avgred=0;
     	avggreen=0;
     	avgblue=0;
    	if((tree.levelfour[i].SumPixel) != 0)
    	{
    	avgred= (tree.levelfour[i].SumRed)/(tree.levelfour[i].SumPixel);
    	avggreen=(tree.levelfour[i].SumGreen)/(tree.levelfour[i].SumPixel);
    	avgblue = (tree.levelfour[i].SumBlue)/(tree.levelfour[i].SumPixel);
    	}


    	int position = tree.levelfour[i].index;
    	int index   = position/64;
    	tree.leveltwo[index].SumRed = tree.leveltwo[index].SumRed - tree.levelfour[i].SumRed;
    	tree.leveltwo[index].SumGreen =  tree.leveltwo[index].SumGreen - tree.levelfour[i].SumGreen;
    	tree.leveltwo[index].SumBlue =  tree.leveltwo[index].SumBlue - tree.levelfour[i].SumBlue;

    	tree.levelfour[i].PaletteIdx=i;
    	p->palette[i][0]=avgred;
    	p->palette[i][1]=avggreen;
    	p->palette[i][2]=avgblue;
    }




   
    int j=0;
    for(i=128;i<192;i++)
    {
<<<<<<< .mine
    	avgred= (tree.leveltwo[j].SumRed)/(tree.leveltwo[j].SumPixel);
    	avggreen=(tree.leveltwo[j].SumGreen)/(tree.leveltwo[j].SumPixel);
    	avgblue = (tree.leveltwo[j].SumBlue)/(tree.leveltwo[j].SumPixel);
    	secondpalette[i][0]=avgred;
    	secondpalette[i][1]=avggreen;
    	secondpalette[i][2]=avgblue;
    	j++
    }
=======
    	avgred=0;
     	avggreen=0;
     	avgblue=0;
     	if((tree.leveltwo[j].SumPixel) != 0)
    	{
    		avgred= (tree.leveltwo[j].SumRed)/(tree.leveltwo[j].SumPixel);
    		avggreen=(tree.leveltwo[j].SumGreen)/(tree.leveltwo[j].SumPixel);
    		avgblue = (tree.leveltwo[j].SumBlue)/(tree.leveltwo[j].SumPixel);		
    	}
    	tree.leveltwo[j].PaletteIdx=i;
    	p->palette[i][0]=avgred;
    	p->palette[i][1]=avggreen;
    	p->palette[i][2]=avgblue;
    	j++;
    }
>>>>>>> .r10680

    fseek(in,0L,SEEK_SET);

    for (y = p->hdr.height; y-- > 0; ) 
    {

	/* Loop over columns from left to right. */
	for (x = 0; p->hdr.width > x; x++) 
		{

	    /* 
	     * Try to read one 16-bit pixel.  On failure, clean up and 
	     * return NULL.
	     */
	    	if (1 != fread (&pixel, sizeof (pixel), 1, in)) 
	    	{
			free (p->img);
			free (p);
	        (void)fclose (in);
			return NULL;
	    	}
	    	
	    	int    jugde=0;
			
			int nfourbitred= (pixel & 0xF000) >> 12;
			int nfourbitgreen= (pixel & 0x0780)>>7;
			int nfourbitblue=  (pixel & 0x001E)>>1;
			int indextfour= (nfourbitred<<8)+ (nfourbitgreen<<4) + nfourbitblue;

			int ntwobitred= (pixel & 0xC000 )>>14;
			int ntwobitgreen= (pixel & 0x0600)>>5;
			int ntwobitblue= (pixel  & 0x0018)>>3;
			int indextwo = (ntwobitred<<4) + (ntwobitgreen<<2) + ntwobitblue;
			int i=0;
			for(i=0;i<128;i++)
			{
				if(tree.levelfour[i].index == indextfour)
				{
					jugde=1;
					break;
				}
			}
			if(jugde)
			{
				p->img[p->hdr.width * y + x] = 64 + tree.levelfour[i].PaletteIdx;
			}
			else
			{

				p->img[p->hdr.width * y + x] = 64 + tree.leveltwo[indextwo].PaletteIdx;

			}
		







		}
	}



    /* All done.  Return success. */
    (void)fclose (in);
    return p;
}


