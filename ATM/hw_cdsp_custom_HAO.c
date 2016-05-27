/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2002 by Sunplus Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author: Jen Hung Yu                                                   *
 *                                                                        *
 **************************************************************************/
#include "general.h" 
#include "stdio.h"
#include "hw_cdsp.h"
#include "hw_disp.h"  
#include "hw_cpu.h" 
#include "hw_sdram.h" 
#include "sysregs.h" 
#include "hw_front.h"
#include "aaa_api.h"
#include "device.h"

/* #define CDSP_FILTER_9X9 */
extern void sysgChipVerGet(UINT8 *pmajor, UINT8 *pminor);
UINT32 GAMMA_TBL[1024] = {
/*0,0,0,1,1,2,2,2,3,3,3,4,4,4,5,5,
6,6,6,7,7,7,8,8,8,9,9,9,10,10,11,11,
11,12,12,12,13,13,13,14,14,14,15,15,16,16,16,17,
17,17,18,18,18,19,19,20,20,20,21,21,21,22,22,22,
23,24,24,25,26,27,28,28,29,30,31,32,32,33,34,35,
36,36,37,38,39,39,40,41,42,43,43,44,45,46,47,47,
48,49,50,51,51,52,53,54,55,55,56,57,58,59,59,60,
61,62,63,63,64,65,66,67,67,68,69,70,71,71,72,73,
74,74,75,75,76,76,77,77,78,78,79,79,80,80,81,81,
82,82,82,83,83,84,84,85,85,86,86,87,87,88,88,89,
89,89,90,90,91,91,92,92,93,93,94,94,95,95,96,96,
97,97,98,98,99,99,100,100,101,101,102,102,103,103,104,104,
105,105,105,106,106,107,107,107,108,108,109,109,109,110,110,111,
111,111,112,112,113,113,113,114,114,115,115,115,116,116,117,117,
118,118,118,119,119,120,120,120,121,121,122,122,122,123,123,124,
124,124,125,125,126,126,126,127,127,128,128,128,129,129,130,130,
131,131,131,132,132,132,133,133,133,134,134,134,135,135,135,136,
136,136,137,137,137,138,138,138,139,139,139,140,140,140,141,141,
142,142,142,143,143,143,144,144,144,145,145,145,146,146,146,147,
147,147,148,148,148,149,149,149,150,150,150,151,151,151,152,152,
153,153,153,153,154,154,154,154,155,155,155,156,156,156,156,157,
157,157,158,158,158,158,159,159,159,160,160,160,160,161,161,161,
162,162,162,162,163,163,163,163,164,164,164,165,165,165,165,166,
166,166,167,167,167,167,168,168,168,169,169,169,169,170,170,170,
170,171,171,171,171,172,172,172,172,173,173,173,173,174,174,174,
174,175,175,175,175,176,176,176,176,177,177,177,177,178,178,178,
178,179,179,179,179,180,180,180,180,181,181,181,181,182,182,182,
182,183,183,183,183,184,184,184,184,185,185,185,185,186,186,186,
186,186,187,187,187,187,187,188,188,188,188,188,189,189,189,189,
189,189,190,190,190,190,190,191,191,191,191,191,192,192,192,192,
192,192,193,193,193,193,193,194,194,194,194,194,195,195,195,195,
195,195,196,196,196,196,196,197,197,197,197,197,198,198,198,198,
198,198,199,199,199,199,199,199,200,200,200,200,200,200,200,201,
201,201,201,201,201,202,202,202,202,202,202,202,203,203,203,203,
203,203,204,204,204,204,204,204,205,205,205,205,205,205,205,206,
206,206,206,206,206,207,207,207,207,207,207,207,208,208,208,208,
208,208,209,209,209,209,209,209,209,209,210,210,210,210,210,210,
210,210,211,211,211,211,211,211,211,211,212,212,212,212,212,212,
212,212,213,213,213,213,213,213,213,213,214,214,214,214,214,214,
214,214,215,215,215,215,215,215,215,215,216,216,216,216,216,216,
216,216,216,217,217,217,217,217,217,217,217,217,218,218,218,218,
218,218,218,218,218,219,219,219,219,219,219,219,219,219,220,220,
220,220,220,220,220,220,220,221,221,221,221,221,221,221,221,221,
222,222,222,222,222,222,222,222,222,222,223,223,223,223,223,223,
223,223,223,224,224,224,224,224,224,224,224,224,225,225,225,225,
225,225,225,225,225,226,226,226,226,226,226,226,226,226,227,227,
227,227,227,227,227,227,227,228,228,228,228,228,228,228,228,228,
229,229,229,229,229,229,229,229,229,229,230,230,230,230,230,230,
230,230,230,231,231,231,231,231,231,231,231,231,232,232,232,232,
232,232,232,232,232,233,233,233,233,233,233,233,233,233,234,234,
234,234,234,234,234,234,234,235,235,235,235,235,235,235,235,235,
236,236,236,236,236,236,236,236,236,236,237,237,237,237,237,237,
237,237,237,238,238,238,238,238,238,238,238,238,239,239,239,239,
239,239,239,239,239,240,240,240,240,240,240,240,240,240,241,241,
241,241,241,241,241,241,241,242,242,242,242,242,242,242,242,242,
243,243,243,243,243,243,243,243,243,243,244,244,244,244,244,244,
244,244,244,245,245,245,245,245,245,245,245,245,246,246,246,246,
246,246,246,246,246,247,247,247,247,247,247,247,247,247,248,248,
248,248,248,248,248,248,248,249,249,249,249,249,249,249,249,249,
250,250,250,250,250,250,250,250,250,250,251,251,251,251,251,251,
251,251,251,251,251,251,252,252,252,252,252,252,252,252,252,252,
252,252,252,252,252,252,252,252,252,252,252,253,253,253,253,253,
253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,
254,254,254,254,254,254,254,254,254,254,255,255,255,255,255,255*/
0,   
1,   
2,   
3,   
4,   
5,   
6,   
7,   
8,   
9,   
10,  
11,  
12,  
13,  
14,  
15,  
16,  
17,  
18,  
19,  
20,  
21,  
22,  
23,  
24,  
25,  
26,  
27,  
28,  
29,  
30,  
31,  
32,  
33,  
34,  
35,  
36,  
37,  
38,  
39,  
40,  
41,  
42,  
43,  
44,  
45,  
46,  
47,  
48,  
49,  
50,  
51,  
52,  
53,  
54,  
55,  
56,  
57,  
58,  
59,  
60,  
60,  
61,  
62,  
63,  
64,  
64,  
65,  
66,  
67,  
68,  
68,  
69,  
70,  
71,  
71,  
72,  
73,  
74,  
74,  
75,  
76,  
77,  
77,  
78,  
79,  
80,  
80,  
81,  
82,  
82,  
83,  
84,  
84,  
85,  
86,  
86,  
87,  
88,  
88,  
89,  
90,  
90,  
91,  
92,  
92,  
93,  
94,  
94,  
95,  
96,  
96,  
97,  
97,  
98,  
99,  
99,  
100, 
100, 
101, 
102, 
102, 
103, 
103, 
104, 
104, 
105, 
106, 
106, 
107, 
107, 
108, 
108, 
109, 
109, 
110, 
110, 
111, 
111, 
112, 
113, 
113, 
114, 
114, 
115, 
115, 
116, 
116, 
117, 
117, 
118, 
118, 
119, 
119, 
120, 
120, 
120, 
121, 
121, 
122, 
122, 
123, 
123, 
124, 
124, 
125, 
125, 
126, 
126, 
126, 
127, 
127, 
128, 
128, 
129, 
129, 
129, 
130, 
130, 
131, 
131, 
132, 
132, 
132, 
133, 
133, 
134, 
134, 
134, 
135, 
135, 
136, 
136, 
136, 
137, 
137, 
137, 
138, 
138, 
139, 
139, 
139, 
140, 
140, 
140, 
141, 
141, 
142, 
142, 
142, 
143, 
143, 
143, 
144, 
144, 
144, 
145, 
145, 
145, 
146, 
146, 
146, 
147, 
147, 
147, 
148, 
148, 
148, 
149, 
149, 
149, 
150, 
150, 
150, 
151, 
151, 
151, 
152, 
152, 
152, 
152, 
153, 
153, 
153, 
154, 
154, 
154, 
155, 
155, 
155, 
155, 
156, 
156, 
156, 
157, 
157, 
157, 
157, 
158, 
158, 
158, 
159, 
159, 
159, 
159, 
160, 
160, 
160, 
160, 
161, 
161, 
161, 
162, 
162, 
162, 
162, 
163, 
163, 
163, 
163, 
164, 
164, 
164, 
164, 
165, 
165, 
165, 
165, 
166, 
166, 
166, 
166, 
167, 
167, 
167, 
167, 
168, 
168, 
168, 
168, 
169, 
169, 
169, 
169, 
170, 
170, 
170, 
170, 
170, 
171, 
171, 
171, 
171, 
172, 
172, 
172, 
172, 
172, 
173, 
173, 
173, 
173, 
174, 
174, 
174, 
174, 
174, 
175, 
175, 
175, 
175, 
176, 
176, 
176, 
176, 
176, 
177, 
177, 
177, 
177, 
177, 
178, 
178, 
178, 
178, 
179, 
179, 
179, 
179, 
179, 
180, 
180, 
180, 
180, 
180, 
181, 
181, 
181, 
181, 
181, 
182, 
182, 
182, 
182, 
182, 
183, 
183, 
183, 
183, 
183, 
184, 
184, 
184, 
184, 
184, 
184, 
185, 
185, 
185, 
185, 
185, 
186, 
186, 
186, 
186, 
186, 
187, 
187, 
187, 
187, 
187, 
188, 
188, 
188, 
188, 
188, 
188, 
189, 
189, 
189, 
189, 
189, 
190, 
190, 
190, 
190, 
190, 
190, 
191, 
191, 
191, 
191, 
191, 
192, 
192, 
192, 
192, 
192, 
192, 
193, 
193, 
193, 
193, 
193, 
194, 
194, 
194, 
194, 
194, 
194, 
195, 
195, 
195, 
195, 
195, 
195, 
196, 
196, 
196, 
196, 
196, 
197, 
197, 
197, 
197, 
197, 
197, 
198, 
198, 
198, 
198, 
198, 
198, 
199, 
199, 
199, 
199, 
199, 
199, 
200, 
200, 
200, 
200, 
200, 
200, 
201, 
201, 
201, 
201, 
201, 
201, 
202, 
202, 
202, 
202, 
202, 
202, 
203, 
203, 
203, 
203, 
203, 
204, 
204, 
204, 
204, 
204, 
204, 
205, 
205, 
205, 
205, 
205, 
205, 
206, 
206, 
206, 
206, 
206, 
206, 
206, 
207, 
207, 
207, 
207, 
207, 
207, 
208, 
208, 
208, 
208, 
208, 
208, 
209, 
209, 
209, 
209, 
209, 
209, 
210, 
210, 
210, 
210, 
210, 
210, 
211, 
211, 
211, 
211, 
211, 
211, 
212, 
212, 
212, 
212, 
212, 
212, 
212, 
213, 
213, 
213, 
213, 
213, 
213, 
214, 
214, 
214, 
214, 
214, 
214, 
215, 
215, 
215, 
215, 
215, 
215, 
215, 
216, 
216, 
216, 
216, 
216, 
216, 
217, 
217, 
217, 
217, 
217, 
217, 
218, 
218, 
218, 
218, 
218, 
218, 
218, 
219, 
219, 
219, 
219, 
219, 
219, 
219, 
220, 
220, 
220, 
220, 
220, 
220, 
221, 
221, 
221, 
221, 
221, 
221, 
221, 
222, 
222, 
222, 
222, 
222, 
222, 
222, 
223, 
223, 
223, 
223, 
223, 
223, 
224, 
224, 
224, 
224, 
224, 
224, 
224, 
225, 
225, 
225, 
225, 
225, 
225, 
225, 
226, 
226, 
226, 
226, 
226, 
226, 
226, 
227, 
227, 
227, 
227, 
227, 
227, 
227, 
227, 
228, 
228, 
228, 
228, 
228, 
228, 
228, 
229, 
229, 
229, 
229, 
229, 
229, 
229, 
230, 
230, 
230, 
230, 
230, 
230, 
230, 
230, 
231, 
231, 
231, 
231, 
231, 
231, 
231, 
232, 
232, 
232, 
232, 
232, 
232, 
232, 
232, 
233, 
233, 
233, 
233, 
233, 
233, 
233, 
233, 
234, 
234, 
234, 
234, 
234, 
234, 
234, 
234, 
235, 
235, 
235, 
235, 
235, 
235, 
235, 
235, 
235, 
236, 
236, 
236, 
236, 
236, 
236, 
236, 
236, 
237, 
237, 
237, 
237, 
237, 
237, 
237, 
237, 
237, 
238, 
238, 
238, 
238, 
238, 
238, 
238, 
238, 
238, 
238, 
239, 
239, 
239, 
239, 
239, 
239, 
239, 
239, 
239, 
240, 
240, 
240, 
240, 
240, 
240, 
240, 
240, 
240, 
240, 
241, 
241, 
241, 
241, 
241, 
241, 
241, 
241, 
241, 
241, 
242, 
242, 
242, 
242, 
242, 
242, 
242, 
242, 
242, 
242, 
242, 
243, 
243, 
243, 
243, 
243, 
243, 
243, 
243, 
243, 
243, 
243, 
243, 
244, 
244, 
244, 
244, 
244, 
244, 
244, 
244, 
244, 
244, 
244, 
244, 
245, 
245, 
245, 
245, 
245, 
245, 
245, 
245, 
245, 
245, 
245, 
245, 
245, 
246, 
246, 
246, 
246, 
246, 
246, 
246, 
246, 
246, 
246, 
246, 
246, 
246, 
246, 
247, 
247, 
247, 
247, 
247, 
247, 
247, 
247, 
247, 
247, 
247, 
247, 
247, 
247, 
247, 
247, 
248, 
248, 
248, 
248, 
248, 
248, 
248, 
248, 
248, 
248, 
248, 
248, 
248, 
248, 
248, 
248, 
248, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
249, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
250, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
251, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
252, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
253, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
254, 
255, 
255, 
255, 
255, 
255, 
255, 
255, 
255, 
255, 
255, 
255, 
255, 
255, 
255, 
255, 
255, 
255, 
255, 
255, 
255  
};

/*
UINT32 GAMMA_TBL[1024] = {
0x00,0x02,0x05,0x07,0x09,0x0B,0x0D,0x0F,0x11,0x12,0x14,0x15,0x17,0x18,0x19,0x1A,
0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x25,0x26,0x27,0x28,0x28,
0x29,0x2A,0x2A,0x2B,0x2C,0x2D,0x2D,0x2E,0x2E,0x2F,0x30,0x30,0x31,0x31,0x32,0x33,
0x33,0x34,0x34,0x35,0x35,0x36,0x36,0x37,0x38,0x38,0x39,0x39,0x3A,0x3A,0x3B,0x3B,
0x3C,0x3C,0x3C,0x3D,0x3D,0x3E,0x3E,0x3F,0x3F,0x40,0x40,0x41,0x41,0x41,0x42,0x42,
0x43,0x43,0x44,0x44,0x44,0x45,0x45,0x46,0x46,0x46,0x47,0x47,0x48,0x48,0x48,0x49,
0x49,0x49,0x4A,0x4A,0x4B,0x4B,0x4B,0x4C,0x4C,0x4C,0x4D,0x4D,0x4D,0x4E,0x4E,0x4E,
0x4F,0x4F,0x50,0x50,0x50,0x51,0x51,0x51,0x52,0x52,0x52,0x53,0x53,0x53,0x54,0x54,
0x54,0x54,0x55,0x55,0x55,0x56,0x56,0x56,0x57,0x57,0x57,0x58,0x58,0x58,0x58,0x59,
0x59,0x59,0x5A,0x5A,0x5A,0x5B,0x5B,0x5B,0x5C,0x5C,0x5C,0x5D,0x5D,0x5D,0x5E,0x5E,
0x5E,0x5F,0x5F,0x5F,0x60,0x60,0x60,0x61,0x61,0x61,0x62,0x62,0x62,0x63,0x63,0x63,
0x64,0x64,0x64,0x65,0x65,0x65,0x66,0x66,0x66,0x67,0x67,0x67,0x68,0x68,0x69,0x69,
0x69,0x6A,0x6A,0x6A,0x6B,0x6B,0x6B,0x6C,0x6C,0x6C,0x6D,0x6D,0x6D,0x6E,0x6E,0x6E,
0x6F,0x6F,0x6F,0x70,0x70,0x71,0x71,0x71,0x72,0x72,0x72,0x73,0x73,0x73,0x74,0x74,
0x75,0x75,0x75,0x76,0x76,0x76,0x77,0x77,0x77,0x78,0x78,0x78,0x79,0x79,0x79,0x7A,
0x7A,0x7A,0x7B,0x7B,0x7B,0x7C,0x7C,0x7C,0x7D,0x7D,0x7D,0x7E,0x7E,0x7E,0x7F,0x7F,
0x7F,0x80,0x80,0x80,0x81,0x81,0x81,0x82,0x82,0x82,0x83,0x83,0x83,0x84,0x84,0x84,
0x84,0x85,0x85,0x85,0x86,0x86,0x86,0x87,0x87,0x87,0x87,0x88,0x88,0x88,0x89,0x89,
0x89,0x8A,0x8A,0x8A,0x8A,0x8B,0x8B,0x8B,0x8C,0x8C,0x8C,0x8C,0x8D,0x8D,0x8D,0x8E,
0x8E,0x8E,0x8E,0x8F,0x8F,0x8F,0x8F,0x90,0x90,0x90,0x91,0x91,0x91,0x91,0x92,0x92,
0x92,0x92,0x93,0x93,0x93,0x94,0x94,0x94,0x94,0x95,0x95,0x95,0x95,0x96,0x96,0x96,
0x96,0x97,0x97,0x97,0x97,0x98,0x98,0x98,0x99,0x99,0x99,0x99,0x9A,0x9A,0x9A,0x9A,
0x9B,0x9B,0x9B,0x9B,0x9C,0x9C,0x9C,0x9C,0x9D,0x9D,0x9D,0x9D,0x9E,0x9E,0x9E,0x9E,
0x9F,0x9F,0x9F,0x9F,0x9F,0xA0,0xA0,0xA0,0xA0,0xA1,0xA1,0xA1,0xA1,0xA2,0xA2,0xA2,
0xA2,0xA3,0xA3,0xA3,0xA3,0xA4,0xA4,0xA4,0xA4,0xA4,0xA5,0xA5,0xA5,0xA5,0xA6,0xA6,
0xA6,0xA6,0xA7,0xA7,0xA7,0xA7,0xA7,0xA8,0xA8,0xA8,0xA8,0xA9,0xA9,0xA9,0xA9,0xA9,
0xAA,0xAA,0xAA,0xAA,0xAB,0xAB,0xAB,0xAB,0xAB,0xAC,0xAC,0xAC,0xAC,0xAD,0xAD,0xAD,
0xAD,0xAD,0xAE,0xAE,0xAE,0xAE,0xAE,0xAF,0xAF,0xAF,0xAF,0xB0,0xB0,0xB0,0xB0,0xB0,
0xB1,0xB1,0xB1,0xB1,0xB1,0xB2,0xB2,0xB2,0xB2,0xB3,0xB3,0xB3,0xB3,0xB3,0xB4,0xB4,
0xB4,0xB4,0xB4,0xB5,0xB5,0xB5,0xB5,0xB5,0xB6,0xB6,0xB6,0xB6,0xB6,0xB7,0xB7,0xB7,
0xB7,0xB7,0xB8,0xB8,0xB8,0xB8,0xB8,0xB9,0xB9,0xB9,0xB9,0xB9,0xBA,0xBA,0xBA,0xBA,
0xBA,0xBB,0xBB,0xBB,0xBB,0xBB,0xBC,0xBC,0xBC,0xBC,0xBC,0xBD,0xBD,0xBD,0xBD,0xBD,
0xBE,0xBE,0xBE,0xBE,0xBE,0xBF,0xBF,0xBF,0xBF,0xBF,0xBF,0xC0,0xC0,0xC0,0xC0,0xC0,
0xC1,0xC1,0xC1,0xC1,0xC1,0xC2,0xC2,0xC2,0xC2,0xC2,0xC2,0xC3,0xC3,0xC3,0xC3,0xC3,
0xC4,0xC4,0xC4,0xC4,0xC4,0xC5,0xC5,0xC5,0xC5,0xC5,0xC5,0xC6,0xC6,0xC6,0xC6,0xC6,
0xC7,0xC7,0xC7,0xC7,0xC7,0xC7,0xC8,0xC8,0xC8,0xC8,0xC8,0xC9,0xC9,0xC9,0xC9,0xC9,
0xC9,0xCA,0xCA,0xCA,0xCA,0xCA,0xCA,0xCB,0xCB,0xCB,0xCB,0xCB,0xCC,0xCC,0xCC,0xCC,
0xCC,0xCC,0xCD,0xCD,0xCD,0xCD,0xCD,0xCD,0xCE,0xCE,0xCE,0xCE,0xCE,0xCF,0xCF,0xCF,
0xCF,0xCF,0xCF,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD1,0xD1,0xD1,0xD1,0xD1,0xD1,0xD2,
0xD2,0xD2,0xD2,0xD2,0xD2,0xD3,0xD3,0xD3,0xD3,0xD3,0xD3,0xD4,0xD4,0xD4,0xD4,0xD4,
0xD4,0xD5,0xD5,0xD5,0xD5,0xD5,0xD5,0xD6,0xD6,0xD6,0xD6,0xD6,0xD6,0xD7,0xD7,0xD7,
0xD7,0xD7,0xD7,0xD8,0xD8,0xD8,0xD8,0xD8,0xD8,0xD9,0xD9,0xD9,0xD9,0xD9,0xD9,0xDA,
0xDA,0xDA,0xDA,0xDA,0xDA,0xDA,0xDB,0xDB,0xDB,0xDB,0xDB,0xDB,0xDC,0xDC,0xDC,0xDC,
0xDC,0xDC,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDE,0xDE,0xDE,0xDE,0xDE,0xDE,0xDE,0xDF,
0xDF,0xDF,0xDF,0xDF,0xDF,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE1,0xE1,0xE1,0xE1,
0xE1,0xE1,0xE2,0xE2,0xE2,0xE2,0xE2,0xE2,0xE2,0xE3,0xE3,0xE3,0xE3,0xE3,0xE3,0xE4,
0xE4,0xE4,0xE4,0xE4,0xE4,0xE4,0xE5,0xE5,0xE5,0xE5,0xE5,0xE5,0xE6,0xE6,0xE6,0xE6,
0xE6,0xE6,0xE6,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE8,0xE8,0xE8,0xE8,0xE8,0xE8,
0xE8,0xE9,0xE9,0xE9,0xE9,0xE9,0xE9,0xEA,0xEA,0xEA,0xEA,0xEA,0xEA,0xEA,0xEB,0xEB,
0xEB,0xEB,0xEB,0xEB,0xEB,0xEC,0xEC,0xEC,0xEC,0xEC,0xEC,0xEC,0xED,0xED,0xED,0xED,
0xED,0xED,0xED,0xED,0xED,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEF,0xEF,0xEF,
0xEF,0xEF,0xEF,0xEF,0xEF,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF1,
0xF1,0xF1,0xF1,0xF1,0xF1,0xF1,0xF1,0xF1,0xF1,0xF2,0xF2,0xF2,0xF2,0xF2,0xF2,0xF2,
0xF2,0xF2,0xF2,0xF2,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,
0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,0xF4,0xF5,0xF5,
0xF5,0xF5,0xF5,0xF5,0xF5,0xF5,0xF5,0xF5,0xF5,0xF5,0xF5,0xF6,0xF6,0xF6,0xF6,0xF6,
0xF6,0xF6,0xF6,0xF6,0xF6,0xF6,0xF6,0xF6,0xF6,0xF7,0xF7,0xF7,0xF7,0xF7,0xF7,0xF7,
0xF7,0xF7,0xF7,0xF7,0xF7,0xF7,0xF7,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,
0xF8,0xF8,0xF8,0xF8,0xF9,0xF9,0xF9,0xF9,0xF9,0xF9,0xF9,0xF9,0xF9,0xF9,0xF9,0xF9,
0xF9,0xF9,0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,
0xFB,0xFB,0xFB,0xFB,0xFB,0xFB,0xFB,0xFB,0xFB,0xFB,0xFB,0xFB,0xFB,0xFB,0xFC,0xFC,
0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFD,0xFD,0xFD,0xFD,
0xFD,0xFD,0xFD,0xFD,0xFD,0xFD,0xFD,0xFD,0xFD,0xFD,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,
0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                  	
};                            
*/
UINT32 LENS_TBL[256] = {
0x80,0x80,0x81,0x81,0x82,0x82,0x83,0x83,0x84,0x84,0x85,0x85,0x86,0x86,0x87,0x87,
0x88,0x88,0x89,0x89,0x8A,0x8A,0x8B,0x8B,0x8C,0x8C,0x8D,0x8D,0x8E,0x8E,0x8F,0x8F,
0x90,0x90,0x91,0x91,0x92,0x92,0x93,0x93,0x94,0x94,0x95,0x95,0x96,0x96,0x97,0x97,
0x98,0x98,0x99,0x99,0x9A,0x9A,0x9B,0x9B,0x9C,0x9C,0x9D,0x9D,0x9E,0x9E,0x9F,0x9F,
0xA0,0xA0,0xA1,0xA1,0xA2,0xA2,0xA3,0xA3,0xA4,0xA4,0xA5,0xA5,0xA6,0xA6,0xA7,0xA7,
0xA8,0xA8,0xA9,0xA9,0xAA,0xAA,0xAB,0xAB,0xAC,0xAC,0xAD,0xAD,0xAE,0xAE,0xAF,0xAF,
0xB0,0xB0,0xB1,0xB1,0xB2,0xB2,0xB3,0xB3,0xB4,0xB4,0xB5,0xB5,0xB6,0xB6,0xB7,0xB7,
0xB8,0xB8,0xB9,0xB9,0xBA,0xBA,0xBB,0xBB,0xBC,0xBC,0xBD,0xBD,0xBE,0xBE,0xBF,0xBF,
0xC0,0xC0,0xC1,0xC1,0xC2,0xC2,0xC3,0xC3,0xC4,0xC4,0xC5,0xC5,0xC6,0xC6,0xC7,0xC7,
0xC8,0xC8,0xC9,0xC9,0xCA,0xCA,0xCB,0xCB,0xCC,0xCC,0xCD,0xCD,0xCE,0xCE,0xCF,0xCF,
0xD0,0xD0,0xD1,0xD1,0xD2,0xD2,0xD3,0xD3,0xD4,0xD4,0xD5,0xD5,0xD6,0xD6,0xD7,0xD7,
0xD8,0xD8,0xD9,0xD9,0xDA,0xDA,0xDB,0xDB,0xDC,0xDC,0xDD,0xDD,0xDE,0xDE,0xDF,0xDF,
0xE0,0xE0,0xE1,0xE1,0xE2,0xE2,0xE3,0xE3,0xE4,0xE4,0xE5,0xE5,0xE6,0xE6,0xE7,0xE7,
0xE8,0xE8,0xE9,0xE9,0xEA,0xEA,0xEB,0xEB,0xEC,0xEC,0xED,0xED,0xEE,0xEE,0xEF,0xEF,
0xF0,0xF0,0xF1,0xF1,0xF2,0xF2,0xF3,0xF3,0xF4,0xF4,0xF5,0xF5,0xF6,0xF6,0xF7,0xF7,
0xF8,0xF8,0xF9,0xF9,0xFA,0xFA,0xFB,0xFB,0xFC,0xFC,0xFD,0xFD,0xFE,0xFE,0xFF,0xFF
};

/**************************************************************************************
*                                                                                     *
*  Function Name : hwCdspInit                                                         *
*                                                                                     *
*  Purposes      :                                                                    *
*    To Initialize cdsp Setting                                                       *
*                                                                                     *
*  Desprictions  :                                                                    *
*    Initialize cdsp                                                                  *
*                                                                                     *
*  Arguments     :                                                                    *
*    cammode : in, the camera operation mode                                          *
*                  0: idle mode                                                       *
*                  1: preview mode                                                    *
*                  2: still camera mode                                               *
*                  3: video clip mode                                                 *
*                  4: PC-camera mode                                                  *
*                  5: playback mode                                                   *
*                                                                                     *
*  Returns       :                                                                    *
*    Error message                                                                    *
*                                                                                     *
*  See also      :                                                                    *
*                                                                                     *
***************************************************************************************/
UINT32 
hwCdspInit(
	UINT32 cammode
)
{
    UINT32 vscaleen/*, tmp0*/;
    cdspReg0_t *pcdspReg0;   
	UINT32 sensorWidth, sensorHeight;
    UINT8 major, minor;
    UINT32 type;
    
    pcdspReg0 = (cdspReg0_t *)(0xb0000000);
	
    dprint("CDSP enter hwCdspInit \n");
    
    if (cammode != 0) {
        return SUCCESS;
    }	    
    
    if ( READ8(0xB00000FF) == 0x05 ) { /*If 536B, please use hwCdspNewAFWinCfg to configure AF window*/
    	hwCdspAFWinCfgExt(0x40,0xE0,0x40,0x40,0x0);
    }
    
    hwCdspBadPixelSet(1, 240, 240, 240);
    
    hwCdspManuOBSet(1, 12); /* Manual OB was not enable automatically in       */
                           /* hwfrontSnap(), customer should enable manual    */
                           /* OB and give OB value here. Customers can also   */
                           /* control the flow themselves in sanp flow        */
                           
                           
    /*hwCdspWbOffsetSet(0xff, 0x06, 0xfa, 0x0);*/
       hwCdspWbOffsetSet(0xff, 0x00, 0xfc, 0x0);
    
    /* hwCdspLenscmpInit(LENS_TBL); */
    hwFrontSensorSizeGet(&sensorWidth, &sensorHeight);
    hwCdspLenscmpCfg(0, sensorWidth/2, sensorHeight/2, 6, 0, 2, 0, 2, 2);

    hwCdspWbGainSet(0x55, 0x40, 0x53, 0x40);
    hwCdspGlobalGainSet(0x20);
    
    #ifdef CDSP_FILTER_9X9
    hwCdspIntplModeSet(1);  /* 1: 9X9 */        /* Fine mode */  
    #else
    hwCdspIntplModeSet(0);  /* 0: 5X5 */
    #endif

    hwCdspFineIntplCfg(0, 0x14, 0x320, 0x50);
    /*hwCdspColorMSet(0x60, 0x1F0, 0x1F0, 0x1F0, 0x60, 0x1F0, 0x1F0, 0x1F0, 0x60);*/
    /*hwCdspColorMSet(0x86, 0x1AC, 0x0E, 0x1F2, 0x33, 0x1C, 0x1F3, 0x1B0, 0x9D);*/
	/*hwCdspColorMSet(0x61, 0x1e5, 0x1fa, 0x1e9, 0x65, 0x1f2, 0x1F8, 0x1d3, 0x75);*/
	hwCdspColorMSet(0x67, 0x1e0, 0x1f9, 0x1eb, 0x62, 0x1f3, 0x1F6, 0x1cb, 0x7f);
    sysgChipVerGet(&major, &minor);
    if (major == 1) /* if version B, turn off gamma */
    	hwCdspGammaSet(0);
    else
    	hwCdspGammaSet(1);
   	
    hwCdspGammaInit(GAMMA_TBL);
    hwCdspRBClampSet(1, 0xFA);
    hwCdspUVClampSet(1, 0x14, 0x0A, 0x05);
    
    vscaleen = (pcdspReg0->vScaleCtr & 0x10); /* Check Vscale enable */ 
    
    vscaleen = 1;
    
    if (vscaleen == 0) {        
        hwCdspEdgelModeSet(1);                  /* Fine mode */        
        hwCdspEdgeFilterSet(EDGE_FINE_F00, EDGE_FINE_F01, EDGE_FINE_F02, EDGE_FINE_F03, EDGE_FINE_F04, 
                            EDGE_FINE_F10, EDGE_FINE_F11, EDGE_FINE_F12, EDGE_FINE_F13, EDGE_FINE_F14,
                            EDGE_FINE_F20, EDGE_FINE_F21, EDGE_FINE_F22, EDGE_FINE_F23, EDGE_FINE_F24,
                            EDGE_FINE_F30, EDGE_FINE_F31, EDGE_FINE_F32, EDGE_FINE_F33, EDGE_FINE_F34,
                            EDGE_FINE_F40, EDGE_FINE_F41, EDGE_FINE_F42, EDGE_FINE_F43, EDGE_FINE_F44);
    }
    else {
        hwCdspEdgelModeSet(0);                  /* Fast mode */
        hwCdspLEdgeFilterSet(9,0,9, 0,4,0, 9,0,9);
        hwCdspLEdgeCfg(0x20, 0x40, 0x04, 0x02, 0x02);
        hwCdspLCoringSet(0x04);         
        hwCdspEdgeFilterSet(EDGE_FAST_F00, EDGE_FAST_F01, EDGE_FAST_F02, EDGE_FAST_F03, EDGE_FAST_F04,
                            EDGE_FINE_F10, EDGE_FINE_F11, EDGE_FINE_F12, EDGE_FINE_F13, EDGE_FINE_F14,
                            EDGE_FINE_F20, EDGE_FINE_F21, EDGE_FINE_F22, EDGE_FINE_F23, EDGE_FINE_F24,
                            EDGE_FINE_F30, EDGE_FINE_F31, EDGE_FINE_F32, EDGE_FINE_F33, EDGE_FINE_F34,
                            EDGE_FINE_F40, EDGE_FINE_F41, EDGE_FINE_F42, EDGE_FINE_F43, EDGE_FINE_F44);
    } 
    
    hwCdspYEdgeCfg(1, 0x40, 0x60, 0x00 , 0x05, 0x04);  
/*hwCdspYEdgeCfg(1, 0x40, 0x60, 0x05 , 0x06, 0x05);*/
    hwCdspYUVCoringSet(0x02, 0x02, 0x02); 
    hwCdspSupTableSet(0x10,0x10,0x10,0x0F,0x0F,0x0F,0x0F,0x0E,0x0E,0x0E,0x0E,0x0E,
                      0x0D,0x0D,0x0D,0x0D);
    hwCdspSuppressionCfg(1, 0x20, 0);
    hwCdspYUVAvgSet(0, 1, 1);
    hwCdspBriContSet(0, 0x14, 0x28);
    hwCdspSatHueSet(0, 0x20, 0x00, 0x40);
    hwCdspRGBWinClamp(0, 0x50, 0x3E8); 
    hwCdspSPWinCfg(YTHR0, YTHR1, YTHR2, YTHR3, YTHR4, U_L1N1, U_L1P1, V_L1N1, V_L1P1, U_L2N1, U_L2P1, V_L2N1, V_L2P1,
    			U_L1N2, U_L1P2, V_L1N2, V_L1P2, U_L2N2, U_L2P2, V_L2N2, V_L2P2, U_L1N3, U_L1P3, V_L1N3, V_L1P3,
    			U_L2N3, U_L2P3, V_L2N3, V_L2P3, U_L1N4, U_L1P4, V_L1N4, V_L1P4, U_L2N4, U_L2P4, V_L2N4, V_L2P4,
    			SINDATA, COSDATA, WEIGHT1, WEIGHT2); 
    hwCdspHisSet(1, 0);
    hwCdspWinHold(0, 0x11);
    
	hwCdspZFactorSet(100, 100);                    /* Zoom 1x */     
    
    hwFrontSensorTypeGet(&type);

    if ( (type == SENSOR_CMOS_OV9620_TYPE) || (type == SENSOR_CMOS_OV7648_TYPE)) {
    	hwCdspPixelSel(0x1);
    }
    else {
    	    hwCdspPixelSel(0x3);     
    }  
    hwCdspProbeSet(1, 20, 2);
    
    dprint("CDSP exit hwCdspInit \n");  
	
    return SUCCESS;             
}


