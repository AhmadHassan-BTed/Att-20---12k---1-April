package tv.kiekko.eqoa.udp;

import java.io.InputStream;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.TimerTask;
import java.util.Vector;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import io.netty.bootstrap.Bootstrap;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.DatagramPacket;
import io.netty.channel.socket.nio.NioDatagramChannel;
import tv.kiekko.eqoa.Util;
import tv.kiekko.eqoa.db.MySQL;
import tv.kiekko.eqoa.geom.Box;
import tv.kiekko.eqoa.geom.Point;
import tv.kiekko.eqoa.geom.QuadTreeTest;
import tv.kiekko.eqoa.login.Account;


public class UDPServer {
        EventLoopGroup group;
        static Map<ConnLookup,Connection> connections=new HashMap<ConnLookup,Connection>();
        static Map<Integer,Entity> entities=new HashMap<Integer,Entity>();


  
       // Send an Entity State ("object update") to all Connections that "see" the Entity.
       // The last parameter may contain a Connection that will not receive the update.
       // Each Connection has a separate list of Entities in range.
  
        public static void broadcastStateExcept(Entity src,Message m,Connection except) {
                synchronized(connections) {
                        for (Connection c: connections.values()) {
                                if (c.stateChannels!=null && c!=except && c.range!=null) {
                                        // Determine the channel number this Connection uses for this Entity.
                                        // -1 means this Connection doesn't "see" this Entity.
                                        int ch=c.range.getChannelOf(src);
                                        if (ch==-1) continue;
                                        Message mm=m.clone();
                                        mm.type=(byte)ch;
                                        c.sendState(mm);
                                        flushConnection(c);
                                }
                        }
                }
        }
        
        public static void broadcastMessage(Entity src,Message m) {
                synchronized(connections) {
                        for (Connection c: connections.values()) {
                                if (src==null || src.connection==c || (c.range!=null && c.range.contains(src))) {
                                        c.sendMessage(m.clone());
                                        flushConnection(c);
                                }
                        }
                }
        }

        public static void broadcastState(Entity src,Message m) {
                broadcastStateExcept(src,m,null);
        }
  
  
  
  

}

