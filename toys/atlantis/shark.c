/* atlantis --- Shows moving 3D sea animals */

#if 0
static const char sccsid[] = "@(#)shark.c	1.2 98/06/16 xlockmore";
#endif

/* Copyright (c) E. Lassauge, 1998. */

/*
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 * The original code for this mode was written by Mark J. Kilgard
 * as a demo for openGL programming.
 * 
 * Porting it to xlock  was possible by comparing the original Mesa's morph3d 
 * demo with it's ported version to xlock, so thanks for Marcelo F. Vianna 
 * (look at morph3d.c) for his indirect help.
 *
 * Thanks goes also to Brian Paul for making it possible and inexpensive
 * to use OpenGL at home.
 *
 * My e-mail address is lassauge@users.sourceforge.net
 *
 * Eric Lassauge  (May-13-1998)
 *
 */

/**
 * (c) Copyright 1993, 1994, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.
 */

class shark:public fish
{
 const float N002[3] = {0.000077, -0.020611, 0.999788};
 const float N003[3] = {0.961425, 0.258729, -0.09339};
 const float N004[3] = {0.510811, -0.769633, -0.383063};
 const float N005[3] = {0.400123, 0.855734, -0.328055};
 const float N006[3] = {-0.770715, 0.610204, -0.18344};
 const float N007[3] = {-0.915597, -0.373345, -0.149316};
 const float N008[3] = {-0.972788, 0.208921, -0.100179};
 const float N009[3] = {-0.939713, -0.312268, -0.139383};
 const float N010[3] = {-0.624138, -0.741047, -0.247589};
 const float N011[3] = {0.591434, -0.768401, -0.244471};
 const float N012[3] = {0.935152, -0.328495, -0.132598};
 const float N013[3] = {0.997102, 0.074243, -0.016593};
 const float N014[3] = {0.969995, 0.241712, -0.026186};
 const float N015[3] = {0.844539, 0.502628, -0.184714};
 const float N016[3] = {-0.906608, 0.386308, -0.169787};
 const float N017[3] = {-0.970016, 0.241698, -0.025516};
 const float N018[3] = {-0.998652, 0.050493, -0.012045};
 const float N019[3] = {-0.942685, -0.333051, -0.020556};
 const float N020[3] = {-0.660944, -0.750276, 0.01548};
 const float N021[3] = {0.503549, -0.862908, -0.042749};
 const float N022[3] = {0.953202, -0.302092, -0.012089};
 const float N023[3] = {0.998738, 0.023574, 0.044344};
 const float N024[3] = {0.979297, 0.193272, 0.060202};
 const float N025[3] = {0.7983, 0.464885, 0.382883};
 const float N026[3] = {-0.75659, 0.452403, 0.472126};
 const float N027[3] = {-0.953855, 0.293003, 0.065651};
 const float N028[3] = {-0.998033, 0.040292, 0.048028};
 const float N029[3] = {-0.977079, -0.204288, 0.059858};
 const float N030[3] = {-0.729117, -0.675304, 0.11114};
 const float N031[3] = {0.598361, -0.792753, 0.116221};
 const float N032[3] = {0.965192, -0.252991, 0.066332};
 const float N033[3] = {0.998201, -0.00279, 0.059892};
 const float N034[3] = {0.978657, 0.193135, 0.070207};
 const float N035[3] = {0.718815, 0.680392, 0.142733};
 const float N036[3] = {-0.383096, 0.906212, 0.178936};
 const float N037[3] = {-0.952831, 0.29259, 0.080647};
 const float N038[3] = {-0.99768, 0.032417, 0.059861};
 const float N039[3] = {-0.982629, -0.169881, 0.0747};
 const float N040[3] = {-0.695424, -0.703466, 0.1467};
 const float N041[3] = {0.359323, -0.915531, 0.180805};
 const float N042[3] = {0.943356, -0.319387, 0.089842};
 const float N043[3] = {0.998272, -0.032435, 0.048993};
 const float N044[3] = {0.978997, 0.193205, 0.065084};
 const float N045[3] = {0.872144, 0.470094, -0.135565};
 const float N046[3] = {-0.664282, 0.737945, -0.119027};
 const float N047[3] = {-0.954508, 0.28857, 0.075107};
 const float N048[3] = {-0.998273, 0.032406, 0.048993};
 const float N049[3] = {-0.979908, -0.193579, 0.048038};
 const float N050[3] = {-0.858736, -0.507202, -0.072938};
 const float N051[3] = {0.643545, -0.763887, -0.048237};
 const float N052[3] = {0.95558, -0.288954, 0.058068};
 const float N058[3] = {0.00005, 0.793007, -0.609213};
 const float N059[3] = {0.91351, 0.235418, -0.331779};
 const float N060[3] = {-0.80797, 0.495, -0.319625};
 const float N061[3] = {0, 0.784687, -0.619892};
 const float N062[3] = {0, -1, 0};
 const float N063[3] = {0, 1, 0};
 const float N064[3] = {0, 1, 0};
 const float N065[3] = {0, 1, 0};
 const float N066[3] = {-0.055784, 0.257059, 0.964784};
 const float N069[3] = {-0.000505, -0.929775, -0.368127};
 const float N070[3] = {0, 1, 0};
 const float P003[3] = {90, 114.73, 724.38};
 const float P005[3] = {27.81, 231.52, 510.43};
 const float P006[3] = {-27.81, 230.43, 509.76};
 const float P008[3] = {-90, 103.84, 718.53};
 const float P009[3] = {-131.1, -165.92, 834.85};
 const float P012[3] = {147.96, -170.89, 845.5};
 const float P013[3] = {180, 0, 2000};
 const float P014[3] = {145.62, 352.67, 2000};
 const float P015[3] = {55.62, 570.63, 2000};
 const float P016[3] = {-55.62, 570.64, 2000};
 const float P017[3] = {-145.62, 352.68, 2000};
 const float P018[3] = {-180, 0.01, 2000};
 const float P019[3] = {-178.2, -352.66, 2001.61};
 const float P020[3] = {-55.63, -570.63, 2000};
 const float P021[3] = {55.62, -570.64, 2000};
 const float P022[3] = {179.91, -352.69, 1998.39};
 const float P058[3] = {0, 1212.72, 2703.08};
 const float P059[3] = {50.36, 0, 108.14};
 const float P060[3] = {-22.18, 0, 108.14};
 const float P062[3] = {516.45, -887.08, 2535.45};
 const float P063[3] = {-545.69, -879.31, 2555.63};
 const float P064[3] = {618.89, -1005.64, 2988.32};
 const float P065[3] = {-635.37, -1014.79, 2938.68};
 const float P066[3] = {0, 1374.43, 3064.18};
 const float P071[3] = {-139.12, -124.96, 997.98};
 const float P072[3] = {-139.24, -110.18, 1020.68};
 const float P073[3] = {-137.33, -94.52, 1022.63};
 const float P074[3] = {-137.03, -79.91, 996.89};
 const float P075[3] = {-135.21, -91.48, 969.14};
 const float P076[3] = {-135.39, -110.87, 968.76};
 const float P077[3] = {150.23, -78.44, 995.53};
 const float P078[3] = {152.79, -92.76, 1018.46};
 const float P079[3] = {154.19, -110.2, 1020.55};
 const float P080[3] = {151.33, -124.15, 993.77};
 const float P081[3] = {150.49, -111.19, 969.86};
 const float P082[3] = {150.79, -92.41, 969.7};
 const float iP002[3] = {0, -36.59, 5687.72};
 const float iP004[3] = {58.24, -146.84, 262.35};
 const float iP007[3] = {-46.09, -146.83, 265.84};
 const float iP010[3] = {-27.81, -285.31, 500};
 const float iP011[3] = {27.81, -285.32, 500};
 const float iP023[3] = {150, 0, 3000};
 const float iP024[3] = {121.35, 293.89, 3000};
 const float iP025[3] = {46.35, 502.93, 2883.09};
 const float iP026[3] = {-46.35, 497.45, 2877.24};
 const float iP027[3] = {-121.35, 293.9, 3000};
 const float iP028[3] = {-150, 0, 3000};
 const float iP029[3] = {-121.35, -304.84, 2853.86};
 const float iP030[3] = {-46.36, -475.52, 3000};
 const float iP031[3] = {46.35, -475.53, 3000};
 const float iP032[3] = {121.35, -304.87, 2853.86};
 const float iP033[3] = {90, 0, 4000};
 const float iP034[3] = {72.81, 176.33, 4000};
 const float iP035[3] = {27.81, 285.32, 4000};
 const float iP036[3] = {-27.81, 285.32, 4000};
 const float iP037[3] = {-72.81, 176.34, 4000};
 const float iP038[3] = {-90, 0, 4000};
 const float iP039[3] = {-72.81, -176.33, 4000};
 const float iP040[3] = {-27.81, -285.31, 4000};
 const float iP041[3] = {27.81, -285.32, 4000};
 const float iP042[3] = {72.81, -176.34, 4000};
 const float iP043[3] = {30, 0, 5000};
 const float iP044[3] = {24.27, 58.78, 5000};
 const float iP045[3] = {9.27, 95.11, 5000};
 const float iP046[3] = {-9.27, 95.11, 5000};
 const float iP047[3] = {-24.27, 58.78, 5000};
 const float iP048[3] = {-30, 0, 5000};
 const float iP049[3] = {-24.27, -58.78, 5000};
 const float iP050[3] = {-9.27, -95.1, 5000};
 const float iP051[3] = {9.27, -95.11, 5000};
 const float iP052[3] = {24.27, -58.78, 5000};
 const float iP061[3] = {0, 1181.61, 6344.65};
 const float iP069[3] = {0, -418.25, 5765.04};
 const float iP070[3] = {0, 1266.91, 6629.6};




       float P002[3];
       float P004[3];
       float P007[3];
       float P010[3];
       float P011[3];
       float P023[3];
       float P024[3];
       float P025[3];
       float P026[3];
       float P027[3];
       float P028[3];
       float P029[3];
       float P030[3];
       float P031[3];
       float P032[3];
       float P033[3];
       float P034[3];
       float P035[3];
       float P036[3];
       float P037[3];
       float P038[3];
       float P039[3];
       float P040[3];
       float P041[3];
       float P042[3];
       float P043[3];
       float P044[3];
       float P045[3];
       float P046[3];
       float P047[3];
       float P048[3];
       float P049[3];
       float P050[3];
       float P051[3];
       float P052[3];
       float P061[3];
       float P069[3];
       float P070[3];



shark()
{

P002[3] = {0, -36.59, 5687.72};
P004[3] = {58.24, -146.84, 262.35};
P007[3] = {-46.09, -146.83, 265.84};
P010[3] = {-27.81, -285.31, 500};
P011[3] = {27.81, -285.32, 500};
P023[3] = {150, 0, 3000};
P024[3] = {121.35, 293.89, 3000};
P025[3] = {46.35, 502.93, 2883.09};
P026[3] = {-46.35, 497.45, 2877.24};
P027[3] = {-121.35, 293.9, 3000};
P028[3] = {-150, 0, 3000};
P029[3] = {-152.21, -304.84, 2858.68};
P030[3] = {-46.36, -475.52, 3000};
P031[3] = {46.35, -475.53, 3000};
P032[3] = {155.64, -304.87, 2863.5};
P033[3] = {90, 0, 4000};
P034[3] = {72.81, 176.33, 4000};
P035[3] = {27.81, 285.32, 4000};
P036[3] = {-27.81, 285.32, 4000};
P037[3] = {-72.81, 176.34, 4000};
P038[3] = {-90, 0, 4000};
P039[3] = {-72.81, -176.33, 4000};
P040[3] = {-27.81, -285.31, 4000};
P041[3] = {27.81, -285.32, 4000};
P042[3] = {72.81, -176.34, 4000};
P043[3] = {30, 0, 5000};
P044[3] = {24.27, 58.78, 5000};
P045[3] = {9.27, 95.11, 5000};
P046[3] = {-9.27, 95.11, 5000};
P047[3] = {-24.27, 58.78, 5000};
P048[3] = {-30, 0, 5000};
P049[3] = {-24.27, -58.78, 5000};
P050[3] = {-9.27, -95.1, 5000};
P051[3] = {9.27, -95.11, 5000};
P052[3] = {24.27, -58.78, 5000};
P061[3] = {0, 1181.61, 6344.65};
P069[3] = {0, -418.25, 5765.04};
P070[3] = {0, 1266.91, 6629.6};
}










 void
Fish001(GLenum cap)
{
	glBegin(cap);
	glNormal3fv(N005);
	glVertex3fv(P005);
	glNormal3fv(N059);
	glVertex3fv(P059);
	glNormal3fv(N060);
	glVertex3fv(P060);
	glNormal3fv(N006);
	glVertex3fv(P006);
	glEnd();
	glBegin(cap);
	glNormal3fv(N015);
	glVertex3fv(P015);
	glNormal3fv(N005);
	glVertex3fv(P005);
	glNormal3fv(N006);
	glVertex3fv(P006);
	glNormal3fv(N016);
	glVertex3fv(P016);
	glEnd();
	glBegin(cap);
	glNormal3fv(N006);
	glVertex3fv(P006);
	glNormal3fv(N060);
	glVertex3fv(P060);
	glNormal3fv(N008);
	glVertex3fv(P008);
	glEnd();
	glBegin(cap);
	glNormal3fv(N016);
	glVertex3fv(P016);
	glNormal3fv(N006);
	glVertex3fv(P006);
	glNormal3fv(N008);
	glVertex3fv(P008);
	glEnd();
	glBegin(cap);
	glNormal3fv(N016);
	glVertex3fv(P016);
	glNormal3fv(N008);
	glVertex3fv(P008);
	glNormal3fv(N017);
	glVertex3fv(P017);
	glEnd();
	glBegin(cap);
	glNormal3fv(N017);
	glVertex3fv(P017);
	glNormal3fv(N008);
	glVertex3fv(P008);
	glNormal3fv(N018);
	glVertex3fv(P018);
	glEnd();
	glBegin(cap);
	glNormal3fv(N008);
	glVertex3fv(P008);
	glNormal3fv(N009);
	glVertex3fv(P009);
	glNormal3fv(N018);
	glVertex3fv(P018);
	glEnd();
	glBegin(cap);
	glNormal3fv(N008);
	glVertex3fv(P008);
	glNormal3fv(N060);
	glVertex3fv(P060);
	glNormal3fv(N009);
	glVertex3fv(P009);
	glEnd();
	glBegin(cap);
	glNormal3fv(N007);
	glVertex3fv(P007);
	glNormal3fv(N010);
	glVertex3fv(P010);
	glNormal3fv(N009);
	glVertex3fv(P009);
	glEnd();
	glBegin(cap);
	glNormal3fv(N009);
	glVertex3fv(P009);
	glNormal3fv(N019);
	glVertex3fv(P019);
	glNormal3fv(N018);
	glVertex3fv(P018);
	glEnd();
	glBegin(cap);
	glNormal3fv(N009);
	glVertex3fv(P009);
	glNormal3fv(N010);
	glVertex3fv(P010);
	glNormal3fv(N019);
	glVertex3fv(P019);
	glEnd();
	glBegin(cap);
	glNormal3fv(N010);
	glVertex3fv(P010);
	glNormal3fv(N020);
	glVertex3fv(P020);
	glNormal3fv(N019);
	glVertex3fv(P019);
	glEnd();
	glBegin(cap);
	glNormal3fv(N010);
	glVertex3fv(P010);
	glNormal3fv(N011);
	glVertex3fv(P011);
	glNormal3fv(N021);
	glVertex3fv(P021);
	glNormal3fv(N020);
	glVertex3fv(P020);
	glEnd();
	glBegin(cap);
	glNormal3fv(N004);
	glVertex3fv(P004);
	glNormal3fv(N011);
	glVertex3fv(P011);
	glNormal3fv(N010);
	glVertex3fv(P010);
	glNormal3fv(N007);
	glVertex3fv(P007);
	glEnd();
	glBegin(cap);
	glNormal3fv(N004);
	glVertex3fv(P004);
	glNormal3fv(N012);
	glVertex3fv(P012);
	glNormal3fv(N011);
	glVertex3fv(P011);
	glEnd();
	glBegin(cap);
	glNormal3fv(N012);
	glVertex3fv(P012);
	glNormal3fv(N022);
	glVertex3fv(P022);
	glNormal3fv(N011);
	glVertex3fv(P011);
	glEnd();
	glBegin(cap);
	glNormal3fv(N011);
	glVertex3fv(P011);
	glNormal3fv(N022);
	glVertex3fv(P022);
	glNormal3fv(N021);
	glVertex3fv(P021);
	glEnd();
	glBegin(cap);
	glNormal3fv(N059);
	glVertex3fv(P059);
	glNormal3fv(N005);
	glVertex3fv(P005);
	glNormal3fv(N015);
	glVertex3fv(P015);
	glEnd();
	glBegin(cap);
	glNormal3fv(N015);
	glVertex3fv(P015);
	glNormal3fv(N014);
	glVertex3fv(P014);
	glNormal3fv(N003);
	glVertex3fv(P003);
	glEnd();
	glBegin(cap);
	glNormal3fv(N015);
	glVertex3fv(P015);
	glNormal3fv(N003);
	glVertex3fv(P003);
	glNormal3fv(N059);
	glVertex3fv(P059);
	glEnd();
	glBegin(cap);
	glNormal3fv(N014);
	glVertex3fv(P014);
	glNormal3fv(N013);
	glVertex3fv(P013);
	glNormal3fv(N003);
	glVertex3fv(P003);
	glEnd();
	glBegin(cap);
	glNormal3fv(N003);
	glVertex3fv(P003);
	glNormal3fv(N012);
	glVertex3fv(P012);
	glNormal3fv(N059);
	glVertex3fv(P059);
	glEnd();
	glBegin(cap);
	glNormal3fv(N013);
	glVertex3fv(P013);
	glNormal3fv(N012);
	glVertex3fv(P012);
	glNormal3fv(N003);
	glVertex3fv(P003);
	glEnd();
	glBegin(cap);
	glNormal3fv(N013);
	glVertex3fv(P013);
	glNormal3fv(N022);
	glVertex3fv(P022);
	glNormal3fv(N012);
	glVertex3fv(P012);
	glEnd();
	glBegin(cap);
	glVertex3fv(P071);
	glVertex3fv(P072);
	glVertex3fv(P073);
	glVertex3fv(P074);
	glVertex3fv(P075);
	glVertex3fv(P076);
	glEnd();
	glBegin(cap);
	glVertex3fv(P077);
	glVertex3fv(P078);
	glVertex3fv(P079);
	glVertex3fv(P080);
	glVertex3fv(P081);
	glVertex3fv(P082);
	glEnd();
}

 void
Fish002(GLenum cap)
{
	glBegin(cap);
	glNormal3fv(N013);
	glVertex3fv(P013);
	glNormal3fv(N014);
	glVertex3fv(P014);
	glNormal3fv(N024);
	glVertex3fv(P024);
	glNormal3fv(N023);
	glVertex3fv(P023);
	glEnd();
	glBegin(cap);
	glNormal3fv(N014);
	glVertex3fv(P014);
	glNormal3fv(N015);
	glVertex3fv(P015);
	glNormal3fv(N025);
	glVertex3fv(P025);
	glNormal3fv(N024);
	glVertex3fv(P024);
	glEnd();
	glBegin(cap);
	glNormal3fv(N016);
	glVertex3fv(P016);
	glNormal3fv(N017);
	glVertex3fv(P017);
	glNormal3fv(N027);
	glVertex3fv(P027);
	glNormal3fv(N026);
	glVertex3fv(P026);
	glEnd();
	glBegin(cap);
	glNormal3fv(N017);
	glVertex3fv(P017);
	glNormal3fv(N018);
	glVertex3fv(P018);
	glNormal3fv(N028);
	glVertex3fv(P028);
	glNormal3fv(N027);
	glVertex3fv(P027);
	glEnd();
	glBegin(cap);
	glNormal3fv(N020);
	glVertex3fv(P020);
	glNormal3fv(N021);
	glVertex3fv(P021);
	glNormal3fv(N031);
	glVertex3fv(P031);
	glNormal3fv(N030);
	glVertex3fv(P030);
	glEnd();
	glBegin(cap);
	glNormal3fv(N013);
	glVertex3fv(P013);
	glNormal3fv(N023);
	glVertex3fv(P023);
	glNormal3fv(N022);
	glVertex3fv(P022);
	glEnd();
	glBegin(cap);
	glNormal3fv(N022);
	glVertex3fv(P022);
	glNormal3fv(N023);
	glVertex3fv(P023);
	glNormal3fv(N032);
	glVertex3fv(P032);
	glEnd();
	glBegin(cap);
	glNormal3fv(N022);
	glVertex3fv(P022);
	glNormal3fv(N032);
	glVertex3fv(P032);
	glNormal3fv(N031);
	glVertex3fv(P031);
	glEnd();
	glBegin(cap);
	glNormal3fv(N022);
	glVertex3fv(P022);
	glNormal3fv(N031);
	glVertex3fv(P031);
	glNormal3fv(N021);
	glVertex3fv(P021);
	glEnd();
	glBegin(cap);
	glNormal3fv(N018);
	glVertex3fv(P018);
	glNormal3fv(N019);
	glVertex3fv(P019);
	glNormal3fv(N029);
	glVertex3fv(P029);
	glEnd();
	glBegin(cap);
	glNormal3fv(N018);
	glVertex3fv(P018);
	glNormal3fv(N029);
	glVertex3fv(P029);
	glNormal3fv(N028);
	glVertex3fv(P028);
	glEnd();
	glBegin(cap);
	glNormal3fv(N019);
	glVertex3fv(P019);
	glNormal3fv(N020);
	glVertex3fv(P020);
	glNormal3fv(N030);
	glVertex3fv(P030);
	glEnd();
	glBegin(cap);
	glNormal3fv(N019);
	glVertex3fv(P019);
	glNormal3fv(N030);
	glVertex3fv(P030);
	glNormal3fv(N029);
	glVertex3fv(P029);
	glEnd();
}

 void
Fish003(GLenum cap)
{
	glBegin(cap);
	glNormal3fv(N032);
	glVertex3fv(P032);
	glNormal3fv(N023);
	glVertex3fv(P023);
	glNormal3fv(N033);
	glVertex3fv(P033);
	glNormal3fv(N042);
	glVertex3fv(P042);
	glEnd();
	glBegin(cap);
	glNormal3fv(N031);
	glVertex3fv(P031);
	glNormal3fv(N032);
	glVertex3fv(P032);
	glNormal3fv(N042);
	glVertex3fv(P042);
	glNormal3fv(N041);
	glVertex3fv(P041);
	glEnd();
	glBegin(cap);
	glNormal3fv(N023);
	glVertex3fv(P023);
	glNormal3fv(N024);
	glVertex3fv(P024);
	glNormal3fv(N034);
	glVertex3fv(P034);
	glNormal3fv(N033);
	glVertex3fv(P033);
	glEnd();
	glBegin(cap);
	glNormal3fv(N024);
	glVertex3fv(P024);
	glNormal3fv(N025);
	glVertex3fv(P025);
	glNormal3fv(N035);
	glVertex3fv(P035);
	glNormal3fv(N034);
	glVertex3fv(P034);
	glEnd();
	glBegin(cap);
	glNormal3fv(N030);
	glVertex3fv(P030);
	glNormal3fv(N031);
	glVertex3fv(P031);
	glNormal3fv(N041);
	glVertex3fv(P041);
	glNormal3fv(N040);
	glVertex3fv(P040);
	glEnd();
	glBegin(cap);
	glNormal3fv(N025);
	glVertex3fv(P025);
	glNormal3fv(N026);
	glVertex3fv(P026);
	glNormal3fv(N036);
	glVertex3fv(P036);
	glNormal3fv(N035);
	glVertex3fv(P035);
	glEnd();
	glBegin(cap);
	glNormal3fv(N026);
	glVertex3fv(P026);
	glNormal3fv(N027);
	glVertex3fv(P027);
	glNormal3fv(N037);
	glVertex3fv(P037);
	glNormal3fv(N036);
	glVertex3fv(P036);
	glEnd();
	glBegin(cap);
	glNormal3fv(N027);
	glVertex3fv(P027);
	glNormal3fv(N028);
	glVertex3fv(P028);
	glNormal3fv(N038);
	glVertex3fv(P038);
	glNormal3fv(N037);
	glVertex3fv(P037);
	glEnd();
	glBegin(cap);
	glNormal3fv(N028);
	glVertex3fv(P028);
	glNormal3fv(N029);
	glVertex3fv(P029);
	glNormal3fv(N039);
	glVertex3fv(P039);
	glNormal3fv(N038);
	glVertex3fv(P038);
	glEnd();
	glBegin(cap);
	glNormal3fv(N029);
	glVertex3fv(P029);
	glNormal3fv(N030);
	glVertex3fv(P030);
	glNormal3fv(N040);
	glVertex3fv(P040);
	glNormal3fv(N039);
	glVertex3fv(P039);
	glEnd();
}

 void
Fish004(GLenum cap)
{
	glBegin(cap);
	glNormal3fv(N040);
	glVertex3fv(P040);
	glNormal3fv(N041);
	glVertex3fv(P041);
	glNormal3fv(N051);
	glVertex3fv(P051);
	glNormal3fv(N050);
	glVertex3fv(P050);
	glEnd();
	glBegin(cap);
	glNormal3fv(N041);
	glVertex3fv(P041);
	glNormal3fv(N042);
	glVertex3fv(P042);
	glNormal3fv(N052);
	glVertex3fv(P052);
	glNormal3fv(N051);
	glVertex3fv(P051);
	glEnd();
	glBegin(cap);
	glNormal3fv(N042);
	glVertex3fv(P042);
	glNormal3fv(N033);
	glVertex3fv(P033);
	glNormal3fv(N043);
	glVertex3fv(P043);
	glNormal3fv(N052);
	glVertex3fv(P052);
	glEnd();
	glBegin(cap);
	glNormal3fv(N033);
	glVertex3fv(P033);
	glNormal3fv(N034);
	glVertex3fv(P034);
	glNormal3fv(N044);
	glVertex3fv(P044);
	glNormal3fv(N043);
	glVertex3fv(P043);
	glEnd();
	glBegin(cap);
	glNormal3fv(N034);
	glVertex3fv(P034);
	glNormal3fv(N035);
	glVertex3fv(P035);
	glNormal3fv(N045);
	glVertex3fv(P045);
	glNormal3fv(N044);
	glVertex3fv(P044);
	glEnd();
	glBegin(cap);
	glNormal3fv(N035);
	glVertex3fv(P035);
	glNormal3fv(N036);
	glVertex3fv(P036);
	glNormal3fv(N046);
	glVertex3fv(P046);
	glNormal3fv(N045);
	glVertex3fv(P045);
	glEnd();
	glBegin(cap);
	glNormal3fv(N036);
	glVertex3fv(P036);
	glNormal3fv(N037);
	glVertex3fv(P037);
	glNormal3fv(N047);
	glVertex3fv(P047);
	glNormal3fv(N046);
	glVertex3fv(P046);
	glEnd();
	glBegin(cap);
	glNormal3fv(N037);
	glVertex3fv(P037);
	glNormal3fv(N038);
	glVertex3fv(P038);
	glNormal3fv(N048);
	glVertex3fv(P048);
	glNormal3fv(N047);
	glVertex3fv(P047);
	glEnd();
	glBegin(cap);
	glNormal3fv(N038);
	glVertex3fv(P038);
	glNormal3fv(N039);
	glVertex3fv(P039);
	glNormal3fv(N049);
	glVertex3fv(P049);
	glNormal3fv(N048);
	glVertex3fv(P048);
	glEnd();
	glBegin(cap);
	glNormal3fv(N039);
	glVertex3fv(P039);
	glNormal3fv(N040);
	glVertex3fv(P040);
	glNormal3fv(N050);
	glVertex3fv(P050);
	glNormal3fv(N049);
	glVertex3fv(P049);
	glEnd();
	glBegin(cap);
	glNormal3fv(N070);
	glVertex3fv(P070);
	glNormal3fv(N061);
	glVertex3fv(P061);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glEnd();
	glBegin(cap);
	glNormal3fv(N061);
	glVertex3fv(P061);
	glNormal3fv(N046);
	glVertex3fv(P046);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glEnd();
	glBegin(cap);
	glNormal3fv(N045);
	glVertex3fv(P045);
	glNormal3fv(N046);
	glVertex3fv(P046);
	glNormal3fv(N061);
	glVertex3fv(P061);
	glEnd();
	glBegin(cap);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glNormal3fv(N061);
	glVertex3fv(P061);
	glNormal3fv(N070);
	glVertex3fv(P070);
	glEnd();
	glBegin(cap);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glNormal3fv(N045);
	glVertex3fv(P045);
	glNormal3fv(N061);
	glVertex3fv(P061);
	glEnd();
}

 void
Fish005(GLenum cap)
{
	glBegin(cap);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glNormal3fv(N044);
	glVertex3fv(P044);
	glNormal3fv(N045);
	glVertex3fv(P045);
	glEnd();
	glBegin(cap);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glNormal3fv(N043);
	glVertex3fv(P043);
	glNormal3fv(N044);
	glVertex3fv(P044);
	glEnd();
	glBegin(cap);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glNormal3fv(N052);
	glVertex3fv(P052);
	glNormal3fv(N043);
	glVertex3fv(P043);
	glEnd();
	glBegin(cap);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glNormal3fv(N051);
	glVertex3fv(P051);
	glNormal3fv(N052);
	glVertex3fv(P052);
	glEnd();
	glBegin(cap);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glNormal3fv(N046);
	glVertex3fv(P046);
	glNormal3fv(N047);
	glVertex3fv(P047);
	glEnd();
	glBegin(cap);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glNormal3fv(N047);
	glVertex3fv(P047);
	glNormal3fv(N048);
	glVertex3fv(P048);
	glEnd();
	glBegin(cap);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glNormal3fv(N048);
	glVertex3fv(P048);
	glNormal3fv(N049);
	glVertex3fv(P049);
	glEnd();
	glBegin(cap);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glNormal3fv(N049);
	glVertex3fv(P049);
	glNormal3fv(N050);
	glVertex3fv(P050);
	glEnd();
	glBegin(cap);
	glNormal3fv(N050);
	glVertex3fv(P050);
	glNormal3fv(N051);
	glVertex3fv(P051);
	glNormal3fv(N069);
	glVertex3fv(P069);
	glEnd();
	glBegin(cap);
	glNormal3fv(N051);
	glVertex3fv(P051);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glNormal3fv(N069);
	glVertex3fv(P069);
	glEnd();
	glBegin(cap);
	glNormal3fv(N050);
	glVertex3fv(P050);
	glNormal3fv(N069);
	glVertex3fv(P069);
	glNormal3fv(N002);
	glVertex3fv(P002);
	glEnd();
}

 void
Fish006(GLenum cap)
{
	glBegin(cap);
	glNormal3fv(N066);
	glVertex3fv(P066);
	glNormal3fv(N016);
	glVertex3fv(P016);
	glNormal3fv(N026);
	glVertex3fv(P026);
	glEnd();
	glBegin(cap);
	glNormal3fv(N015);
	glVertex3fv(P015);
	glNormal3fv(N066);
	glVertex3fv(P066);
	glNormal3fv(N025);
	glVertex3fv(P025);
	glEnd();
	glBegin(cap);
	glNormal3fv(N025);
	glVertex3fv(P025);
	glNormal3fv(N066);
	glVertex3fv(P066);
	glNormal3fv(N026);
	glVertex3fv(P026);
	glEnd();
	glBegin(cap);
	glNormal3fv(N066);
	glVertex3fv(P066);
	glNormal3fv(N058);
	glVertex3fv(P058);
	glNormal3fv(N016);
	glVertex3fv(P016);
	glEnd();
	glBegin(cap);
	glNormal3fv(N015);
	glVertex3fv(P015);
	glNormal3fv(N058);
	glVertex3fv(P058);
	glNormal3fv(N066);
	glVertex3fv(P066);
	glEnd();
	glBegin(cap);
	glNormal3fv(N058);
	glVertex3fv(P058);
	glNormal3fv(N015);
	glVertex3fv(P015);
	glNormal3fv(N016);
	glVertex3fv(P016);
	glEnd();
}

 void
Fish007(GLenum cap)
{
	glBegin(cap);
	glNormal3fv(N062);
	glVertex3fv(P062);
	glNormal3fv(N022);
	glVertex3fv(P022);
	glNormal3fv(N032);
	glVertex3fv(P032);
	glEnd();
	glBegin(cap);
	glNormal3fv(N062);
	glVertex3fv(P062);
	glNormal3fv(N032);
	glVertex3fv(P032);
	glNormal3fv(N064);
	glVertex3fv(P064);
	glEnd();
	glBegin(cap);
	glNormal3fv(N022);
	glVertex3fv(P022);
	glNormal3fv(N062);
	glVertex3fv(P062);
	glNormal3fv(N032);
	glVertex3fv(P032);
	glEnd();
	glBegin(cap);
	glNormal3fv(N062);
	glVertex3fv(P062);
	glNormal3fv(N064);
	glVertex3fv(P064);
	glNormal3fv(N032);
	glVertex3fv(P032);
	glEnd();
}

 void
Fish008(GLenum cap)
{
	glBegin(cap);
	glNormal3fv(N063);
	glVertex3fv(P063);
	glNormal3fv(N019);
	glVertex3fv(P019);
	glNormal3fv(N029);
	glVertex3fv(P029);
	glEnd();
	glBegin(cap);
	glNormal3fv(N019);
	glVertex3fv(P019);
	glNormal3fv(N063);
	glVertex3fv(P063);
	glNormal3fv(N029);
	glVertex3fv(P029);
	glEnd();
	glBegin(cap);
	glNormal3fv(N063);
	glVertex3fv(P063);
	glNormal3fv(N029);
	glVertex3fv(P029);
	glNormal3fv(N065);
	glVertex3fv(P065);
	glEnd();
	glBegin(cap);
	glNormal3fv(N063);
	glVertex3fv(P063);
	glNormal3fv(N065);
	glVertex3fv(P065);
	glNormal3fv(N029);
	glVertex3fv(P029);
	glEnd();
}

 void
Fish009(GLenum cap)
{
	glBegin(cap);
	glVertex3fv(P059);
	glVertex3fv(P012);
	glVertex3fv(P009);
	glVertex3fv(P060);
	glEnd();
	glBegin(cap);
	glVertex3fv(P012);
	glVertex3fv(P004);
	glVertex3fv(P007);
	glVertex3fv(P009);
	glEnd();
}

 void
Fish_1(GLenum cap)
{
	Fish004(cap);
	Fish005(cap);
	Fish003(cap);
	Fish007(cap);
	Fish006(cap);
	Fish002(cap);
	Fish008(cap);
	Fish009(cap);
	Fish001(cap);
}

 void
Fish_2(GLenum cap)
{
	Fish005(cap);
	Fish004(cap);
	Fish003(cap);
	Fish008(cap);
	Fish006(cap);
	Fish002(cap);
	Fish007(cap);
	Fish009(cap);
	Fish001(cap);
}

 void
Fish_3(GLenum cap)
{
	Fish005(cap);
	Fish004(cap);
	Fish007(cap);
	Fish003(cap);
	Fish002(cap);
	Fish008(cap);
	Fish009(cap);
	Fish001(cap);
	Fish006(cap);
}

 void
Fish_4(GLenum cap)
{
	Fish005(cap);
	Fish004(cap);
	Fish008(cap);
	Fish003(cap);
	Fish002(cap);
	Fish007(cap);
	Fish009(cap);
	Fish001(cap);
	Fish006(cap);
}

 void
Fish_5(GLenum cap)
{
	Fish009(cap);
	Fish006(cap);
	Fish007(cap);
	Fish001(cap);
	Fish002(cap);
	Fish003(cap);
	Fish008(cap);
	Fish004(cap);
	Fish005(cap);
}

 void
Fish_6(GLenum cap)
{
	Fish009(cap);
	Fish006(cap);
	Fish008(cap);
	Fish001(cap);
	Fish002(cap);
	Fish007(cap);
	Fish003(cap);
	Fish004(cap);
	Fish005(cap);
}

 void
Fish_7(GLenum cap)
{
	Fish009(cap);
	Fish001(cap);
	Fish007(cap);
	Fish005(cap);
	Fish002(cap);
	Fish008(cap);
	Fish003(cap);
	Fish004(cap);
	Fish006(cap);
}

 void
Fish_8(GLenum cap)
{
	Fish009(cap);
	Fish008(cap);
	Fish001(cap);
	Fish002(cap);
	Fish007(cap);
	Fish003(cap);
	Fish005(cap);
	Fish004(cap);
	Fish006(cap);
}

void
Draw()
{
	float       mat[4][4];
	int         n;
	float       seg1, seg2, seg3, seg4, segup;
	float       thrash, chomp;
	GLenum      cap;

	htail = (int) (htail - (int) (5 * v)) % 360;

	thrash = 50 * v;

	seg1 = 0.6 * thrash * sin(htail * RRAD);
	seg2 = 1.8 * thrash * sin((htail + 45) * RRAD);
	seg3 = 3 * thrash * sin((htail + 90) * RRAD);
	seg4 = 4 * thrash * sin((htail + 110) * RRAD);

	chomp = 0;
	if (v > 2) {
		chomp = -(v - 2) * 200;
	}
	P004[1] = iP004[1] + chomp;
	P007[1] = iP007[1] + chomp;
	P010[1] = iP010[1] + chomp;
	P011[1] = iP011[1] + chomp;

	P023[0] = iP023[0] + seg1;
	P024[0] = iP024[0] + seg1;
	P025[0] = iP025[0] + seg1;
	P026[0] = iP026[0] + seg1;
	P027[0] = iP027[0] + seg1;
	P028[0] = iP028[0] + seg1;
	P029[0] = iP029[0] + seg1;
	P030[0] = iP030[0] + seg1;
	P031[0] = iP031[0] + seg1;
	P032[0] = iP032[0] + seg1;
	P033[0] = iP033[0] + seg2;
	P034[0] = iP034[0] + seg2;
	P035[0] = iP035[0] + seg2;
	P036[0] = iP036[0] + seg2;
	P037[0] = iP037[0] + seg2;
	P038[0] = iP038[0] + seg2;
	P039[0] = iP039[0] + seg2;
	P040[0] = iP040[0] + seg2;
	P041[0] = iP041[0] + seg2;
	P042[0] = iP042[0] + seg2;
	P043[0] = iP043[0] + seg3;
	P044[0] = iP044[0] + seg3;
	P045[0] = iP045[0] + seg3;
	P046[0] = iP046[0] + seg3;
	P047[0] = iP047[0] + seg3;
	P048[0] = iP048[0] + seg3;
	P049[0] = iP049[0] + seg3;
	P050[0] = iP050[0] + seg3;
	P051[0] = iP051[0] + seg3;
	P052[0] = iP052[0] + seg3;
	P002[0] = iP002[0] + seg4;
	P061[0] = iP061[0] + seg4;
	P069[0] = iP069[0] + seg4;
	P070[0] = iP070[0] + seg4;

	vtail += ((dtheta - vtail) * 0.1);

	if (vtail > 0.5) {
		vtail = 0.5;
	} else if (vtail < -0.5) {
		vtail = -0.5;
	}
	segup = thrash * vtail;

	P023[1] = iP023[1] + segup;
	P024[1] = iP024[1] + segup;
	P025[1] = iP025[1] + segup;
	P026[1] = iP026[1] + segup;
	P027[1] = iP027[1] + segup;
	P028[1] = iP028[1] + segup;
	P029[1] = iP029[1] + segup;
	P030[1] = iP030[1] + segup;
	P031[1] = iP031[1] + segup;
	P032[1] = iP032[1] + segup;
	P033[1] = iP033[1] + segup * 5;
	P034[1] = iP034[1] + segup * 5;
	P035[1] = iP035[1] + segup * 5;
	P036[1] = iP036[1] + segup * 5;
	P037[1] = iP037[1] + segup * 5;
	P038[1] = iP038[1] + segup * 5;
	P039[1] = iP039[1] + segup * 5;
	P040[1] = iP040[1] + segup * 5;
	P041[1] = iP041[1] + segup * 5;
	P042[1] = iP042[1] + segup * 5;
	P043[1] = iP043[1] + segup * 12;
	P044[1] = iP044[1] + segup * 12;
	P045[1] = iP045[1] + segup * 12;
	P046[1] = iP046[1] + segup * 12;
	P047[1] = iP047[1] + segup * 12;
	P048[1] = iP048[1] + segup * 12;
	P049[1] = iP049[1] + segup * 12;
	P050[1] = iP050[1] + segup * 12;
	P051[1] = iP051[1] + segup * 12;
	P052[1] = iP052[1] + segup * 12;
	P002[1] = iP002[1] + segup * 17;
	P061[1] = iP061[1] + segup * 17;
	P069[1] = iP069[1] + segup * 17;
	P070[1] = iP070[1] + segup * 17;

	glPushMatrix();

	glTranslatef(0, 0, -3000);

	glGetFloatv(GL_MODELVIEW_MATRIX, &mat[0][0]);
	n = 0;
	if (mat[0][2] >= 0) {
		n += 1;
	}
	if (mat[1][2] >= 0) {
		n += 2;
	}
	if (mat[2][2] >= 0) {
		n += 4;
	}
	glScalef(2, 1, 1);

	glEnable(GL_CULL_FACE);
	cap = wire ? GL_LINE_LOOP : GL_POLYGON;
	switch (n) {
		case 0:
			Fish_1(cap);
			break;
		case 1:
			Fish_2(cap);
			break;
		case 2:
			Fish_3(cap);
			break;
		case 3:
			Fish_4(cap);
			break;
		case 4:
			Fish_5(cap);
			break;
		case 5:
			Fish_6(cap);
			break;
		case 6:
			Fish_7(cap);
			break;
		case 7:
			Fish_8(cap);
			break;
	}
	glDisable(GL_CULL_FACE);

	glPopMatrix();
}
