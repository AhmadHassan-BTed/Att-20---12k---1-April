package tv.kiekko.eqoa.udp;

import java.util.List;

import org.recast4j.detour.StraightPathItem;
import org.recast4j.detour.crowd.Crowd;
import org.recast4j.detour.crowd.CrowdAgentParams;

import tv.kiekko.eqoa.file.Detour;
import tv.kiekko.eqoa.geom.Point;

/* Movement system for NPC's and players. Messy, work in progress */


public class EntityMovement {
	Point movement_target=new Point(0,0,0);
	long movement_started;
	long next_movement_update;
	long last_movement;
	float movement_speed;
	Entity entity;
	Entity chasing;
	float max_speed;

	public static Detour detour;
	
	public static boolean init() {
		try {
			detour=new Detour();
			return true;
		} catch(Exception ex) {
			ex.printStackTrace();
			return false;
		}
	}
	
	
	EntityMovement(Entity e) {
		entity=e;
		max_speed=11.5f;
	}
	
	public Entity getChasing() {
		return chasing;
	}
	
	public boolean isChasing() {
		return chasing!=null;
	}

	
	public void setMaxSpeed(float f) {
		max_speed=f;
	}
	
	// Entity.position is in fact the point where its last movement vector started.
  	// Movement speed is this.movement_speed.
  	// 1-byte approximation is vx,vy,vz
	// movement_started is the timestamp of when the movement started
	// The momentary position is calculated here using these.
  
	Point getPosition() {
		if (movement_started==0) {
			return entity.position;
		}
		float delay=(System.currentTimeMillis()-movement_started)/1000.0f;
		Point d=waypoint.sub(entity.position);
		float dd=d.distance();
		if (dd>0) {
			d.x=movement_speed*d.x/dd*delay;
			d.y=movement_speed*d.y/dd*delay;
			d.z=movement_speed*d.z/dd*delay;
		}
		Point ret=entity.position.add(d);
		return ret;
	}
	
	public void stopChasing() {
		chasing=null;
		stopMovement();
	}

	
	public void moveTo(Point p) {
		chasing=null;
		entity.setPosition(getPosition());
		if (movement_target==null) movement_target=p.copy();
		else movement_target.set(p);
		movement_started=0;
		//UDPServer.Log("movement to "+movement_target+" for "+entity);
		updateMovement();
	}

	
	
	public void moveTo(Entity e) {
		if (e.world!=entity.world) {
			UDPServer.Log("movement: moveTo failed, wrong world "+entity+" -> "+e);
		}
		entity.setPosition(getPosition());
		chasing=e;
		movement_started=0;
		//UDPServer.Log("movement to "+e+" for "+entity);
		updateMovement();
	}
	
	public void stopMovement() {
		movement_speed=0;
		entity.setPosition(getPosition());
		movement_started=0;
		entity.velocity.zero();
		entity.animation=0;
		movement_target.x=0;
		//UDPServer.Log("movement stop "+this);
		entity.updateAndBroadcast();
	}

	
	long last_path;
	List<StraightPathItem> path;
	// 0x4446A60B	wisp
	
	Entity[] debugWisps;

	
	void initDebugWisps() {
		if (debugWisps!=null) return;
		debugWisps=new Entity[10];
		for (int i=0; i<debugWisps.length; i++) {
			debugWisps[i]=new Entity(0x70000+i,"waypoint"+i,entity.position.x,entity.position.y-1000,entity.position.z);
			debugWisps[i].setModel(0x4446A60B);
			//debugWisps[i].setScale(0.5f);
			debugWisps[i].updateAndBroadcast();
		}
	}
	
	void markClosestPoly() {
		Point p=detour.onGround(entity.position,entity.world);
		System.err.println("closest="+p);
		initDebugWisps();
		debugWisps[0].setPosition(p);
		debugWisps[0].updateAndBroadcast();
	}
	
	void maybeUpdatePath(long now,boolean debug) {
		if (now-last_path < 1) return;
		last_path=now;
		path = detour.getStraightPath(entity.position,movement_target,entity.world);
		// the first waypoint is always your own position?
		if (path!=null && path.size()>0)
			path.remove(0);
		if (debug) {
			UDPServer.Log("updated path for "+entity.getName()+": "+entity.position+" -> "+movement_target);
			if (path==null) { UDPServer.Log("empty"); return; }
			for (StraightPathItem p : path) {
				UDPServer.Log(new Point(p.getPos()).toString());
			}
		}
		if (path!=null && debug) {
			if (debugWisps==null) initDebugWisps();
			int i=0;
			di=0;
			for (StraightPathItem p : path) {
				if (i<debugWisps.length) {
					debugWisps[i].setPosition(new Point(p.getPos()));
					debugWisps[i].name="waypoint "+(i+1)+"/"+path.size();
					debugWisps[i].updateAndBroadcast();
					i++;
				}
			}
			for (; i<debugWisps.length; i++) {
				debugWisps[i].setPosition(0,0,0);
				debugWisps[i].updateAndBroadcast();
			}
		}
	}
	
	public void resetPath() {
		last_path=0;
	}
	
	// did we cross/pass by this point in the last movement
	// prevPosition -> entity.position
	
	boolean didCross(Point p) {
		if (p.sub(entity.position).distance2() < 0.3f*0.3f) return true;
		if (p.sub(prevPosition).distance2() < 0.3f*0.3f) return true;
		Point d=p.sub(prevPosition);
		Point m=entity.position.sub(prevPosition);
		if (m.distance2() < d.distance2()) return false;
		float dd=d.distance();
		if (m.x==0 && m.y==0 && m.z==0) return false;
		m.unitify();
		m.multiplyWith(dd);
		m.addTo(prevPosition);
		if (m.sub(p).distance2() < 0.3f*0.3f) return true;
		return false;
	}
	
	Point waypoint;
	int di;
	Point prevPosition;
	
	public void updateMovement() {
		long now=System.currentTimeMillis();
		final boolean debug=false;
    		next_movement_update=0;
		last_movement=now;
		if (prevPosition!=null) prevPosition.set(entity.position);
		else prevPosition=entity.position.copy();
		if (debug) UDPServer.Log("updateMovement "+entity.getName()+" chasing="+chasing+" movement_target="+movement_target); 
		if (chasing!=null && (chasing.isDead() || chasing.world!=entity.world || chasing.disappeared)) {
			UDPServer.Log("updateMovement: "+entity.getName()+" stop chasing "+chasing.getName());
			chasing=null;
			stopMovement();
			return;
		}
		if (chasing!=null) {
			if (true) {
				Point attackPos=calculateAttackPosition(chasing);
				movement_target.set(attackPos);
			}
			movement_target.set(chasing.position);
		}
    		if (movement_target.x==0) return;
		if (movement_started!=0) {
			entity.setPosition(getPosition());
		}		
		Point d=movement_target.sub(entity.position);
		float total_dist=d.distance();
		if (debug)
			UDPServer.Log("movement update: "+entity.getName()+" target="+movement_target+" dist="+total_dist+" d="+d+" entity.pos="+entity.position+" target="+movement_target);

		if (total_dist<=(chasing==null ? 1 : 3)) {
			if (debug) UDPServer.Log(entity.getName()+" arrived");
			stopMovement();
			if (chasing!=null) next_movement_update=now+100; // ???
			return;
		}

		waypoint=movement_target;
		float dd=total_dist;
		
		maybeUpdatePath(now,debug);
		
		if (dd > 1 && path!=null && path.size() > 0) {
			int i=0;
			Point wp;
			if (debug) UDPServer.Log(entity.getName()+" moved "+prevPosition+" -> "+entity.position);
			while(path.size()>0 && didCross(wp=new Point(path.get(0).getPos()))) {
				path.remove(0);
				if (debug) {
					UDPServer.Log(entity.getName()+" crossed waypoint "+wp);
				}
			}
			if (!path.isEmpty()) {
				waypoint=new Point(path.get(0).getPos());
				if (debug) UDPServer.Log(entity.name+" next waypoint: "+waypoint+" i="+i+"/"+path.size());
				d=waypoint.sub(entity.position);
				dd=d.distance();
			}
		}
		
		float speed=5;
		if (total_dist > 10 || chasing!=null) speed=max_speed;
		else if (chasing==null) speed=5;
		if (total_dist<2) speed=1;
		if (speed>max_speed) speed=max_speed;
		d.multiplyWith(1f/dd);
		entity.setVelocity(d.x*speed,d.y*speed,d.z*speed);
		movement_started=now;
		entity.animation=(speed < 5.5f ? 1 : 3);
		if (chasing!=null) {
			// sideways walking animations = 4 and 5
			if (speed < 4) {
				float movement_angle=(float)Math.atan2(d.x,d.z);
				float diff=Math.abs(entity.facing-movement_angle);
				if (diff > Math.PI*2f) diff-=Math.PI*2f;
				else if (diff<-Math.PI*2f) diff+=Math.PI*2f;
				if (diff > 0.25f*Math.PI) {
					// maybe works, maybe not
					if (entity.facing > movement_angle) entity.animation=4;
					else entity.animation=5;
				}
			}
		}
    		entity.faceTo(waypoint);
		movement_speed=speed;
		if (debug) UDPServer.Log("movement "+entity+" to "+waypoint+" d="+d+" dd="+total_dist+" v="+entity.velocity);
		entity.updateAndBroadcast();
    		// schedule the next movement update so that this will be called about when the destination is reached
		if (dd>0.5f) dd-=0.5f;
		int t=(int)(1000f*dd/speed);
		if (chasing!=null && t>100) t=100;
		next_movement_update=now+t;
		if (debug) UDPServer.Log(entity.getName()+" movement scheduling next t="+t+" dd="+dd+" walk_speed="+speed);
	}
	
	public boolean isMoving() {
		if (chasing!=null) return true;
		if (movement_speed>0) return true;
		return false;
	}

	
  	// simple sector system to prevent all NPC's attacking someone from standing on top of each other
  
	Entity[] attackerInSector=new Entity[9];
	
	void updateAttackerSectors(Entity remove) {
		for (int i=0; i<attackerInSector.length; i++) {
			if (attackerInSector[i]==null) continue;
			if (attackerInSector[i]==remove) { attackerInSector[i]=null; continue; }
			if (attackerInSector[i].isDead()) { attackerInSector[i]=null; continue; }
			float d2;
			if ((d2=attackerInSector[i].position.sub(entity.position).distance2()) > 4*4) {
				attackerInSector[i]=null;
				continue;
			}
		}
	}
	

	Point calculateAttackPosition(Entity chasing) {
		chasing.movement.updateAttackerSectors(entity);
		Point cp=chasing.getPosition().copy();
		Point d=entity.position.sub(cp);
		float dist=d.distance();
		float a=(float)Math.atan2(d.x,d.z);
		float sectorSize=(float)(2.0*Math.PI/attackerInSector.length);
		a+=sectorSize*0.5f;
		if (a<0) a+=2f*Math.PI;
		int preferred=(int)(a*attackerInSector.length/(2.0*Math.PI));
		Float found=null;
		for (int i=0; i<attackerInSector.length/2+1; i++) {
			int trysector=(preferred+i) % attackerInSector.length;
			//System.out.println("trying sector "+trysector);
			if (chasing.movement.attackerInSector[trysector]==null) {
				chasing.movement.attackerInSector[trysector]=entity;
				found=sectorSize*trysector;
				break;
			}
			trysector=(attackerInSector.length+preferred-i) % attackerInSector.length;
			//System.out.println("2. trying sector "+trysector);
			if (chasing.movement.attackerInSector[trysector]==null) {
				chasing.movement.attackerInSector[trysector]=entity;
				found=sectorSize*trysector;
				break;
			}
		}
		if (found!=null) {
			d.x=(float)Math.sin(found);
			d.z=(float)Math.cos(found);
			d.y*=1f/dist;
			//System.out.println("points at "+d);
		} else  d.unitify();
		d.multiplyWith(3);
		//return cp.add(d);
		return detour.fixAttackPoint(cp,d,entity.world);
		
	}


	

}
