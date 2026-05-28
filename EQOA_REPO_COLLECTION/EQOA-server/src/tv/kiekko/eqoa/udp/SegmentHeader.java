package tv.kiekko.eqoa.udp;

import io.netty.buffer.ByteBuf;
import tv.kiekko.eqoa.Util;

public class SegmentHeader {
	int flags;
	int size;
	int instance;
	int data_0x40000;
	long src_addr;
	long local_endpoint;
	long remote_endpoint;
	
	public SegmentHeader() {
		
	}
	
	
	static boolean isLocalHwEndpoint(long ep) {
		if (ep==0xffffffffL)
			return false;
		return ((ep & 0xffff0000) == 0xffff0000);
	}

	static boolean isSoftwareEndpoint(long ep) {
		return !(0xfffff7ffL < ep - 0x800);
	}
	
	public void read(ByteBuf buf,long src_addr,long endpoint) {
		int flags_len=(int)Util.readPacked32(buf);
		flags=flags_len & 0xfffff800;
		size=flags_len & 0x7ff;
		if ((flags & 0x2000) != 0) instance=buf.readInt();
		if ((flags & 0x40000) != 0) data_0x40000=buf.readInt();
		if ((flags & 0x8000) == 0) this.src_addr=src_addr;
		else this.src_addr=buf.readLong();
		if ((flags & 0x1000) == 0) local_endpoint=Util.readPacked64(buf);
		else local_endpoint = this.src_addr;
		if ((flags & 0x800) == 0) remote_endpoint=endpoint;
		else remote_endpoint=Util.readPacked64(buf);
	}
	
	public void write(ByteBuf buf) {
		if (isLocalHwEndpoint(local_endpoint)) {
			flags |= 0x1000;
		}
		Util.writePacked32(buf,flags | size);
		if ((flags & 0x2000)!=0) { 
			buf.writeInt(instance);
		}
		if ((flags & 0x40000) != 0) {
			buf.writeInt(data_0x40000);
		}
		if ((flags&0x8000)!=0) {
			buf.writeLong(src_addr);
		}
		if ((~flags & 0x1000) != 0) {
			Util.writePacked64(buf,local_endpoint);
		}
		if ((flags & 0x800)!=0) {
			Util.writePacked64(buf,remote_endpoint);
		}
	}
	
	
	public String toString() {
		return String.format("[SegmentHeader flags=%x body_size=%x instance=%x data_0x40000=%x src_addr=%x local_ep=%x remote_ep=%x]",
				flags,size,instance,data_0x40000,src_addr,local_endpoint,remote_endpoint);
	}

	
	
}
