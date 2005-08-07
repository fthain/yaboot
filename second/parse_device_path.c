/* $Id$ */
#ifndef DEVPATH_TEST
#include <errors.h>
#include <file.h>
#include <debug.h>
#else
#define dump_boot_fspec_t(p) do { __dump_boot_fspec_t(__FUNCTION__,__LINE__,p); } while(0)
#endif
#include <string.h>
#include <stdlib.h>

#define I(m) DEBUG_F(" %p "#m " '%d' \n", &p->m, p->m);
#define S(m) DEBUG_F(" %p "#m " %p '%s' \n", &p->m, p->m, p->m ? p->m : "");
void __dump_boot_fspec_t (const char *fn, int l, const struct boot_fspec_t *p)
{
	DEBUG_F("called from '%s(%u)' '%p'\n", fn, l, p);
	if (!p)
		return;
	S(dev);
	I(part);
	S(file);
	I(type);
	S(device);
	S(partition);
	S(directory);
	S(ip_before_filename);
	S(ip_after_filename);
	S(filename);
}
#undef S
#undef I

static void parse_block_device(struct boot_fspec_t *result)
{
	char *ip;
#if 0
	prom_printf("%s\n", __FUNCTION__);
#endif
	result->part = strtol(result->partition, &ip, 10);
	if (result->part)
		*ip++ = '\0';
	else
		result->partition = "";
	if (',' == ip[0])
		ip++;
	result->directory = ip;
	result->filename = strrchr(result->directory, '/');
	if (!result->filename)
		result->filename = strrchr(result->directory, '\\');
	if (result->filename) {
		memmove(result->filename + 2, result->filename + 1, strlen(result->filename + 1) + 1);
		result->filename++;
		result->filename[0] = '\0';
		result->filename++;
	} else {
		result->filename = result->directory;
		result->directory = "";
	}
}

static void parse_net_device(struct boot_fspec_t *result)
{
	char *p;
#if 0
	prom_printf("%s\n", __FUNCTION__);
#endif
	p = result->partition;

	if (strncmp("bootp", p, 5) == 0) {
		p = strchr(p, ',');
		if (!p)
			goto bootp;
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
		goto bootp;
	*p = '\0';
	p++;
	result->filename = p;
	p = strchr(p, ',');
	if (p) {
		*p = '\0';
		p++;
		result->ip_after_filename = p;
	}
bootp:
	result->ip_before_filename = result->partition;

      out:
	result->partition = NULL;
}

int new_parse_device_path(const char *imagepath, struct boot_fspec_t *result)
{
	DEBUG_F("imagepath '%s'\n", imagepath);
	if (!imagepath)
		return 0;

	result->device = malloc(strlen(imagepath) + 2);
	if (!result->device)
		return 0;
	strcpy(result->device, imagepath);
	result->partition = strchr(result->device, ':');
	if (result->partition) {
		*result->partition++ = '\0';
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
	}
	dump_boot_fspec_t(result);
	return 1;
}

int new_parse_file_to_load_path(const char *imagepath, struct boot_fspec_t *result, const struct boot_fspec_t *b, const struct default_device *d)
{
	enum device_type type;
	char *p, *dev, *comma, *dir, part[42];
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
			return new_parse_device_path(imagepath, result);
		}
		imagepath = ++p;
	}

	type = b->type;
	dev = b->device;
	if (d) {
		switch (d->type) {
			case TYPE_BLOCK:
			case TYPE_NET:
				type = d->type;
				dev = d->device;
				break;
			default:
				d = NULL;
				break;
		}
	}

	switch (type) {
		case TYPE_BLOCK:
			part[0] = '\0';
			comma = dir = "";
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
				if (imagepath[0] != '/' && imagepath[0] != '\\' && !d)
					dir = b->directory;
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
				len = strlen(dev) + 1 + strlen(b->ip_before_filename) + 1 + strlen(imagepath);
				if (b->ip_after_filename)
					len += 1 + strlen(b->ip_after_filename);
			}
			len += 2;
			p = malloc(len);
			if (!p)
				return 0;
			if (d)
				sprintf(p, "%s:%s", dev, imagepath);
			else {
				if (b->ip_after_filename)
					sprintf(p, "%s:%s,%s,%s", dev, b->ip_before_filename, imagepath, b->ip_after_filename);
				else
					sprintf(p, "%s:%s,%s", dev, b->ip_before_filename, imagepath);
			}
#if defined(DEBUG) || defined(DEVPATH_TEST)
			prom_printf("parsing net path '%s' with len %d\n", p, len);
#endif
			break;
		default:
			prom_printf("type %d of '%s' not handled\n", result->type, result->device);
			return 0;
	}
	return new_parse_device_path(p, result);
}

int new_set_def_device(const char *dev, const char *p, struct default_device *def)
{
	int n;
	char *endp;

	DEBUG_F("dev '%s' part '%s' def %p\n", dev, p, def);
	memset(def, 0, sizeof(*def));
	def->part = -1;

	if (dev) {
		def->device = strdup(dev);
		endp = strchr(def->device, ':');
		if (endp)
			endp[0] = '\0';
		def->type = prom_get_devtype(def->device);
	}

	if (p) {
		n = simple_strtol(p, &endp, 10);
		if (endp != p && *endp == 0)
			def->part = n;	       
	}
	return (dev || p);
}
