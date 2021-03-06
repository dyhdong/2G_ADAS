#include <zephyr.h>
#include <stdio.h>
#include <string.h>
#include "gpd.h"
#include "gps.h"
#include "my_misc.h"
#define GPD_BUFSIZE	256

struct nmea_info {
	u32_t		id;
	u32_t		len;
	char		buf[GPS_NMEA_BUFSIZE];
};

static struct k_mutex	mutex;
static char		nmea_buf[GPS_NMEA_BUFSIZE];

static struct k_timer	gps_timer;
static char		read_buf[GPD_BUFSIZE];
static struct nmea_info nmea_info;


int gps_nmea_write(const char *nema, size_t len)
{
	int rc;
	
	k_mutex_lock(&mutex, K_FOREVER);
	rc = (len > GPS_NMEA_BUFSIZE) ? GPS_NMEA_BUFSIZE : len;
	memcpy(nmea_buf, nema, rc);
	k_mutex_unlock(&mutex);

	return rc;
}

/**
 * @brief  only support GNRMC now.
 */
int gps_nmea_read(const char *sentence, char *buf, size_t len)
{
	int rc = 0;

	if (strcmp(sentence, "$GNRMC"))
		return -1;

	k_mutex_lock(&mutex, K_FOREVER);
	strncpy(buf, nmea_buf, len - 1);
	buf[len] = '\0';
	rc = strlen(buf);
	k_mutex_unlock(&mutex);

	return rc;
}

int gps_rtcm_write(const char *buf, size_t len)
{
	return gpd_send(buf, len);
}

/**
 * @brief  Scan NMEA sentences generated by GPS device, for one kind sentence,
 *	   such as $GNRMC, $GNGGA, etc.
 *
 *	   Can only be called by function gps_timer_func().
 *
 * @param  np:		nmea_info stored to.  Found if np->id not NULL.
 * @param  buf:		buffer with NMEA messags from GPS device
 * @param  pattern:	"$GNRMC", "$GNGGA", etc.
 *
 * @return bytes scaned.
 */
static int nmea_scan(struct nmea_info *np,
		const char *buf, int len, const char *pattern)
{
	static u32_t nmea_id;	/* identify the nmea sentence */
	static u32_t state;
	static u32_t pos_dst = 0;
	char c;
	int i;
	
	if (strlen(pattern) != 6) {
		printf("%s: wrong pattern {%s}\n", __func__, pattern);
		return -1;
	}
	for (i = 0; i < len; i++) {
		c = buf[i];
		if (state <= 5) {
			if (c == pattern[state]) {
				state++;
				np->buf[pos_dst++] = c;
			} else {
				state = 0;
				pos_dst = 0;
			}
		} else if (state == 6) {
			if (c == '\r')
				state = 7;
			np->buf[pos_dst++] = c;
		} else if (state == 7) {
			if (c == '\n') {
				state = 8;
				np->buf[pos_dst++] = c;
				break;
			} else {
				state = 0;
				pos_dst = 0;
			}
		}
	}
	if (state == 8) {
		np->buf[pos_dst] = '\0';
		np->id = ++nmea_id;
		np->len = pos_dst;
		pos_dst = 0;
		state = 0;
		return i;
	}
	return len;
}

static void gps_timer_func(struct k_timer *timer)
{
	static struct nmea_info *np = &nmea_info;
	char *buf;
	int rest, len;
	rest = gpd_read(read_buf, GPD_BUFSIZE);
    if(rest > 0)
    {
        printk("...........................................................\n");
        for(len =0;len<rest;len++)
        {
            printk("%c ",read_buf[len]);
        }
        printk("...........................................................\n");
    }
    //print_log("::::::::::::gpd read %d.................%d\n",rest,gpdread);
	buf = read_buf;
	while (rest > 0) {
		len = nmea_scan(np, buf, rest, "$GNRMC");
		if (nmea_info.id) {
			gps_nmea_write(np->buf, np->len);
			np->id = 0;
			np->len = 0;
		}
		buf += len;
		rest -= len;
	}
    //k_timer_start(&gps_timer, 100, 25);
    
}

int gps_init(void)
{
	int rc;

	if ((rc = gpd_init()) != 0)
		return rc;

	k_timer_init(&gps_timer, gps_timer_func, NULL);
	k_timer_start(&gps_timer, 100, 25);

	return 0;
}

int gps_fini(void)
{
	int rc;
	
	rc = gpd_fini();
	k_timer_stop(&gps_timer);

	return rc;
}
