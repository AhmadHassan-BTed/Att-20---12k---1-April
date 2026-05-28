package tv.kiekko.eqoa.udp;

import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import org.json.JSONObject;
import org.json.JSONString;
import org.recast4j.detour.NavMesh;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import tv.kiekko.eqoa.Util;
import tv.kiekko.eqoa.geom.Path;
import tv.kiekko.eqoa.geom.Point;
import tv.kiekko.eqoa.udp.UDPServer.EntityBucket;


// Entity = a player or an NPC

public class Entity {

        public int id;
        Point position;
        float facing;
        int hp, maxhp;
        int animation;
        int mhWeapon;
        int ohWeapon;
        int shield;
        int target;
        EntityMovement movement;
        int level=1;
        long last_combat;
        Point spawnPosition;
        String name;
        int npcType=0;
        int attackspeed=3000;
        Point velocity;
        int model=0;
        int model2=0;
        ByteBuf state;
        Connection connection;
        int con=0;
        int targetable=1;
        float scale=1;

  
	public Point getPosition() {
		return movement.getPosition();
	}
	
	public Point getRotation() {
		return new Point(facing,0,0);
	}
	
	
	// this is called only from MessageProcessor.processPlayerControl()
  
        public void playerMoveTo(float x,float y,float z) {
                setPosition(x,y,z);
                updateAndBroadcast();
        }


	// This is called after we have done some changes to this Entity (e.g. changed animation sequence or gear or facing)
	// and want to send the new state to any players who see this
	
        public void updateAndBroadcast() {
                updateState();
		// create an "object update" message
                Message m=new Message(state,-1);
		// send it to everyone in range except this Entity's own connection (if any)
                UDPServer.broadcastStateExcept(this,m,connection);
		// if this Entity has a connection, meaning it's a player, send the
		// state to its own connection too, but on channel 0
                if (connection!=null) {
                        m.setType(0);
                        connection.sendState(m);
                }
        }

	
        // Update the state ByteBuf to send information about this Entity to players.
        // This server uses the shorter state message, 160 bytes?
  
        public void updateState() {
                Point pos=getPosition();        // getPosition() interpolates based on movement speed and start time
                // position coordinates as 24-bit fixed floating point
                setMedium(6,(int)Math.round(pos.x*128.0));
                setMedium(9,(int)Math.round(pos.y*128.0));
                setMedium(12,(int)Math.round(pos.z*128.0));
                byte[] b=name.getBytes();
                state.setBytes(116,b);
                for (int i=b.length; i<24; i++)
                        state.setByte(116+i,0);
                state.setByte(140,level);
                // velocity x,y,z 8-bit each
                int svx=Math.round(speed_adjust*velocity.x);
                int svy=Math.round(speed_adjust*velocity.y);
                int svz=Math.round(speed_adjust*velocity.z);
                if (svx>127) { System.out.println("WARNING: svx="+svx); svx=127; }
                if (svx<-128) { System.out.println("WARNING: svx="+svx); svx=-128; }
                if (svz>127) { System.out.println("WARNING: svz="+svz); svz=127; }
                if (svz<-128) { System.out.println("WARNING: svz="+svz); svz=-128; }
                state.setByte(40,svx);
                state.setByte(41,svy);
                state.setByte(42,svz);
                // 0=south, 128=north, 64=east, 192=west
                state.setByte(15,(int)Math.round(128.0*facing/Math.PI));
                state.setByte(17, world);
                state.setByte(58,animation);
                state.setInt(28,model);
                state.setInt(32, model2);
                state.setFloat(36,scale);
                int npcType=this.npcType;
                state.setInt(67,mhWeapon /* e.g. 0xd335f2f5*/ );
                state.setInt(71,ohWeapon);
                state.setInt(75,shield /* e.g. 0xd21ec0c5*/ ); 
                state.setByte(93,gear[0]);
                state.setByte(94,gear[1]);
                state.setByte(95,gear[2]);
                state.setByte(96,gear[3]);
                state.setByte(97,gear[4]);
                state.setByte(98,gear[5]);
                state.setByte(99,gearcol[0]);
                state.setByte(100,gearcol[1]);
                state.setByte(101,gearcol[2]);
                state.setByte(102,gearcol[3]);
                state.setByte(103,gearcol[4]);
                state.setByte(104,gearcol[5]);

                // if this Entity has a Connection (it's a player) get looks from Character
                if (connection!=null) {
                        npcType |= 0x80;
                        Character ch=connection.getCharacter();
                        state.setByte(105,ch.haircol);
                        state.setByte(106,ch.hairlen);
                        state.setByte(107,ch.hairstyle);
                        state.setByte(108,ch.face);
                        state.setByte(109,-1);
                        con=2;
                        targetable=0;
                } else {
                        // it's an NPC
                        state.setByte(105,looks[0]);
                        state.setByte(106,looks[1]);
                        state.setByte(107,looks[2]);
                        state.setByte(108,looks[3]);
                        state.setByte(109,robeStyle);
                }
                state.setByte(27,(int)(255f*hp/maxhp));
                state.setShort(146,npcType);
                state.setByte(141,0); // something to do with movement
                state.setByte(142,con);
                state.setByte(143,targetable);
                state.setByte(26, hp==0 ? 3 : 1); // hp flag: show hp
                state.setInt(59,target);
       }

  
        void setMedium(int offset,int i) {
                state.setByte(offset,i>>16);
                state.setByte(offset+1,i>>8);
                state.setByte(offset+2,i);
        }
  
  
}
