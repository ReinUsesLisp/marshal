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
#ifndef _MARSHAL_FORMAT_H_
#define _MARSHAL_FORMAT_H_

#define M_NIL '0'
#define M_TRUE 'T'
#define M_FALSE 'F'
#define M_INTEGER 'i'
#define M_BIGNUM 'l'
#define M_FLOAT 'f'
#define M_SYMBOL ':'
#define M_ARRAY '['
#define M_HASH '{'
#define M_HASH_DEFAULT '}'
#define M_OLD_STRING '"'
#define M_IVAR 'I'
#define M_CLASS 'c'
#define M_MODULE 'm'
#define M_OBJECT 'o'
#define M_USERDEF 'u'

#define M_SYMLINK ';'
#define M_OBJECT_REF '@'

#define M_STRING '"'
#define M_REGEX '/'

#endif /* _MARSHAL_FORMAT_H_ */
