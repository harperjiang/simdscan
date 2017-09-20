/*
 * template.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: harper
 */

#include "template.h"

int mm_extract_epi32(__m128i data, int index) {
    switch (index) {
        case 0:
            return _mm_extract_epi32(data, 0);
        case 1:
            return _mm_extract_epi32(data, 1);
        case 2:
            return _mm_extract_epi32(data, 2);
        case 3:
        default:
            return _mm_extract_epi32(data, 3);
    }
}

__m128i mm_alignr_epi8(__m128i a, __m128i b, int offset) {
    switch (offset) {
        case 0:
            return b;
        case 1:
            return _mm_alignr_epi8(a, b, 1);
        case 2:
            return _mm_alignr_epi8(a, b, 2);
        case 3:
            return _mm_alignr_epi8(a, b, 3);
        case 4:
            return _mm_alignr_epi8(a, b, 4);
        case 5:
            return _mm_alignr_epi8(a, b, 5);
        case 6:
            return _mm_alignr_epi8(a, b, 6);
        case 7:
            return _mm_alignr_epi8(a, b, 7);
        case 8:
            return _mm_alignr_epi8(a, b, 8);
        case 9:
            return _mm_alignr_epi8(a, b, 9);
        case 10:
            return _mm_alignr_epi8(a, b, 10);
        case 11:
            return _mm_alignr_epi8(a, b, 11);
        case 12:
            return _mm_alignr_epi8(a, b, 12);
        case 13:
            return _mm_alignr_epi8(a, b, 13);
        case 14:
            return _mm_alignr_epi8(a, b, 14);
        case 15:
            return _mm_alignr_epi8(a, b, 15);
        default:
            return b;
    }
}

__m128d mm_blend_pd(__m128d a, __m128d b, int imm) {
    switch (imm) {
        case 0:
            return _mm_blend_pd(a, b, 0);
        case 1:
            return _mm_blend_pd(a, b, 1);
        case 2:
            return _mm_blend_pd(a, b, 2);
        case 3:
            return _mm_blend_pd(a, b, 3);
        default:
            return _mm_blend_pd(a, b, 0);
    }
}

int mm256_extract_epi32(__m256i data, int index) {
    switch (index) {
        case 0:
            return _mm256_extract_epi32(data, 0);
        case 1:
            return _mm256_extract_epi32(data, 1);
        case 2:
            return _mm256_extract_epi32(data, 2);
        case 3:
            return _mm256_extract_epi32(data, 3);
        case 4:
            return _mm256_extract_epi32(data, 4);
        case 5:
            return _mm256_extract_epi32(data, 5);
        case 6:
            return _mm256_extract_epi32(data, 6);
        case 7:
            return _mm256_extract_epi32(data, 7);
        default:
            return _mm256_extract_epi32(data, 0);
    }
}


__m256d mm256_blend_pd(__m256d a, __m256d b, int imm) {
    switch (imm) {
        case 0:
            return _mm256_blend_pd(a, b, 0);
        case 1:
            return _mm256_blend_pd(a, b, 1);
        case 2:
            return _mm256_blend_pd(a, b, 2);
        case 3:
            return _mm256_blend_pd(a, b, 3);
        case 4:
            return _mm256_blend_pd(a, b, 4);
        case 5:
            return _mm256_blend_pd(a, b, 5);
        case 6:
            return _mm256_blend_pd(a, b, 6);
        case 7:
            return _mm256_blend_pd(a, b, 7);
        case 8:
            return _mm256_blend_pd(a, b, 8);
        case 9:
            return _mm256_blend_pd(a, b, 9);
        case 10:
            return _mm256_blend_pd(a, b, 10);
        case 11:
            return _mm256_blend_pd(a, b, 11);
        case 12:
            return _mm256_blend_pd(a, b, 12);
        case 13:
            return _mm256_blend_pd(a, b, 13);
        case 14:
            return _mm256_blend_pd(a, b, 14);
        case 15:
            return _mm256_blend_pd(a, b, 15);
        default:
            return _mm256_blend_pd(a, b, 0);
    }
}

__m256i mm256_alignr_epi8(__m256i a, __m256i b, int offset) {
    switch (offset) {
        case 0:
            return b;
        case 1:
            return _mm256_alignr_epi8(a, b, 1);
        case 2:
            return _mm256_alignr_epi8(a, b, 2);
        case 3:
            return _mm256_alignr_epi8(a, b, 3);
        case 4:
            return _mm256_alignr_epi8(a, b, 4);
        case 5:
            return _mm256_alignr_epi8(a, b, 5);
        case 6:
            return _mm256_alignr_epi8(a, b, 6);
        case 7:
            return _mm256_alignr_epi8(a, b, 7);
        case 8:
            return _mm256_alignr_epi8(a, b, 8);
        case 9:
            return _mm256_alignr_epi8(a, b, 9);
        case 10:
            return _mm256_alignr_epi8(a, b, 10);
        case 11:
            return _mm256_alignr_epi8(a, b, 11);
        case 12:
            return _mm256_alignr_epi8(a, b, 12);
        case 13:
            return _mm256_alignr_epi8(a, b, 13);
        case 14:
            return _mm256_alignr_epi8(a, b, 14);
        case 15:
            return _mm256_alignr_epi8(a, b, 15);
        case 16:
            return _mm256_alignr_epi8(a, b, 16);
        case 17:
            return _mm256_alignr_epi8(a, b, 17);
        case 18:
            return _mm256_alignr_epi8(a, b, 18);
        case 19:
            return _mm256_alignr_epi8(a, b, 19);
        case 20:
            return _mm256_alignr_epi8(a, b, 20);
        case 21:
            return _mm256_alignr_epi8(a, b, 21);
        case 22:
            return _mm256_alignr_epi8(a, b, 22);
        case 23:
            return _mm256_alignr_epi8(a, b, 23);
        case 24:
            return _mm256_alignr_epi8(a, b, 24);
        case 25:
            return _mm256_alignr_epi8(a, b, 25);
        case 26:
            return _mm256_alignr_epi8(a, b, 26);
        case 27:
            return _mm256_alignr_epi8(a, b, 27);
        case 28:
            return _mm256_alignr_epi8(a, b, 28);
        case 29:
            return _mm256_alignr_epi8(a, b, 29);
        case 30:
            return _mm256_alignr_epi8(a, b, 30);
        case 31:
            return _mm256_alignr_epi8(a, b, 31);
        default:
            return b;
    }
}

__m512i mm512_blend_epi64(__m512i a, __m512i b, int imm) {
    switch (imm) {
        case 0:
            return _mm512_mask_blend_epi64(0, a, b);
        case 1:
            return _mm512_mask_blend_epi64(1, a, b);
        case 2:
            return _mm512_mask_blend_epi64(2, a, b);
        case 3:
            return _mm512_mask_blend_epi64(3, a, b);
        case 4:
            return _mm512_mask_blend_epi64(4, a, b);
        case 5:
            return _mm512_mask_blend_epi64(5, a, b);
        case 6:
            return _mm512_mask_blend_epi64(6, a, b);
        case 7:
            return _mm512_mask_blend_epi64(7, a, b);
        case 8:
            return _mm512_mask_blend_epi64(8, a, b);
        case 9:
            return _mm512_mask_blend_epi64(9, a, b);
        case 10:
            return _mm512_mask_blend_epi64(10, a, b);
        case 11:
            return _mm512_mask_blend_epi64(11, a, b);
        case 12:
            return _mm512_mask_blend_epi64(12, a, b);
        case 13:
            return _mm512_mask_blend_epi64(13, a, b);
        case 14:
            return _mm512_mask_blend_epi64(14, a, b);
        case 15:
            return _mm512_mask_blend_epi64(15, a, b);
        case 16:
            return _mm512_mask_blend_epi64(16, a, b);
        case 17:
            return _mm512_mask_blend_epi64(17, a, b);
        case 18:
            return _mm512_mask_blend_epi64(18, a, b);
        case 19:
            return _mm512_mask_blend_epi64(19, a, b);
        case 20:
            return _mm512_mask_blend_epi64(20, a, b);
        case 21:
            return _mm512_mask_blend_epi64(21, a, b);
        case 22:
            return _mm512_mask_blend_epi64(22, a, b);
        case 23:
            return _mm512_mask_blend_epi64(23, a, b);
        case 24:
            return _mm512_mask_blend_epi64(24, a, b);
        case 25:
            return _mm512_mask_blend_epi64(25, a, b);
        case 26:
            return _mm512_mask_blend_epi64(26, a, b);
        case 27:
            return _mm512_mask_blend_epi64(27, a, b);
        case 28:
            return _mm512_mask_blend_epi64(28, a, b);
        case 29:
            return _mm512_mask_blend_epi64(29, a, b);
        case 30:
            return _mm512_mask_blend_epi64(30, a, b);
        case 31:
            return _mm512_mask_blend_epi64(31, a, b);
        case 32:
            return _mm512_mask_blend_epi64(32, a, b);
        case 33:
            return _mm512_mask_blend_epi64(33, a, b);
        case 34:
            return _mm512_mask_blend_epi64(34, a, b);
        case 35:
            return _mm512_mask_blend_epi64(35, a, b);
        case 36:
            return _mm512_mask_blend_epi64(36, a, b);
        case 37:
            return _mm512_mask_blend_epi64(37, a, b);
        case 38:
            return _mm512_mask_blend_epi64(38, a, b);
        case 39:
            return _mm512_mask_blend_epi64(39, a, b);
        case 40:
            return _mm512_mask_blend_epi64(40, a, b);
        case 41:
            return _mm512_mask_blend_epi64(41, a, b);
        case 42:
            return _mm512_mask_blend_epi64(42, a, b);
        case 43:
            return _mm512_mask_blend_epi64(43, a, b);
        case 44:
            return _mm512_mask_blend_epi64(44, a, b);
        case 45:
            return _mm512_mask_blend_epi64(45, a, b);
        case 46:
            return _mm512_mask_blend_epi64(46, a, b);
        case 47:
            return _mm512_mask_blend_epi64(47, a, b);
        case 48:
            return _mm512_mask_blend_epi64(48, a, b);
        case 49:
            return _mm512_mask_blend_epi64(49, a, b);
        case 50:
            return _mm512_mask_blend_epi64(50, a, b);
        case 51:
            return _mm512_mask_blend_epi64(51, a, b);
        case 52:
            return _mm512_mask_blend_epi64(52, a, b);
        case 53:
            return _mm512_mask_blend_epi64(53, a, b);
        case 54:
            return _mm512_mask_blend_epi64(54, a, b);
        case 55:
            return _mm512_mask_blend_epi64(55, a, b);
        case 56:
            return _mm512_mask_blend_epi64(56, a, b);
        case 57:
            return _mm512_mask_blend_epi64(57, a, b);
        case 58:
            return _mm512_mask_blend_epi64(58, a, b);
        case 59:
            return _mm512_mask_blend_epi64(59, a, b);
        case 60:
            return _mm512_mask_blend_epi64(60, a, b);
        case 61:
            return _mm512_mask_blend_epi64(61, a, b);
        case 62:
            return _mm512_mask_blend_epi64(62, a, b);
        case 63:
            return _mm512_mask_blend_epi64(63, a, b);
        case 64:
            return _mm512_mask_blend_epi64(64, a, b);
        case 65:
            return _mm512_mask_blend_epi64(65, a, b);
        case 66:
            return _mm512_mask_blend_epi64(66, a, b);
        case 67:
            return _mm512_mask_blend_epi64(67, a, b);
        case 68:
            return _mm512_mask_blend_epi64(68, a, b);
        case 69:
            return _mm512_mask_blend_epi64(69, a, b);
        case 70:
            return _mm512_mask_blend_epi64(70, a, b);
        case 71:
            return _mm512_mask_blend_epi64(71, a, b);
        case 72:
            return _mm512_mask_blend_epi64(72, a, b);
        case 73:
            return _mm512_mask_blend_epi64(73, a, b);
        case 74:
            return _mm512_mask_blend_epi64(74, a, b);
        case 75:
            return _mm512_mask_blend_epi64(75, a, b);
        case 76:
            return _mm512_mask_blend_epi64(76, a, b);
        case 77:
            return _mm512_mask_blend_epi64(77, a, b);
        case 78:
            return _mm512_mask_blend_epi64(78, a, b);
        case 79:
            return _mm512_mask_blend_epi64(79, a, b);
        case 80:
            return _mm512_mask_blend_epi64(80, a, b);
        case 81:
            return _mm512_mask_blend_epi64(81, a, b);
        case 82:
            return _mm512_mask_blend_epi64(82, a, b);
        case 83:
            return _mm512_mask_blend_epi64(83, a, b);
        case 84:
            return _mm512_mask_blend_epi64(84, a, b);
        case 85:
            return _mm512_mask_blend_epi64(85, a, b);
        case 86:
            return _mm512_mask_blend_epi64(86, a, b);
        case 87:
            return _mm512_mask_blend_epi64(87, a, b);
        case 88:
            return _mm512_mask_blend_epi64(88, a, b);
        case 89:
            return _mm512_mask_blend_epi64(89, a, b);
        case 90:
            return _mm512_mask_blend_epi64(90, a, b);
        case 91:
            return _mm512_mask_blend_epi64(91, a, b);
        case 92:
            return _mm512_mask_blend_epi64(92, a, b);
        case 93:
            return _mm512_mask_blend_epi64(93, a, b);
        case 94:
            return _mm512_mask_blend_epi64(94, a, b);
        case 95:
            return _mm512_mask_blend_epi64(95, a, b);
        case 96:
            return _mm512_mask_blend_epi64(96, a, b);
        case 97:
            return _mm512_mask_blend_epi64(97, a, b);
        case 98:
            return _mm512_mask_blend_epi64(98, a, b);
        case 99:
            return _mm512_mask_blend_epi64(99, a, b);
        case 100:
            return _mm512_mask_blend_epi64(100, a, b);
        case 101:
            return _mm512_mask_blend_epi64(101, a, b);
        case 102:
            return _mm512_mask_blend_epi64(102, a, b);
        case 103:
            return _mm512_mask_blend_epi64(103, a, b);
        case 104:
            return _mm512_mask_blend_epi64(104, a, b);
        case 105:
            return _mm512_mask_blend_epi64(105, a, b);
        case 106:
            return _mm512_mask_blend_epi64(106, a, b);
        case 107:
            return _mm512_mask_blend_epi64(107, a, b);
        case 108:
            return _mm512_mask_blend_epi64(108, a, b);
        case 109:
            return _mm512_mask_blend_epi64(109, a, b);
        case 110:
            return _mm512_mask_blend_epi64(110, a, b);
        case 111:
            return _mm512_mask_blend_epi64(111, a, b);
        case 112:
            return _mm512_mask_blend_epi64(112, a, b);
        case 113:
            return _mm512_mask_blend_epi64(113, a, b);
        case 114:
            return _mm512_mask_blend_epi64(114, a, b);
        case 115:
            return _mm512_mask_blend_epi64(115, a, b);
        case 116:
            return _mm512_mask_blend_epi64(116, a, b);
        case 117:
            return _mm512_mask_blend_epi64(117, a, b);
        case 118:
            return _mm512_mask_blend_epi64(118, a, b);
        case 119:
            return _mm512_mask_blend_epi64(119, a, b);
        case 120:
            return _mm512_mask_blend_epi64(120, a, b);
        case 121:
            return _mm512_mask_blend_epi64(121, a, b);
        case 122:
            return _mm512_mask_blend_epi64(122, a, b);
        case 123:
            return _mm512_mask_blend_epi64(123, a, b);
        case 124:
            return _mm512_mask_blend_epi64(124, a, b);
        case 125:
            return _mm512_mask_blend_epi64(125, a, b);
        case 126:
            return _mm512_mask_blend_epi64(126, a, b);
        case 127:
            return _mm512_mask_blend_epi64(127, a, b);
        case 128:
            return _mm512_mask_blend_epi64(128, a, b);
        case 129:
            return _mm512_mask_blend_epi64(129, a, b);
        case 130:
            return _mm512_mask_blend_epi64(130, a, b);
        case 131:
            return _mm512_mask_blend_epi64(131, a, b);
        case 132:
            return _mm512_mask_blend_epi64(132, a, b);
        case 133:
            return _mm512_mask_blend_epi64(133, a, b);
        case 134:
            return _mm512_mask_blend_epi64(134, a, b);
        case 135:
            return _mm512_mask_blend_epi64(135, a, b);
        case 136:
            return _mm512_mask_blend_epi64(136, a, b);
        case 137:
            return _mm512_mask_blend_epi64(137, a, b);
        case 138:
            return _mm512_mask_blend_epi64(138, a, b);
        case 139:
            return _mm512_mask_blend_epi64(139, a, b);
        case 140:
            return _mm512_mask_blend_epi64(140, a, b);
        case 141:
            return _mm512_mask_blend_epi64(141, a, b);
        case 142:
            return _mm512_mask_blend_epi64(142, a, b);
        case 143:
            return _mm512_mask_blend_epi64(143, a, b);
        case 144:
            return _mm512_mask_blend_epi64(144, a, b);
        case 145:
            return _mm512_mask_blend_epi64(145, a, b);
        case 146:
            return _mm512_mask_blend_epi64(146, a, b);
        case 147:
            return _mm512_mask_blend_epi64(147, a, b);
        case 148:
            return _mm512_mask_blend_epi64(148, a, b);
        case 149:
            return _mm512_mask_blend_epi64(149, a, b);
        case 150:
            return _mm512_mask_blend_epi64(150, a, b);
        case 151:
            return _mm512_mask_blend_epi64(151, a, b);
        case 152:
            return _mm512_mask_blend_epi64(152, a, b);
        case 153:
            return _mm512_mask_blend_epi64(153, a, b);
        case 154:
            return _mm512_mask_blend_epi64(154, a, b);
        case 155:
            return _mm512_mask_blend_epi64(155, a, b);
        case 156:
            return _mm512_mask_blend_epi64(156, a, b);
        case 157:
            return _mm512_mask_blend_epi64(157, a, b);
        case 158:
            return _mm512_mask_blend_epi64(158, a, b);
        case 159:
            return _mm512_mask_blend_epi64(159, a, b);
        case 160:
            return _mm512_mask_blend_epi64(160, a, b);
        case 161:
            return _mm512_mask_blend_epi64(161, a, b);
        case 162:
            return _mm512_mask_blend_epi64(162, a, b);
        case 163:
            return _mm512_mask_blend_epi64(163, a, b);
        case 164:
            return _mm512_mask_blend_epi64(164, a, b);
        case 165:
            return _mm512_mask_blend_epi64(165, a, b);
        case 166:
            return _mm512_mask_blend_epi64(166, a, b);
        case 167:
            return _mm512_mask_blend_epi64(167, a, b);
        case 168:
            return _mm512_mask_blend_epi64(168, a, b);
        case 169:
            return _mm512_mask_blend_epi64(169, a, b);
        case 170:
            return _mm512_mask_blend_epi64(170, a, b);
        case 171:
            return _mm512_mask_blend_epi64(171, a, b);
        case 172:
            return _mm512_mask_blend_epi64(172, a, b);
        case 173:
            return _mm512_mask_blend_epi64(173, a, b);
        case 174:
            return _mm512_mask_blend_epi64(174, a, b);
        case 175:
            return _mm512_mask_blend_epi64(175, a, b);
        case 176:
            return _mm512_mask_blend_epi64(176, a, b);
        case 177:
            return _mm512_mask_blend_epi64(177, a, b);
        case 178:
            return _mm512_mask_blend_epi64(178, a, b);
        case 179:
            return _mm512_mask_blend_epi64(179, a, b);
        case 180:
            return _mm512_mask_blend_epi64(180, a, b);
        case 181:
            return _mm512_mask_blend_epi64(181, a, b);
        case 182:
            return _mm512_mask_blend_epi64(182, a, b);
        case 183:
            return _mm512_mask_blend_epi64(183, a, b);
        case 184:
            return _mm512_mask_blend_epi64(184, a, b);
        case 185:
            return _mm512_mask_blend_epi64(185, a, b);
        case 186:
            return _mm512_mask_blend_epi64(186, a, b);
        case 187:
            return _mm512_mask_blend_epi64(187, a, b);
        case 188:
            return _mm512_mask_blend_epi64(188, a, b);
        case 189:
            return _mm512_mask_blend_epi64(189, a, b);
        case 190:
            return _mm512_mask_blend_epi64(190, a, b);
        case 191:
            return _mm512_mask_blend_epi64(191, a, b);
        case 192:
            return _mm512_mask_blend_epi64(192, a, b);
        case 193:
            return _mm512_mask_blend_epi64(193, a, b);
        case 194:
            return _mm512_mask_blend_epi64(194, a, b);
        case 195:
            return _mm512_mask_blend_epi64(195, a, b);
        case 196:
            return _mm512_mask_blend_epi64(196, a, b);
        case 197:
            return _mm512_mask_blend_epi64(197, a, b);
        case 198:
            return _mm512_mask_blend_epi64(198, a, b);
        case 199:
            return _mm512_mask_blend_epi64(199, a, b);
        case 200:
            return _mm512_mask_blend_epi64(200, a, b);
        case 201:
            return _mm512_mask_blend_epi64(201, a, b);
        case 202:
            return _mm512_mask_blend_epi64(202, a, b);
        case 203:
            return _mm512_mask_blend_epi64(203, a, b);
        case 204:
            return _mm512_mask_blend_epi64(204, a, b);
        case 205:
            return _mm512_mask_blend_epi64(205, a, b);
        case 206:
            return _mm512_mask_blend_epi64(206, a, b);
        case 207:
            return _mm512_mask_blend_epi64(207, a, b);
        case 208:
            return _mm512_mask_blend_epi64(208, a, b);
        case 209:
            return _mm512_mask_blend_epi64(209, a, b);
        case 210:
            return _mm512_mask_blend_epi64(210, a, b);
        case 211:
            return _mm512_mask_blend_epi64(211, a, b);
        case 212:
            return _mm512_mask_blend_epi64(212, a, b);
        case 213:
            return _mm512_mask_blend_epi64(213, a, b);
        case 214:
            return _mm512_mask_blend_epi64(214, a, b);
        case 215:
            return _mm512_mask_blend_epi64(215, a, b);
        case 216:
            return _mm512_mask_blend_epi64(216, a, b);
        case 217:
            return _mm512_mask_blend_epi64(217, a, b);
        case 218:
            return _mm512_mask_blend_epi64(218, a, b);
        case 219:
            return _mm512_mask_blend_epi64(219, a, b);
        case 220:
            return _mm512_mask_blend_epi64(220, a, b);
        case 221:
            return _mm512_mask_blend_epi64(221, a, b);
        case 222:
            return _mm512_mask_blend_epi64(222, a, b);
        case 223:
            return _mm512_mask_blend_epi64(223, a, b);
        case 224:
            return _mm512_mask_blend_epi64(224, a, b);
        case 225:
            return _mm512_mask_blend_epi64(225, a, b);
        case 226:
            return _mm512_mask_blend_epi64(226, a, b);
        case 227:
            return _mm512_mask_blend_epi64(227, a, b);
        case 228:
            return _mm512_mask_blend_epi64(228, a, b);
        case 229:
            return _mm512_mask_blend_epi64(229, a, b);
        case 230:
            return _mm512_mask_blend_epi64(230, a, b);
        case 231:
            return _mm512_mask_blend_epi64(231, a, b);
        case 232:
            return _mm512_mask_blend_epi64(232, a, b);
        case 233:
            return _mm512_mask_blend_epi64(233, a, b);
        case 234:
            return _mm512_mask_blend_epi64(234, a, b);
        case 235:
            return _mm512_mask_blend_epi64(235, a, b);
        case 236:
            return _mm512_mask_blend_epi64(236, a, b);
        case 237:
            return _mm512_mask_blend_epi64(237, a, b);
        case 238:
            return _mm512_mask_blend_epi64(238, a, b);
        case 239:
            return _mm512_mask_blend_epi64(239, a, b);
        case 240:
            return _mm512_mask_blend_epi64(240, a, b);
        case 241:
            return _mm512_mask_blend_epi64(241, a, b);
        case 242:
            return _mm512_mask_blend_epi64(242, a, b);
        case 243:
            return _mm512_mask_blend_epi64(243, a, b);
        case 244:
            return _mm512_mask_blend_epi64(244, a, b);
        case 245:
            return _mm512_mask_blend_epi64(245, a, b);
        case 246:
            return _mm512_mask_blend_epi64(246, a, b);
        case 247:
            return _mm512_mask_blend_epi64(247, a, b);
        case 248:
            return _mm512_mask_blend_epi64(248, a, b);
        case 249:
            return _mm512_mask_blend_epi64(249, a, b);
        case 250:
            return _mm512_mask_blend_epi64(250, a, b);
        case 251:
            return _mm512_mask_blend_epi64(251, a, b);
        case 252:
            return _mm512_mask_blend_epi64(252, a, b);
        case 253:
            return _mm512_mask_blend_epi64(253, a, b);
        case 254:
            return _mm512_mask_blend_epi64(254, a, b);
        case 255:
            return _mm512_mask_blend_epi64(255, a, b);
        default:
            return a;
    }
}