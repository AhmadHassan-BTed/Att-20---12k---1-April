package tv.kiekko.eqoa.udp;

import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import tv.kiekko.eqoa.Util;
import tv.kiekko.eqoa.udp.Connection.StateChannel;

// Parsing a segment body into Message objects, handling flags, acks, etc.
// This is where it gets ugly... but as far as I know it's the best implementation that currently exists ;)

public class SegmentBody {
        byte flags;
        int instance_local;
        boolean instance_local_ok;
        boolean arrival_positive;
        short arrival_number;
        short flush_ack;
        long unknown_packed;
        boolean guaranteed_ack_present_maybe;
        short guaranteed_ack;
        long guaranteed_ack_mask;
        short seqnum;
        short flags2;
        List<Message> message_list;
        int unknown3;
        byte[] unknown2;
        StateAck[] stateAcks;
        
        
        static class StateAck {
                byte channel;
                short seqnum;
        }
        

        public String toString() {
                return String.format("[SegmentBody flags=%x arr=%x flush_ack=%x g_ack=%x seq=%x flags2=%x msgs=%d ins=%x ilo="+instance_local_ok+" arri+="+arrival_positive+"]",
                                flags,arrival_number,flush_ack,guaranteed_ack,seqnum,flags2,message_list.size(),instance_local);
        }
        
        public SegmentBody() {
                message_list=new ArrayList<Message>();
                unknown2=new byte[0xf8];
        }
        
        public boolean read(Connection connection,SegmentHeader header,ByteBuf buf) throws Exception {
                int body_start=buf.readerIndex();
                // connection.cstat_r++;
                if (((header.flags & 0x2000) != 0) && (connection !=  null)) {
                        if ((connection.flags & 0x20) == 0) {
                                if ((~header.flags & 0x80000) != 0) {
                                        connection.Log("connection has NOT received an instance... header="+header);
                                } else {
                                if (connection.instance_remote != header.instance) {
                                        connection.Log("instance mismatch: connection.instance_remote="+connection.instance_remote+" header.instance="+header.instance);
                                        connection.instance_remote=header.instance;
                                }
                           }
                        }
                }       
                flags=buf.readByte();
                //connection.Log(String.format("read segment body flags = %02x, have connection="+(connection!=null),flags));
                if ((flags & 0x40)!=0) {
                        instance_local=buf.readInt();
                }
                instance_local_ok=true;
                if (connection != null && ((~connection.flags & 0x40) != 0)) {
                        if ((~flags & 0x40) == 0) {
                                if (instance_local != connection.instance_remote) {
                                        instance_local_ok = false;
                                        connection.Log("instance_local not ok: instance mismatch "+String.format("%x <-> %x",instance_local,connection.instance_remote)); // ???
                                }
                    } else {
                      instance_local_ok = false;
                      //connection.Log("instance_local not ok: flags");
                    }
                }               

                arrival_positive = true;
                arrival_number=buf.readShort();
                
                if (connection != null) {
                        int diff = (short)(arrival_number-connection.arrival_number);
                        if (diff < 1) {
                                if ((diff == 0) || ((1 << ~diff & connection.arrival_history) != 0)) {
                                        connection.Log("discard: arrival number/history");
                                        connection.Log(String.format("diff="+diff+" 1<<~diff = %x history=%x and=%x",1<<~diff,connection.arrival_history,~diff & connection.arrival_history));
                                return false;
                            }
                                if (0x3f < diff) {
                                        //cstat_discarded_old++
                                        connection.Log("discard: old, based on arrival number");
                                return false;
                                }
                                arrival_positive=false;
                                //connection.Log("-> arrival_positive=false");
                        }
                        if (arrival_positive) { // goto workaround
                                boolean gotohack=false;
                                if (diff < 0x1001) {
                                        if (diff < 2) {
                                        //bVar3 = segm_body->flags;
                                        } else {
                                                connection.Log("leader stuff unimplemented");
                                                gotohack=true;
                                        }
                                        gotohack=true;
                                }
                                if (!gotohack) {
                                        connection.Log("increment("+diff+") > CONNECTION_FLUSH...");
                                        connection.Log("this="+this);
                                        connection.Log("header="+header);
                                        // cstat_discarded_corrupt++
                                        return false;
                                }
                        }
                }
                
               
            if ((flags & 1) == 0) {
                        //LAB_004b78fc:
            } else {
                flush_ack=buf.readShort();
                if ((flags & 4) != 0) {
                        unknown_packed=Util.readPacked64(buf);
                        connection.Log("unknown packed "+String.format("%x", unknown_packed));
                }
                if (connection == null) {
                        //bVar3 = segm_body->flags;
                        } else {
                                if (instance_local_ok) {
                                        if (-1 < (short)(flush_ack - connection.flush_tx)) {
                                                //segment is acking a segment that ... flush_ack=32767 connection.flush_tx=-32768                                               
                                                connection.Log("segment is acking a segment that ... flush_ack="+flush_ack+" connection.flush_tx="+connection.flush_tx);
                                                return false;
                                        }
                                }
                        }
              }
                                
                guaranteed_ack_present_maybe=false;
                if ((flags & 2) == 0) {
                    //bVar3 = segm_body->flags;
                } else {
                    guaranteed_ack_present_maybe = instance_local_ok;
                    guaranteed_ack=buf.readShort();
                    if ((flags & 8) != 0) {
                        guaranteed_ack_mask=Util.readPacked64(buf);
                    }
                    
                    if (connection == null) {
                      //bVar3 = segm_body->flags;
                    } else {
                      if (!instance_local_ok) {
                          //bVar3 = segm_body->flags;
                      } else {
                        //uVar1 = segm_body->guaranteed_ack;
                        if (-1 < (int)((guaranteed_ack - connection.guaranteed_tx) * 0x10000)) {
                                connection.Log("guaranteed ack bad: "+guaranteed_ack+" - "+connection.guaranteed_tx);
                                return false;
                        }
                        if ((int)((guaranteed_ack - connection.acked) * 0x10000) < 0) {
                                guaranteed_ack_present_maybe = false;
                                if (arrival_positive)
                                        connection.Log("maybe bad, guaranteed_ack - connection.acked = "+guaranteed_ack+"-"+connection.acked+"="+(guaranteed_ack-connection.acked));
                        }
                        int diff=guaranteed_ack-connection.acked;
                        if (0x1000 < diff) {
                                connection.Log("messages acked > ...");
                                return false;
                        }
                        long uVar6;
                        if (diff < 0x40) {
                          uVar6 = guaranteed_ack_mask >> diff;
                        } else {
                          uVar6 = 0;
                        }
                        if (((guaranteed_ack_mask ^ uVar6) & uVar6) != 0) {
                                guaranteed_ack_present_maybe = false;
                                if (arrival_positive)
                                        connection.Log(String.format("maybe bad, guaranteed_ack_mask=%x, uVar6=%x, xor=%x",guaranteed_ack_mask,uVar6,(guaranteed_ack_mask^uVar6)&uVar6));
                        }
                        if (!arrival_positive) {
                                //
                        } else {
                                if (!guaranteed_ack_present_maybe) {
                                        connection.Log("guaranteed_ack_mask fail: some of the previously acked packets have become 'un-acked'");
                                        connection.Log(String.format("guaranteed_ack=%x",guaranteed_ack));
                                        return false;
                                }
                        }
                      }
                    }
                }
                
                
                int iVar22 = 0;
                if ((flags & 0x10) != 0) {
                    if (stateAcks==null) stateAcks=new StateAck[248];
                    while(true) {
                        int ch=buf.readByte()&0xff;
                        //connection.Log("stateAcks: i="+iVar22+" ch="+String.format("%02x",ch));
                        if (stateAcks[iVar22]==null) stateAcks[iVar22]=new StateAck();
                        stateAcks[iVar22].channel=(byte)ch; // dst
                        if (ch==0xf8) break;
                        if (ch > 0xf8) {
                                connection.Log("stateAcks: illegal channel "+String.format("%x",ch));
                                return false;
                        }
                        if (iVar22 > 0 && ch <= ((int)(stateAcks[iVar22-1].channel)&0xff)) {
                                connection.Log("state acks must come in increasing order");
                                return false;
                        }
                        if (iVar22 == 0xf8) {
                                connection.Log("this should've been the terminating");
                                return false;
                        }
                        stateAcks[iVar22].seqnum=buf.readShort();
                        iVar22++;
                    }
                    if (instance_local_ok) {
                        if (connection.stateChannels==null) {
                                connection.Log("there are no state channels");
                                return false;
                        }
                        int bVar3 = ((int)stateAcks[0].channel)&0xff;
                        int uVar1 = stateAcks[0].seqnum;
                        int i=0;
                        if (bVar3 != 0xf8) {
                                do {
                                        StateChannel stc=connection.stateChannels[bVar3];
                                        int uVar2=stc.seqnum;
                                        if (-1 < (short)(uVar1 - uVar2)) {
                                                connection.Log("can't ack more than we've sent, yo");
                                                return false;
                                        }
                                        if (arrival_positive) {
                                                uVar2=stc.seqnum2;
                                                if ((short)(uVar1-uVar2) < 0) { // do we want (short) cast dunno
                                                        connection.Log("can't ack less than we've already");
                                                        return false;
                                                }
                                        }
                                        i++;
                                        bVar3=((int)stateAcks[i].channel)&255;
                                        uVar1=stateAcks[i].seqnum;
                                } while (bVar3 != 0xf8);
                        }
                    }
                    //connection.Log("stateAcks done");
                }
                
        
                if (connection != null) {
                        
                        if ((~connection.flags & 0x20) != 0) {
                                long _time = System.currentTimeMillis();
                        }
                }
                        
                        int type;
                        int size;

                        while(true) { // goto fix
                        
                        while(true) {
                                if (buf.readerIndex()-body_start >= header.size) { 
                                        if (connection == null) return true; // ok
                                        if (arrival_positive) return true; //ok
                                        connection.Log("buf end, out of sequence?");
                                        return true;
                                }
                                type=buf.readByte()&0xff;
                                size=Util.readSize(buf);
                                if (0x7fe < size) {
                                        connection.Log("size > SEGMENT_HEADER....");
                                        return false;
                                }
                                if (2 < ((type + 7)&0xff)) { break; }
                                // f9, fa, fb
                            seqnum=buf.readShort();
                            flags2 |= 3;
                            if (connection == null) {
                                if (false /*message_list_contains_type_seqnum(message_list,type,seqnum)*/) {
                                        System.out.println("message appears twice");
                                        return false;
                                }
                                    int _time = 0;
                                    Object endpoint = null;
                                    Message msg=new Message(connection,header.local_endpoint,header.remote_endpoint,null,size,(byte)type,seqnum,_time);
                                    msg.setData(buf.readBytes(size));
                                    message_list.add(msg);
                            } else {
                                int uVar15 = (connection.guaranteed_rx + 0x1000) & 0xffff;
                                if (0 < (seqnum - uVar15) * 0x10000) {
                                        connection.Log("seqnum > ... uVar15="+uVar15+" seqnum="+seqnum);
                                        return false;
                                }
                                if (false /* connection_has_reliable(connection,seqnum) */) {
                                        /*
                                        buffer_skip(buffer,(uint)size);
                                        connection->cstat_guaranteed_recv_dups = connection->cstat_guaranteed_recv_dups + 1;
                                        */
                                        buf.readBytes(size);
                                }
                                // goto
                                long _time = System.currentTimeMillis();
                                Message msg=new Message(connection,header.local_endpoint,header.remote_endpoint,null,size,(byte)type,seqnum,_time);
                                //connection->cstat_guaranteed_recv = connection->cstat_guaranteed_recv + 1;
                                //connection.Log("reading message data: "+Util.dump(buf,buf.readerIndex(),buf.writerIndex()));
                                ByteBuf bytes=buf.readBytes(size);
                                // TODO: bytes.release() ?
                                //connection.Log("msg size="+size+" read="+bytes.writerIndex());
                                msg.setData(bytes);
                                message_list.add(msg);
                            }
                        }
                        
                        if (type == 0xfc) {
                                seqnum = 0;
                                if (!arrival_positive) {
                                        connection.Log("skipping 0xfc message");
                                        buf.skipBytes(size);
                                } else {
                                        long _time = 0;
                                        if (connection != null) {
                                                _time = System.currentTimeMillis();
                                        }
                                        Message msg=new Message(connection,header.local_endpoint,
                                                             header.remote_endpoint,null,size,(byte)type,(short)0xffff,_time);
                                        connection.Log("reading 0xfc message size="+size);
                                        msg.setData(buf.readBytes(size));
                                        synchronized(connection.messages_in) {
                                                connection.messages_in.add(msg);
                                        }
                                }
                        } else if (0xf7 < type) {
                                connection.Log("invalid message type "+String.format("%02x",type));
                                return false;
                        } else {
                                flags2|=5;
                                connection.allocateStateChannels();
                                StateChannel stc = connection.stateChannels[type];
                                seqnum=buf.readShort();
                                byte refnum=buf.readByte();
                                if (refnum >= 0x21) { connection.Log("bad refnum? "+refnum); return false; }
                                // size check
                                // seqnum endpoint check
                                int iVar19=(short)(seqnum-stc.unknown4);
                                if (iVar19>0) {
                                        if (iVar19 > 0x1000) {
                                                connection.Log("corrupt: iVar19="+iVar19);
                                                return false;
                                        }
                                        Message msg=new Message(connection,header.local_endpoint,header.remote_endpoint,null,size,(byte)type,seqnum,System.currentTimeMillis());
                                        msg.refnum=refnum;
                                        message_list.add(msg);
                                        //connection.Log("rle start at "+buf.readerIndex());
                                        msg.data=runLengthDecode(buf,size);
                                        //connection.Log("rle'd msg: "+msg);
                                        if (refnum!=0) {
                                                int local_c0=seqnum-refnum;
                                                Message r=lookupMessage(stc.list2,(short)local_c0);
                                                if (r==null) {
                                                        connection.Log("refnum does not exist");
                                                        return false;
                                                }
                                                xorData(msg.data,r.data);
                                                //connection.Log("xorred: "+msg);
                                        }
                                        // is this right? adding duplicate message?
                                        message_list.add(msg.duplicate());
                                        continue;
                                }
                                connection.Log("dup state? skip-rle iVar19="+iVar19+" seqnum="+seqnum+" stc.unknown4="+stc.unknown4);
                                runLengthDecode(buf,size);
                                unknown2[unknown3] = (byte)type;
                                unknown3++;
                        }
                        } // goto-fix
        }
        
        
        
        
        static void xorData(ByteBuf dst,ByteBuf src) {
                int len=dst.writerIndex();
                int len2=src.writerIndex();
                if (len2<len) len=len2;
                for (int i=0; i<len; i++) {
                        dst.setByte(i,dst.getByte(i)^src.getByte(i));
                }
        }
        
        static Message lookupMessage(List<Message> list,short seqnum) {
                for (Message m : list) {
                        if (m.seqnum==seqnum) return m;
                }
                return null;
        }
        
        static ByteBuf runLengthDecode(ByteBuf buf,int len) {
                ByteBuf ret=Unpooled.buffer(64).order(ByteOrder.LITTLE_ENDIAN);
                int repeat_zeroes, copy_bytes;
                while(true) {
                        int b=buf.readByte()&0xff;
                        if (b==0) break;
                        copy_bytes = b & 0x7f;
                    if ((b & 0x80) == 0) {
                      copy_bytes = b>>4;
                      repeat_zeroes = b & 0xf;
                    } else {
                        repeat_zeroes=buf.readByte();
                    }
                    for (int i=0; i<repeat_zeroes; i++) {
                        ret.writeByte(0);
                    }
                    if (copy_bytes!=0) {
                        ByteBuf c=buf.readBytes(copy_bytes);
                        ret.writeBytes(c);
                        c.release();
                    }
                }
                return ret;
        }
        
        
        
        
        
        
}
