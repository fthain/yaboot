/* $Id$ */
#ifndef DEVPATH_TEST
#include <errors.h>
#include <file.h>
#include <debug.h>
#else
#define dump_path_description(p) do { __dump_path_description(__FUNCTION__,__LINE__,p); } while(0)
#endif
#include <string.h>
#include <stdlib.h>

#define I(m) DEBUG_F(" %p "#m " '%d' \n", &p->m, p->m);
#define S(m) DEBUG_F(" %p "#m " %p '%s' \n", &p->m, p->m, p->m ? p->m : "");
void __dump_path_description (const char *fn, int l, const struct path_description *p)
{
	DEBUG_F("called from '%s(%u)' '%p'\n", fn, l, p);
	if (!p)
		return;
	I(part);
	I(type);
	S(device);
	S(u.d.s1);
	S(u.d.s2);
	S(filename);
}
#undef S
#undef I

static void parse_block_device(struct path_description *result)
{
	char *ip;
#if 0
	prom_printf("%s\n", __FUNCTION__);
#endif
	if (!result->u.b.partition)
		return;

	result->part = strtol(result->u.b.partition, &ip, 10);
	DEBUG_F("part '%d', partition '%s', ip '%s'\n", result->part, result->u.b.partition, ip);
	if (result->part)
		*ip++ = '\0';
	else {
		result->part = -1;
		result->u.b.partition = "";
	}
	if (',' == ip[0])
		ip++;
	result->u.b.directory = ip;
	result->filename = strrchr(result->u.b.directory, '/');
	if (!result->filename)
		result->filename = strrchr(result->u.b.directory, '\\');
	if (result->filename) {
		memmove(result->filename + 2, result->filename + 1, strlen(result->filename + 1) + 1);
		result->filename++;
		result->filename[0] = '\0';
		result->filename++;
	} else {
		result->filename = result->u.b.directory;
		result->u.b.directory = "";
	}
}

static void parse_net_device(struct path_description *result)
{
	char *p;
#if 0
	prom_printf("%s\n", __FUNCTION__);
#endif
	if (!result->u.n.ip_before_filename)
		return;

	p = result->u.n.ip_before_filename;

	if (strncmp("bootp", p, 5) == 0) {
		p = strchr(p, ',');
		if (!p) {
			result->u.n.ip_before_filename[5] = ',';
		//	result->u.n.ip_before_filename[0] = '\0';
			goto out;
		}
		p++;
	}
	if (strncmp("speed=", p, 6) == 0) {
		p = strchr(p, ',');
		if (!p)
			goto out;
		p++;
	}
	if (strncmp("duplex=", p, 7) == 0) {
		p = strchr(p, ',');
		if (!p)
			goto out;
		p++;
	}
	p = strchr(p, ',');
	if (!p)
		goto out;
	*p = '\0';
	p++;
	result->filename = p;
	p = strchr(p, ',');
	if (p) {
		*p = '\0';
		p++;
		result->u.n.ip_after_filename = p;
	}
      out:
	return;
}

int parse_device_path(const char *imagepath, struct path_description *result)
{
	DEBUG_F("imagepath '%s'\n", imagepath);
	if (!imagepath)
		return 0;

	result->device = malloc(strlen(imagepath) + 2);
	if (!result->device)
		return 0;
	strcpy(result->device, imagepath);
	result->u.d.s1 = strchr(result->device, ':');
	if (result->u.d.s1)
		*result->u.d.s1++ = '\0';
	result->type = prom_get_devtype(result->device);
	switch (result->type) {
	case TYPE_BLOCK:
		parse_block_device(result);
		break;
	case TYPE_NET:
		parse_net_device(result);
		break;
	default:
		prom_printf("type %d of '%s' not handled\n", result->type, result->device);
		return 0;
	}
	dump_path_description(result);
	return 1;
}

char *path_description_to_string(const struct path_description *input)
{
	int len;
	char part[42], *path;

	if (!input)
		return NULL;
	dump_path_description(input);
	path = NULL;
	len = strlen(input->device);
	len += strlen(input->filename);
	len += 1 + 1 + 1; /* : , \0 */
	switch (input->type) {
		case TYPE_BLOCK:
			if (input->part > 0)
				sprintf(part, "%d,", input->part);
			else {
				part[0] = ',';
				part[1] = '\0';
			}
			len += strlen(part);
			len += strlen(input->u.b.directory);
			path = malloc(len);
			if (path)
				sprintf(path, "%s:%s%s%s", input->device, part,
					input->u.b.directory, input->filename);
			break;
		case TYPE_NET:
			len += strlen(input->u.n.ip_before_filename);
			if (input->u.n.ip_after_filename) {
				len++;
				len += strlen(input->u.n.ip_after_filename);
			}
			path = malloc(len);
			if (path)
				sprintf(path, "%s:%s,%s%s%s", input->device,
					input->u.n.ip_before_filename, input->filename,
					input->u.n.ip_after_filename ? "," : "",
					input->u.n.ip_after_filename ? input->u.n.ip_after_filename : "");
			break;
		default:
			break;
	}
	return path;
}

int imagepath_to_path_description(const char *imagepath, struct path_description *result, const struct path_description *default_device)
{
	char *past_device, *comma, *dir, *pathname;
	char part[42];
	int len;
	DEBUG_F("imagepath '%s'\n", imagepath);

	if (!imagepath)
		return 0;

	memset(result, 0, sizeof(*result));
	past_device = strchr(imagepath, ':');
	if (past_device) {
		if (strncmp("&device;:", imagepath, 9) != 0)  {
#if defined(DEBUG) || defined(DEVPATH_TEST)
			prom_printf("parsing full path '%s'\n", imagepath);
#endif
			return parse_device_path(imagepath, result);
		}
		past_device++;
	}
	comma = dir = "";
	pathname = NULL;
	switch (default_device->type) {
		case TYPE_BLOCK:
			part[0] = '\0';
			/* parse_device_path will look for a partition number */
			if (past_device)
				len = strlen(default_device->device) + 1 + strlen(past_device);
			else {
				if (default_device->part > 0)
					sprintf(part, "%d", default_device->part);
				comma = ",";
				if (imagepath[0] != '/' && imagepath[0] != '\\')
					dir = default_device->u.b.directory;
				len = strlen(default_device->device) + 1 + strlen(part) + 1 + strlen(dir) + strlen(imagepath);
			}
			len += 2;
			pathname = malloc(len);
			if (pathname)
				sprintf(pathname, "%s:%s%s%s%s", default_device->device, part, comma, dir,
						past_device ? past_device : imagepath);
#if defined(DEBUG) || defined(DEVPATH_TEST)
			prom_printf("parsing block path '%s'\n", pathname);
#endif
			break;
		case TYPE_NET:
			if (past_device)
				len = strlen(default_device->device) + 1 + strlen(past_device);
			else {
				len = strlen(default_device->device) + 1;
				len += strlen(default_device->u.n.ip_before_filename) + 1;
				len += strlen(imagepath);
				if (default_device->u.n.ip_after_filename) {
					len += 1 + strlen(default_device->u.n.ip_after_filename);
					comma = ",";
				}
			}
			len += 2;
			pathname = malloc(len);
			if (pathname)
				sprintf(pathname, "%s:%s,%s%s%s", default_device->device, default_device->u.n.ip_before_filename,
						past_device ? past_device : imagepath, comma,
						default_device->u.n.ip_after_filename ? default_device->u.n.ip_after_filename : "");
#if defined(DEBUG) || defined(DEVPATH_TEST)
			prom_printf("parsing net path '%s'\n", pathname);
#endif
			break;
		default:
			;
	}
	len = parse_device_path(pathname, result);
	if (pathname)
		free(pathname);
	return len;
}

int parse_file_to_load_path(const char *imagepath, struct path_description *result, const struct path_description *b, const struct default_device *d)
{
	enum device_type type;
	char *p, *dev, *comma, *dir, part[42], *ip_b;
	int len, partition = -1;

	DEBUG_F("imagepath '%s'\n", imagepath);

	if (!imagepath)
		return 0;

	memset(result, 0, sizeof(*result));
	p = strchr(imagepath, ':');
	if (p) {
		if (strncmp("&device;:", imagepath, 9) != 0)  {
#if defined(DEBUG) || defined(DEVPATH_TEST)
			prom_printf("parsing full path '%s'\n", imagepath);
#endif
			return parse_device_path(imagepath, result);
		}
		imagepath = ++p;
	}

	type = b->type;
	dev = b->device;
	if (d) {
		switch (d->type) {
			case TYPE_BLOCK:
			case TYPE_NET:
				if (d->device) {
					type = d->type;
					dev = d->device;
				}
				break;
			default:
				d = NULL;
				break;
		}
	}

	ip_b = comma = dir = "";
	switch (type) {
		case TYPE_BLOCK:
			part[0] = '\0';
			if (p)
				len = strlen(dev) + 1 + strlen(imagepath);
			else {
				if (d && d->part >= 0)
					partition = d->part;
				else
					if (b->part >= 0)
						partition = b->part;
				if (partition >= 0)
					sprintf(part, "%d", partition);
				comma = ",";
				if (imagepath[0] != '/' && imagepath[0] != '\\' && !d && b->u.b.directory)
					dir = b->u.b.directory;
				len = strlen(dev) + 1 + strlen(part) + 1 + strlen(dir) + strlen(imagepath);
			}
			len += 2;
			p = malloc(len);
			if (!p)
				return 0;
			sprintf(p, "%s:%s%s%s%s", dev, part, comma, dir, imagepath);
#if defined(DEBUG) || defined(DEVPATH_TEST)
			prom_printf("parsing block path '%s' with len %d\n", p, len);
#endif
			break;
		case TYPE_NET:
			if (d)
				len = strlen(dev) + 1;
			else {
				if (b->u.n.ip_before_filename)
					ip_b = b->u.n.ip_before_filename;
				len = strlen(dev) + 1 + strlen(ip_b) + 1 + strlen(imagepath);
				if (b->u.n.ip_after_filename)
					len += 1 + strlen(b->u.n.ip_after_filename);
			}
			len += 2;
			p = malloc(len);
			if (!p)
				return 0;
			if (d)
				sprintf(p, "%s:%s", dev, imagepath);
			else {
				if (b->u.n.ip_after_filename)
					sprintf(p, "%s:%s,%s,%s", dev, ip_b, imagepath, b->u.n.ip_after_filename);
				else
					sprintf(p, "%s:%s,%s", dev, ip_b, imagepath);
			}
#if defined(DEBUG) || defined(DEVPATH_TEST)
			prom_printf("parsing net path '%s' with len %d\n", p, len);
#endif
			break;
		default:
			prom_printf("type %d of '%s' not handled\n", result->type, result->device);
			return 0;
	}
	return parse_device_path(p, result);
}

void set_default_device(const char *dev, const char *partition, struct path_description *default_device)
{
	int n;
	char *endp;

	DEBUG_F("dev '%s' part '%s'\n", dev, partition);

	if (dev) {
		endp = strdup(dev);
		if (!endp)
			return;
		default_device->device = endp;
		endp = strchr(default_device->device, ':');
		if (endp)
			endp[0] = '\0';
		default_device->type = prom_get_devtype(default_device->device);
	}

	if (partition) {
		n = simple_strtol(partition, &endp, 10);
		if (endp != partition && *endp == 0) {
			if (TYPE_UNSET == default_device->type)
				default_device->type = TYPE_BLOCK;
			default_device->part = n;	       
		}
	}
}
