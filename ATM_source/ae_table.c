/********************************** Still mode ************************************/
#include <stdio.h>
#include <string.h>
#include "general.h"
#include "hw_cdsp.h"
#include "aaa_api.h"
#include "hw_front.h"
#include "device.h"
#include "aaa.h"
#include "exif_api.h"
#include "ui_main.h"
#include "os_api.h"
#include "xiaoao.h"
#include "aaa_api.h"
#if 0
AEstr S_LVtbl50[120]={/* for preview(Default) */
#if 0
{0,98,95,0},{0,98,93,0},{0,98,91,0},{0,98,89,0},{0,98,87,0},
{0,98,86,0},{0,98,84,0},{0,98,83,0},{0,99,82,0},{0,100,81,0},
{0,98,31,0},{0,98,29,0},{0,98,27,0},{0,98,25,0},{0,98,23,0},
{0,98,22,0}, {0,98,20,0},
/*{0,200,8,0},*/
{0,98,19,0},{0,99,18,0},{0,100,17,0},
/*70*/
{0,101,16,0},{0,108,16,0},{0,109,15,0},{0,109,14,0},{0,125,15,0},
{0,125,14,0},{0,144,15,0},{0,144,14,0},{0,165,15,0},{0,165,14,0},
#endif
{0,98,95,0},{0,98,93,0},{0,98,91,0},{0,98,89,0},{0,98,87,0},
{0,98,86,0},{0,98,84,0},{0,98,83,0},{0,99,82,0},{0,100,81,0},
{0,83,14,0},{0,83,13,0},{0,83,12,0},{0,83,11,0},{0,83,10,0},
{0,83,9,0}, {0,83,8,0},{0,90,13,0},{0,90,12,0},{0,90,11,0},
/*70*/
{0,101,16,0},{0,108,16,0},{0,109,15,0},{0,109,14,0},{0,125,15,0},
{0,125,14,0},{0,144,15,0},{0,144,14,0},{0,165,15,0},{0,165,14,0},
{0,203,16,0},{0,245,18,0},{0,248,17,0},{0,250,16,0},{0,251,15,0},
{0,251,14,0},{0,327,17,0},{0,330,16,0},{0,331,15,0},{0,331,14,0},
{0,507,20,0},{0,492,18,0},{0,497,17,0},{0,502,16,0},{0,502,15,0},
{0,505,14,0},{0,1005,26,0},{0,995,24,0},{0,985,22,0},{0,995,21,0},
{0,1015,20,0},{0,985,18,0},{0,995,17,0},{0,1005,16,0},{0,1005,15,0},
{0,1015,14,0},{0,1082,14,0},{0,1159,14,0},{0,1247,14,0},{0,1331,14,0},
{0,1427,14,0},{0,1539,14,0},{0,1642,14,0},{0,1759,14,0},{0,1894,14,0},
{0,2189,15,0},{0,2189,14,0},{0,2345,14,0},{0,2662,15,0},{0,2662,14,0},
{0,2897,14,0},{0,3078,14,0},{0,3284,14,0},{0,3518,14,0},{0,3789,14,0},
{0,4926,17,0},{0,4691,15,0},{0,4691,14,0},{0,8210,23,0},{0,5795,15,0},
{0,5795,14,0},{0,6157,14,0},{0,6568,14,0},{0,7037,14,0},{0,7578,14,0},
{0,9852,17,0},{0,9852,16,0},{0,14074,21,0},{0,16420,23,0},{0,12315,16,0},
{0,12315,15,0},{0,12315,14,0},{0,14074,15,0},{0,14074,14,0},{0,19704,18,0},
{0,19704,17,0},{0,19704,16,0},{0,32840,25,0},{0,32840,23,0},{0,24630,16,0},
{0,24630,15,0},{0,24630,14,0},{0,49261,26,0},{0,32840,16,0},{0,32840,15,0},
{0,49261,21,0},{0,49261,20,0},{0,49261,18,0},{0,49261,17,0},{0,49261,16,0},
{0,49261,15,0},{0,49261,14,0},{0,98522,26,0},{0,98522,24,0},{0,98522,23,0},
{0,98522,21,0},{0,98522,20,0},{0,98522,18,0},{0,98522,17,0},{0,98522,16,0}/*,
{0,98522,15,0},{0,98522,14,0}*/
};


/*  Output_AETable_60_for_53x  */
AEstr S_LVtbl60[120]={/* for Preview(Default) */
{0,98,95,0},{0,98,93,0},{0,98,91,0},{0,98,89,0},{0,98,87,0},
{0,98,86,0},{0,98,84,0},{0,98,83,0},{0,99,82,0},{0,100,81,0},
{0,98,31,0},{0,98,29,0},{0,98,27,0},{0,101,26,0},{0,108,26,0},
{0,117,26,0},
{0,149,31,0},
/*{0,200,8,0},*/
{0,147,28,0},{0,149,27,0},{0,148,25,0},
/*70*/
{0,197,31,0},{0,197,29,0},{0,197,27,0},{0,235,30,0},{0,235,28,0},
{0,235,26,0},{0,298,31,0},{0,299,29,0},{0,298,27,0},{0,296,25,0},
{0,294,23,0},{0,299,22,0},{0,294,20,0},{0,297,19,0},{0,302,18,0},
{0,305,17,0},{0,392,20,0},{0,392,19,0},{0,398,18,0},{0,403,17,0},
{0,407,16,0},{0,600,22,0},{0,589,20,0},{0,597,19,0},{0,604,18,0},
{0,611,17,0},{0,1201,31,0},{0,1201,29,0},{0,1201,27,0},{0,1187,25,0},
{0,1187,23,0},{0,1201,22,0},{0,1187,20,0},{0,1201,19,0},{0,1216,18,0},
{0,1231,17,0},{0,1247,16,0},{0,1247,15,0},{0,1247,14,0},{0,1331,14,0},
{0,1427,14,0},{0,1539,14,0},{0,1642,14,0},{0,1759,14,0},{0,1894,14,0},
{0,2189,15,0},{0,2189,14,0},{0,2345,14,0},{0,2662,15,0},{0,2662,14,0},
{0,2897,14,0},{0,3078,14,0},{0,3284,14,0},{0,3518,14,0},{0,3789,14,0},
{0,4926,17,0},{0,4691,15,0},{0,4691,14,0},{0,8210,23,0},{0,5795,15,0},
{0,5795,14,0},{0,6157,14,0},{0,6568,14,0},{0,7037,14,0},{0,7578,14,0},
{0,9852,17,0},{0,9852,16,0},{0,14074,21,0},{0,16420,23,0},{0,12315,16,0},
{0,12315,15,0},{0,12315,14,0},{0,14074,15,0},{0,14074,14,0},{0,19704,18,0},
{0,19704,17,0},{0,19704,16,0},{0,32840,25,0},{0,32840,23,0},{0,24630,16,0},
{0,24630,15,0},{0,24630,14,0},{0,49261,26,0},{0,32840,16,0},{0,32840,15,0},
{0,49261,21,0},{0,49261,20,0},{0,49261,18,0},{0,49261,17,0},{0,49261,16,0},
{0,49261,15,0},{0,49261,14,0},{0,98522,26,0},{0,98522,24,0},{0,98522,23,0},
{0,98522,21,0},{0,98522,20,0},{0,98522,18,0},{0,98522,17,0},{0,98522,16,0}/*,
{0,98522,15,0},{0,98522,14,0}*/
};
#endif
#if 1
AEstr S_LVtbl50[120]={/* Jane1 */
{0,83,32,0},{1,83,30,0},{2,83,28,0},{3,83,26,0},{4,84,25,0},
{5,83,23,0},{6,90,23,0},{7,83,20,0},{8,84,19,0},{9,90,19,0},
{10,83,16,0},{11,83,15,0},{12,83,14,0},{13,83,13,0},{14,101,15,0},
{15,101,14,0},{16,101,13,0},{17,83,10,0},{18,90,10,0},{19,125,13,0},

{20,83,8,0},{21,100,9,0},{22,142,12,0},{23,100,8,0},{24,202,15,0},
{25,144,10,0},{26,125,8,0},{27,166,10,0},{28,142,8,0},{29,250,13,0},
{30,166,8,0},{31,200,9,0},{32,333,14,0},{33,201,8,0},{34,250,9,0},
{35,500,17,0},{36,250,8,0},{37,333,10,0},{38,500,14,0},{39,500,13,0},
{40,333,8,0},{41,1015,23,0},{42,1003,21,0},{43,504,10,0},{44,500,9,0},
{45,1003,17,0},{46,500,8,0},{47,1003,15,0},{48,1003,14,0},{49,1003,13,0},
{50,1003,12,0},{51,1003,11,0},{52,1015,11,0},{53,1009,10,0},{54,1003,9,0},
{55,1015,9,0},{56,1003,8,0},{57,1066,8,0},{58,1146,8,0},{59,1229,8,0},
{60,1314,8,0},{61,1413,8,0},{62,1514,8,0},{63,1615,8,0},{64,1748,8,0},
{65,1863,8,0},{66,1995,8,0},{67,2146,8,0},{68,2292,8,0},{69,2458,8,0},
{70,2650,8,0},{71,2826,8,0},{72,3028,8,0},{73,3261,8,0},{74,3944,9,0},
{75,3769,8,0},{76,4038,8,0},{77,4846,9,0},{78,4584,8,0},{79,4988,8,0},
{80,5300,8,0},{81,5653,8,0},{82,6057,8,0},{83,6523,8,0},{84,11307,13,0},
{85,8480,9,0},{86,8076,8,0},{87,13047,12,0},{88,24230,21,0},{89,9977,8,0},
{90,10600,8,0},{91,11307,8,0},{92,12115,8,0},{93,13047,8,0},{94,24230,14,0},
{95,16961,9,0},{96,24230,12,0},{97,28268,13,0},{98,33922,15,0},{99,42402,17,0},
{100,21201,8,0},{101,28268,10,0},{102,24230,8,0},{103,42402,13,0},{104,28268,8,0},
{105,33922,9,0},{106,56537,14,0},{107,56537,13,0},{108,42402,9,0},{109,84805,17,0},
{110,42402,8,0},{111,56537,10,0},{112,84805,14,0},{113,84805,13,0},{114,56537,8,0},
{115,169611,23,0},{116,169611,21,0},{117,84805,10,0},{118,84805,9,0},{119,169611,17,0}
};
#endif
#if 0
AEstr S_LVtbl50[120]={/* Sun */
{0,83,32,0},{1,83,30,0},{2,83,28,0},{3,83,26,0},{4,84,25,0},
{5,83,23,0},{6,90,23,0},{7,83,20,0},{8,84,19,0},{9,90,19,0},

{10,83,14,0},{11,83,13,0},{12,83,12,0},{13,83,11,0},{14,83,10,0},
{15,83,9,0}, {16,83,8,0},{17,98,12,0},{18,98,11,0},{19,98,10,0},

{20,90,12,0},{21,90,11,0},{22,100,11,0},{23,100,10,0},{24,125,11,0},
{25,142,12,0},{26,142,8,0},{27,144,10,0},{28,166,11,0},{29,166,9,0},

{30,200,9,0},{31,201,8,0},{32,202,10,0},{33,250,11,0},{34,250,9,0},
{35,250,8,0},{36,333,11,0},{37,333,10,0},{38,333,9,0},{39,500,12,0},

{40,500,11,0},{41,500,10,0},{42,500,9,0},{43,500,8,0},{44,504,10,0},
{45,1003,21,0},{46,1003,17,0},{47,1003,15,0},{48,1003,14,0},{49,1003,13,0},

{50,1003,12,0},{51,1003,11,0},{54,1003,9,0},{56,1003,8,0},{53,1009,10,0},
{52,1015,11,0},{55,1015,9,0},{57,1066,8,0},{58,1146,8,0},{59,1229,8,0},

{60,1314,8,0},{61,1413,8,0},{62,1514,8,0},{63,1615,8,0},{64,1748,8,0},
{65,1863,8,0},{66,1995,8,0},{67,2146,8,0},{68,2292,8,0},{69,2458,8,0},

{70,2650,8,0},{71,2826,8,0},{72,3028,8,0},{73,3261,8,0},{74,3944,9,0},
{75,3769,8,0},{76,4038,8,0},{77,4846,9,0},{78,4584,8,0},{79,4988,8,0},

{80,5300,8,0},{81,5653,8,0},{82,6057,8,0},{83,6523,8,0},{84,8480,9,0},
{85,8076,8,0},{86,9977,8,0},{87,10600,8,0},{88,11307,11,0},{89,11307,8,0},

{90,12115,8,0},{91,13047,11,0},{92,13047,8,0},{93,16961,9,0},{94,21201,8,0},
{95,24230,14,0},{96,24230,13,0},{97,24230,12,0},{98,28268,13,0},{99,24230,8,0},

{100,28268,10,0},{101,28268,8,0},{102,33922,13,0},{103,33922,9,0},{104,42402,14,0},
{105,42402,13,0},{106,42402,9,0},{107,42402,8,0},{108,56537,14,0},{109,56537,13,0},

{110,56537,10,0},{111,56537,8,0},{112,84805,13,0},{113,84805,12,0},{114,84805,11,0},
{115,84805,10,0},{116,84805,9,0},{117,169611,16,0},{118,169611,14,0},{119,169611,12,0}
};
#endif
#if 0
AEstr S_LVtbl50[120]={	/* Jane2 */
{0,83,32,0},{1,83,30,0},{2,83,28,0},{3,83,26,0},{4,84,25,0},
{5,83,23,0},{6,90,23,0},{7,83,20,0},{8,84,19,0},{9,90,19,0},
{10,83,16,0},{11,83,15,0},{12,83,14,0},{13,83,13,0},{14,101,15,0},
{15,101,14,0},{16,101,13,0},{17,83,10,0},{18,90,10,0},{19,125,13,0},
{20,83,8,0},{21,100,9,0},{22,166,14,0},{23,166,13,0},{24,200,15,0},
{25,144,10,0},{26,125,8,0},{27,166,10,0},{28,142,8,0},{29,250,13,0},
{30,166,8,0},{31,200,9,0},{32,333,14,0},{33,333,13,0},{34,333,12,0},
{35,500,17,0},{36,250,8,0},{37,333,10,0},{38,500,14,0},{39,500,13,0},
{40,997,24,0},{41,1003,23,0},{42,1003,21,0},{43,504,10,0},{44,500,9,0},
{45,1003,17,0},{46,500,8,0},{47,1003,15,0},{48,1003,14,0},{49,1003,13,0},
{50,1003,12,0},{51,1003,11,0},{52,1015,11,0},{53,1009,10,0},{54,1003,9,0},
{55,1015,9,0},{56,1003,8,0},{57,1066,8,0},{58,1146,8,0},{59,1229,8,0},
{60,1314,8,0},{61,1413,8,0},{62,1514,8,0},{63,1615,8,0},{64,1748,8,0},
{65,1863,8,0},{66,1995,8,0},{67,2146,8,0},{68,2292,8,0},{69,2458,8,0},
{70,2650,8,0},{71,2826,8,0},{72,3028,8,0},{73,3261,8,0},{74,3944,9,0},
{75,3769,8,0},{76,4038,8,0},{77,4846,9,0},{78,4584,8,0},{79,4988,8,0},
{80,5300,8,0},{81,5653,8,0},{82,6057,8,0},{83,6523,8,0},{84,11307,13,0},
{85,8480,9,0},{86,8076,8,0},{87,13047,12,0},{88,24230,21,0},{89,9977,8,0},
{90,10600,8,0},{91,11307,8,0},{92,12115,8,0},{93,13047,8,0},{94,24230,14,0},
{95,16961,9,0},{96,24230,12,0},{97,28268,13,0},{98,33922,15,0},{99,42402,17,0},
{100,21201,8,0},{101,28268,10,0},{102,24230,8,0},{103,42402,13,0},{104,28268,8,0},
{105,33922,9,0},{106,56537,14,0},{107,56537,13,0},{108,42402,9,0},{109,84805,17,0},
{110,42402,8,0},{111,56537,10,0},{112,84805,14,0},{113,84805,13,0},{114,56537,8,0},
{115,169611,23,0},{116,169611,21,0},{117,84805,10,0},{118,84805,9,0},{119,169611,17,0}
};
#endif

AEstr S_LVtbl60[120]={
{0,85,31,0},{1,85,29,0},{2,85,27,0},{3,85,25,0},{4,86,24,0},
{5,85,22,0},{6,87,21,0},{7,85,19,0},{8,86,18,0},{9,87,17,0},
{10,93,17,0},{11,100,17,0},{12,100,16,0},{13,101,15,0},{14,86,12,0},
{15,85,11,0},{16,92,11,0},{17,100,11,0},{18,86,9,0},{19,92,9,0},
{20,100,9,0},{21,120,10,0},{22,100,8,0},{23,109,8,0},{24,173,12,0},
{25,171,11,0},{26,133,8,0},{27,200,11,0},{28,172,9,0},{29,601,29,0},
{30,200,9,0},{31,400,17,0},{32,201,8,0},{33,243,9,0},{34,406,14,0},
{35,404,13,0},{36,300,9,0},{37,400,11,0},{38,1202,31,0},{39,1202,29,0},
{40,400,9,0},{41,1202,25,0},{42,403,8,0},{43,601,11,0},{44,1220,21,0},
{45,1202,19,0},{46,601,9,0},{47,1220,17,0},{48,610,8,0},{49,1220,15,0},
{50,1220,14,0},{51,1220,13,0},{52,1211,12,0},{53,1202,11,0},{54,1202,10,0},
{55,1220,10,0},{56,1202,9,0},{57,1284,9,0},{58,1229,8,0},{59,1314,8,0},
{60,1413,8,0},{61,1514,8,0},{62,1615,8,0},{63,1748,8,0},{64,1863,8,0},
{65,1995,8,0},{66,2146,8,0},{67,2292,8,0},{68,2458,8,0},{69,2650,8,0},
{70,2826,8,0},{71,3028,8,0},{72,3261,8,0},{73,3944,9,0},{74,3769,8,0},
{75,4038,8,0},{76,4846,9,0},{77,4584,8,0},{78,4988,8,0},{79,5300,8,0},
{80,5653,8,0},{81,6057,8,0},{82,6523,8,0},{83,11307,13,0},{84,8480,9,0},
{85,8076,8,0},{86,13047,12,0},{87,24230,21,0},{88,9977,8,0},{89,10600,8,0},
{90,11307,8,0},{91,12115,8,0},{92,13047,8,0},{93,24230,14,0},{94,16961,9,0},
{95,24230,12,0},{96,28268,13,0},{97,33922,15,0},{98,42402,17,0},{99,21201,8,0},
{100,28268,10,0},{101,24230,8,0},{102,42402,13,0},{103,28268,8,0},{104,33922,9,0},
{105,56537,14,0},{106,56537,13,0},{107,42402,9,0},{108,84805,17,0},{109,42402,8,0},
{110,56537,10,0},{111,84805,14,0},{112,84805,13,0},{113,56537,8,0},{114,169611,23,0},
{115,169611,21,0},{116,84805,10,0},{117,84805,9,0},{118,169611,17,0},{119,84805,8,0}
};
/*************************************************************************************/

/********************************** Movie mode ************************************/
/*AEstr M_LVtbl60[121] = { 
	{50,99,0x66}, {51,99,0x65}, {52,119,0x67}, {53,119,0x66}, 
	{54,119,0x65}, {55,109,0x63}, {56,149,0x66}, {57,171,0x67},
	{58,99,0x60}, {59,99,0x3d}, {60,109,0x3e}, {61,109,0x3a},
	{62,109,0x36}, {63,119,0x37}, {64,133,0x39}, {65,149,0x3c},
	{66,149,0x38}, {67,171,0x3c}, {68,171,0x38}, {69,199,0x3d},
	{70,199,0x39}, {71,199,0x35}, {72,199,0x31}, {73,199,0x2e}, 
	{74,199,0x2b}, {75,199,0x28}, {76,199,0x25}, {77,199,0x23},
	{78,199,0x21}, {79,199,0x1e}, {80,199,0x1c}, {81,199,0x1a},
	{82,199,0x19}, {83,199,0x17}, {84,199,0x15}, {85,199,0x14},
	{86,199,0x13}, {87,239,0x15}, {88,239,0x14}, {89,239,0x12},
   	{90,239,0x11}, {91,239,0x10}, {92,239,0x0f}, {93,239,0x0e},
    	{94,239,0x0d}, {95,239,0x0c}, {96,299,0x0e}, {97,299,0x0d},
    	{98,399,0x10}, {99,399,0x0f}, {100,399,0x0e}, {101,599,0x14},
    	{102,599,0x13}, {103,1201,0x23}, {104,1201,0x20}, {105,1201,0x1e},
    	{106,1201,0x1c}, {107,1201,0x1a}, {108,1201,0x19}, {109,1201,0x17},
	{110,1201,0x15}, {111,1201,0x14}, {112,1223,0x13}, {113,1520,0x16},
	{114,1475,0x14}, {115,1672,0x15}, {116,1618,0x13}, {117,2006,0x16},
	{118,1858,0x13}, {119,2090,0x14}, {120,2134,0x13}, {121,2047,0x11},
	{122,2333,0x12}, {123,2229,0x10}, {124,2388,0x10}, {125,2388,0x0f},
	{126,2388,0x0e}, {127,2388,0x0d}, {128,2333,0x0c}, {129,2951,0x0e},
	{130,3135,0x0e}, {131,3135,0x0d}, {132,3344,0x0d}, {133,3583,0x0d},
	{134,3859,0x0d}, {135,4777,0x0f}, {136,4777,0x0e}, {137,4013,0x0b},
	{138,6270,0x10},  {139,4180,0x0a}, {140,6270,0x0e}, {141,6270,0x0d},
	{142,6688,0x0d}, {143,5016,0x09}, {144,7718,0x0d}, {145,7718,0x0c},
	{146,14333,0x15}, {147,16722,0x17}, {148,12541,0x10}, {149,12541,0x0f},
	{150,12541,0x0e}, {151,12541,0x0d}, {152,12541,0x0c}, {153,14333,0x0d},
	{154,14333,0x0c}, {155,16722,0x0d}, {156,20066,0x0f}, {157,25083,0x11},
	{158,25083,0x10}, {159,25083,0x0f}, {160,25083,0x0e}, {161,25083,0x0d},
	{162,25083,0x0c}, {163,20066,0x09}, {164,33444,0x0e}, {165,33444,0x0d},
	{166,100334,0x25}, {167,50167,0x11}, {168,50167,0x10}, {169,50167,0x0f},
	{170,50167,0x0e}
};

AEstr M_LVtbl50[121] = {
	{50,99,0x66}, {51,99,0x65}, {52,111,0x66}, {53,111,0x65}, 
	{54,111,0x64}, {55,99,0x62}, {56,124,0x64}, {57,124,0x63},
	{58,111,0x61}, {59,99,0x3d}, {60,99,0x39}, {61,111,0x3b},
	{62,111,0x37}, {63,111,0x33}, {64,111,0x30}, {65,111,0x2d},

	{66,124,0x2f}, {67,124,0x2c}, {68,124,0x29}, {69,124,0x26},

	{70,245,0x39}, {71,199,0x35}, {72,199,0x32}, {73,199,0x2e}, 
	{74,199,0x2b}, {75,199,0x28}, {76,249,0x2f}, {77,249,0x2c},
	{78,249,0x29}, {79,249,0x26}, {80,249,0x23}, {81,249,0x21},
	{82,249,0x1f}, {83,249,0x1d}, {84,249,0x1b}, {85,249,0x19},
	{86,499,0x2f}, {87,499,0x2c}, {88,499,0x29}, {89,499,0x26},
   	{90,499,0x23}, {91,499,0x21}, {92,499,0x1f}, {93,499,0x1d},
    	{94,499,0x1b}, {95,499,0x19}, {96,499,0x17}, {97,499,0x16},
    	{98,499,0x14}, {99,499,0x13}, {100,499,0x12}, {101,998,0x21},
    	{102,998,0x1f}, {103,998,0x1d}, {104,998,0x1b}, {105,998,0x19},
    	{106,998,0x17}, {107,998,0x16}, {108,1023,0x15}, {109,998,0x13},
	{110,998,0x12}, {111,1023,0x11}, {112,1090,0x11}, {113,1045,0x0f},
	{114,1114,0x0f}, {115,1114,0x0e}, {116,1194,0x0e}, {117,1194,0x0d},
	{118,1166,0x0c}, {119,1254,0x0c}, {120,1355,0x0c}, {121,1320,0x0b},
	{122,1166,0x09}, {123,1520,0x0b}, {124,1475,0x0a}, {125,1433,0x09},
	{126,1700,0x0a}, {127,1824,0x0a}, {128,1760,0x09}, {129,1893,0x09},
	{130,1791,0x08}, {131,1929,0x08}, {132,2333,0x09}, {133,3040,0x0b},
	{134,3859,0x0d}, {135,4777,0x0f}, {136,4777,0x0e}, {137,4013,0x0b},
	{138,6270,0x10},  {139,4180,0x0a}, {140,6270,0x0e}, {141,6270,0x0d},
	{142,6688,0x0d}, {143,5016,0x09}, {144,7718,0x0d}, {145,7718,0x0c},
	{146,14333,0x15}, {147,16722,0x17}, {148,12541,0x10}, {149,12541,0x0f},
	{150,12541,0x0e}, {151,12541,0x0d}, {152,12541,0x0c}, {153,14333,0x0d},
	{154,14333,0x0c}, {155,16722,0x0d}, {156,20066,0x0f}, {157,25083,0x11},
	{158,25083,0x10}, {159,25083,0x0f}, {160,25083,0x0e}, {161,25083,0x0d},
	{162,25083,0x0c}, {163,20066,0x09}, {164,33444,0x0e}, {165,33444,0x0d},
	{166,100334,0x25}, {167,50167,0x11}, {168,50167,0x10}, {169,50167,0x0f},
	{170,50167,0x0e}
};*/
AEstr M_LVtbl50[120]={/* for preview(Default) */
{0,98,95,0},{1,98,93,0},{2,98,91,0},{3,98,89,0},{4,99,88,0},
{5,98,86,0},{6,99,85,0},{7,98,83,0},{8,98,82,0},{9,99,81,0},
{10,98,31,0},{11,98,29,0},{12,98,27,0},{13,100,26,0},{14,109,26,0},
{15,124,28,0},{16,123,26,0},{17,142,28,0},{18,163,30,0},{19,163,28,0},
{20,196,31,0},{21,196,29,0},{22,196,27,0},{23,247,32,0},{24,248,30,0},
{25,248,28,0},{26,247,26,0},{27,254,25,0},{28,327,30,0},{29,327,28,0},
{30,327,26,0},{31,327,24,0},{32,331,23,0},{33,327,21,0},{34,331,20,0},
{35,337,19,0},{36,327,17,0},{37,327,16,0},{38,327,15,0},{39,327,14,0},
{40,503,20,0},{41,491,18,0},{42,491,17,0},{43,494,16,0},{44,497,15,0},
{45,497,14,0},{46,995,26,0},{47,983,24,0},{48,1007,23,0},{49,983,21,0},
{50,1007,20,0},{51,983,18,0},{52,983,17,0},{53,995,16,0},{54,995,15,0},
{55,995,14,0},{56,1070,14,0},{57,1143,14,0},{58,1226,14,0},{59,1321,14,0},
{60,1410,14,0},{61,1510,14,0},{62,1626,14,0},{63,1880,15,0},{64,1880,14,0},
{65,2014,14,0},{66,2286,15,0},{67,2286,14,0},{68,2488,14,0},{69,2643,14,0},
{70,2820,14,0},{71,3021,14,0},{72,3253,14,0},{73,4230,17,0},{74,4028,15,0},
{75,4028,14,0},{76,7050,23,0},{77,4976,15,0},{78,4976,14,0},{79,5287,14,0},
{80,5640,14,0},{81,6043,14,0},{82,6507,14,0},{83,8460,17,0},{84,8460,16,0},
{85,12086,21,0},{86,14100,23,0},{87,10575,16,0},{88,10575,15,0},{89,10575,14,0},
{90,12086,15,0},{91,12086,14,0},{92,14100,15,0},{93,16920,17,0},{94,16920,16,0},
{95,16920,15,0},{96,28200,23,0},{97,21150,16,0},{98,21150,15,0},{99,21150,14,0},
{100,42301,26,0},{101,28200,16,0},{102,28200,15,0},{103,28200,14,0},{104,42301,20,0},
{105,42301,18,0},{106,42301,17,0},{107,42301,16,0},{108,42301,15,0},{109,42301,14,0},
{110,84602,26,0},{111,84602,24,0},{112,84602,23,0},{113,84602,21,0},{114,84602,20,0},
{115,84602,18,0},{116,84602,17,0},{117,84602,16,0},{118,84602,15,0},{119,84602,14,0}
};


/*  Output_AETable_60_for_53x  */
AEstr M_LVtbl60[120]={/* for Preview(Default) */
{0,98,95,0},{1,98,93,0},{2,98,91,0},{3,98,89,0},{4,99,88,0},
{5,98,86,0},{6,99,85,0},{7,98,83,0},{8,98,82,0},{9,99,81,0},
{10,98,31,0},{11,98,29,0},{12,98,27,0},{13,100,26,0},{14,107,26,0},
{15,119,27,0},{16,147,31,0},{17,147,29,0},{18,147,27,0},{19,147,25,0},
{20,196,31,0},{21,196,29,0},{22,196,27,0},{23,235,30,0},{24,235,28,0},
{25,239,27,0},{26,294,31,0},{27,295,29,0},{28,294,27,0},{29,294,25,0},
{30,301,24,0},{31,295,22,0},{32,302,21,0},{33,294,19,0},{34,297,18,0},
{35,302,17,0},{36,304,16,0},{37,305,15,0},{38,305,14,0},{39,399,17,0},
{40,400,16,0},{41,402,15,0},{42,402,14,0},{43,591,19,0},{44,595,18,0},
{45,604,17,0},{46,608,16,0},{47,1191,29,0},{48,1191,27,0},{49,1191,25,0},
{50,1208,24,0},{51,1191,22,0},{52,1208,21,0},{53,1191,19,0},{54,1191,18,0},
{55,1208,17,0},{56,1226,16,0},{57,1226,15,0},{58,1226,14,0},{59,1321,14,0},
{60,1410,14,0},{61,1510,14,0},{62,1626,14,0},{63,1880,15,0},{64,1880,14,0},
{65,2014,14,0},{66,2286,15,0},{67,2286,14,0},{68,2488,14,0},{69,2643,14,0},
{70,2820,14,0},{71,3021,14,0},{72,3253,14,0},{73,4230,17,0},{74,4028,15,0},
{75,4028,14,0},{76,7050,23,0},{77,4976,15,0},{78,4976,14,0},{79,5287,14,0},
{80,5640,14,0},{81,6043,14,0},{82,6507,14,0},{83,8460,17,0},{84,8460,16,0},
{85,12086,21,0},{86,14100,23,0},{87,10575,16,0},{88,10575,15,0},{89,10575,14,0},
{90,12086,15,0},{91,12086,14,0},{92,14100,15,0},{93,16920,17,0},{94,16920,16,0},
{95,16920,15,0},{96,28200,23,0},{97,21150,16,0},{98,21150,15,0},{99,21150,14,0},
{100,42301,26,0},{101,28200,16,0},{102,28200,15,0},{103,28200,14,0},{104,42301,20,0},
{105,42301,18,0},{106,42301,17,0},{107,42301,16,0},{108,42301,15,0},{109,42301,14,0},
{110,84602,26,0},{111,84602,24,0},{112,84602,23,0},{113,84602,21,0},{114,84602,20,0},
{115,84602,18,0},{116,84602,17,0},{117,84602,16,0},{118,84602,15,0},{119,84602,14,0}
};
/**********************************************************************************/

