// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Numerics;
using QuadTrees;
using ReturnHome.Server.EntityObject;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Utilities;
using ReturnHome.Server.Zone;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes;

namespace ReturnHome.Server.Managers
{
    public static class MapManager
    {
        private static ConcurrentDictionary<World, Map> _gameDict;
        //private static ObjectID class

        //Initializes all the map qudtree's
        public static void Initialize()
        {
            //make game map Dictionary
            _gameDict = new ConcurrentDictionary<World, Map>();
            _gameDict.TryAdd(World.Tunaria, new Map("Tunaria"));
            _gameDict.TryAdd(World.RatheMountains, new Map("Rathe Mountains"));
            _gameDict.TryAdd(World.Odus, new Map("Odus"));
            _gameDict.TryAdd(World.LavaStorm, new Map("Lava Storm"));
            _gameDict.TryAdd(World.PlaneOfSky, new Map("Plane of Sky"));
            _gameDict.TryAdd(World.Secrets, new Map("Secrets"));
            foreach (var m in _gameDict)
                m.Value.Initialize();
        }
        
        //Adds entity to world buffers, to be loaded in on next tick
        public static void Add(Entity e)
        {
            if(_gameDict.TryGetValue(e.World, out Map m))
                m.AddObject(e);

            else
                Logger.Err($"Error grabbing world: {e.World}");
        }

        public static void UpdatePosition(Entity e)
        {
            if (_gameDict.TryGetValue(e.World, out Map m))
                m.UpdatePosition(e);

            else
                Logger.Err($"Error Updating Object position, World: {e.World} Object: {e.CharName}");
        }

        public static void RemoveObject(Entity e)
        {
            if(e == null)
                return;
            if(_gameDict.TryGetValue(e.World, out Map m))
                m.RemoveObject(e);

            else
                Logger.Err($"Error grabbing world: {e.World}");
        }

        public static List<Entity> QueryObjects(Entity e, float Radius)
        {
            if (_gameDict.TryGetValue(e.World, out Map m))
                return m.Query(e, Radius);

            return default;
        }

        //Will force each map to do a bulk drop in, then distribute objects if needed
        public static void UpdateMaps()
        {
            foreach (var m in _gameDict)
            {
                m.Value.AddBulkObjects();
                m.Value.RemoveBulkObjects();
                m.Value.QueryObjectsForDistribution();
            }
        }
    }
}
