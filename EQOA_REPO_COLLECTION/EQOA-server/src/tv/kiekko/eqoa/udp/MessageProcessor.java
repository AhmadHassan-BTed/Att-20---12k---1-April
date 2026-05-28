package tv.kiekko.eqoa.udp;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import tv.kiekko.eqoa.Util;
import tv.kiekko.eqoa.db.Coach;
import tv.kiekko.eqoa.file.Examples;
import tv.kiekko.eqoa.geom.Point;
import tv.kiekko.eqoa.login.Account;
import tv.kiekko.eqoa.udp.message.CreateCharacter;
import tv.kiekko.eqoa.udp.message.ServerListMessage;


// This class handles "business logic" for a Connection


public class MessageProcessor {
        Connection connection;
        int target_id;
        Combatant combatant;
        
        public MessageProcessor(Connection c) {
                connection=c;
        }
        
        void Log(String s) {
                connection.Log(s);
        }
        

        public void processMessages() throws IOException {
                Thread.currentThread().setName("processMessages "+connection);
                synchronized(connection.messages_in) {
                        for (Message m : connection.messages_in) {
                                try {
                                        processMessage(m);
                                } catch(Exception ex) {
                                        Log("processMessage failed");
                                        ex.printStackTrace(connection.out);
                                }
                        }
                        connection.messages_in.clear();
                }
                UDPServer.flushConnection(connection);
        }
  
          void processMessage(Message m) throws IOException {
                //Log("processing "+m);
                int t=m.type&0xff;
                switch(t) {
                case 0xf9:
                        processSystemMessage(m);
                        break;
                case 0xfa:
                        Log("HUOM FRAGMENTOITU, BUG?");
                        break;
                case 0xfb:
                        processGuaranteedMessage(m);
                        break;
                case 0xfc:
                        processBestEffortMessage(m);
                        break;
                case 0xfd:
                        //disconnected
                        break;
                case 0xff:
                        // connected
                        break;
                case 0x40:
                        processPlayerControl(m);
                        break;
                default:
                        Log("can't process message type "+String.format("%02x",t));
                }
        }
        
        static int getShortBE(ByteBuf data,int off) {
                return ((data.getByte(off)&0xff)<<8)|(data.getByte(off+1)&0xff);
        }
        
        void processPlayerControl(Message m) {
                float playerx,playery,playerz;
                ByteBuf data=m.data;
                int facing=data.getByte(22)+128;        // for some reason 180 degrees adjustment needed?
                // convert fixed floating point in the player update message to world coordinates
                playerx=(32000.0f+4000.0f)*Util.getMedium(data,1)/0xffffff-4000.0f;
                playery=(1000.0f+1000.0f)*Util.getMedium(data,4)/0xffffff-1000.0f;
                playerz=(32000.0f+4000.0f)*Util.getMedium(data,7)/0xffffff-4000.0f;
                // velocities
                int vxfixed=getShortBE(data,10);
                int vyfixed=getShortBE(data,12);
                int vzfixed=getShortBE(data,14);
                float vx=15.3f*2*vxfixed/0xffff-15.3f;
                // float vy= ???
                float vz=15.3f*2*vzfixed/0xffff-15.3f;
                int animation=data.getByte(35);
                //Log("player pos = "+String.format("%f, %f, %f",playerx,playery,playerz)+" anim="+animation+" vx="+vx+" vz="+vz);
                if (connection.entity==null) {
                        Log("entity missing?");
                        return;
                }
                connection.entity.setFacing(facing);
                connection.entity.setAnimation(animation);
                connection.entity.setVelocity(vx,0,vz);
                connection.entity.playerMoveTo(playerx,playery,playerz);
        }


}
