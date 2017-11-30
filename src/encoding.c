/*
 * This file is part of Marshal.
 *
 * Marshal is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Marshal is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Marshal.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <string.h>
#include "marshal.h"

static struct pair
{
	char *name;
	int id;
}
pairs[] = {
	{ "", MARSHAL_ENCODING_ASCII_8BIT },
	{ "T", MARSHAL_ENCODING_UTF_8 },
	{ "UTF-8", MARSHAL_ENCODING_UTF_8 },
	{ "F", MARSHAL_ENCODING_US_ASCII },
	{ "US-ASCII", MARSHAL_ENCODING_US_ASCII },
	{ "UTF-16BE", MARSHAL_ENCODING_UTF_16BE },
	{ "UTF-16LE", MARSHAL_ENCODING_UTF_16LE },
	{ "UTF-32BE", MARSHAL_ENCODING_UTF_32BE },
	{ "UTF-32LE", MARSHAL_ENCODING_UTF_32LE },
	{ "UTF-16", MARSHAL_ENCODING_UTF_16 },
	{ "UTF-32", MARSHAL_ENCODING_UTF_32 },
	{ "UTF8-MAC", MARSHAL_ENCODING_UTF8_MAC },
	{ "EUC-JP", MARSHAL_ENCODING_EUC_JP },
	{ "Windows-31J", MARSHAL_ENCODING_Windows_31J },
	{ "Big5", MARSHAL_ENCODING_Big5 },
	{ "Big5-HKSCS", MARSHAL_ENCODING_Big5_HKSCS },
	{ "Big5-UAO", MARSHAL_ENCODING_Big5_UAO },
	{ "CP949", MARSHAL_ENCODING_CP949 },
	{ "Emacs-Mule", MARSHAL_ENCODING_Emacs_Mule },
	{ "EUC-KR", MARSHAL_ENCODING_EUC_KR },
	{ "EUC-TW", MARSHAL_ENCODING_EUC_TW },
	{ "GB2312", MARSHAL_ENCODING_GB2312 },
	{ "GB18030", MARSHAL_ENCODING_GB18030 },
	{ "GBK", MARSHAL_ENCODING_GBK },
	{ "ISO-8859-1", MARSHAL_ENCODING_ISO_8859_1 },
	{ "ISO-8859-2", MARSHAL_ENCODING_ISO_8859_2 },
	{ "ISO-8859-3", MARSHAL_ENCODING_ISO_8859_3 },
	{ "ISO-8859-4", MARSHAL_ENCODING_ISO_8859_4 },
	{ "ISO-8859-5", MARSHAL_ENCODING_ISO_8859_5 },
	{ "ISO-8859-6", MARSHAL_ENCODING_ISO_8859_6 },
	{ "ISO-8859-7", MARSHAL_ENCODING_ISO_8859_7 },
	{ "ISO-8859-8", MARSHAL_ENCODING_ISO_8859_8 },
	{ "ISO-8859-9", MARSHAL_ENCODING_ISO_8859_9 },
	{ "ISO-8859-10", MARSHAL_ENCODING_ISO_8859_10 },
	{ "ISO-8859-11", MARSHAL_ENCODING_ISO_8859_11 },
	{ "ISO-8859-13", MARSHAL_ENCODING_ISO_8859_13 },
	{ "ISO-8859-14", MARSHAL_ENCODING_ISO_8859_14 },
	{ "ISO-8859-15", MARSHAL_ENCODING_ISO_8859_15 },
	{ "ISO-8859-16", MARSHAL_ENCODING_ISO_8859_16 },
	{ "KOI8-R", MARSHAL_ENCODING_KOI8_R },
	{ "KOI8-U", MARSHAL_ENCODING_KOI8_U },
	{ "Shift-JIS", MARSHAL_ENCODING_Shift_JIS },
	{ "Windows-1250", MARSHAL_ENCODING_Windows_1250 },
	{ "Windows-1251", MARSHAL_ENCODING_Windows_1251 },
	{ "Windows-1252", MARSHAL_ENCODING_Windows_1252 },
	{ "Windows-1253", MARSHAL_ENCODING_Windows_1253 },
	{ "Windows-1254", MARSHAL_ENCODING_Windows_1254 },
	{ "Windows-1257", MARSHAL_ENCODING_Windows_1257 },
	{ "IBM437", MARSHAL_ENCODING_IBM437 },
	{ "IBM737", MARSHAL_ENCODING_IBM737 },
	{ "IBM775", MARSHAL_ENCODING_IBM775 },
	{ "CP850", MARSHAL_ENCODING_CP850 },
	{ "IBM852", MARSHAL_ENCODING_IBM852 },
	{ "CP852", MARSHAL_ENCODING_CP852 },
	{ "IBM855", MARSHAL_ENCODING_IBM855 },
	{ "CP855", MARSHAL_ENCODING_CP855 },
	{ "IBM857", MARSHAL_ENCODING_IBM857 },
	{ "IBM860", MARSHAL_ENCODING_IBM860 },
	{ "IBM861", MARSHAL_ENCODING_IBM861 },
	{ "IBM862", MARSHAL_ENCODING_IBM862 },
	{ "IBM863", MARSHAL_ENCODING_IBM863 },
	{ "IBM864", MARSHAL_ENCODING_IBM864 },
	{ "IBM865", MARSHAL_ENCODING_IBM865 },
	{ "IBM866", MARSHAL_ENCODING_IBM866 },
	{ "IBM869", MARSHAL_ENCODING_IBM869 },
	{ "Windows-1258", MARSHAL_ENCODING_Windows_1258 },
	{ "GB1988", MARSHAL_ENCODING_GB1988 },
	{ "macCentEuro", MARSHAL_ENCODING_macCentEuro },
	{ "macCroatian", MARSHAL_ENCODING_macCroatian },
	{ "macCyrillic", MARSHAL_ENCODING_macCyrillic },
	{ "macGreek", MARSHAL_ENCODING_macGreek },
	{ "macIceland", MARSHAL_ENCODING_macIceland },
	{ "macRoman", MARSHAL_ENCODING_macRoman },
	{ "macRomania", MARSHAL_ENCODING_macRomania },
	{ "macThai", MARSHAL_ENCODING_macThai },
	{ "macTurkish", MARSHAL_ENCODING_macTurkish },
	{ "macUkraine", MARSHAL_ENCODING_macUkraine },
	{ "CP950", MARSHAL_ENCODING_CP950 },
	{ "CP951", MARSHAL_ENCODING_CP951 },
	{ "IBM037", MARSHAL_ENCODING_IBM037 },
	{ "stateless-ISO_2022-JP", MARSHAL_ENCODING_stateless_ISO_2022_JP },
	{ "eucJP-ms", MARSHAL_ENCODING_eucJP_ms },
	{ "CP51932", MARSHAL_ENCODING_CP51932 },
	{ "EUC-JIS-2004", MARSHAL_ENCODING_EUC_JIS_2004 },
	{ "GB12345", MARSHAL_ENCODING_GB12345 },
	{ "ISO-2022-JP", MARSHAL_ENCODING_ISO_2022_JP },
	{ "ISO-2022-JP-2", MARSHAL_ENCODING_ISO_2022_JP_2 },
	{ "CP50220", MARSHAL_ENCODING_CP50220 },
	{ "CP50221", MARSHAL_ENCODING_CP50221 },
	{ "Windows-1256", MARSHAL_ENCODING_Windows_1256 },
	{ "Windows-1255", MARSHAL_ENCODING_Windows_1255 },
	{ "TIS-620", MARSHAL_ENCODING_TIS_620 },
	{ "Windows-874", MARSHAL_ENCODING_Windows_874 },
	{ "MacJapanese", MARSHAL_ENCODING_MacJapanese },
	{ "UTF-7", MARSHAL_ENCODING_UTF_7 },
	{ "UTF8-DoCoMo", MARSHAL_ENCODING_UTF8_DoCoMo },
	{ "SJIS-DoCoMo", MARSHAL_ENCODING_SJIS_DoCoMo },
	{ "UTF8-KDDI", MARSHAL_ENCODING_UTF8_KDDI },
	{ "SJIS-KDDI", MARSHAL_ENCODING_SJIS_KDDI },
	{ "ISO-2022-JP-KDDI", MARSHAL_ENCODING_ISO_2022_JP_KDDI },
	{ "stateless-ISO-2022-JP-KDDI", MARSHAL_ENCODING_stateless_ISO_2022_JP_KDDI },
	{ "UTF8-SoftBank", MARSHAL_ENCODING_UTF8_SoftBank },
	{ "SJIS-SoftBank", MARSHAL_ENCODING_SJIS_SoftBank }
};

int
marshal_encoding_name_to_id(const char *name)
{
	int i;
	for (i = 0; i < (int)(sizeof(pairs) / sizeof(pairs[0])); i++)
	{
		struct pair *p = &pairs[i];
		if (0 == strcmp(name, p->name))
			return p->id;
	}
	return -1;
}

const char *
marshal_encoding_id_to_name(int id)
{
	int i;
	for (i = 0; i < (int)(sizeof(pairs) / sizeof(pairs[0])); i++)
	{
		struct pair *p = &pairs[i];
		if (id == p->id)
			return p->name;
	}
	return NULL;
}

