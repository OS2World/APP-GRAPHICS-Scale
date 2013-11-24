/* SCALE.C  v1.0
   scale TGA and interpolate values
   code last modified by U. A. Mueller, 27 Dec. 97.*/
#include <stdio.h>
#include <stdlib.h>
#include <mem.h>
#include <string.h>

#define prog_version "1.0"
#define prog_date    "27. Dec 1997"
#define size_fact    3*sizeof(UI)

typedef struct Image_Line_Struct IMAGE_LINE;
typedef float UI;

struct Image_Line_Struct
{
  UI red, green, blue;
};

int main (int argc, char *argv[])
{
   FILE          *inf, *outf;
   unsigned char InText[19];
   unsigned int  width_1, width_2, height_1, height_2, old_width, old_height, new_width, new_height,
                 x, y, i, j, new_x, factor;
   UI            diff_blue, diff_green, diff_red, tmp_factor_col, tmp_blue, tmp_green, tmp_red;
   IMAGE_LINE    *act_line, *sec_line, *out_line, *sec_out_line;

   fprintf (stderr, "Scale TGA files v%s by U. A. Mueller %s\n", prog_version, prog_date);

   if (argc < 4)
   {
      fprintf (stderr, "Usage: %s <infile> <outfile> <scale factor>\n", argv[0]);
      exit (1);
   }

   if (strcmp (argv[1], argv[2]) == 0)
   {
      fprintf (stderr, "Input and output file must be different\n");
      exit (1);
   }

   if ((inf = fopen (argv[1], "rb")) == NULL)
   {
      perror (argv[1]);
      exit (1);
   }

   if ((outf = fopen(argv[2], "wb")) == NULL)
   {
     fprintf (stderr, "can't open output file %s\n", argv[2]);
     exit (1);
   }

   factor = atoi (argv[3]);

   if (factor < 2)
   {
     fprintf (stderr, "factor must be at least 2\n");
     exit (1);
   }

   /* Read header and calculate new size */
   fscanf  (inf, "%18c", InText);
   width_1  = InText[12];
   width_2  = InText[13];
   height_1 = InText[14];
   height_2 = InText[15];
   old_width  = width_1  + (width_2  << 8);
   old_height = height_1 + (height_2 << 8);
   fprintf (stderr, "%s:\t%d x %d\n", argv[1], old_height, old_width);
   fprintf (stderr, "Factor:\t%d\n", factor);
   new_width  = old_width * factor;
   new_height = old_height * factor;
   fprintf (stderr, "%s:\t%d x %d\n", argv[2], new_height, new_width);

   /* Write TGA image header: y origin set to "First_Line", 24 bits/pixel (16 million colors) */
   /* Bitmask, pertinent bit: top-down raster */
   fprintf (outf, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            (short)(new_width % 256), (short)(new_width / 256),
            (short)(new_height % 256), (short)(new_height / 256), 24, 32);
   act_line     = (IMAGE_LINE *)malloc ((size_t)(size_fact * old_width));
   sec_line     = (IMAGE_LINE *)malloc ((size_t)(size_fact * old_width));
   out_line     = (IMAGE_LINE *)malloc ((size_t)(size_fact * new_width));
   sec_out_line = (IMAGE_LINE *)malloc ((size_t)(size_fact * new_width));

   /* Read first line */
   for (x = 0; x < old_width; x++)
   {
     sec_line[x].blue  = getc(inf);
     sec_line[x].green = getc(inf);
     sec_line[x].red   = getc(inf);
   }

   /* Read and process lines */
   for (y = 0; y < old_height; y++)
   {
     for (x = 0, new_x = 0; x < old_width; x++)
     {
       /* write next point */
       out_line[new_x].blue  = sec_line[x].blue;
       out_line[new_x].green = sec_line[x].green;
       out_line[new_x].red   = sec_line[x].red;
       fprintf (outf, "%c%c%c", (char)out_line[new_x].blue, (char)out_line[new_x].green, (char)out_line[new_x].red);
       new_x++;

       if (x == (old_width - 1))
       { /* last point in row */
         diff_blue  = tmp_blue  = (sec_line[x].blue  - out_line[new_x - 2].blue)  / factor;
         diff_green = tmp_green = (sec_line[x].green - out_line[new_x - 2].green) / factor;
         diff_red   = tmp_red   = (sec_line[x].red   - out_line[new_x - 2].red)   / factor;
       }
       else
       {
         diff_blue  = tmp_blue  = (sec_line[x + 1].blue  - sec_line[x].blue)  / factor;
         diff_green = tmp_green = (sec_line[x + 1].green - sec_line[x].green) / factor;
         diff_red   = tmp_red   = (sec_line[x + 1].red   - sec_line[x].red)   / factor;
       }

       for (i = 1; i < factor; i++)
       {
         out_line[new_x].blue  = sec_line[x].blue  + tmp_blue;
         out_line[new_x].green = sec_line[x].green + tmp_green;
         out_line[new_x].red   = sec_line[x].red   + tmp_red;
         fprintf (outf, "%c%c%c", (char)out_line[new_x].blue, (char)out_line[new_x].green, (char)out_line[new_x].red);
         new_x++;
         tmp_blue  += diff_blue;
         tmp_green += diff_green;
         tmp_red   += diff_red;
       }

       /* read next point */
       if (y < (old_height - 1))
       {
         act_line[x].blue  = getc(inf);
         act_line[x].green = getc(inf);
         act_line[x].red   = getc(inf);
       }
     }

     memcpy (sec_out_line, out_line, new_width * size_fact);

     for (i = 1; i < factor; i++)
     {
       tmp_factor_col = (UI)i / (UI)factor;

       for (x = 0, new_x = 0; x < old_width; x++)
       {
         out_line[new_x].blue  = sec_line[x].blue  + (act_line[x].blue  - sec_line[x].blue)  * tmp_factor_col;
         out_line[new_x].green = sec_line[x].green + (act_line[x].green - sec_line[x].green) * tmp_factor_col;
         out_line[new_x].red   = sec_line[x].red   + (act_line[x].red   - sec_line[x].red)   * tmp_factor_col;
         fprintf (outf, "%c%c%c", (char)out_line[new_x].blue, (char)out_line[new_x].green, (char)out_line[new_x].red);
         new_x++;

         if (x == (old_width - 1))
         { /* last point in row */
           tmp_blue  = (act_line[x].blue  - out_line[new_x - 2].blue)  / factor;
           tmp_green = (act_line[x].green - out_line[new_x - 2].green) / factor;
           tmp_red   = (act_line[x].red   - out_line[new_x - 2].red)   / factor;
         }
         else
         {
           tmp_blue  = (act_line[x + 1].blue  - act_line[x].blue)  / factor;
           tmp_green = (act_line[x + 1].green - act_line[x].green) / factor;
           tmp_red   = (act_line[x + 1].red   - act_line[x].red)   / factor;
         }

         diff_blue  = act_line[x].blue  + tmp_blue;
         diff_green = act_line[x].green + tmp_green;
         diff_red   = act_line[x].red   + tmp_red;

         for (j = 1; j < factor; j++)
         {
           out_line[new_x].blue  = sec_out_line[new_x].blue  + (diff_blue  - sec_out_line[new_x].blue)  * tmp_factor_col;
           out_line[new_x].green = sec_out_line[new_x].green + (diff_green - sec_out_line[new_x].green) * tmp_factor_col;
           out_line[new_x].red   = sec_out_line[new_x].red   + (diff_red   - sec_out_line[new_x].red)   * tmp_factor_col;
           fprintf (outf, "%c%c%c", (char)out_line[new_x].blue, (char)out_line[new_x].green, (char)out_line[new_x].red);
           diff_blue  += tmp_blue;
           diff_green += tmp_green;
           diff_red   += tmp_red;
           new_x++;
         }
       }
     }

     memcpy (sec_line, act_line, old_width * size_fact);
   }

   fclose (inf);
   fclose (outf);
   free (inf);
   free (outf);
   return (0);
}
