// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ReturnHome.Server.Zone
{
    using System;
    using System.Collections.Generic;
    using System.Drawing;
    using System.Numerics;
    using QuadTrees;
    using ReturnHome.Server.EntityObject;
    using ReturnHome.Server.EntityObject.Player;
    using ReturnHome.Server.Managers;

    public class Map
    {
        private QuadTreePointF<Entity> _qtree;
        private List<Character> _playerList = new();
        private List<Entity> _entityBuffer = new();
        private List<Entity> _removeBuffer = new();
        //private quadTree 
        private string _name;
        public string Name
        {
            get { return _name; }
            set
            {
                _name = value;
            }

        }

        public Map(string name)
        {
            Name = name;
        }

        public void Initialize()
        {
            if (Name == "Tunaria")
                _qtree = new QuadTreePointF<Entity>(new RectangleF(3000.0f, 3000.0f, 24000.0f, 30000.0f));

            else if (Name == "Odus")
                _qtree = new QuadTreePointF<Entity>(new RectangleF(3000.0f, 1000.0f, 10000.0f, 12000.0f));

            else if (Name == "Rathe Mountains")
                _qtree = new QuadTreePointF<Entity>(new RectangleF(3000.0f, 2000.0f, 7000.0f, 8000.0f));

            else if (Name == "Lava Storm")
                _qtree = new QuadTreePointF<Entity>(new RectangleF(4000.0f, 4000.0f, 4000.0f, 2000.0f));

            else if (Name == "Secrets")
                _qtree = new QuadTreePointF<Entity>(new RectangleF(4000.0f, 2000.0f, 2000.0f, 6000.0f));

            else if (Name == "Plane of Sky")
                _qtree = new QuadTreePointF<Entity>(new RectangleF(4000.0f, 4000.0f, 2000.0f, 4000.0f));

            else
            {
                Console.WriteLine("Error occured loading map. Shutting Server down.");
                return;
            }

            Console.WriteLine($"Starting QuadTree for {Name}");
        }
        
        //Queue's entities to be added into the map on next tick
        public void AddObject(Entity e)
        {
            _entityBuffer.Add(e);

            //Idea if for this to be temporary just for testing purposes
            e.map = this;

            if (e.isPlayer)
                _playerList.Add((Character)e);
        }

        //Called by server tick to bulk add entities
        public void AddBulkObjects()
        {
            _qtree.AddBulk(_entityBuffer);
            _entityBuffer.Clear();
        }

        public void QueryObjectsForDistribution()
        {
            List<Entity> entityList = new();

            foreach (Character entity in _playerList)
            {
                _qtree.GetObjects(new RectangleF(entity.x - 100f, entity.z - 100f, 200f, 150.0f), entityList);

                //Sort Character List
                entityList = entityList.OrderBy(x => Vector3.Distance(new Vector3(entity.x, entity.y, entity.z), new Vector3(x.x, x.y, x.z))).ToList();

                entity.characterSession.rdpCommIn.connectionData.AddChannelObjects(entityList);

                entityList.Clear();
            }
        }

        public void UpdatePosition(Entity e)
        {
            _qtree.Move(e);
        }

        public void RemoveObject(Entity e)
        {
            _removeBuffer.Add(e);
        }

        public void RemoveBulkObjects()
        {
            foreach(Entity e in _removeBuffer)
            {
                _qtree.Remove(e);
                if (e.isPlayer)
                    _playerList.Remove((Character)e);
            }

            _removeBuffer.Clear();
        }

        public List<Entity> Query(Entity e, float Radius)
        {
            List<Entity> entityList = new();
            _qtree.GetObjects(new RectangleF(e.x - (Radius / 2), e.z - (Radius / 2), Radius, Radius), entityList);

            return entityList;
        }
    }
}
