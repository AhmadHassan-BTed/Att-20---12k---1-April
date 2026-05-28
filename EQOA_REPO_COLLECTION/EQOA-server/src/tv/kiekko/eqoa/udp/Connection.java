package tv.kiekko.eqoa.udp;

import java.io.PrintStream;
import java.net.InetSocketAddress;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.Vector;

import org.json.JSONObject;
import org.json.JSONString;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.ChannelHandlerContext;
import tv.kiekko.eqoa.Util;
import tv.kiekko.eqoa.geom.Point;
import tv.kiekko.eqoa.login.Account;

/*

This class represents a player's connection to the server.

A lot of this is hardly readable because a lot of this was "blindly" adapted from PS2 code
without necessarily understanding what it does. Meanings of some fields/variables wasn't clear
when writing this so they're named "unknown" etc.

Still, this is the only existing implementation of stable connections for now AFAIK and probably
easier to figure out than the original PS2 machine language code.

It's not directly runnable/compilable, released mainly as documentation to help other development efforts.
Some code, e.g. ping calculations and scheduling packet resends are still missing from this file.

*/


public class Connection implements ErrorDisplay, JSONString {
        UDPServer server;      
        
        int instance_local;
        int instance_remote;
        long lastArrivalTime;
        long last_instance_arrival;
        int desired_flush_time;
        long some_timestamp;
        List<Message> messages_in;
        List<Message> unacked_messages;
        List<Message> messages_out;
        short flags;
        short timeout;
        short flush_tx;
        short arrival_number;
        long arrival_history;
        short guaranteed_tx;
        short guaranteed_rx;
        short acked;
        long guaranteed_ack_mask;
        int last_route_flush_number;
        int timeout1, timeout2, timeout3;
        boolean terminated;
        byte unknown3_1;
        long local_endpoint;
        long remote_endpoint;
        long instanceLastSent;
        long created;
        short unknown12;
        long unknown12_6;       // Didn't find where this is set on PS2?
        Entity entity;          // The player's physical entity
        Character character;    // Contains info about their chosen character, name, race, looks, class, etc.
        Account account;        // Login, password, etc.
        long last_ping;
        int version;
        EntityInRange range;
        PrintStream out;        // for logging
        boolean old;            // kludge
        MessageProcessor mp;    // handles "game business logic"
        Timer sendTimer;        
        ChannelHandlerContext ctx;
        InetSocketAddress address; // can you get this from ctx?
  
  
        // State is a special type of message, used for things that
        // are continuously updated between server and client,
        // e.g. entity positions and animations.
  
        class StateChannel {
                List<Message> list1;
                List<Message> list2;
                byte unknown5;
                short unknown4;
                short seqnum;
                short seqnum2;
                byte unknown6;
                Message message;
                
                public StateChannel() {
                        list1=new ArrayList<Message>();
                        list2=new ArrayList<Message>();
                        seqnum=1;
                }
        }
        
        StateChannel[] stateChannels;
  
        public void Log(String s) {
                out.println(String.format("[%.3f] %s",(System.currentTimeMillis()-server.start_time)/1000.0,s));
        }  
        
        public void printErr(String s) {
                Log("printErr: "+s);
                sendMessage(UDPServer.colorTextMessage(s,UDPServer.COLOR_ERROR));
        }
  
        public void processMessages() {
                try {
                        mp.processMessages();
                } catch(Exception ex) {
                        Log("processMessages failed for "+this);
                        ex.printStackTrace(out);
                }
        }

        // constructor, src=remote, dst=local
        
        public Connection(long src,long dst,InetSocketAddress addr) {
                flush_tx=1;
                guaranteed_tx=1;
                timeout1=5000;
                timeout2=30000;
                timeout3=60000;
                last_route_flush_number=-1;
                messages_in=new ArrayList<Message>();
                unacked_messages=new ArrayList<Message>();
                messages_out=new ArrayList<Message>();
                // arrival_history should possibly be initialized with 0xff's?
                //if (dst!=0xfffe) flags=(short)(flags|0x40);
                created=System.currentTimeMillis();
                flags=0x40;   // in packet logs it seems to have this value initially
                local_endpoint=dst;
                remote_endpoint=src;
                address=addr;
                mp=new MessageProcessor(this);
                sendTimer=new Timer();
                out = Log.get(String.format("conn-%s",addr));
        }
  
        public void print(String s) {
                sendMessage(UDPServer.textMessage(s));
        }

        public boolean hasResendable() {
                long now=System.currentTimeMillis();
                synchronized(messages_out) {
                        int resend_time=calcResendTime();
                        //Log("hasResendable: resend_time="+resend_time);
                        if (resend_time>=60000) return false;   // give up
                        int sz=unacked_messages.size();
                        while(sz>0) {
                                sz--;
                                Message last=unacked_messages.get(sz);
                                //if (last.isState()) continue;
                                //Log("hasResendable: last="+last.toShortString()+" age="+(now-last.time));
                                if (now-last.time >= resend_time) {
                                        return true;
                                }
                        }
                        return false;
                }
        }
  
  
  
        // this basically constructs one UDP packet to be sent out
  
        public ByteBuf flush() {
                if (terminated) return null;
                long now=this.last_flush=System.currentTimeMillis();
                ByteBuf msgbuf=Unpooled.buffer(256).order(ByteOrder.LITTLE_ENDIAN);
                ByteBuf outbuf=Unpooled.buffer(256).order(ByteOrder.LITTLE_ENDIAN);

                outbuf.writeShort((short)(local_endpoint&0xffffL));
                outbuf.writeShort((short)(remote_endpoint&0xffffL));
                
                // does the connection need to be reset? if so, probably do something here
                                
                writeBodyStart(msgbuf);
                
                // send stuff from messages_out
                // and resend sufficiently old messages from unacked_messages               
                
                synchronized(messages_out) {
                        int i=0;
                        int resend_time=calcResendTime();
                        
                        while(i<unacked_messages.size() && msgbuf.writerIndex()<1024) {
                                Message resend=unacked_messages.get(i);
                                //UDPServer.Log("unacked "+i+" last (re)send="+(now-resend.time)+" age="+(now-resend.created)+": "+resend.toShortString()+" resend_time="+resend_time);
                                // disconnect if the client doesn't ack a message for 60 seconds
                                if (now-resend.time > 60000) {
                                        UDPServer.Log("too old unacked, terminating: "+resend.toShortString());
                                        this.terminate(0);
                                        return null;
                                }
                                if (now-resend.time > resend_time) {
                                        unacked_messages.remove(i);
                                        //Log("appending unacked "+resend.toShortString()+" age="+(now-resend.time));
                                        Log("resending "+resend.toShortString()+" age="+(now-resend.time));
                                        this.addRttSample(now-resend.time);
                                        resend.write(this,msgbuf,unacked_messages,resend.flush);
                                        if (!resend.isState())
                                                unacked_messages.add(resend);   // back to the end of queue. state will go in send()
                                        resend.time=now;
                                } else break; // time order so there shouldn't be any older
                                i++;
                        }
                        
                        while(!messages_out.isEmpty() && msgbuf.writerIndex()<1024) {
                                Message m=messages_out.remove(0);
                                int uVar1 = (int)(m.type + 7) & 0xff;
                                if (uVar1 < 3) {                                
                                        m.seqnum=guaranteed_tx;
                                        guaranteed_tx++;
                                }
                                //Log("appending msg "+m.toShortString());
                                m.write(this,msgbuf,messages_out,flush_tx);
                                m.time=now;
                                if (uVar1 < 3) unacked_messages.add(m);
                                //Log("appended "+account+" msg "+m.toShortString()+", uVar1="+uVar1+" guaranteed="+(uVar1<3)+" buf size now "+msgbuf.writerIndex());
                        }
                }
          
                SegmentHeader header=new SegmentHeader();
                header.local_endpoint=local_endpoint;
                header.remote_endpoint=remote_endpoint;

                //Log("should we send instance_local? "+String.format("local=%x remote=%x",instance_local,instance_remote));
                if (instance_local!=0 && sendInstanceLocal()) {
                        header.flags|=0x2000;
                        if (!hasInstanceAck()) header.flags|=0x80000;
                        // apparently things will break if header.instance is empty after this point
                        if (header.instance==0) header.instance=instance_local;
                        if (header.instance==0) header.instance=instance_remote;
                        if (header.instance==0) header.instance=0x777;          // ??? just anything?
                        if (sendInstancePush()) header.flags|=0x4000;
                }
        
                // put together the packet in outbuf
                // src and dst already written above
                // add header, body (msgbuf), and CRC
                header.size=msgbuf.writerIndex();
                header.write(outbuf);
                outbuf.writeBytes(msgbuf);
                outbuf.writeInt(Util.calculateCRC(outbuf));             

                if ((flags & 0x200)==0) {
                    instanceLastSent=System.currentTimeMillis();
                    flags |= 0x200;
                    lastArrivalTime=instanceLastSent;
                }
                return outbuf;
        }
        
        
        long getAckMask() {
                return 0;
        }
  
        // Adapted from the function called connection_flush() on PS2
        
        void writeBodyStart(ByteBuf buf) {
            int conn_flags=this.flags;
            int some_flags=0;
            if (((conn_flags & 1) == 0) && ((conn_flags & 2) == 0)) {
                  //
            } else {
              long local_a0=getAckMask();
              some_flags = 2;
            }
            if ((conn_flags & 4) != 0) {
              some_flags |= 0x10;
            }
            int body_flags = some_flags;
            if (((conn_flags & 1) != 0)) {
                body_flags = some_flags | 1;
                if (unknown12_6 != 0) {
                        body_flags |= 5;
                }
            }
            if ((conn_flags & 8) != 0) {
                body_flags |= 0x40;
            }
            if (unknown3_1<0x11) {
                body_flags|=0x20;
            }
            //Log("write body flags: "+String.format("%x",body_flags_maybe));
            buf.writeByte(body_flags);
        
            if ((body_flags & 0x40) != 0) {
                //Log("write body instance: "+String.format("%x", instance_remote));
                buf.writeInt(instance_remote);
            }
        
            //Log("write body arrival number: "+String.format("%x",flush_tx));
            buf.writeShort(flush_tx);
        
            if (((body_flags & 1) != 0)) {
                //Log("write body flush_ack: "+String.format("%x",arrival_number));
                buf.writeShort(arrival_number);                                 // vastaanottajalle body.flush_ack?
                if ((body_flags & 4) != 0) {
                        //Log("write body unknown12_6: "+String.format("%x", unknown12_6));
                        Util.writePacked64(buf,unknown12_6);
                }
            }
        
            if (((body_flags & 2) != 0)) {
                //Log("write body guaranteed_tx(?): "+String.format("%x", guaranteed_rx));
                buf.writeShort(guaranteed_rx);
            }
            if ((body_flags & 0x10) == 0) {
              //iVar5 = param_2->eof;
            } else {
                int i=0;
                //Log("writing state acks");
                do {
                  if (stateChannels[i].unknown5 != 0) {
                    buf.writeByte(i);
                    buf.writeShort(stateChannels[i].unknown4);
                    //Log("ch="+String.format("%02x",i)+" unknown4="+String.format("%x",stateChannels[i].unknown4));
                    stateChannels[i].unknown5--;
                  }
                  i++;
                } while (i < 0xf8);
                buf.writeByte(0xf8);
            }
            // connection_flush end
            flush_tx++;
            flags=(short)(conn_flags & 0xfff0);
        }
  
        public void allocateStateChannels() {
                if (stateChannels!=null)
                  return;
                stateChannels=new StateChannel[0xf8];
                for (int i=0; i<stateChannels.length; i++)
                        stateChannels[i]=new StateChannel();
        }
        
        boolean hasInstanceAck() {
                return (flags & 0x40)!=0;
        }
        
        boolean sendInstancePush() {
                return (flags & 0x80)!=0;
        }
        

        // instance_local should be sent based on some flags or time
  
        boolean sendInstanceLocal() {
                if (((flags & 0x40) != 0)) {
                        long t;
                        if ((t=System.currentTimeMillis()-instanceLastSent) < 30000) {
                                 //Log("not sending instance local, t="+t);
                           return false;
                        } //else Log("sending instance local: time");
                } //else Log("sending instance local: flags & 0x40 = "+((flags&0x40)!=0));
                return true;
        }
  
  
        public void sendMessage(Message m) {
                if (m.sentTo!=null) {
                        if (m.sentTo!=this) {
                                Log("sendMessage failed: already sent to "+m.sentTo+": "+m);
                                return;
                        }
                } else {
                        m.sentTo=this;
                }
                if (m.isState()) {
                        sendState(m);
                        return;
                }
                else synchronized(messages_out) {
                        // fragment logic here, needed with the big Message when entering world
                        if (m.type==(byte)0xfb) {
                                int frag=0;
                                ByteBuf buf=m.getData();
                                buf.readerIndex(0);
                                while(buf.readableBytes() > 1156) {
                                        ByteBuf piece=buf.readBytes(1156);
                                        Message p=new Message(piece,0xfa);
                                        p.mustRelease=true;
                                        messages_out.add(p);
                                        frag++;
                                }
                                ByteBuf last=buf.readBytes(buf.readableBytes());
                                Message l=new Message(last,0xfb);
                                l.mustRelease=true;
                                messages_out.add(l);
                                frag++;
                                if (frag>1) Log("fragments: "+frag);
                        } else {
                                messages_out.add(m);
                        }
                }
                UDPServer.flushConnection(this);
        }
  
  
  
        public void sendState(Message m) {
                Message local_80;
                allocateStateChannels();
                synchronized(messages_out) {
                        StateChannel psVar7 = stateChannels[((int)m.type)&255];
                        Message psVar6 = psVar7.message;
                        if (psVar6==null && !psVar7.list1.isEmpty()) psVar6 = psVar7.list1.get(0);
                        local_80 = psVar6;
                        //Log("sendState sz="+m.size+" prev msg="+(local_80==null?"-":local_80.toShortString())+" this msg="+(m==null?"-":m.toShortString()));
                        boolean unchanged = (m.size == 0);
                        if (local_80 != null) {
                          unchanged = false;
                          int lVar4 = local_80.size;
                          if (lVar4 == m.size) {
                            ByteBuf uVar5 = local_80.data;
                            unchanged=uVar5.equals(m.data);
                          }
                        }
                        if (!unchanged) {
                                //Log("state changed");
                                long time = System.currentTimeMillis();
                                local_80=new Message(this,local_endpoint,remote_endpoint,m.data.copy(),m.size,(byte)m.type,(short)0xffff,time);
                                byte bVar1 = psVar7.unknown6;
                                psVar6 = psVar7.message;
                                //Log("unknown6="+bVar1+" new msg="+local_80.toShortString());
                                if (bVar1 == 1) {
                                        int i=messages_out.indexOf(psVar7.message);
                                        //Log("replacing i="+i);
                                        if (i>=0) messages_out.set(i,local_80);
                                } else {
                                        if (bVar1 < 2) {
                                                if (bVar1 == 0) {
                                                        messages_out.add(local_80);
                                                }
                                        } else {
                                                if (bVar1 == 2) {
                                                        messages_out.add(local_80);
                                                        addRttSample(psVar7.message);
                                                        if (!unacked_messages.remove(psVar7.message))
                                                                Log("sendState: failed to remove from unacked: "+psVar7.message);
                                                }
                                        }
                              }
                              psVar7.unknown6 = 1;
                              psVar7.message = local_80;
                        }               
                }
                UDPServer.flushConnection(this);
        }
  

        // Do we have to send out a packet?
  
        public boolean needsFlush() {
                if (terminated) return false;
                if (!messages_out.isEmpty()) { /*Log("needsFlush: "+messages_out.size()+" messages_out");*/ return true; }
                // need to ack something?
                if ((flags&3)!=0) { /*Log("needsFlush: flags");*/ return true; }
                if (resetFlag!=0) { /*Log("needsFlush: resetFlag");*/ return true; }
                if (hasResendable()) { /*Log("needsFlush: resendable");*/ return true; }
                return false;
        }

  
          public void recordSegment(SegmentHeader header,SegmentBody body) {
                lastArrivalTime=System.currentTimeMillis();
                if ((header.flags & 0x2000) == 0) {
                } else {
                    last_instance_arrival=lastArrivalTime;
                    if ((~flags & 0x20) != 0) {
                        flags |= 0x20;
                        //Log(String.format("setting con.instance_remote=%x, was %x",header.instance,instance_remote));
                        instance_remote = header.instance;
                    }
                    if ((header.flags & 0x80000) == 0) {
                    } else {
                        if ((flags & 0xf) == 0) {
                        some_timestamp = System.currentTimeMillis() + 100;
                      }
                      flags |= 9;
                    }
                }
                if (body==null) return; // ???
                if (!body.arrival_positive) {
                        unknown3_1=0;
                } else {
                        if (unknown3_1!=-1) unknown3_1++;
                        if ((body.flags & 0x20) == 0) {
                          flags |= 0x10;
                        } else {
                          flags &= 0xffef;
                        }
                }
                //Log("arrival_positive="+body.arrival_positive+" instance_local_ok="+body.instance_local_ok);
                if (!body.arrival_positive) {
                        arrival_history |= 1 << ((((arrival_number - body.arrival_number) * 0x10000) >> 0x10) -1);
                } else {
                        arrival_number++;
                        arrival_history=(arrival_history<<1)|1;
                    long lVar5 = (((body.arrival_number - arrival_number) * 0x10000) >> 0x10);
                    //Log("body.arrival_number="+body.arrival_number+" connection.arrival_number="+arrival_number+" diff="+String.format("%x",lVar5));
                    if (0 < lVar5) {
                        arrival_history = arrival_history << lVar5;
                    }
                    arrival_number = body.arrival_number;
                }
                if (body.instance_local_ok) {
                        recordAcks(body);
                }
                recordSegmentMessages(body);
                //route_resort(connection);
        }
  
  
        void recordSegmentMessages(SegmentBody body) {
                if (body.flags2!=0) {
                        if ((flags&0xf)==0) {
                                some_timestamp=System.currentTimeMillis()+100;
                        }
                        flags |= body.flags2;
                        int iVar4 = 0;
                        // this is possibly about duplicate states?
                        if (0 < body.unknown3) {
                          do {
                            iVar4++;
                            stateChannels[body.unknown2[iVar4]].unknown5 = 2;
                          } while (iVar4 < body.unknown3);
                        }                   
                        
                }
                while(!body.message_list.isEmpty()) {
                        Message m=body.message_list.remove(0);
                        recordMessage(m,body.message_list);
                }
        }
  
  
        void recordAcks(SegmentBody body) {
          synchronized(messages_out) {

                if ((body.flags & 0x40) != 0) {
                        flags |= 0x40;
                }
                if ((body.flags & 1) == 0) {
                } else {
                        if ((body.flags & 4) == 0) {
                        } else {
                                // something missing here
                                // PS2 call connection_record_flush_ack()
                        }
                        if (0 < body.flush_ack - unknown12) {
                                unknown12 = body.flush_ack;
                        }
                }
                if (body.guaranteed_ack_present_maybe) {
                        recordGuaranteedAck(body.guaranteed_ack,body.guaranteed_ack_mask);
                }
                if ((body.flags & 0x10) != 0) {
                        recordStateAcks(body.stateAcks);
                }
          }
        }

  
          void recordStateAcks(SegmentBody.StateAck[] acks) {
                int ch;
                int i=0;
                //Log("recordStateAcks first="+acks[0].channel+"/"+acks[0].seqnum);
                synchronized(messages_out) {
                        long now=System.currentTimeMillis();
                        while((ch=acks[i].channel&0xff)!=0xf8) {
                          StateChannel stc=stateChannels[ch];
                          short seq = acks[i].seqnum;
                          if (0 < ((((int)seq)&0xffff)-(((int)stc.seqnum2)&0xffff))*0x10000) {
                            if (stc.unknown6 == 2) {
                              short uVar3 = stc.message.seqnum;
                              if (uVar3 ==  seq) {
                                stc.unknown6 = 0;
                                //Log("recordStateAcks: removing from unacked: "+stc.message.toShortString()+" set chan "+ch+" unknown6=0");
                                addRttSample(stc.message);
                                stc.message.created=now;
                                boolean removed = unacked_messages.remove(stc.message);
                                if (!removed) Log("remove failed");
                                stc.message=null;
                              }
                            }
                            stc.seqnum2=seq;
                            Message.seqnum_remove_thru(stc.list1,((int)seq)&0xffff);
                          }
                          i++;
                        }
                }               
        }

          void recordGuaranteedAck(short guaranteed_ack, long guaranteed_ack_mask2) {
                int mi=0,removed=0;
                synchronized(messages_out) {
                        while(unacked_messages.size()>mi) {
                                Message m=this.unacked_messages.get(mi);
                                int i=((int)m.type)&0xff;
                                if (i==0xfb || i==0xfa || i==0xf9) {
                                        int sn = m.seqnum;
                                        i = (sn - (guaranteed_ack & 0xffff));
                                        if (i < 1) {
                                                //Log("recordGuaranteedAck: removing msg[1]: "+m.toShortString());
                                                unacked_messages.remove(mi);
                                                addRttSample(m);
                                                mi--;
                                                removed++;
                                        } else {
                                                if ((i != 1) && (i-2) < 0x40 && (1 << (i-2) & guaranteed_ack_mask) != 0) {
                                                        //Log("recordGuaranteedAck: removing msg[2]: "+m.toShortString());
                                                        unacked_messages.remove(mi);
                                                        addRttSample(m);
                                                        mi--;
                                                        removed++;
                                                }
                                        }
                                }
                                mi++;
                        }
                        acked = (short)guaranteed_ack;
                }
                if (removed>5) Log("recordGuaranteedAck "+guaranteed_ack+" removed "+removed);
        }
        
        
        void moveDelayedToArrivals(Message m) {
            if (m.seqnum!=guaranteed_rx+1) {
              Log("seqnum="+m.seqnum+" guaranteed_rx+1="+(guaranteed_rx+1)+" -> returning");
              return;
            }
            guaranteed_rx=m.seqnum;
            synchronized(messages_in) {
                messages_in.add(m);
            }
        }

        public MessageProcessor getMessageProcessor() {
                return mp;
        }
  
        void recordMessage(Message m, List<Message> message_list) {
                //Log("connection recording message "+String.format("%02x",m.type)+": "+m.toShortString());
                int t=((int)m.type)&0xff;
                switch(t) {
                case 0xfa:
                case 0xf9:
                case 0xfb:
                        moveDelayedToArrivals(m);
                        break;
                case 0xfc:
                case 0xfe:
                case 0xff:
                        Log("what to do here msg type="+String.format("%x",t));
                        break;
                default:        // state
                        recordStateMessage(m,message_list);
                        enteredGame();
                }
        }
  
  
        void recordStateMessage(Message m,List<Message> message_list) {
                StateChannel stc=stateChannels[m.type];
                stc.unknown4=m.seqnum;
                stc.unknown5 = 2;
                int refnum=m.refnum;
                if (refnum == 0) {
                        Message.seqnum_remove_thru(stc.list2,(short)(m.seqnum-0x20));
                } else {
                        Message.seqnum_remove_thru(stc.list2,(short)(m.seqnum-refnum));
                }
                if (!message_list.isEmpty()) {
                        Message a=message_list.remove(0);
                        stc.list2.add(a);
                }
                synchronized(messages_in) {
                        messages_in.add(m);
                }
        }
  
        public void terminate(int reason) {
                Log("terminating("+reason+")");
                terminated=true;
                try {
                        sendTimer.cancel();
                } catch(Exception e) { } // already canceled
        }
        
        boolean hasInstanceRemote() {
                return (flags&0x20)!=0;
        }

  
        // JSON representation for Sandstorm Control Panel
  
        @Override
        public String toJSONString() {
                JSONObject json=new JSONObject();
                json.put("account", account==null?null:account.toString());
                json.put("character",character==null?null:character.name);
                json.put("entity",entity);
                json.put("rtt",getAvgRtt());
                json.put("src",String.format("%04x",remote_endpoint&0xffff));
                json.put("dst",String.format("%04x",local_endpoint&0xffff));
                json.put("flags", flags);
                json.put("flush_tx",flush_tx);
                json.put("arrival",arrival_number);
                json.put("messages_in",messages_in.size());
                json.put("unacked_messages",unacked_messages.size());
                json.put("guaranteed_tx",guaranteed_tx);
                json.put("guaranteed_rx", guaranteed_rx);
                json.put("connStats",mp==null?null:mp.connStats);
                json.put("entitiesInRange",this.range);
                json.put("version",version);
                return json.toString();
        }

        
        
}


  
           
