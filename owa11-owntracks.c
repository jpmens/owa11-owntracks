/*
 * owa11-owntracks
 * Copyright (C) 2016 Jan-Piet Mens <jpmens@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "json.h"

void chop(char *portion)
{
	if (*portion == 'D' && strncmp(portion, "D|O|", 4) == 0) {
		char *buf = strdup(portion + 4);
		char *p, *bufsave = buf;
		char *l, *js;
		int virgin = true;
		JsonNode *o;

		o = json_mkobject();

		for (p = strtok_r(buf, "|", &l); p && *p; p = strtok_r(NULL, "|", &l)) {
			char *bp;
			fprintf(stderr, "\t%s\n", p);

			if (virgin) {
				virgin = !virgin;

				json_append_member(o, "_type", json_mkstring("location"));
				json_append_member(o, "tst", json_mknumber(atol(p)));
				json_append_member(o, "tid", json_mkstring("ow"));
				json_append_member(o, "t", json_mkstring("t"));
				continue;
			}

			/* n;value */
			if ((bp = strchr(p, ';')) != NULL) {
				int subtype = atoi(p);
				bp++;

				switch (subtype) {
					case 2:
						json_append_member(o, "batt", json_mknumber(atof(bp)));
						break;
					case 5:
						json_append_member(o, "lat", json_mknumber(atof(bp)));
						break;
					case 6:
						json_append_member(o, "lon", json_mknumber(atof(bp)));
						break;
					case 12:
						json_append_member(o, "trip", json_mknumber(atol(bp)));
						break;
					case 16:
						json_append_member(o, "vel", json_mknumber(atol(bp)));
						break;
					case 19:
						json_append_member(o, "cog", json_mknumber(atol(bp)));
						break;

					// FIXME: no alt?
				}
			}
		}

		if ((js = json_stringify(o, NULL)) != NULL) {
			printf("%s\n", js);
			fflush(stdout);
			free(js);
		}
		json_delete(o);

		free(bufsave);
	}
}

void parse(char *payload)
{
	char *buf = strdup(payload);
	char *p, *bufsave = buf;

	for (p = strtok(buf, "#"); p && *p; p = strtok(NULL, "#")) {
		fprintf(stderr, "%s\n", p);
		chop(p);
	}
	free(bufsave);
}

int main()
{
	char buf[8192];

	// parse(IN);

	while (fgets(buf, sizeof(buf), stdin) != NULL) {
		buf[strlen(buf) - 1] = 0;
		char *sp = strchr(buf, ' ');

		sp = sp ? sp + 1 : buf;
		parse(sp);
	}
	return 0;
}
