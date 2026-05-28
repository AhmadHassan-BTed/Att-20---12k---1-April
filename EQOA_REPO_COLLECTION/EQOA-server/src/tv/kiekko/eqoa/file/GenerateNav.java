package tv.kiekko.eqoa.file;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.recast4j.demo.builder.TileNavMeshBuilder;
import org.recast4j.demo.geom.DemoInputGeomProvider;
import org.recast4j.demo.io.ObjImporter;
import org.recast4j.detour.MeshData;
import org.recast4j.detour.NavMesh;
import org.recast4j.detour.NavMeshBuilder;
import org.recast4j.detour.NavMeshDataCreateParams;
import org.recast4j.detour.NavMeshParams;
import org.recast4j.detour.Tupple2;
import org.recast4j.detour.io.MeshSetReader;
import org.recast4j.detour.io.MeshSetWriter;
import org.recast4j.recast.AreaModification;
import org.recast4j.recast.PolyMesh;
import org.recast4j.recast.PolyMeshDetail;
import org.recast4j.recast.Recast;
import org.recast4j.recast.RecastBuilder;
import org.recast4j.recast.RecastBuilder.RecastBuilderResult;
import org.recast4j.recast.RecastBuilderConfig;
import org.recast4j.recast.RecastConfig;
import org.recast4j.recast.RecastConstants;
import org.recast4j.recast.geom.InputGeomProvider;
import org.recast4j.recast.geom.TriMesh;

import tv.kiekko.eqoa.file.WorldZoneProxies.ZoneProxy;
import tv.kiekko.eqoa.geom.Box;
import tv.kiekko.eqoa.geom.Point;

// This program is used to generate Recast/Detour navmeshes from Tunaria.ESF or other world .ESF files
// Running this probably requires a few gigabytes of memory and may take hours or days.

public class GenerateNav {

	public final static int TILESIZE=50;

	final static int world=0;
	
	static HashMap<String,String> debugGrid=new HashMap<String,String>();
	
	
	static boolean isGenerated(Box box) {
		float x=box.getMax().x-TILESIZE;
		float z=box.getMax().z-TILESIZE;
		x=TILESIZE*Math.round(x/TILESIZE);
		z=TILESIZE*Math.round(z/TILESIZE);
		String outFile="nav"+world+"/nav-"+(int)x+"-"+(int)z+".mesh";
		return new File(outFile).exists();
	}
	
	static void round(Box box) {
		box.minx=TILESIZE*Math.round(box.minx/TILESIZE);
		box.minz=TILESIZE*Math.round(box.minz/TILESIZE);
		box.maxx=TILESIZE*Math.round(box.maxx/TILESIZE);
		box.maxz=TILESIZE*Math.round(box.maxz/TILESIZE);
	}
	
  
  // generate navmeshes for a 500 x 500 terrain .OBJ file
  // save 50 x 50 tiles in separate files
  
	public static List<String> convert(String objFile,Box box,Box zoneBox) throws IOException  {
		long started=System.currentTimeMillis();
		InputGeomProvider geometry=new ObjImporter().load(new FileInputStream(objFile));
		List<String> outfiles=new ArrayList<String>();
		
		RecastConfig cfg = new RecastConfig(RecastConstants.PartitionType.WATERSHED,
                0.2f, 0.2f,
                2, 0.6f, 0.6f, 70,
                8, 20,
                20, 1, 6,
                6,2, (int)((TILESIZE)/0.2f), new AreaModification(1,7));
		
		float[] bmin=geometry.getMeshBoundsMin();
		float[] bmax=geometry.getMeshBoundsMax();
		for (int i=0; i<3; i+=2) {
			bmin[i]=TILESIZE*Math.round(bmin[i]/TILESIZE);
			bmax[i]=TILESIZE*Math.round(bmax[i]/TILESIZE);

		}	
		
		System.out.println("geometry min="+new Point(geometry.getMeshBoundsMin())+" max="+new Point(geometry.getMeshBoundsMax()));
		System.out.println("box="+box);
		
		
		RecastBuilder rcBuilder = new RecastBuilder();
    Log("Building RC Result.");
		RecastBuilderResult[][] results = rcBuilder.buildTiles(geometry, cfg,1);
    Log("Build done.");
		
		
		
        for (int tx=0; tx<results.length; tx++) {
        	for (int ty=0; ty<results[tx].length; ty++) {
        		
        		int wx=(int)(box.getMin().x+TILESIZE*tx);
		        int wy=(int)(box.getMin().z+TILESIZE*ty);

		        String outFile="nav"+world+"/nav-"+wx+"-"+wy+".mesh";
		        
		        
		        System.out.println("generating "+outFile);
		        
        		if (tx==0 && box.getMin().x!=zoneBox.getMin().x) {
        			System.out.println("skip left "+outFile);
        			continue;
        		}
        		if (ty==0 && box.getMin().z!=zoneBox.getMin().z) {
        			System.out.println("skip top "+outFile);
        			continue; 
        		}
        		if (tx==results.length-1 && box.getMax().x!=zoneBox.getMax().x) {
        			System.out.println("skip right "+outFile);
        			continue;
        		}
        		if (ty==results[tx].length-1 && box.getMax().z!=zoneBox.getMax().z) {
        			System.out.println("skip bottom "+outFile);
        			continue; 
        		}
        		
		        if (debugGrid.containsKey(outFile)) {
		        	System.out.println(outFile+" was created at "+debugGrid.get(outFile));
		        	System.out.println("now box="+box+" tx="+tx+" ty="+ty);
		        	System.exit(0);
		        }
		        debugGrid.put(outFile,"box="+box+" tx="+tx+" ty="+ty);
        		
        		RecastBuilder.RecastBuilderResult rcResult=results[tx][ty];
				System.out.println("result["+tx+"]["+ty+"]="+rcResult);
				if (rcResult==null) System.exit(0);
		        PolyMesh pMesh = rcResult.getMesh();
		        Log("Get Mesh done.");
		        for (int i = 0; i < pMesh.npolys; ++i) {
		            pMesh.flags[i] = 1;
		        }
		        PolyMeshDetail dMesh = rcResult.getMeshDetail();
		        Log("Get Mesh Details done. dMesh="+dMesh);
		        if (dMesh==null) continue;
		        NavMeshDataCreateParams params = new NavMeshDataCreateParams();
		        params.verts = pMesh.verts;
		        params.vertCount = pMesh.nverts;
		        params.polys = pMesh.polys;
		        params.polyAreas = pMesh.areas;
		        params.polyFlags = pMesh.flags;
		        params.polyCount = pMesh.npolys;
		        params.nvp = pMesh.nvp;
		        params.detailMeshes = dMesh.meshes;
		        params.detailVerts = dMesh.verts;
		        params.detailVertsCount = dMesh.nverts;
		        params.detailTris = dMesh.tris;
		        params.detailTriCount = dMesh.ntris;
		        params.walkableHeight = cfg.walkableHeight*cfg.ch;
		        params.walkableRadius = cfg.walkableRadius*cfg.cs;
		        params.walkableClimb = cfg.walkableClimb*cfg.ch;
		        params.bmin = pMesh.bmin;
		        params.bmax = pMesh.bmax;
		        params.cs = cfg.cs;
		        params.ch = cfg.ch;
		        params.buildBvTree = true;
		        
		        params.offMeshConVerts = new float[0];                    
		        params.offMeshConRad = new float[0];
		        params.offMeshConDir = new int[0];
		        params.offMeshConAreas = new int[0];
		        params.offMeshConFlags = new int[0];
		        params.offMeshConUserID = new int[0];
		        params.offMeshConCount = 0;
		        params.tileX = (int)((wx / TILESIZE);
		        params.tileY = (int)((wy / TILESIZE);
		        
		        
		        Log("Building Nav Mesh MeshData. tileX="+params.tileX+" tileY="+params.tileY);
		        MeshData meshData = NavMeshBuilder.createNavMeshData(params);   
		        Log("Building Done.");                    
		        
		        FileOutputStream fos = new FileOutputStream(outFile);
		        MeshSetWriter writer = new MeshSetWriter();                               
		        Log("Creating Nav Mesh "+outFile);
		        NavMesh navMesh = new NavMesh(meshData, 6, 0);                                         
		        Log("Saving Nav Mesh.");
		        writer.write(fos, navMesh, ByteOrder.LITTLE_ENDIAN, true);
		        fos.close();
		        Log("NavMesh saved, t="+(System.currentTimeMillis()-started));
		        outfiles.add(outFile);
			}
        }
        return outfiles;
	}
	

	static void Log(String s) {
		System.out.println(s);
	}
	
	// create a 500 x 500 terrain .OBJ file
	
	static void writeObj(ObjFile file,Box box,String objFile,int zone) throws IOException {
		ObjExport e = new ObjExport();
		e.setExportType(1);
		ObjInfo root = file.getRoot();
		List<ObjInfo> zones = root.getChild(ObjType.World).getChildren(ObjType.Zone);
		Zone z = (Zone) zones.get(zone).getObj();
		e.setSizeCutoff(1);
		e.setExportBox(box);
		LODSprite.setLowlevel(true);
		e.addAll(z.getSpritePlacements(),file);
		e.write(objFile);
	}
	
	
	static List<String> generateNav(ObjFile file,Box box,Box zoneBox,int zone) throws IOException {
		String objFile="tmp-"+(int)box.getMin().x+"-"+(int)box.getMin().z+".obj";
		writeObj(file,box,objFile,zone);
		List<String> ret = convert(objFile,box,zoneBox);
		new File(objFile).delete();
		return ret;
	}
                                 
  // Generating navmeshes for a whole 2000 x 2000 zone turned out
  // infeasible - I ran out of memory and it took too many hours.
  // Here we generate the navmeshes for a 500 x 500 area at once.
  // To ensure that the 50 x 50 tiles work properly seamlessly
  // everywhere, the 500 x 500 areas overlap by 100 distance units.                                

	// 0 .. 500, 400 .. 900, 800 .. 1300, 1200 .. 1700, 1600 .. 2000
	
	
	final static int[] ranges_min=new int[] {   0, 400,  800, 1200, 1600 }; 
	final static int[] ranges_max=new int[] { 500, 900, 1300, 1700, 2000 };
	
	
	static void createTilesForZone(ObjFile file,Box zoneBox,int zone) throws IOException {
		Box box=null;
		Set<String> meshFiles=new HashSet<String>();
		
		for (int rx=0; rx<ranges_min.length; rx++) {
			for (int ry=0; ry<ranges_min.length; ry++) {
				box=new Box();
				box.add(new Point(zoneBox.getMin().x+ranges_min[rx],zoneBox.getMin().y,zoneBox.getMin().z+ranges_min[ry]));
				box.add(new Point(zoneBox.getMin().x+ranges_max[rx],zoneBox.getMax().y,zoneBox.getMin().z+ranges_max[ry]));
				System.out.println("rx="+rx+" ry="+ry+" box="+box);
				List<String> tiles = generateNav(file,box,zoneBox,zone);
				if (tiles!=null) meshFiles.addAll(tiles);
			}
		}

	}
	
	static String tunariaPath = "Tunaria.esf";

	
	public static void main(String[] a) throws IOException {
		if (a.length > 0)
			tunariaPath = a[0];
		
		ObjFile file = new ObjFile(tunariaPath);
		
		
		WorldZoneProxies proxies = (WorldZoneProxies)file.getRoot().getChild(ObjType.WorldBase)
				.getChild(ObjType.WorldZoneProxies).getObj();
		
		for (int z=0; z<proxies.zones.length; z++) {
			System.out.println("========= ZONE "+z+" =======");
			Box zoneBox=proxies.zones[z].box.copy();
			round(zoneBox);
			if (isGenerated(zoneBox)) continue;
			System.out.println("zoneBox="+zoneBox);
			createTilesForZone(file,zoneBox,z);
		}
	}
	
	
}
