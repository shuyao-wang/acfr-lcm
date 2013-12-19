/* LCM type definition class file
 * This file was automatically generated by lcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package bot_core;
 
import java.io.*;
import java.nio.*;
import java.util.*;
import lcm.lcm.*;
 
public final class raw_t implements lcm.lcm.LCMEncodable
{
    public long utime;
    public int length;
    public byte data[];
 
    public raw_t()
    {
    }
 
    public static final long LCM_FINGERPRINT;
    public static final long LCM_FINGERPRINT_BASE = 0x182b8da2dc0260c7L;
 
    static {
        LCM_FINGERPRINT = _hashRecursive(new ArrayList<Class>());
    }
 
    public static long _hashRecursive(ArrayList<Class> classes)
    {
        if (classes.contains(bot_core.raw_t.class))
            return 0L;
 
        classes.add(bot_core.raw_t.class);
        long hash = LCM_FINGERPRINT_BASE
            ;
        classes.remove(classes.size() - 1);
        return (hash<<1) + ((hash>>63)&1);
    }
 
    public void encode(DataOutput outs) throws IOException
    {
        outs.writeLong(LCM_FINGERPRINT);
        _encodeRecursive(outs);
    }
 
    public void _encodeRecursive(DataOutput outs) throws IOException
    {
        outs.writeLong(this.utime); 
 
        outs.writeInt(this.length); 
 
        if (this.length > 0)
            outs.write(this.data, 0, length);
 
    }
 
    public raw_t(byte[] data) throws IOException
    {
        this(new LCMDataInputStream(data));
    }
 
    public raw_t(DataInput ins) throws IOException
    {
        if (ins.readLong() != LCM_FINGERPRINT)
            throw new IOException("LCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static bot_core.raw_t _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        bot_core.raw_t o = new bot_core.raw_t();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        this.utime = ins.readLong();
 
        this.length = ins.readInt();
 
        this.data = new byte[(int) length];
        ins.readFully(this.data, 0, length); 
    }
 
    public bot_core.raw_t copy()
    {
        bot_core.raw_t outobj = new bot_core.raw_t();
        outobj.utime = this.utime;
 
        outobj.length = this.length;
 
        outobj.data = new byte[(int) length];
        if (this.length > 0)
            System.arraycopy(this.data, 0, outobj.data, 0, this.length); 
        return outobj;
    }
 
}

