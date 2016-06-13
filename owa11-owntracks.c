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


/*
 * Append a member to the Json node at `o'.
 */

void append(JsonNode *o, int subtype, char *str_value)
{
	switch (subtype) {
		case 1:	/* external voltage, double, V */
			break;
		case 2: /* Internal battery voltage, double, V */
			json_append_member(o, "batt", json_mknumber(atof(str_value)));
			break;
		case 3: /* Movement given by accelerometer, bool */
			break;
		case 4: /* Range (position accuracy), double */
			break;
		case 5: /* Latitude, double */
			json_append_member(o, "lat", json_mknumber(atof(str_value)));
			break;
		case 6: /* Longitude, double */
			json_append_member(o, "lon", json_mknumber(atof(str_value)));
			break;
		case 7: /* Cell ID, long int */
			break;
		case 8: /* LAC (Location Area Code), long int */
			break;
		case 9: /* PLMN, integer */
			break;
		case 10: /* RSSI GSM Received signal strength indicator (enum 0-5) */
			break;
		case 11: /* GPS coverage, boolean */
			break;

		case 12:
			json_append_member(o, "trip", json_mknumber(atol(str_value)));
			break;
		case 13: /* unused */
		case 14: /* unused */
		case 15: /* unused */
			break;
		case 16:
			json_append_member(o, "vel", json_mknumber(atol(str_value)));
			break;
		case 17: /* Last valid position timestamp, Unix epoch, long int */
			break;
		case 18: /* Outside geofence flag, boolean */
			break;
		case 19: /* Course over ground, double */
			json_append_member(o, "cog", json_mknumber(atol(str_value)));
			break;
		case 40: /* 1-Wire temperature 1, double */
			break;
		case 41: /* 1-Wire temperature 2, double */
			break;
		case 42: /* 1-Wire temperature 3, double */
			break;
		case 43: /* 1-Wire temperature 4, double */
			break;

		case 50: /* IO0 sample mean, double */
			break;
		case 51: /* IO1 sample mean, double */
			break;


		// FIXME: no alt?
	}
}

/*
 * Parse the elements out of an owa11 "record" (e.g. D|O|xxx|yyy|...)
 * and construct an OwnTracks JSON message to stdout. Most of the xxx
 * and yyy are actually subtype;value, where subtype is an integer
 * which represents the meaning of value.
 */

void parse_to_json(char *portion)
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
#if DEBUG
			fprintf(stderr, "\t%s\n", p);
#endif

			if (virgin) {
				virgin = !virgin;

				json_append_member(o, "_type", json_mkstring("location"));
				json_append_member(o, "tst", json_mknumber(atol(p)));
				json_append_member(o, "tid", json_mkstring("ow"));
				json_append_member(o, "t", json_mkstring("t"));
				continue;
			}

			/* subtype;value */
			if ((bp = strchr(p, ';')) != NULL) {
				int subtype = atoi(p);
				bp++;

				append(o, subtype, bp);

			}
		}

		if ((js = json_stringify(o, NULL)) != NULL) {
			printf("%s\n", js);
			free(js);
		}
		json_delete(o);

		free(bufsave);
	}
}

/*
 * Split up individual portions of the MQTT payload which are delineated
 * by '#' into their own 'records', and handle them individually. The
 * owa11 can stuff several of these into a single MQTT publish.
 */

void do_line(char *payload)
{
	char *buf = strdup(payload);
	char *p, *bufsave = buf;

	for (p = strtok(buf, "#"); p && *p; p = strtok(NULL, "#")) {
#if DEBUG
		fprintf(stderr, "%s\n", p);
#endif
		parse_to_json(p);
	}
	free(bufsave);
}

int main(int argc, char **argv)
{
	char buf[8192];

	setbuf(stdout, NULL);

	/* Read "topic payload" or "payload" */
	while (fgets(buf, sizeof(buf), stdin) != NULL) {
		buf[strlen(buf) - 1] = 0;
		char *sp = strchr(buf, ' ');

		sp = sp ? sp + 1 : buf;
		do_line(sp);
	}
	return 0;
}
