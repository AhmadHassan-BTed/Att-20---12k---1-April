using System;
using System.Collections.Concurrent;

namespace ReturnHome.Server.EntityObject
{
    public class ObjectIDCreator
    {
        private ConcurrentDictionary<uint, Entity> _playerObjectID = new ConcurrentDictionary<uint, Entity>();
        private bool _isNPC;
        private uint _minSize;
        private uint _maxSize;
        private byte _increase;
        private uint _current;
        private uint Current
        {
            get { return _current; }
            set
            {
                if (value > _maxSize)
                {
                    _current = _minSize;
                }

                else
                {
                    _current = value;
                }
            }
        }

        //if this is for npc, true
        //players are false
        public ObjectIDCreator(bool isNPC)
        {
            _isNPC = isNPC;
            if (_isNPC)
            {
                _minSize = 150000;
                _maxSize = 199999;
                Current = 150000;
                _increase = 1;
            }

            else
            {
                _minSize = 200000;
                _maxSize = 249999;
                Current = 200000;
                _increase = 2;
            }
            //Primary use is for this to be for adding npc's dynamically, we currently create player Object id's another way, but should likely be incorporated here, too/
            //Doing this for now for testing
            _maxSize = 249999;
        }

        //Basic Add for adding characters to the tracking object
        public bool AddEntity(Entity ent)
        {
            if (_playerObjectID.TryAdd(ent.ObjectID, ent))
                return true;

            return false;
        }

        public bool GenerateID(Entity entity, out uint ObjectID)
        {
            ObjectID = 0;
            ushort temp = 0;
            if (entity != null)
            {
                while (true)
                {
                    if (_playerObjectID.TryAdd(Current, entity))
                    {
                        ObjectID = Current;
                        Current += _increase;
                        return true;
                    }
                    Current += 1;
                    temp += 1;

                    //Shouldn't ever get here hopefully
                    if (temp == 20000)
                        Console.WriteLine("An error has occured where we couldn't assign a object ID, please escalate");
                }
            }

            //Entity class was null for some reason?
            return false;
        }

        public bool QueryEntity(uint val, out Entity ent)
        {

            if (val > _maxSize)
            {
                ent = default;
                return false;
            }

            _playerObjectID.TryGetValue(val, out Entity ent1);
            ent = ent1;

            return true;
        }

        public bool RemoveEntity(uint val)
        {
            return _playerObjectID.TryRemove(val, out Entity _);
        }
    }
}
