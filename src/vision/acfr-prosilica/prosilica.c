#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <linux/limits.h>
#include <unistd.h>
#include <pthread.h>

#include <PvApi.h>
#include <PvRegIo.h>

#include "perls-common/timeutil.h"
#include "perls-common/timestamp.h"

#include "prosilica.h"

const char *
prosilica_error_string (tPvErr err)
{
    static char errstr[32];
    switch (err)
    {
    case ePvErrSuccess:
        sprintf (errstr, "ePvErrSuccess");
        break;
    case ePvErrCameraFault:
        sprintf (errstr, "ePvErrCameraFault");
        break;
    case ePvErrInternalFault:
        sprintf (errstr, "ePvErrInternalFault");
        break;
    case ePvErrBadHandle:
        sprintf (errstr, "ePvErrBadHandle");
        break;
    case ePvErrBadParameter:
        sprintf (errstr, "ePvErrBadParameter");
        break;
    case ePvErrBadSequence:
        sprintf (errstr, "ePvErrBadSequence");
        break;
    case ePvErrBandwidth:
        sprintf (errstr, "ePvErrBandwidth");
        break;
    case ePvErrNotFound:
        sprintf (errstr, "ePvErrNotFound");
        break;
    case ePvErrAccessDenied:
        sprintf (errstr, "ePvErrAccessDenied");
        break;
    case ePvErrUnplugged:
        sprintf (errstr, "ePvErrUnplugged");
        break;
    case ePvErrInvalidSetup:
        sprintf (errstr, "ePvErrInvalidSetup");
        break;
    case ePvErrResources:
        sprintf (errstr, "ePvErrResources");
        break;
    case ePvErrQueueFull:
        sprintf (errstr, "ePvErrQueueFull");
        break;
    case ePvErrBufferTooSmall:
        sprintf (errstr, "ePvErrBufferTooSmall");
        break;
    case ePvErrCancelled:
        sprintf (errstr, "ePvErrCancelled");
        break;
    case ePvErrDataLost:
        sprintf (errstr, "ePvErrDataLost");
        break;
    case ePvErrDataMissing:
        sprintf (errstr, "ePvErrDataMissing");
        break;
    case ePvErrTimeout:
        sprintf (errstr, "ePvErrTimeout");
        break;
    case ePvErrOutOfRange:
        sprintf (errstr, "ePvErrOutOfRange");
        break;
    case ePvErrWrongType:
        sprintf (errstr, "ePvErrWrongType");
        break;
    case ePvErrForbidden:
        sprintf (errstr, "ePvErrForbidden");
        break;
    case ePvErrUnavailable:
        sprintf (errstr, "ePvErrUnavailable");
        break;
    case ePvErrFirewall:
        sprintf (errstr, "ePvErrFirewall");
        break;
    default:
        sprintf (errstr, "unknown");
    }
    return errstr;
}

int
prosilica_get_timestamp (tPvHandle handle, unsigned long *TimestampLo, unsigned long *TimestampHi)
{
    /* Timestamp example code from url below
     * http://www.alliedvisiontec.com/fileadmin/content/PDF/Support/Application_Notes/ApplNote-timestamp.pdf
     */
    unsigned long control_address = 0x0944;
    unsigned long latch_data = 0x0001 << 1;
    unsigned long ts_address[2] = {0x0948, 0x094C};
    unsigned long ts_data[2];
    unsigned long num_complete;
    tPvErr err;


    // latch
    err = PvRegisterWrite (handle, 1, &control_address, &latch_data, &num_complete);
    if (err != ePvErrSuccess)
    {
        PROSILICA_ERROR (err, "PvRegisterWrite(latch_data)");
        return -1;
    }

    // read timestamp
    err = PvRegisterRead (handle, 2, &ts_address[0], &ts_data[0], &num_complete);
    if (err != ePvErrSuccess)
    {
        PROSILICA_ERROR (err, "PvRegisterRead(ts_data)");
        return -1;
    }

    *TimestampHi = ts_data[0];
    *TimestampLo = ts_data[1];
    return 0;
}


pvattribute_t
prosilica_get_attribute (tPvHandle handle, const char* label)
{
    // init pvattribute
    pvattribute_t pvatt =
    {
        .datatype = ePvDatatypeUnknown,
        .label = "",
        .value = ""
    };

    tPvAttributeInfo info;
    tPvErr err = PvAttrInfo (handle, label, &info);
    if (err != ePvErrSuccess)
    {
        PROSILICA_ERROR (err, "PvAttrInfo(%s)", label);
        return pvatt;
    }
    else
    {
        pvatt.datatype = info.Datatype;
        strncpy (pvatt.label, label, 256);
    }

    if (pvatt.datatype == ePvDatatypeCommand)
        return pvatt;

    err = PvAttrIsAvailable (handle, label);
    if (err != ePvErrSuccess)
    {
        PROSILICA_ERROR (err, "PvAttrIsAvailable(%s)", label);
        return pvatt;
    }

    char value[256] = "";
    switch (pvatt.datatype)
    {
    case ePvDatatypeString:
        err = PvAttrStringGet (handle, label, value, 256, NULL);
        if (err != ePvErrSuccess)
        {
            PROSILICA_ERROR (err, "PvAttrStringGet(%s)", label);
            return pvatt;
        }
        break;
    case ePvDatatypeEnum:
        err = PvAttrEnumGet (handle, label, value, 256, NULL);
        if (err != ePvErrSuccess)
        {
            PROSILICA_ERROR (err, "PvAttrEnumGet(%s)", label);
            return pvatt;
        }
        break;
    case ePvDatatypeUint32:
    {
        tPvUint32 lvalue;
        err = PvAttrUint32Get (handle, label, &lvalue);
        if (err == ePvErrSuccess)
            sprintf (value, "%lu", lvalue);
        else
        {
            PROSILICA_ERROR (err, "PvAttrUint32Get(%s)", label);
            return pvatt;
        }
        break;
    }
    case ePvDatatypeFloat32:
    {
        tPvFloat32 fvalue;
        err = PvAttrFloat32Get (handle, label, &fvalue);
        if (err == ePvErrSuccess)
            sprintf (value, "%g", fvalue);
        else
        {
            PROSILICA_ERROR (err, "PvAttrFloat32Get(%s)", label);
            return pvatt;
        }
        break;
    }
    default:
        return pvatt;
    } // switch (info.Datatype)
    strncpy (pvatt.value, value, 256);

    return pvatt;
}


int
prosilica_set_attribute (tPvHandle handle, const char* label, const char* value)
{
    tPvAttributeInfo info;
    tPvErr err = PvAttrInfo (handle, label, &info);
    if (err)
    {
        PROSILICA_ERROR (err, "PvAttrInfo(%s)", label);
        return -1;
    }

    err = PvAttrIsAvailable (handle, label);
    if (err != ePvErrSuccess)
    {
        PROSILICA_ERROR (err, "PvAttrIsAvailable(%s)", label);
        return -1;
    }

    switch (info.Datatype)
    {
    case ePvDatatypeCommand:
    {
        err = PvCommandRun (handle, label);
        if (err == ePvErrSuccess)
            return 0;
        else
        {
            PROSILICA_ERROR (err, "PvCommandRun(%s)", label);
            return -1;
        }
    }
    case ePvDatatypeString:
    {
        err = PvAttrStringSet (handle, label, value);
        if (err == ePvErrSuccess)
            return 0;
        else
        {
            PROSILICA_ERROR (err, "PvAttrStringSet(%s)", label);
            return -1;
        }
    }
    case ePvDatatypeEnum:
    {
        err = PvAttrEnumSet (handle, label, value);
        if (err == ePvErrSuccess)
            return 0;
        else
        {
            PROSILICA_ERROR (err, "PvAttrEnumSet(%s) = %s", label, value);
            return -1;
        }
    }
    case ePvDatatypeUint32:
    {
        tPvUint32 lvalue = atol (value);
        tPvUint32 lmin, lmax;
        err = PvAttrRangeUint32 (handle, label, &lmin, &lmax);
        if (err == ePvErrSuccess)
        {
            if (lvalue < lmin)
            {
                PROSILICA_ERROR (NULL, "desired %s=%u exceeds PvAttrRangeUint32 [%u,%u]; clipping",
                                 label, (int) lvalue, (int) lmin, (int) lmax);
                lvalue = lmin;
            }
            else if (lmax < lvalue)
            {
                PROSILICA_ERROR (NULL, "desired %s=%u exceeds PvAttrRangeUint32 [%u,%u]; clipping",
                                 label, (int) lvalue, (int) lmin, (int) lmax);
                lvalue = lmax;
            }

            err = PvAttrUint32Set (handle, label, lvalue);
            if (err == ePvErrSuccess)
                return 0;
            else
            {
                PROSILICA_ERROR (err, "PvAttrUint32Set(%s)", label);
                return -1;
            }
        }
        else
        {
            PROSILICA_ERROR (err, "PvAttrRangeUint32(%s)", label);
            return -1;
        }
    }
    case ePvDatatypeFloat32:
    {
        tPvFloat32 fvalue = atof (value);
        tPvFloat32 fmin, fmax;
        err = PvAttrRangeFloat32 (handle, label, &fmin, &fmax);
        if (err == ePvErrSuccess)
        {
            if (fvalue < fmin)
            {
                PROSILICA_ERROR (NULL, "desired %s=%g exceeds PvAttrRangeFloat32 [%g,%g]; clipping",
                                 label, (float) fvalue, (float) fmin, (float) fmax);
                fvalue = fmin;
            }
            else if (fmax < fvalue)
            {
                PROSILICA_ERROR (NULL, "desired %s=%g exceeds PvAttrRangeFloat32 [%g,%g]; clipping",
                                 label, (float) fvalue, (float) fmin, (float) fmax);
                fvalue = fmax;
            }

            err = PvAttrFloat32Set (handle, label, fvalue);
            if (err == ePvErrSuccess)
                return 0;
            else
            {
                PROSILICA_ERROR (err, "PvAttrFloat32Set(%s)", label);
                return -1;
            }
        }
        else
        {
            PROSILICA_ERROR (err, "PvAttrRangeFloat32(%s)", label);
            return -1;
        }
    }
    default:
        return -1;
    } // switch (info.Datatype)
}

senlcm_prosilica_t *
prosilica_get_pvattributes (tPvHandle handle)
{
    senlcm_prosilica_t *pvatt = malloc (sizeof (senlcm_prosilica_t));
    pvatt->self = 1;

    tPvAttrListPtr listptr;
    unsigned long  listlen;
    tPvErr err = PvAttrList (handle, &listptr, &listlen);
    if (err != ePvErrSuccess)
    {
        PROSILICA_ERROR (err, "PvAttrList()");
        free (pvatt);
        return NULL;
    }

    pvatt->n_attributes = 0;
    pvatt->PvAttributes = calloc (listlen, sizeof (senlcm_prosilica_attribute_t));
    for (int i=0; i < (int) listlen; i++)
    {
        const char *label = listptr[i];
        pvattribute_t att = prosilica_get_attribute (handle, label);

        if (att.datatype == ePvDatatypeString || att.datatype == ePvDatatypeEnum ||
                att.datatype == ePvDatatypeUint32 || att.datatype == ePvDatatypeFloat32)
        {

            pvatt->PvAttributes[pvatt->n_attributes].label = strdup (att.label);
            pvatt->PvAttributes[pvatt->n_attributes].value = strdup (att.value);
            pvatt->n_attributes++;
        }
    }

    return pvatt;
}


void
prosilica_set_pvattributes (tPvHandle handle, const senlcm_prosilica_t *pvatt)
{
    for (int i=0; i<pvatt->n_attributes; i++)
        prosilica_set_attribute (handle, pvatt->PvAttributes[i].label, pvatt->PvAttributes[i].value);
}

int
prosilica_load_attfile (tPvHandle handle, const char *attfile)
{
    FILE *file = fopen (attfile, "r");
    if (!file)
        return -1;

    int ret = 0, linenum = 0;
    char line[256];
    while (!feof (file))
    {
        if (fgets (line, sizeof line, file))
        {
            linenum++;

            // skip comments
            char *comment = strchr (line, '#');
            if (comment == line)
                continue;

            // skip whitespace
            int ret = 0;
            for (size_t i=0; i<strlen (line); i++)
                if (ret += isgraph (line[i]))
                    break;
            if (!ret)
                continue;

            // parse and set attribute
            char *equal = strchr (line, '='), label[256], value[256];
            if (equal && 1==sscanf (line, "%[^ =]s", label) && 1==sscanf (equal+1, "%s", value))
            {
                if (prosilica_set_attribute (handle, label, value))
                {
                    ret--;
                    PROSILICA_ERROR (NULL, "%s:%d:%s", attfile, linenum, line);
                }
            }
            else
                PROSILICA_ERROR (NULL, "error parsing: %s", line);
        }
    }
    fclose (file);
    return ret;
}

int
prosilica_load_config (tPvHandle handle, int index)
{
    char ConfigFileIndex[16];
    switch (index)
    {
    case 0:
        sprintf (ConfigFileIndex, "Factory");
        break;
    case 1:
        sprintf (ConfigFileIndex, "1");
        break;
    case 2:
        sprintf (ConfigFileIndex, "2");
        break;
    case 3:
        sprintf (ConfigFileIndex, "3");
        break;
    case 4:
        sprintf (ConfigFileIndex, "4");
        break;
    case 5:
        sprintf (ConfigFileIndex, "5");
        break;
    default:
        PROSILICA_ERROR (NULL, "unrecognized ConfigFileIndex");
        return -1;
    }

    tPvErr err;
    err = PvAttrEnumSet (handle, "ConfigFileIndex", ConfigFileIndex);
    if (err != ePvErrSuccess)
    {
        PROSILICA_ERROR (err, "PvAttrEnumSet()");
        return -1;
    }

    err = PvCommandRun (handle, "ConfigFileLoad");
    if (err != ePvErrSuccess)
    {
        PROSILICA_ERROR (err, "PvCommandRun");
        return -1;
    }
    timeutil_usleep (100e3);

    return 0;
}


int
prosilica_camera_online (unsigned long uid)
{
    tPvCameraInfo list[10];
    int ncameras = PvCameraList (list, 10, NULL);
    for (size_t i=0; i<ncameras; i++)
        if (list[i].UniqueId == uid)
            return 1;

    return 0;
}

int
prosilica_bandwidth_used (tPvHandle handle, prosilica_bandwidth_t *bw)
{
    tPvErr err;

    err = PvAttrUint32Get (handle, "StreamBytesPerSecond", &bw->StreamBytesPerSecond);
    if (err != ePvErrSuccess)
    {
        PROSILICA_ERROR (err, "PvAttrUint32Set()");
        return -1;
    }

    tPvUint32 TotalBytesPerFrame;
    err = PvAttrUint32Get (handle, "TotalBytesPerFrame", &TotalBytesPerFrame);
    if (err != ePvErrSuccess)
    {
        PROSILICA_ERROR (err, "PvAttrUint32Get()");
        return -1;
    }

    float FrameRate;
    err = PvAttrFloat32Get (handle, "FrameRate", &FrameRate);
    if (err != ePvErrSuccess)
    {
        PROSILICA_ERROR (err, "PvAttrFloat32Get()");
        return -1;
    }

    bw->Used = FrameRate * TotalBytesPerFrame;
    bw->Percentage = (double) bw->Used / (double) bw->StreamBytesPerSecond * 100.;

    return 0;
}

int
prosilica_multicam (tPvHandle handle, int ncameras)
{
    tPvErr err;

    tPvUint32 sbps_min, sbps_max;
    err = PvAttrRangeUint32 (handle, "StreamBytesPerSecond", &sbps_min, &sbps_max);
    if (err != ePvErrSuccess)
    {
        PROSILICA_ERROR (err, "PvAttrRangeUint32()");
        return -1;
    }

    tPvUint32 sbps = sbps_max / ncameras;
    err = PvAttrUint32Set (handle, "StreamBytesPerSecond", sbps);
    if (err == ePvErrSuccess)
        return 0;
    else
    {
        PROSILICA_ERROR (err, "PvAttrUint32Set(StreamBytesPerSecond)");
        return -1;
    }
}

tPvFrame *
prosilica_alloc_frames (tPvHandle handle, int nframes)
{
    unsigned long TotalBytesPerFrame;
    tPvErr err = PvAttrUint32Get (handle, "TotalBytesPerFrame", &TotalBytesPerFrame);
    if (err != ePvErrSuccess)
    {
        PROSILICA_ERROR (err, "PvAttrUint32Get()");
        abort ();
    }

    int failed = 0;
    tPvFrame *Frames = malloc (nframes * sizeof (tPvFrame));
    for (int i=0; i<nframes; i++)
    {
        Frames[i].AncillaryBuffer = NULL;
        Frames[i].AncillaryBufferSize = 0;

        Frames[i].ImageBuffer = malloc (TotalBytesPerFrame);
        if (Frames[i].ImageBuffer)
            Frames[i].ImageBufferSize = TotalBytesPerFrame;
        else
        {
            failed = 1;
            break;
        }
    }

    if (failed)
    {
        prosilica_free_frames (Frames, nframes);
        PROSILICA_ERROR (NULL, "unable to malloc %d frames of total size %ld", nframes, nframes*TotalBytesPerFrame);
        return NULL;
    }

    return Frames;
}

tPvFrame *
prosilica_dup_frames (tPvFrame *Frames, int nframes)
{
    int failed = 0;
    tPvFrame *dupFrames = malloc (nframes * sizeof (tPvFrame));
    if (!dupFrames)
        return NULL;

    memcpy (dupFrames, Frames, nframes * sizeof (tPvFrame));
    for (int i=0; i<nframes; i++)
    {
        dupFrames[i].ImageBuffer = malloc (Frames[i].ImageBufferSize);
        dupFrames[i].AncillaryBuffer = malloc (Frames[i].AncillaryBufferSize);
        if (!dupFrames[i].ImageBuffer || !dupFrames[i].AncillaryBuffer)
        {
            failed = 1;
            break;
        }

        memcpy (dupFrames[i].ImageBuffer, Frames[i].ImageBuffer, Frames[i].ImageBufferSize);
        memcpy (dupFrames[i].AncillaryBuffer, Frames[i].AncillaryBuffer, Frames[i].AncillaryBufferSize);
    }

    if (failed)
    {
        prosilica_free_frames (Frames, nframes);
        PROSILICA_ERROR (NULL, "unable to malloc memory for duplicate Frames");
        return NULL;
    }

    return dupFrames;
}


void
prosilica_free_frames (tPvFrame *Frames, int nframes)
{
    for (int i=0; i<nframes; i++)
    {
        free (Frames[i].ImageBuffer);
        free (Frames[i].AncillaryBuffer);
    }
    free (Frames);
}

size_t
prosilica_npixels (tPvFrame *Frame)
{
    if (Frame->Format == ePvFmtMono8  || Frame->Format == ePvFmtBayer8 ||
            Frame->Format == ePvFmtMono16 || Frame->Format == ePvFmtBayer16)
        return Frame->Width*Frame->Height;
    else
        return 3 * Frame->Width*Frame->Height; // rgb, yuv
}

int
prosilica_bitshift_frame (tPvFrame *Frame)
{
    if (Frame->BitDepth == 12)
    {
        size_t npixels = prosilica_npixels (Frame);
        uint16_t *ptr = Frame->ImageBuffer;
        for (int i=0; i<npixels; i++)
            ptr[i]<<=4;
        return 0;
    }
    else
    {
        PROSILICA_ERROR (NULL, "Frame is not 12 bits");
        return -1;
    }
}


int
prosilica_save_tiff (const tPvFrame *Frame, unsigned long uid, const char *dirname, const char *filename,
                     bool color_interpolate, unsigned short compression, unsigned short quality)
{
    // image file name
    char fullname[PATH_MAX];
    snprintf (fullname, sizeof fullname, "%s/%s", dirname, filename);

    // open TIFF file
    TIFF *image = TIFFOpen (fullname, "w");
    if (!image)
    {
        PROSILICA_ERROR (NULL, "TIFFOpen() failed");
        return -1;
    }

    // TIFF buffer pointer
    const int npixels = Frame->Height * Frame->Width;
    tPvImageFormat Format;
    void *ImageBuffer;
    bool freeImageBuffer;
    if (color_interpolate && Frame->Format == ePvFmtBayer8)
    {
        Format = ePvFmtRgb24;
        freeImageBuffer = true;
        ImageBuffer = malloc (3 * npixels * sizeof (uint8_t));
        uint8_t *RGB = ImageBuffer;
        PvUtilityColorInterpolate (Frame, RGB, RGB+1, RGB+2, 2, 0);
    }
    else if (color_interpolate && Frame->Format == ePvFmtBayer16)
    {
        Format = ePvFmtRgb48;
        freeImageBuffer = true;
        ImageBuffer = malloc (3 * npixels * sizeof (uint16_t));
        uint16_t *RGB = ImageBuffer;
        PvUtilityColorInterpolate (Frame, RGB, RGB+1, RGB+2, 2, 0);
    }
    else
    {
        Format = Frame->Format;
        freeImageBuffer = false;
        ImageBuffer = Frame->ImageBuffer;
    }

    // TIFF pixel properties
    size_t stride;
    bool sixteenbit;
    switch (Format)
    {
    case ePvFmtMono8:
    case ePvFmtBayer8:
        TIFFSetField (image, TIFFTAG_SAMPLESPERPIXEL, 1);
        TIFFSetField (image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
        stride = Frame->Width;
        sixteenbit = false;
        break;
    case ePvFmtMono16:
    case ePvFmtBayer16:
        TIFFSetField (image, TIFFTAG_SAMPLESPERPIXEL, 1);
        TIFFSetField (image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
        stride = Frame->Width;
        sixteenbit = true;
        break;
    case ePvFmtRgb24:
        TIFFSetField (image, TIFFTAG_SAMPLESPERPIXEL, 3);
        TIFFSetField (image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
        stride = 3 * Frame->Width;
        sixteenbit = false;
        break;
    case ePvFmtRgb48:
        TIFFSetField (image, TIFFTAG_SAMPLESPERPIXEL, 3);
        TIFFSetField (image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
        stride = 3 * Frame->Width;
        sixteenbit = true;
        break;
    case ePvFmtYuv411:
    case ePvFmtYuv422:
    case ePvFmtYuv444:
    case ePvFmtBgr24:
    case ePvFmtRgba32:
    case ePvFmtBgra32:
    default:
        PROSILICA_ERROR (NULL, "unsupported pixel format: %u", Format);
        return -1;
    }

    // TIFF attributes
    TIFFSetField (image, TIFFTAG_IMAGEWIDTH, Frame->Width);
    TIFFSetField (image, TIFFTAG_IMAGELENGTH, Frame->Height);
    TIFFSetField (image, TIFFTAG_ROWSPERSTRIP, 1);
    TIFFSetField (image, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField (image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    switch (compression)
    {
    case COMPRESSION_JPEG:
        if (sixteenbit)
        {
            TIFFSetField (image, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
            PROSILICA_ERROR (NULL, "tiff jpeg compression does not support 16-bit imagery, saving uncompressed");
        }
        else
        {
            TIFFSetField (image, TIFFTAG_COMPRESSION, COMPRESSION_JPEG);
            TIFFSetField (image, TIFFTAG_JPEGQUALITY, quality);
            TIFFSetField (image, TIFFTAG_ROWSPERSTRIP, 64); /* must be multiple of 8 for jpeg */
        }
        break;
    case COMPRESSION_LZW:
        TIFFSetField (image, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
        TIFFSetField (image, TIFFTAG_PREDICTOR, PREDICTOR_HORIZONTAL);
        break;
    case COMPRESSION_DEFLATE:
        TIFFSetField (image, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
        TIFFSetField (image, TIFFTAG_PREDICTOR, PREDICTOR_HORIZONTAL);
        break;
    default:
        TIFFSetField (image, TIFFTAG_COMPRESSION, compression);
    }

    // TIFF header: camera uid
    char uid_str[128];
    snprintf (uid_str, sizeof uid_str, "%lu", uid);
    TIFFSetField (image, TIFFTAG_MODEL, uid_str);

    // TIFF header: pFrame params
    char description[1024];
    unsigned int BayerPattern = 0;
    if (Format == ePvFmtBayer8 || Format == ePvFmtBayer16)
        BayerPattern = Frame->BayerPattern;
    snprintf (description, sizeof description,
              "%u,%lu,%lu,%lu,%lu,%lu,%lu,%u,%lu,%u,%lu,%lu,%lu",
              Frame->Status, Frame->ImageSize, Frame->AncillarySize, Frame->Width, Frame->Height,
              Frame->RegionX, Frame->RegionY, Frame->Format, Frame->BitDepth, BayerPattern,
              Frame->FrameCount, Frame->TimestampLo, Frame->TimestampHi);
    TIFFSetField (image, TIFFTAG_IMAGEDESCRIPTION, description);

    // write TIFF buffer to disk
    int ret = 0;
    if (sixteenbit)   /*16-bit*/
    {
        TIFFSetField (image, TIFFTAG_BITSPERSAMPLE, 16);
        uint16_t *row_ptr = ImageBuffer;
        for (size_t i=0; i < Frame->Height; i++)
        {
            ret |= TIFFWriteScanline (image, row_ptr, i, 0);
            row_ptr += stride;
        }
    }
    else   /*8-bit*/
    {
        TIFFSetField (image, TIFFTAG_BITSPERSAMPLE, 8);
        uint8_t *row_ptr = ImageBuffer;
        for (size_t i=0; i < Frame->Height; i++)
        {
            ret |= TIFFWriteScanline (image, row_ptr, i, 0);
            row_ptr += stride;
        }
    }
    if (ret < 0)
        PROSILICA_ERROR (NULL, "error writing data to tiff");

    TIFFClose (image);

    if (freeImageBuffer)
        free (ImageBuffer);

    return ret;
}
