package tv.kiekko.eqoa.file;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.List;

import org.recast4j.detour.DefaultQueryFilter;
import org.recast4j.detour.FindDistanceToWallResult;
import org.recast4j.detour.FindNearestPolyResult;
import org.recast4j.detour.MeshData;
import org.recast4j.detour.NavMesh;
import org.recast4j.detour.NavMeshParams;
import org.recast4j.detour.NavMeshQuery;
import org.recast4j.detour.Result;
import org.recast4j.detour.StraightPathItem;
import org.recast4j.detour.io.MeshSetReader;

import tv.kiekko.eqoa.geom.Box;
import tv.kiekko.eqoa.geom.Point;
import tv.kiekko.eqoa.udp.UDPServer;
import tv.kiekko.eqoa.udp.AreaInfo;


// This class handles loading and unloading Recast/Detour navmesh tiles and using them for pathfinding.
// Having pathfinding geometry in memory for the whole world would be very inefficient because only a
// tiny fraction of it is seen by players at a given time (especially with the low number of players now).
// In order to run with our current box with 4 Gb memory, we load and unload the geometry on demand
// when players enter an area. The dynamically loaded Detour/Recast tiles are sized 50x50 coordinate units (meters?).
// Each tile is stored in a separate pre-generated file.


public class Detour {
        NavMesh[] mesh;             // Recast/Detour NavMesh for each world
        NavMeshQuery[] query;       // NavMeshQuery for them
        DefaultQueryFilter filter;
        float[] halfExtents;
        
        public final static int MAX_WORLD=8;    // Tunaria = 0, etc.
        
        public boolean load(int tx,int ty,int world) {
                // file name based on world and coordinates
                String filename="../nav"+world+"/nav-"+tx+"-"+ty+".mesh";
                try {
                        if (!new File(filename).exists()) {
                                System.err.println("not found: "+filename);
                                return true; // hmm
                        }
                        // load the tile
                        NavMesh tmesh=new MeshSetReader().read(new FileInputStream(filename),6);
                        MeshData d;
                        // create a new NavMesh for this world if it doesn't exist yet
                        if (mesh[world]==null) {
                                NavMeshParams params = tmesh.getParams();
                                params.maxTiles=10000;
                                mesh[world]=new NavMesh(params, 6);
                                params.orig[0]=0;
                                params.orig[1]=0;
                                params.orig[2]=0;
                        }
                        // add the tile
                        d = tmesh.getTile(0).data;
                        d.header.x=(int)(tx/GenerateNav.TILESIZE);
                        d.header.y=(int)(ty/GenerateNav.TILESIZE);
                        synchronized(mesh[world]) {
                                mesh[world].addTile(d, 0, 0);
                        }
                        return true;
                } catch(Exception ex) {
                        System.err.println("failed: "+filename);
                        ex.printStackTrace(System.out);
                        return false;
                }
        }

          public void unload(int tx,int ty,int world) {
                if (mesh[world]==null) {
                        System.err.println("unload: world "+world+" not loaded");
                        return;
                }
                synchronized(mesh[world]) {
                        long ref = mesh[world].getTileRefAt((int)tx/GenerateNav.TILESIZE,(int)ty/GenerateNav.TILESIZE,0);
                        if (ref==0) {
                                System.err.println("unload: no tile found at "+tx+","+ty);
                                return;
                        }
                        try {
                                mesh[world].removeTile(ref);
                        } catch(Exception ex) {
                                System.err.println("failed to remove tile "+ref+" world="+world);
                                ex.printStackTrace();
                        }
                }
        }
        
        // load all tiles for an "area" which is currently 600x600 units
  
        public boolean loadArea(AreaInfo zi) {
                Box box=zi.getBox();
                int world=zi.getWorld();
                for (int tx=(int)box.minx; tx<box.maxx; tx+=GenerateNav.TILESIZE)
                        for (int ty=(int)box.minz; ty<box.maxz; ty+=GenerateNav.TILESIZE) {
                                if (!load(tx,ty,world)) {
                                        return false;
                                }
                        }
                // after adding tiles to this world's NavMesh, recreate the NavMeshQuery
                synchronized(mesh[world]) {
                        UDPServer.Log("detour tiles "+world+": "+mesh[world].getTileCount());
                        query[world] = new NavMeshQuery(mesh[world]);
                }
                return true;
        }

          public void unloadArea(AreaInfo zi) {
                Box box=zi.getBox();
                int world=zi.getWorld();
                for (int tx=(int)box.minx; tx<box.maxx; tx+=GenerateNav.TILESIZE)
                        for (int ty=(int)box.minz; ty<box.maxz; ty+=GenerateNav.TILESIZE)
                                unload(tx,ty,world);            
                synchronized(mesh) {
                        UDPServer.Log("detour tiles "+world+": "+mesh[world].getTileCount());
                        query[world] = new NavMeshQuery(mesh[world]);
                }
        }

  
        // constructor
  
        public Detour() throws IOException {
                filter = new DefaultQueryFilter(65535,0, new float[] { 1f, 10f, 1f, 1f, 2f, 1.5f });            
                halfExtents=new float[] { 20,500,20 };
                mesh=new NavMesh[MAX_WORLD];
                query=new NavMeshQuery[MAX_WORLD];
        }

  
        // pathfinding methods
  
        public List<StraightPathItem> getStraightPath(Point from,Point to,int world) {
                if (query[world]==null) return null;
                Result<FindNearestPolyResult> polyFrom = query[world].findNearestPoly(from.array(),halfExtents,filter);
                Result<FindNearestPolyResult> polyTo = query[world].findNearestPoly(to.array(), halfExtents, filter);
                Result<List<Long>> path = query[world].findPath(polyFrom.result.getNearestRef(),polyTo.result.getNearestRef(), polyFrom.result.getNearestPos(), polyTo.result.getNearestPos(), filter);
                Result<List<StraightPathItem>> ret = query[world].findStraightPath(from.array(), to.array(), path.result, 100, NavMeshQuery.DT_STRAIGHTPATH_ALL_CROSSINGS);
                return ret.result;
        }
        
        
        public Point onGround(Point p,int world) {
                if (query[world]==null) return p;
                Result<FindNearestPolyResult> poly=query[world].findNearestPoly(p.array(), halfExtents, filter);
                float[] pos=poly.result.getNearestPos();
                if (pos==null) return p;
                return new Point(pos);
        }
        
        public long getPolyRef(Point p,int world) {
                if (query[world]==null) return 0;
                Result<FindNearestPolyResult> poly=query[world].findNearestPoly(p.array(), halfExtents, filter);
                return poly.result.getNearestRef();
        }
        
        public NavMesh getNavMesh(int world) {
                return mesh[world];
        }


       // fix a position for an attacking NPC, 3 meters from the target
  
       public Point fixAttackPoint(Point cp, Point d,int world) {
                if (query[world]==null) return cp.add(d);
                long ref=getPolyRef(cp,world);
                Result<FindDistanceToWallResult> res = query[world].findDistanceToWall(ref, cp.array(),3, filter);
                if (res==null || res.result==null)
                        return cp.add(d);
                float wd=res.result.getDistance();
                if (wd>3) wd=3;
                d.unitify();
                d.multiplyWith(wd);
                return cp.add(d);
        }
  
}

  
  
  
  
  
