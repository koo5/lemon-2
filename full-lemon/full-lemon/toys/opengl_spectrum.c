/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2000  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
 *  Wed May 24 10:49:37 CDT 2000
 *  Fixes to threading/context creation for the nVidia X4 drivers by 
 *  Christian Zander <phoenix@minion.de>
 */


#include <math.h>



#define NUM_BANDS 16

static GLfloat heights[16][16], scale;


static void draw_rectangle(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2)
{
	if(y1 == y2)
	{
	
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y1, z1);
		glVertex3f(x2, y2, z2);
		
		glVertex3f(x2, y2, z2);
		glVertex3f(x1, y2, z2);
		glVertex3f(x1, y1, z1);
	}
	else
	{
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y1, z2);
		glVertex3f(x2, y2, z2);
		
		glVertex3f(x2, y2, z2);
		glVertex3f(x1, y2, z1);
		glVertex3f(x1, y1, z1);
	}
}

static void draw_bar(GLfloat x_offset, GLfloat z_offset, GLfloat height, GLfloat red, GLfloat green, GLfloat blue )
{
	GLfloat width = 0.1;

	glColor3f(red,green,blue);
	draw_rectangle(x_offset, height, z_offset, x_offset + width, height, z_offset + 0.1);
	draw_rectangle(x_offset, 0, z_offset, x_offset + width, 0, z_offset + 0.1);
	
	glColor3f(0.5 * red, 0.5 * green, 0.5 * blue);
	draw_rectangle(x_offset, 0.0, z_offset + 0.1, x_offset + width, height, z_offset + 0.1);
	draw_rectangle(x_offset, 0.0, z_offset, x_offset + width, height, z_offset );

	glColor3f(0.25 * red, 0.25 * green, 0.25 * blue);
	draw_rectangle(x_offset, 0.0, z_offset , x_offset, height, z_offset + 0.1);	
	draw_rectangle(x_offset + width, 0.0, z_offset , x_offset + width, height, z_offset + 0.1);

	
}

static void draw_bars(void)
{
	gint x,y;
	GLfloat x_offset, z_offset, r_base, b_base;

	glBegin(GL_TRIANGLES);
	for(y = 0; y < 16; y++)
	{
		z_offset = -1.6 + ((15 - y) * 0.2);

		b_base = y * (1.0 / 15);
		r_base = 1.0 - b_base;
			
		for(x = 0; x < 16; x++)
		{
			x_offset = -1.6 + (x * 0.2);			

			draw_bar(x_offset, z_offset, heights[y][x], r_base - (x * (r_base / 15.0)), x * (1.0 / 15), b_base);
		}
	}
	glEnd();
}

static void oglspectrum_gen_heights(gint16 data[2][256])
{
	gint i,c;
	gint y;
	GLfloat val;
	static GLfloat scale=0;
	if(!scale)
	    scale = 1.0 / log(256.0);
	gint xscale[] = {0, 1, 2, 3, 5, 7, 10, 14, 20, 28, 40, 54, 74, 101, 137, 187, 255};

	for(y = 15; y > 0; y--)
	{
		for(i = 0; i < 16; i++)
		{
			heights[y][i] = heights[y - 1][i];
		}
	}
	
	for(i = 0; i < NUM_BANDS; i++)
	{
		for(c = xscale[i], y = 0; c < xscale[i + 1]; c++)
		{
			if(data[0][c] > y)
				y = data[0][c];
		}
		y >>= 7;
		if(y > 0)
			val = (log(y) * scale);
		else
			val = 0;

				
		heights[0][i] = val;
	}
}

