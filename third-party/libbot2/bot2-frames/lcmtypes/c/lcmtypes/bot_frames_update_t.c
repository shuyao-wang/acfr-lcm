/** THIS IS AN AUTOMATICALLY GENERATED FILE.  DO NOT MODIFY
 * BY HAND!!
 *
 * Generated by lcm-gen
 **/

#include <string.h>
#include "lcmtypes/bot_frames_update_t.h"

static int __bot_frames_update_t_hash_computed;
static int64_t __bot_frames_update_t_hash;

int64_t __bot_frames_update_t_hash_recursive(const __lcm_hash_ptr *p)
{
    const __lcm_hash_ptr *fp;
    for (fp = p; fp != NULL; fp = fp->parent)
        if (fp->v == __bot_frames_update_t_get_hash)
            return 0;

    const __lcm_hash_ptr cp = { p, (void*)__bot_frames_update_t_get_hash };
    (void) cp;

    int64_t hash = 0xe81404582309d273LL
         + __int64_t_hash_recursive(&cp)
         + __string_hash_recursive(&cp)
         + __string_hash_recursive(&cp)
         + __double_hash_recursive(&cp)
         + __double_hash_recursive(&cp)
        ;

    return (hash<<1) + ((hash>>63)&1);
}

int64_t __bot_frames_update_t_get_hash(void)
{
    if (!__bot_frames_update_t_hash_computed) {
        __bot_frames_update_t_hash = __bot_frames_update_t_hash_recursive(NULL);
        __bot_frames_update_t_hash_computed = 1;
    }

    return __bot_frames_update_t_hash;
}

int __bot_frames_update_t_encode_array(void *buf, int offset, int maxlen, const bot_frames_update_t *p, int elements)
{
    int pos = 0, thislen, element;

    for (element = 0; element < elements; element++) {

        thislen = __int64_t_encode_array(buf, offset + pos, maxlen - pos, &(p[element].utime), 1);
        if (thislen < 0) return thislen; else pos += thislen;

        thislen = __string_encode_array(buf, offset + pos, maxlen - pos, &(p[element].frame), 1);
        if (thislen < 0) return thislen; else pos += thislen;

        thislen = __string_encode_array(buf, offset + pos, maxlen - pos, &(p[element].relative_to), 1);
        if (thislen < 0) return thislen; else pos += thislen;

        thislen = __double_encode_array(buf, offset + pos, maxlen - pos, p[element].trans, 3);
        if (thislen < 0) return thislen; else pos += thislen;

        thislen = __double_encode_array(buf, offset + pos, maxlen - pos, p[element].quat, 4);
        if (thislen < 0) return thislen; else pos += thislen;

    }
    return pos;
}

int bot_frames_update_t_encode(void *buf, int offset, int maxlen, const bot_frames_update_t *p)
{
    int pos = 0, thislen;
    int64_t hash = __bot_frames_update_t_get_hash();

    thislen = __int64_t_encode_array(buf, offset + pos, maxlen - pos, &hash, 1);
    if (thislen < 0) return thislen; else pos += thislen;

    thislen = __bot_frames_update_t_encode_array(buf, offset + pos, maxlen - pos, p, 1);
    if (thislen < 0) return thislen; else pos += thislen;

    return pos;
}

int __bot_frames_update_t_encoded_array_size(const bot_frames_update_t *p, int elements)
{
    int size = 0, element;
    for (element = 0; element < elements; element++) {

        size += __int64_t_encoded_array_size(&(p[element].utime), 1);

        size += __string_encoded_array_size(&(p[element].frame), 1);

        size += __string_encoded_array_size(&(p[element].relative_to), 1);

        size += __double_encoded_array_size(p[element].trans, 3);

        size += __double_encoded_array_size(p[element].quat, 4);

    }
    return size;
}

int bot_frames_update_t_encoded_size(const bot_frames_update_t *p)
{
    return 8 + __bot_frames_update_t_encoded_array_size(p, 1);
}

int __bot_frames_update_t_decode_array(const void *buf, int offset, int maxlen, bot_frames_update_t *p, int elements)
{
    int pos = 0, thislen, element;

    for (element = 0; element < elements; element++) {

        thislen = __int64_t_decode_array(buf, offset + pos, maxlen - pos, &(p[element].utime), 1);
        if (thislen < 0) return thislen; else pos += thislen;

        thislen = __string_decode_array(buf, offset + pos, maxlen - pos, &(p[element].frame), 1);
        if (thislen < 0) return thislen; else pos += thislen;

        thislen = __string_decode_array(buf, offset + pos, maxlen - pos, &(p[element].relative_to), 1);
        if (thislen < 0) return thislen; else pos += thislen;

        thislen = __double_decode_array(buf, offset + pos, maxlen - pos, p[element].trans, 3);
        if (thislen < 0) return thislen; else pos += thislen;

        thislen = __double_decode_array(buf, offset + pos, maxlen - pos, p[element].quat, 4);
        if (thislen < 0) return thislen; else pos += thislen;

    }
    return pos;
}

int __bot_frames_update_t_decode_array_cleanup(bot_frames_update_t *p, int elements)
{
    int element;
    for (element = 0; element < elements; element++) {

        __int64_t_decode_array_cleanup(&(p[element].utime), 1);

        __string_decode_array_cleanup(&(p[element].frame), 1);

        __string_decode_array_cleanup(&(p[element].relative_to), 1);

        __double_decode_array_cleanup(p[element].trans, 3);

        __double_decode_array_cleanup(p[element].quat, 4);

    }
    return 0;
}

int bot_frames_update_t_decode(const void *buf, int offset, int maxlen, bot_frames_update_t *p)
{
    int pos = 0, thislen;
    int64_t hash = __bot_frames_update_t_get_hash();

    int64_t this_hash;
    thislen = __int64_t_decode_array(buf, offset + pos, maxlen - pos, &this_hash, 1);
    if (thislen < 0) return thislen; else pos += thislen;
    if (this_hash != hash) return -1;

    thislen = __bot_frames_update_t_decode_array(buf, offset + pos, maxlen - pos, p, 1);
    if (thislen < 0) return thislen; else pos += thislen;

    return pos;
}

int bot_frames_update_t_decode_cleanup(bot_frames_update_t *p)
{
    return __bot_frames_update_t_decode_array_cleanup(p, 1);
}

int __bot_frames_update_t_clone_array(const bot_frames_update_t *p, bot_frames_update_t *q, int elements)
{
    int element;
    for (element = 0; element < elements; element++) {

        __int64_t_clone_array(&(p[element].utime), &(q[element].utime), 1);

        __string_clone_array(&(p[element].frame), &(q[element].frame), 1);

        __string_clone_array(&(p[element].relative_to), &(q[element].relative_to), 1);

        __double_clone_array(p[element].trans, q[element].trans, 3);

        __double_clone_array(p[element].quat, q[element].quat, 4);

    }
    return 0;
}

bot_frames_update_t *bot_frames_update_t_copy(const bot_frames_update_t *p)
{
    bot_frames_update_t *q = (bot_frames_update_t*) malloc(sizeof(bot_frames_update_t));
    __bot_frames_update_t_clone_array(p, q, 1);
    return q;
}

void bot_frames_update_t_destroy(bot_frames_update_t *p)
{
    __bot_frames_update_t_decode_array_cleanup(p, 1);
    free(p);
}

int bot_frames_update_t_publish(lcm_t *lc, const char *channel, const bot_frames_update_t *p)
{
      int max_data_size = bot_frames_update_t_encoded_size (p);
      uint8_t *buf = (uint8_t*) malloc (max_data_size);
      if (!buf) return -1;
      int data_size = bot_frames_update_t_encode (buf, 0, max_data_size, p);
      if (data_size < 0) {
          free (buf);
          return data_size;
      }
      int status = lcm_publish (lc, channel, buf, data_size);
      free (buf);
      return status;
}

struct _bot_frames_update_t_subscription_t {
    bot_frames_update_t_handler_t user_handler;
    void *userdata;
    lcm_subscription_t *lc_h;
};
static
void bot_frames_update_t_handler_stub (const lcm_recv_buf_t *rbuf,
                            const char *channel, void *userdata)
{
    int status;
    bot_frames_update_t p;
    memset(&p, 0, sizeof(bot_frames_update_t));
    status = bot_frames_update_t_decode (rbuf->data, 0, rbuf->data_size, &p);
    if (status < 0) {
        fprintf (stderr, "error %d decoding bot_frames_update_t!!!\n", status);
        return;
    }

    bot_frames_update_t_subscription_t *h = (bot_frames_update_t_subscription_t*) userdata;
    h->user_handler (rbuf, channel, &p, h->userdata);

    bot_frames_update_t_decode_cleanup (&p);
}

bot_frames_update_t_subscription_t* bot_frames_update_t_subscribe (lcm_t *lcm,
                    const char *channel,
                    bot_frames_update_t_handler_t f, void *userdata)
{
    bot_frames_update_t_subscription_t *n = (bot_frames_update_t_subscription_t*)
                       malloc(sizeof(bot_frames_update_t_subscription_t));
    n->user_handler = f;
    n->userdata = userdata;
    n->lc_h = lcm_subscribe (lcm, channel,
                                 bot_frames_update_t_handler_stub, n);
    if (n->lc_h == NULL) {
        fprintf (stderr,"couldn't reg bot_frames_update_t LCM handler!\n");
        free (n);
        return NULL;
    }
    return n;
}

int bot_frames_update_t_subscription_set_queue_capacity (bot_frames_update_t_subscription_t* subs,
                              int num_messages)
{
    return lcm_subscription_set_queue_capacity (subs->lc_h, num_messages);
}

int bot_frames_update_t_unsubscribe(lcm_t *lcm, bot_frames_update_t_subscription_t* hid)
{
    int status = lcm_unsubscribe (lcm, hid->lc_h);
    if (0 != status) {
        fprintf(stderr,
           "couldn't unsubscribe bot_frames_update_t_handler %p!\n", hid);
        return -1;
    }
    free (hid);
    return 0;
}

