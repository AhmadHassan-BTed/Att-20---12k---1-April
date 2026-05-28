package tv.kiekko.eqoa.udp;

import java.nio.ByteOrder;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import tv.kiekko.eqoa.Util;
import tv.kiekko.eqoa.udp.Connection.StateChannel;


public class Message {
        ByteBuf data;
        byte type;
        Connection connection;
        long local_endpoint;
        long remote_endpoint;
        int size;
        short seqnum;
        long time;              // last (re)sent
        long created;           // first sent
        int refnum;
        short flush;

        // things break if you send the same Message to several connections (without cloning the object etc),
        // this is just to detect any bugs like that
  
        Connection sentTo;

  
        public void setType(int i) {
                type=(byte)i;
        }
        
        public Message clone() {
                Message m=new Message(null,local_endpoint,remote_endpoint,data,size,type,seqnum,time);
                m.created=this.created;
                m.refnum=this.refnum;
                m.flush=this.flush;
                return m;
        }

  
        public Message(Connection connection, long local_endpoint, long remote_endpoint, ByteBuf data,int size, byte type, short seqnum, long time) {
                this.connection=connection;
                this.local_endpoint=local_endpoint;
                this.remote_endpoint=remote_endpoint;
                this.data=data;
                this.size=size;
                this.type=type;
                this.seqnum=seqnum;
                this.time=time;
                this.data=data;
                created=time;
        }

        public Message(ByteBuf data,int type) {
                this(data,data.writerIndex(),(byte)type,(short)0);
                data.readerIndex(0);
                if (data.writerIndex()==0 && type!=0x40) {
                        new Exception("probably fail message? size=0").printStackTrace();
                }
                created=System.currentTimeMillis();
        }
        
        public Message(ByteBuf data,int size,byte type,short seqnum) {
                this.data=data;
                this.size=size;
                this.type=type;
                this.seqnum=seqnum;
                created=System.currentTimeMillis();
        }
  
        public ByteBuf getData() {
                return data;
        }
        
        public Message duplicate() {
                Message m=new Message(null,local_endpoint,remote_endpoint,data,size,type,seqnum,time);
                m.refnum=refnum;
                m.flush=flush;
                return m;
        }
        
  
        // Netty memory management concerns
  
        boolean mustRelease=false;
        
        public void setData(ByteBuf data) {
                if (this.data!=null) this.data.release();
                this.data=data;
                mustRelease=true;
        }
        
        public void release() {
                if (mustRelease && this.data!=null) {
                        this.data.release();
                        this.data=null;
                }
        }

        @Override
        protected void finalize() throws Throwable {
                release();
                super.finalize();
        }


        public String toShortString() {
                return "[Message("+String.format("%02x",type)+"#"+seqnum+(data==null?"":",read="+data.readerIndex()+"/"+data.writerIndex())+")]";
        }

        public void write(Connection con,ByteBuf buf,List<Message> msg_list,short flush_tx) {
                if (isState()) writeState(con,buf,flush_tx);
                else writeMessage(buf,flush_tx);
        }
        
        void Log(String s) {
                if (connection!=null) connection.Log(s);
                else UDPServer.Log(s);
        }
  
        public void writeMessage(ByteBuf buf,short flush_tx) {
                int uVar1 = (int)(type + 7) & 0xff;
                buf.writeByte(type);
                Util.writeSize(buf,(short)size);
                if (uVar1 < 3) {
                        buf.writeShort(seqnum);
                }
                buf.writeBytes(data,0,data.writerIndex());
                if (uVar1 < 3) {
                  // todo something here
                }
                this.flush=flush_tx;  // ?
        }

  
        static void seqnum_remove_thru(List<Message> vlist,int sn) {
                sn&=0xffff;
                int removed=0;
                while(!vlist.isEmpty()) {
                        Message msg=vlist.get(0);
                        int mseq=(int)msg.seqnum;
                        if (((short)mseq-(sn&0xffff))*0x10000<0) {
                                vlist.remove(msg);
                                removed++;
                        } else {
                                return;
                        }
                }
        }

        public boolean isState() {
                return (((int)type)&0xff) < 0xf8;
        }
  
        // don't ask me what this is
        // but it works
  
        public void writeState(Connection con,ByteBuf buf,short flush_tx) {
            synchronized(con.messages_out) {
                    int _type = type&255;
                    int local_b4 = 0;
                    int local_ac = size&0xffff;
                    StateChannel stc = con.stateChannels[_type];
                    short stc_seqnum = stc.seqnum;
                    short _seqnum = (short)(stc_seqnum + 1);
                    if (_seqnum-stc.seqnum2 != 0x1000) {
                        List<Message> vlist = stc.list1;
                        if (stc.unknown6 == 1) {
                                stc.seqnum = _seqnum;
                                this.seqnum=stc_seqnum;
                                ByteBuf data=this.data;
                                int size = this.size;
                                _type=this.type;
                                _seqnum=this.seqnum;
                                long time = System.currentTimeMillis();
                                Message msg=new Message(con,con.local_endpoint,con.remote_endpoint,data,size,(byte)_type,_seqnum,time);
                                vlist.add(msg);
                        }
        
                        _seqnum=this.seqnum;
                        seqnum_remove_thru(vlist,_seqnum-0x20&0xffff);
                        ByteBuf xorred=null;
                        if (!vlist.isEmpty()) {
                                Message msg2 = vlist.get(0);
                                short msg2_seqnum=msg2.seqnum;
                                if (msg2_seqnum == stc.seqnum2) {
                                        local_b4=(int)_seqnum-(int)msg2_seqnum&0xff;
                                        int msg_size = this.size;
                                        int msg2_size = msg2.size;
                                        Message _msg2 = msg2;
                                        if (msg_size < msg2_size) {
                                                _msg2 = this;
                                        }
                                        int size = _msg2.size;
                                        ByteBuf data = this.data;
                                        ByteBuf src2 = msg2.data;
                                        xorred=xor_data2(data,src2,size);
                                        ByteBuf iVar3 = this.data;
                                        int iVar4 = this.size;
                                        try {
                                                xorred.writerIndex(size);
                                                xorred.writeBytes(iVar3.slice(size,iVar4-size),iVar4-size);
                                        } catch(Throwable t) {
                                                t.printStackTrace();
                                                System.err.println("xorred fail size="+size+" iVar4="+iVar4+" iVar3="+iVar3);
                                        }
                                } else {
                                        xorred=data;
                                }
                        } else {
                                Log("empty xorbuf?");
                                xorred=Unpooled.buffer(local_ac);
                        }
                        Message uVar4=this;
                        buf.writeByte(_type);
                        Util.writeSize(buf,(short)local_ac);
                        buf.writeShort(_seqnum);
                        buf.writeByte(local_b4);
                        ByteBuf rle=runLengthEncode(xorred,local_ac);
                        buf.writeBytes(rle);
                        stc.unknown6 = 2;
                        uVar4.time=System.currentTimeMillis();
                        uVar4.flush=flush_tx;
                        con.unacked_messages.add(uVar4);
                        return;
                  }
            } // synchronized
            Log("writeState failed, terminate?");
        }

  
        static ByteBuf xor_data2(ByteBuf src1,ByteBuf src2, int size) {
                ByteBuf ret=Unpooled.buffer(size);
                for (int i=0; i<size; i++) {
                        ret.setByte(i,src1.getByte(i)^src2.getByte(i));
                }
                return ret;
        }

  
  
  
  
  /*
  
  Run length encoding is used when sending State channels as follows (comment written from memory so might not be 100% correct):
  
  - Zero byte means end.

  - If the byte has the highest bit set (0x80), then the rest of the byte is the number of bytes to copy,
  and the next byte contains the number of zeroes.

  - If the highest bit is not set, then the high nibble is the number of bytes to copy,
  and the low nibble is the number of zeroes.
  
  Zeroes are inserted first, bytes copied after that.
  
  */
  
  
  
  static ByteBuf runLengthEncode(ByteBuf buf,int len) {
    ByteBuf ret=Unpooled.buffer(64).order(ByteOrder.LITTLE_ENDIAN);
    int start=buf.readerIndex();
    int end=start+len;
    int i=start;
    int j;
    while(i<end) {
      j=i;
      while(j < end && buf.getByte(j) == 0 && j-i < 0x7f)
        j++;
      int zeroes=j-i;
      while(j < end && (buf.getByte(j) != 0 || j == end-1 || buf.getByte(j+1) != 0) && j-i-zeroes < 0x7f)
        j++;
      int copy=j-zeroes-i;
      if (copy == 0 && zeroes == 0)
        break;
      ret.writeByte(copy|0x80);
      ret.writeByte(zeroes);
      ByteBuf slice = buf.slice(i+zeroes,copy);
      ret.writeBytes(slice);
      i+=copy+zeroes;
    }
    ret.writeByte(0);
    return ret;
  }

}
