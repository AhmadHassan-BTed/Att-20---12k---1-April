package tv.kiekko.eqoa.udp;

import java.util.ArrayList;
import java.util.List;

import tv.kiekko.eqoa.file.Detour;
import tv.kiekko.eqoa.file.GenerateNav;
import tv.kiekko.eqoa.geom.Box;
import tv.kiekko.eqoa.geom.Point;

// This class keeps track of areas that have players.
// NPC's and pathfinding data are loaded on demand when a player enters a new area,
// and unloaded after a period of inactivity.


public class AreaInfo {
        Box box;
        int world;
        boolean active;
        long last_activity;
        // list of NPC's in this area that have to be destroyed when unloading it
        List<Entity> entities;

        public final static int gridbox_size_x=600;
        public final static int gridbox_size_z=600;

        static AreaInfo[][][] zones;      // Some naming inconsistency here. An "area" is currently smaller than a zone

        static {
                zones=new AreaInfo[Detour.MAX_WORLD][][];
                initWorld(0,28000,34000);       // tunaria
                initWorld(1,10000,10000);       // oggok
                initWorld(2,14000,14000);       // odus
        }
        
        static void initWorld(int w,int maxx,int maxz) {
                zones[w]=new AreaInfo[maxx/gridbox_size_x+1][maxz/gridbox_size_z+1];
        }
        
        
        public static AreaInfo get(float x,float z,int world) {
                return get((int)(x/gridbox_size_x),(int)(z/gridbox_size_z),world);
        }
        
        public static AreaInfo get(int gx,int gz,int world) {
                AreaInfo[][] ai=zones[world];
                if (ai==null || gx<0 || gz<0 || gx>=ai.length || gz>=ai[gx].length)
                        return null;
                if (ai[gx][gz]==null) {
                        ai[gx][gz]=new AreaInfo(gx,gz,world);
                }
                return ai[gx][gz];
        }

        public AreaInfo(int gx,int gz,int w) {
                box=new Box();
                world=w;
                box.add(new Point(gx*gridbox_size_x,0,gz*gridbox_size_z));
                box.add(new Point((gx+1)*gridbox_size_x,0,(gz+1)*gridbox_size_z));
                active=false;
                entities=new ArrayList<Entity>();
        }

        public synchronized void setActive(boolean b) {
                if (b) last_activity=System.currentTimeMillis();
                if (active==b) return;
                active=b;
                UDPServer.Log("setting zone active="+b+" world="+world+" box="+box);
                if (b) {
                        UDPServer.run(new Runnable() {
                                @Override
                                public void run() {
                                        if (EntityMovement.detour.loadArea(AreaInfo.this))                              
                                                UDPServer.sql.loadRoamers(AreaInfo.this);
                                        else {
                                                UDPServer.Log("failed to activate zone "+box+"/"+world+", canceling");
                                                setActive(false);
                                        }
                                }
                        });
                } else {
                        UDPServer.run(new Runnable() {
                                @Override
                                public void run() {
                                        UDPServer.removeEntities(entities);
                                        entities.clear();
                                        EntityMovement.detour.unloadArea(AreaInfo.this);
                                }
                        });
                }
                UDPServer.Log("done");
        }
        
        public void addEntity(Entity e) {
                entities.add(e);
        }
        public Box getBox() {
                return box;
        }
        
        public int getWorld() {
                return world;
        }
        
        Point center;
        
        public Point getCenter() {
                if (center==null) center=box.getCenter();
                return center;
        }
        
        public static void activateNearbyAreas(Point pos,int world) {
                Box box=new Box();
                box.add(pos);
                box.enlarge(500);
                for (int x=(int)(box.minx/gridbox_size_x); x<=(int)(box.maxx/gridbox_size_x); x++) {
                        for (int z=(int)(box.minz/gridbox_size_z); z<=(int)(box.maxz/gridbox_size_z); z++) {
                                AreaInfo zi=get(x,z,world);
                                if (zi==null) continue;
                                if (zi.getCenter().sub(pos).distance2() > 500*500) continue;
                                zi.setActive(true);
                        }
                }
        }
        
        public static void activateNearbyZones(Entity e) {
                if (e==null) return;
                activateNearbyAreas(e.position,e.world);
        }
        
        final static int INACTIVITY_LIMIT=1000*60*1;
  
        // this is called periodically
  
        public static void sleepZones() {
                long now=System.currentTimeMillis();
                for (int w=0; w<zones.length; w++) {
                        if (zones[w]==null) continue;
                        for (int x=0; x<zones[w].length; x++) {
                                if (zones[w][x]==null) continue;
                                for (int z=0; z<zones[w][x].length; z++) {
                                        if (zones[w][x][z]==null || zones[w][x][z].active==false) continue;
                                        if (now-zones[w][x][z].last_activity > INACTIVITY_LIMIT)
                                                zones[w][x][z].setActive(false);
                                }
                        }
                }
        }
